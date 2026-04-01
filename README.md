# Orph

## Build/Release artifact policy

Kebijakan repo untuk artefak native:

- **Lokasi artefak rilis resmi yang boleh dibagikan:** `libs/<abi>/`
  - Contoh saat ini: `libs/armeabi-v7a/libAkSoundEngine2.so`.
- **Lokasi non-resmi (hasil build lokal) tidak boleh di-commit:** `obj/local/...`, `obj/...`, dan output sementara lain dari NDK.
- Jika build dilakukan penuh oleh CI/pipeline, binary hasil build **tidak perlu disimpan di Git**. Simpan hanya artefak release final yang memang akan didistribusikan.

Checklist singkat sebelum release:
1. Pastikan hanya binary release yang berada di `libs/<abi>/`.
2. Hapus salinan duplikat seperti `obj/local/**/libAkSoundEngine2.so` bila ada.
3. Verifikasi `.gitignore` menolak seluruh lokasi non-resmi.

## Symbol mismatch validation (pre-release)

Tambahan utilitas untuk cek simbol IL2CPP kritikal terhadap dump `.cs`.

### File validasi simbol
- Validator: `tools/symbol_validation/validate_symbols.py`
- Daftar simbol kritikal: `tools/symbol_validation/critical_symbols.json`
- Workflow update dump: `tools/symbol_validation/update_from_dump.sh`
- Lokasi dump standar: `artifacts/dumps/`
- Lokasi report per versi dump: `reports/symbol_validation/`

### Workflow update dump baru (URL/file)
Gunakan script berikut supaya alur update dump konsisten:

```bash
tools/symbol_validation/update_from_dump.sh <dump_url_atau_path_file>
```

Contoh untuk dump rilis `v2`:

```bash
tools/symbol_validation/update_from_dump.sh \
  https://github.com/BowlerXD/Orph/releases/download/v2/com.mobile.legends_2.1.61.11705.cs
```

Yang dilakukan script:
1. Simpan dump ke `artifacts/dumps/<nama_file_dump>.cs`.
2. Jalankan `validate_symbols.py` terhadap dump tersebut.
3. Generate report versi dump ke:
   - `reports/symbol_validation/<nama_file_dump_tanpa_.cs>.json`
   - `reports/symbol_validation/<nama_file_dump_tanpa_.cs>.txt`

Opsional agar command gagal jika ada `MISSING`/`SIGNATURE_MISMATCH`:

```bash
tools/symbol_validation/update_from_dump.sh <dump_url_atau_path_file> --fail-on-issues
```

### Menjalankan validator secara manual
```bash
python3 tools/symbol_validation/validate_symbols.py \
  --dump /path/ke/dump.cs
```

### Format status report
- `FOUND`: simbol ditemukan dan signature cocok.
- `MISSING`: class/member tidak ditemukan di dump.
- `SIGNATURE_MISMATCH`: method ada, tapi jumlah argumen (`arg_count`) beda.

### Baseline dump kompatibilitas
- Baseline validator saat ini: `com.mobile.legends_2.1.61.11705.cs` (rilis `v2`, 2026-03-31).
- Contoh validasi baseline:
  ```bash
  tools/symbol_validation/update_from_dump.sh \
    https://github.com/BowlerXD/Orph/releases/download/v2/com.mobile.legends_2.1.61.11705.cs
  ```
- Hasil baseline saat ini: `FOUND=68`, `MISSING=4`, `SIGNATURE_MISMATCH=0`.
- Daftar `MISSING` baseline: `BattleBridge::Instance`, `BattleBridge::bStartBattle`, `BattleBridge::OnSignReport`, `ACInterface::LoadCert`.

### Checklist release (singkat)
Sebelum merge update dump/simbol:
- [ ] Update simbol kritikal pada `tools/symbol_validation/critical_symbols.json` jika ada perubahan offset/signature dari dump terbaru.
- [ ] Jalankan validator (direkomendasikan lewat `update_from_dump.sh`).
- [ ] Review semua entri `MISSING` di report, pastikan memang expected atau diperbaiki dulu sebelum merge.

Catatan: utilitas ini standalone dan tidak mengubah build utama.

## Cara kerja fitur Anti AFK (tab Setting)

Fitur **Anti AFK (AI Control)** berjalan sebagai **virtual pulse**, bukan klik/tap nyata:

1. Toggle di tab **Setting** mengubah flag `Config.AntiAfkOnAIControl`.
2. Setiap frame render, engine memanggil `TickVirtualAntiAfk(inMatch)`.
3. Pulse hanya dikirim bila:
   - toggle aktif, dan
   - match valid/sedang berjalan (`inMatch == true`).
4. Ada cooldown **60 detik** antar pulse.
5. Pulse dikirim lewat API internal skill (`ShowSelfPlayer_TryUseSkill2`) dan gerak analog kecil (`MoveDir`) saat AI mengambil kontrol, untuk bantu mencegah status AFK tanpa input fisik pengguna.


## xHook debug policy (release)

Untuk menjaga build release tetap bersih dari peningkatan verbosity log saat runtime:

- JNI `NativeHandler.enableDebug(...)` tetap diekspos untuk kompatibilitas signature Java lama, tetapi implementasinya **no-op**.
- `xhook_enable_debug(...)` dan `xh_core_enable_debug(...)` dinonaktifkan untuk jalur app flow normal, sehingga level log xHook tidak bisa dinaikkan ke `DEBUG` dari sisi aplikasi.
- Kebijakan ini bersifat permanen untuk alur release kecuali ada perubahan desain eksplisit pada layer native.
