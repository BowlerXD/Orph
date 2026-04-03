import tempfile
import unittest
from pathlib import Path

import importlib.util
import sys

MODULE_PATH = Path(__file__).resolve().parents[1] / "validate_symbols.py"
spec = importlib.util.spec_from_file_location("validate_symbols", MODULE_PATH)
validate_symbols = importlib.util.module_from_spec(spec)
assert spec and spec.loader
sys.modules[spec.name] = validate_symbols
spec.loader.exec_module(validate_symbols)

extract_method_signature = validate_symbols.extract_method_signature
parse_dump = validate_symbols.parse_dump


class ExtractMethodSignatureTests(unittest.TestCase):
    def test_parses_generic_nested_type(self):
        line = (
            "private static System.Collections.Generic.Dictionary<System.String, "
            "Foo.Bar<Baz.Qux<System.Int32>>> BuildMap(System.Int32 value);"
        )
        self.assertEqual(extract_method_signature(line), ("BuildMap", "System.Int32 value"))

    def test_parses_pointer_and_nullable(self):
        line = (
            "internal unsafe System.Nullable<System.Int32>* Resolve(System.Byte* ptr, "
            "System.Nullable<System.Int32> value);"
        )
        self.assertEqual(
            extract_method_signature(line),
            ("Resolve", "System.Byte* ptr, System.Nullable<System.Int32> value"),
        )

    def test_parses_extra_modifiers(self):
        line = (
            "public static readonly override extern unsafe System.Int32 "
            "Compute(ref System.Int32& a, out System.Int32& b);"
        )
        self.assertEqual(
            extract_method_signature(line),
            ("Compute", "ref System.Int32& a, out System.Int32& b"),
        )

    def test_parses_ctor_and_cctor(self):
        self.assertEqual(
            extract_method_signature("public System.Void .ctor(System.String str);"),
            (".ctor", "System.String str"),
        )
        self.assertEqual(
            extract_method_signature("private static System.Void .cctor();"),
            (".cctor", ""),
        )

    def test_parses_generic_compiler_generated_method_name(self):
        line = "internal System.Int32 <BuildTailoringTables>b__17_0(System.Object a, System.Object b);"
        self.assertEqual(
            extract_method_signature(line),
            ("<BuildTailoringTables>b__17_0", "System.Object a, System.Object b"),
        )

    def test_returns_none_for_unparseable_line(self):
        line = "public .ctor(System.String invalid);"
        self.assertIsNone(extract_method_signature(line))


class ParseDumpFallbackTests(unittest.TestCase):
    def test_unparseable_line_does_not_create_false_found_method(self):
        dump = """// Namespace: Demo
public class Foo {
    public .ctor(System.String invalid);
    public System.Void RealMethod();
}
"""
        with tempfile.TemporaryDirectory() as tmp:
            dump_path = Path(tmp) / "sample.cs"
            dump_path.write_text(dump, encoding="utf-8")
            parsed = parse_dump(dump_path)

        methods = parsed["Demo.Foo"].methods
        self.assertNotIn(".ctor", methods)
        self.assertIn("RealMethod", methods)

    def test_supports_method_overloads(self):
        dump = """// Namespace: Demo
public class Overload {
    public System.Void Ping();
    public System.Void Ping(System.Int32 value);
    public System.Void Ping(System.Int32 value, System.String name);
}
"""
        with tempfile.TemporaryDirectory() as tmp:
            dump_path = Path(tmp) / "sample.cs"
            dump_path.write_text(dump, encoding="utf-8")
            parsed = parse_dump(dump_path)

        self.assertEqual(parsed["Demo.Overload"].methods["Ping"], {0, 1, 2})

    def test_ignores_braces_inside_line_comments_for_class_scope(self):
        dump = """// Namespace: Demo
public class Foo {
    public System.Int32 A; // fake brace }
    public System.Void RealMethod();
}
"""
        with tempfile.TemporaryDirectory() as tmp:
            dump_path = Path(tmp) / "sample.cs"
            dump_path.write_text(dump, encoding="utf-8")
            parsed = parse_dump(dump_path)

        self.assertIn("Demo.Foo", parsed)
        self.assertIn("A", parsed["Demo.Foo"].fields)
        self.assertIn("RealMethod", parsed["Demo.Foo"].methods)


if __name__ == "__main__":
    unittest.main()
