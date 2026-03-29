# Orph

## Symbol mismatch validation (pre-release)

Tambahan utilitas untuk cek simbol IL2CPP kritikal terhadap dump `.cs`.

### File yang ditambahkan
- Script: `tools/symbol_validation/validate_symbols.py`
- Daftar simbol kritikal: `tools/symbol_validation/critical_symbols.json`
- Output report default:
  - `reports/symbol_validation_report.json`
  - `reports/symbol_validation_report.txt`

### Cara menjalankan
```bash
python3 tools/symbol_validation/validate_symbols.py \
  --dump /path/ke/dump.cs
```

Contoh dengan output custom:
```bash
python3 tools/symbol_validation/validate_symbols.py \
  --dump /path/ke/dump.cs \
  --report-json reports/release_symbol_report.json \
  --report-txt reports/release_symbol_report.txt
```

Jika ingin pipeline gagal saat ada issue (`MISSING`/`SIGNATURE_MISMATCH`):
```bash
python3 tools/symbol_validation/validate_symbols.py \
  --dump /path/ke/dump.cs \
  --fail-on-issues
```

### Format status report
- `FOUND`: simbol ditemukan dan signature cocok.
- `MISSING`: class/member tidak ditemukan di dump.
- `SIGNATURE_MISMATCH`: method ada, tapi jumlah argumen (`arg_count`) beda.

Catatan: utilitas ini standalone dan tidak mengubah build utama.
