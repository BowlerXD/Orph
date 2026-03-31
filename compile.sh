#!/usr/bin/env bash
set -euo pipefail

if [ ! -f jni/Android.mk ]; then
  echo "Error: jni/Android.mk tidak ditemukan. Pastikan dijalankan dari root project." >&2
  exit 1
fi

if [ ! -f jni/Application.mk ]; then
  echo "Error: jni/Application.mk tidak ditemukan. Pastikan konfigurasi NDK tersedia." >&2
  exit 1
fi

NDK_PROJECT_PATH=jni NDK_APPLICATION_MK=jni/Application.mk ~/ndk-aide/ndk-build
