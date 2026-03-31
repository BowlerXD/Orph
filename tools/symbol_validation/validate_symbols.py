#!/usr/bin/env python3
"""Validate critical IL2CPP symbols against a dumped .cs file."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple


METHOD_DECL_RE = re.compile(
    r"^\s*(?:\[[^\]]+\]\s*)*"
    r"(?P<access>public|private|protected|internal)\s+"
    r"(?P<tail>.+?)\s*\((?P<args>[^)]*)\)"
)

FIELD_DECL_RE = re.compile(
    r"^\s*(?:\[[^\]]+\]\s*)*(?:public|private|protected|internal)\s+"
    r"(?:static\s+|readonly\s+|const\s+|volatile\s+)*[\w<>,\[\]\.\?\s]+\s+([A-Za-z_]\w*)\s*(?:=|;)"
)

CLASS_DECL_RE = re.compile(r"^\s*(?:public|private|internal|protected)?\s*(?:abstract\s+|sealed\s+|partial\s+)*class\s+([A-Za-z_]\w*)")
NAMESPACE_COMMENT_RE = re.compile(r"^\s*//\s*Namespace:\s*(.*)$")
VALID_SYMBOL_TYPES = {"class", "field", "method"}
ISSUE_STATUSES = {"MISSING", "SIGNATURE_MISMATCH"}
METHOD_MODIFIERS = {
    "static",
    "virtual",
    "override",
    "sealed",
    "extern",
    "unsafe",
    "new",
    "abstract",
    "partial",
    "async",
    "readonly",
    "final",
}


@dataclass
class ClassData:
    methods: Dict[str, Set[int]]
    fields: Set[str]


@dataclass
class SymbolResult:
    symbol_type: str
    namespace: str
    class_name: str
    member_name: Optional[str]
    expected_arg_count: Optional[int]
    status: str
    notes: str

    def key(self) -> str:
        base = f"{self.namespace}.{self.class_name}".strip(".")
        if self.member_name:
            base += f"::{self.member_name}"
        return base


@dataclass(frozen=True)
class SymbolSpec:
    symbol_type: str
    namespace: str
    class_name: str
    member_name: Optional[str]
    expected_arg_counts: Optional[Set[int]]

    @property
    def expected_arg_count(self) -> Optional[int]:
        if not self.expected_arg_counts:
            return None
        return min(self.expected_arg_counts)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Validate critical symbols against IL2CPP dump .cs")
    parser.add_argument("--dump", required=True, type=Path, help="Path to dump .cs file")
    parser.add_argument(
        "--symbols",
        type=Path,
        default=Path("tools/symbol_validation/critical_symbols.json"),
        help="Path to critical symbols JSON",
    )
    parser.add_argument(
        "--report-json",
        type=Path,
        default=Path("reports/symbol_validation_report.json"),
        help="Output JSON report path",
    )
    parser.add_argument(
        "--report-txt",
        type=Path,
        default=Path("reports/symbol_validation_report.txt"),
        help="Output text report path",
    )
    parser.add_argument(
        "--fail-on-issues",
        action="store_true",
        help="Exit code 1 when MISSING/SIGNATURE_MISMATCH is found",
    )
    return parser.parse_args()


def split_top_level_args(args_blob: str) -> List[str]:
    parts: List[str] = []
    current: List[str] = []
    depth_angle = 0
    depth_round = 0
    depth_square = 0

    for char in args_blob:
        if char == "," and depth_angle == 0 and depth_round == 0 and depth_square == 0:
            segment = "".join(current).strip()
            if segment:
                parts.append(segment)
            current = []
            continue

        current.append(char)

        if char == "<":
            depth_angle += 1
        elif char == ">":
            depth_angle = max(0, depth_angle - 1)
        elif char == "(":
            depth_round += 1
        elif char == ")":
            depth_round = max(0, depth_round - 1)
        elif char == "[":
            depth_square += 1
        elif char == "]":
            depth_square = max(0, depth_square - 1)

    tail = "".join(current).strip()
    if tail:
        parts.append(tail)

    return parts


def parse_arg_count(args_blob: str) -> int:
    stripped = args_blob.strip()
    if not stripped:
        return 0
    parts = split_top_level_args(stripped)
    return sum(1 for p in parts if p and p != "void")


def extract_method_signature(line: str) -> Optional[Tuple[str, str]]:
    method_match = METHOD_DECL_RE.match(line)
    if not method_match:
        return None

    tail = method_match.group("tail").strip()
    args_blob = method_match.group("args")
    if not tail:
        return None

    tokens = tail.split()
    while tokens and tokens[0] in METHOD_MODIFIERS:
        tokens.pop(0)

    if len(tokens) < 2:
        return None

    method_name = tokens[-1]
    if not re.match(r"^(?:\.[A-Za-z_]\w*|[A-Za-z_<>]\S*)$", method_name):
        return None

    return method_name, args_blob


def parse_dump(dump_path: Path) -> Dict[str, ClassData]:
    classes: Dict[str, ClassData] = {}

    current_namespace = ""
    current_class_fqcn: Optional[str] = None
    class_brace_depth = 0
    classes_detected = 0
    methods_detected = 0
    lines_processed = 0
    progress_interval = 500_000

    with dump_path.open("r", encoding="utf-8", errors="ignore") as dump_file:
        for raw_line in dump_file:
            lines_processed += 1
            line = raw_line.rstrip("\r\n")

            if lines_processed % progress_interval == 0:
                print(
                    (
                        f"[parse_dump] processed {lines_processed:,} lines "
                        f"(classes={classes_detected:,}, methods={methods_detected:,})"
                    ),
                    file=sys.stderr,
                )

            ns_match = NAMESPACE_COMMENT_RE.match(line)
            if ns_match:
                ns_value = ns_match.group(1).strip()
                current_namespace = "" if ns_value in ("<global>", "-") else ns_value
                continue

            class_match = CLASS_DECL_RE.match(line)
            if class_match:
                class_name = class_match.group(1)
                current_class_fqcn = f"{current_namespace}.{class_name}".strip(".")
                is_new_class = current_class_fqcn not in classes
                classes.setdefault(current_class_fqcn, ClassData(methods={}, fields=set()))
                if is_new_class:
                    classes_detected += 1
                class_brace_depth = line.count("{") - line.count("}")
                continue

            if current_class_fqcn:
                class_data = classes[current_class_fqcn]

                method_signature = extract_method_signature(line)
                if method_signature:
                    method_name, args_blob = method_signature
                    arg_count = parse_arg_count(args_blob)
                    class_data.methods.setdefault(method_name, set()).add(arg_count)
                    methods_detected += 1

                field_match = FIELD_DECL_RE.match(line)
                if field_match:
                    class_data.fields.add(field_match.group(1))

                class_brace_depth += line.count("{") - line.count("}")
                if class_brace_depth <= 0:
                    current_class_fqcn = None
                    class_brace_depth = 0

    print(
        (
            f"[parse_dump] completed {lines_processed:,} lines "
            f"(classes={classes_detected:,}, methods={methods_detected:,})"
        ),
        file=sys.stderr,
    )

    return classes


def load_critical_symbols(path: Path) -> List[SymbolSpec]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, list):
        raise ValueError("Critical symbols JSON must be a list")
    return [parse_symbol_definition(symbol, index) for index, symbol in enumerate(data)]


def parse_symbol_definition(symbol: dict, index: int) -> SymbolSpec:
    if not isinstance(symbol, dict):
        raise ValueError(f"symbol at index {index} must be an object")

    symbol_type = symbol.get("type")
    if symbol_type not in VALID_SYMBOL_TYPES:
        raise ValueError(
            f"symbol at index {index} has invalid type '{symbol_type}', "
            f"expected one of {sorted(VALID_SYMBOL_TYPES)}"
        )

    if not isinstance(symbol.get("class"), str) or not symbol["class"]:
        raise ValueError(f"symbol at index {index} must include a non-empty 'class'")

    namespace = symbol.get("namespace", "")
    if namespace is not None and not isinstance(namespace, str):
        raise ValueError(f"symbol at index {index} has invalid 'namespace' (must be a string)")

    if symbol_type in {"field", "method"}:
        if not isinstance(symbol.get("name"), str) or not symbol["name"]:
            raise ValueError(f"{symbol_type} symbol at index {index} must include a non-empty 'name'")

    expected_arg_counts = normalize_expected_arg_counts(symbol)
    if symbol_type != "method" and expected_arg_counts is not None:
        raise ValueError(f"{symbol_type} symbol at index {index} cannot define arg_count/arg_counts")

    return SymbolSpec(
        symbol_type=symbol_type,
        namespace=namespace or "",
        class_name=symbol["class"],
        member_name=symbol.get("name"),
        expected_arg_counts=expected_arg_counts,
    )


def normalize_expected_arg_counts(symbol: dict) -> Optional[Set[int]]:
    if "arg_counts" in symbol:
        counts = symbol["arg_counts"]
        if not isinstance(counts, list) or not counts or not all(isinstance(v, int) for v in counts):
            raise ValueError("method symbol arg_counts must be a non-empty list of ints")
        return set(counts)

    arg_count = symbol.get("arg_count")
    if arg_count is None:
        return None
    if not isinstance(arg_count, int):
        raise ValueError("method symbol arg_count must be an int")
    return {arg_count}


def evaluate_symbols(symbols: List[SymbolSpec], classes: Dict[str, ClassData]) -> List[SymbolResult]:
    results: List[SymbolResult] = []
    classes_by_name: Dict[str, List[str]] = {}

    for fqcn in classes:
        class_name = fqcn.rsplit(".", 1)[-1]
        classes_by_name.setdefault(class_name, []).append(fqcn)

    for symbol in symbols:
        symbol_type = symbol.symbol_type
        namespace = symbol.namespace
        class_name = symbol.class_name
        member_name = symbol.member_name
        expected_arg_counts = symbol.expected_arg_counts

        fqcn = f"{namespace}.{class_name}".strip(".")
        class_data = classes.get(fqcn)
        class_lookup_note: Optional[str] = None

        if not class_data:
            fallback_matches = classes_by_name.get(class_name, [])
            if len(fallback_matches) == 1:
                class_data = classes[fallback_matches[0]]
                class_lookup_note = (
                    f"class matched by name only (expected '{fqcn}', found '{fallback_matches[0]}')"
                )
            elif len(fallback_matches) > 1:
                class_lookup_note = (
                    f"ambiguous class name '{class_name}' in dump: {', '.join(sorted(fallback_matches))}"
                )

        if symbol_type == "class":
            if class_data:
                notes = class_lookup_note or "class exists in dump"
                results.append(SymbolResult(symbol_type, namespace, class_name, None, None, "FOUND", notes))
            else:
                notes = class_lookup_note or "class not found in dump"
                results.append(SymbolResult(symbol_type, namespace, class_name, None, None, "MISSING", notes))
            continue

        if not class_data:
            results.append(
                SymbolResult(
                    symbol_type,
                    namespace,
                    class_name,
                    member_name,
                    symbol.expected_arg_count,
                    "MISSING",
                    class_lookup_note or "class not found in dump",
                )
            )
            continue

        if symbol_type == "field":
            if member_name in class_data.fields:
                status, notes = "FOUND", "field found"
            else:
                status, notes = "MISSING", "field not found"
            results.append(SymbolResult(symbol_type, namespace, class_name, member_name, None, status, notes))
            continue

        if symbol_type == "method":
            status, notes = evaluate_method_symbol(member_name or "", class_data, expected_arg_counts)
            results.append(
                SymbolResult(
                    symbol_type,
                    namespace,
                    class_name,
                    member_name,
                    symbol.expected_arg_count,
                    status,
                    notes,
                )
            )
            continue

        raise ValueError(f"Unknown symbol type: {symbol_type}")

    return results


def evaluate_method_symbol(
    member_name: str, class_data: ClassData, expected_arg_counts: Optional[Set[int]]
) -> Tuple[str, str]:
    available = class_data.methods.get(member_name, set())
    if not available:
        return "MISSING", "method not found"

    if expected_arg_counts is None:
        return "FOUND", "method found"

    matched_counts = available & expected_arg_counts
    if matched_counts:
        if len(expected_arg_counts) == 1:
            return "FOUND", f"matched arg_count={next(iter(matched_counts))}"
        expected = ", ".join(str(v) for v in sorted(expected_arg_counts))
        matched = ", ".join(str(v) for v in sorted(matched_counts))
        return "FOUND", f"matched arg_count=[{matched}] from expected=[{expected}]"

    counts = ", ".join(str(v) for v in sorted(available))
    expected = ", ".join(str(v) for v in sorted(expected_arg_counts))
    return "SIGNATURE_MISMATCH", f"expected arg_count=[{expected}], found arg_count=[{counts}]"


def write_reports(results: List[SymbolResult], json_path: Path, txt_path: Path) -> None:
    json_path.parent.mkdir(parents=True, exist_ok=True)
    txt_path.parent.mkdir(parents=True, exist_ok=True)

    grouped = {"FOUND": [], "MISSING": [], "SIGNATURE_MISMATCH": []}
    for result in results:
        grouped.setdefault(result.status, []).append(
            {
                "symbol_type": result.symbol_type,
                "symbol": result.key(),
                "member": result.member_name,
                "expected_arg_count": result.expected_arg_count,
                "notes": result.notes,
            }
        )

    payload = {
        "summary": {k: len(v) for k, v in grouped.items()},
        "results": grouped,
    }
    json_path.write_text(json.dumps(payload, indent=2), encoding="utf-8")

    lines = [
        "# Symbol Validation Report",
        "",
        f"FOUND: {len(grouped['FOUND'])}",
        f"MISSING: {len(grouped['MISSING'])}",
        f"SIGNATURE_MISMATCH: {len(grouped['SIGNATURE_MISMATCH'])}",
        "",
    ]

    for status in ("MISSING", "SIGNATURE_MISMATCH", "FOUND"):
        lines.append(f"## {status}")
        if not grouped[status]:
            lines.append("- (none)")
        else:
            for item in grouped[status]:
                lines.append(f"- {item['symbol']} -> {item['notes']}")
        lines.append("")

    txt_path.write_text("\n".join(lines).rstrip() + "\n", encoding="utf-8")


def main() -> int:
    args = parse_args()

    if not args.dump.exists():
        raise FileNotFoundError(f"Dump file not found: {args.dump}")

    symbols = load_critical_symbols(args.symbols)
    classes = parse_dump(args.dump)
    results = evaluate_symbols(symbols, classes)
    write_reports(results, args.report_json, args.report_txt)

    has_issues = any(r.status in ISSUE_STATUSES for r in results)
    return 1 if has_issues and args.fail_on_issues else 0


if __name__ == "__main__":
    raise SystemExit(main())
