#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APP="$SCRIPT_DIR/build/appDriverStation.app/Contents/MacOS/appDriverStation"

if [[ ! -x "$APP" ]]; then
    echo "找不到執行檔，請先執行：bash \"$SCRIPT_DIR/compile.sh\"" >&2
    exit 1
fi

# 執行應用程式
exec "$APP"
