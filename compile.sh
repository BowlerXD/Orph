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

TMP_OBJ_DIR="$(mktemp -d "${TMPDIR:-/tmp}/orph-ndk-obj.XXXXXX")"
trap 'rm -rf "${TMP_OBJ_DIR}"' EXIT

NDK_PROJECT_PATH=. \
APP_BUILD_SCRIPT=jni/Android.mk \
NDK_APPLICATION_MK=jni/Application.mk \
NDK_OUT="${TMP_OBJ_DIR}" \
NDK_LIBS_OUT="${PWD}/libs" \
~/ndk-aide/ndk-build "$@"
