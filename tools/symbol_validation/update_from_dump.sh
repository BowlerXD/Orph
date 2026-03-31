#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

usage() {
  cat <<'USAGE'
Usage:
  tools/symbol_validation/update_from_dump.sh <dump_url_or_file_path> [--fail-on-issues]

Examples:
  tools/symbol_validation/update_from_dump.sh \
    https://github.com/BowlerXD/Orph/releases/download/v2/com.mobile.legends_2.1.61.11705.cs

  tools/symbol_validation/update_from_dump.sh /tmp/com.mobile.legends_2.1.61.11705.cs --fail-on-issues
USAGE
}

if [[ $# -lt 1 ]]; then
  usage
  exit 1
fi

SOURCE="$1"
shift

FAIL_FLAG=""
if [[ $# -gt 0 ]]; then
  if [[ "$1" == "--fail-on-issues" ]]; then
    FAIL_FLAG="--fail-on-issues"
  else
    echo "Unknown argument: $1" >&2
    usage
    exit 1
  fi
fi

DUMP_DIR="${REPO_ROOT}/artifacts/dumps"
REPORT_DIR="${REPO_ROOT}/reports/symbol_validation"
mkdir -p "${DUMP_DIR}" "${REPORT_DIR}"

is_url=false
if [[ "${SOURCE}" =~ ^https?:// ]]; then
  is_url=true
fi

if [[ "${is_url}" == true ]]; then
  filename="$(basename "${SOURCE}")"
  filename="${filename%%\?*}"
  if [[ -z "${filename}" || "${filename}" == "/" ]]; then
    echo "Unable to detect filename from URL: ${SOURCE}" >&2
    exit 1
  fi
  dump_path="${DUMP_DIR}/${filename}"
  echo "[1/3] Download dump from URL -> ${dump_path}"
  curl -fL --retry 3 --retry-delay 2 -o "${dump_path}" "${SOURCE}"
else
  source_path="$(python3 -c 'import os,sys; print(os.path.abspath(sys.argv[1]))' "${SOURCE}")"
  if [[ ! -f "${source_path}" ]]; then
    echo "Dump file not found: ${source_path}" >&2
    exit 1
  fi
  filename="$(basename "${source_path}")"
  dump_path="${DUMP_DIR}/${filename}"
  if [[ "${source_path}" == "${dump_path}" ]]; then
    echo "[1/3] Dump file already in standard location -> ${dump_path}"
  else
    echo "[1/3] Copy dump file -> ${dump_path}"
    cp -f "${source_path}" "${dump_path}"
  fi
fi

version_name="${filename%.cs}"
report_json="${REPORT_DIR}/${version_name}.json"
report_txt="${REPORT_DIR}/${version_name}.txt"

cmd=(
  python3 "${REPO_ROOT}/tools/symbol_validation/validate_symbols.py"
  --dump "${dump_path}"
  --report-json "${report_json}"
  --report-txt "${report_txt}"
)

if [[ -n "${FAIL_FLAG}" ]]; then
  cmd+=("${FAIL_FLAG}")
fi

echo "[2/3] Run symbol validator"
"${cmd[@]}"

echo "[3/3] Done"
echo "Dump saved at: ${dump_path}"
echo "Report JSON  : ${report_json}"
echo "Report TXT   : ${report_txt}"
