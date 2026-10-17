#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

# 配置與建構
cmake -S "$SCRIPT_DIR" -B "$SCRIPT_DIR/build" -DCMAKE_BUILD_TYPE=Release "$@"
cmake --build "$SCRIPT_DIR/build" --parallel

# 執行應用程式
exec bash "$SCRIPT_DIR/run.sh"
