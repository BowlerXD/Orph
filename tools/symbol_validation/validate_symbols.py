#!/usr/bin/env python3
"""Validate critical IL2CPP symbols against a dumped .cs file."""

from __future__ import annotations

import argparse
import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple


METHOD_DECL_RE = re.compile(
    r"^\s*(?:\[[^\]]+\]\s*)*(?:public|private|protected|internal)\s+"
    r"(?:static\s+|virtual\s+|override\s+|sealed\s+|extern\s+|unsafe\s+|new\s+|abstract\s+|partial\s+)*"
    r"[\w<>,\[\]\.?\s]+\s+([A-Za-z_]\w*)\s*\(([^)]*)\)"
)

FIELD_DECL_RE = re.compile(
    r"^\s*(?:\[[^\]]+\]\s*)*(?:public|private|protected|internal)\s+"
    r"(?:static\s+|readonly\s+|const\s+|volatile\s+)*[\w<>,\[\]\.?\s]+\s+([A-Za-z_]\w*)\s*(?:=|;)"
)

CLASS_DECL_RE = re.compile(r"^\s*(?:public|private|internal|protected)?\s*(?:abstract\s+|sealed\s+|partial\s+)*class\s+([A-Za-z_]\w*)")
NAMESPACE_COMMENT_RE = re.compile(r"^\s*//\s*Namespace:\s*(.*)$")


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


def parse_arg_count(args_blob: str) -> int:
    stripped = args_blob.strip()
    if not stripped:
        return 0
    parts = [p.strip() for p in stripped.split(",")]
    return sum(1 for p in parts if p and p != "void")


def parse_dump(dump_path: Path) -> Dict[str, ClassData]:
    classes: Dict[str, ClassData] = {}

    current_namespace = ""
    current_class_fqcn: Optional[str] = None
    class_brace_depth = 0

    for raw_line in dump_path.read_text(encoding="utf-8", errors="ignore").splitlines():
        ns_match = NAMESPACE_COMMENT_RE.match(raw_line)
        if ns_match:
            ns_value = ns_match.group(1).strip()
            current_namespace = "" if ns_value in ("<global>", "-") else ns_value
            continue

        class_match = CLASS_DECL_RE.match(raw_line)
        if class_match:
            class_name = class_match.group(1)
            current_class_fqcn = f"{current_namespace}.{class_name}".strip(".")
            classes.setdefault(current_class_fqcn, ClassData(methods={}, fields=set()))
            class_brace_depth = raw_line.count("{") - raw_line.count("}")
            continue

        if current_class_fqcn:
            class_data = classes[current_class_fqcn]

            method_match = METHOD_DECL_RE.match(raw_line)
            if method_match:
                method_name, args_blob = method_match.groups()
                arg_count = parse_arg_count(args_blob)
                class_data.methods.setdefault(method_name, set()).add(arg_count)

            field_match = FIELD_DECL_RE.match(raw_line)
            if field_match:
                class_data.fields.add(field_match.group(1))

            class_brace_depth += raw_line.count("{") - raw_line.count("}")
            if class_brace_depth <= 0:
                current_class_fqcn = None
                class_brace_depth = 0

    return classes


def load_critical_symbols(path: Path) -> List[dict]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, list):
        raise ValueError("Critical symbols JSON must be a list")
    return data


def evaluate_symbols(symbols: List[dict], classes: Dict[str, ClassData]) -> List[SymbolResult]:
    results: List[SymbolResult] = []

    for symbol in symbols:
        symbol_type = symbol["type"]
        namespace = symbol.get("namespace", "")
        class_name = symbol["class"]
        member_name = symbol.get("name")
        arg_count = symbol.get("arg_count")

        fqcn = f"{namespace}.{class_name}".strip(".")
        class_data = classes.get(fqcn)

        if symbol_type == "class":
            if class_data:
                results.append(SymbolResult(symbol_type, namespace, class_name, None, None, "FOUND", "class exists in dump"))
            else:
                results.append(SymbolResult(symbol_type, namespace, class_name, None, None, "MISSING", "class not found in dump"))
            continue

        if not class_data:
            results.append(
                SymbolResult(symbol_type, namespace, class_name, member_name, arg_count, "MISSING", "class not found in dump")
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
            available = class_data.methods.get(member_name or "", set())
            if not available:
                status, notes = "MISSING", "method not found"
            elif arg_count in available:
                status, notes = "FOUND", f"matched arg_count={arg_count}"
            else:
                status = "SIGNATURE_MISMATCH"
                counts = ", ".join(str(v) for v in sorted(available))
                notes = f"expected arg_count={arg_count}, found arg_count=[{counts}]"

            results.append(SymbolResult(symbol_type, namespace, class_name, member_name, arg_count, status, notes))
            continue

        raise ValueError(f"Unknown symbol type: {symbol_type}")

    return results


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

    has_issues = any(r.status in {"MISSING", "SIGNATURE_MISMATCH"} for r in results)
    return 1 if has_issues and args.fail_on_issues else 0


if __name__ == "__main__":
    raise SystemExit(main())
