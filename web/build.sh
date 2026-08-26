#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# Build the WebAssembly demo of the Course Registration System.
#
# One-time setup (installs the Emscripten compiler, ~a few minutes):
#
#   git clone https://github.com/emscripten-core/emsdk.git
#   cd emsdk
#   ./emsdk install latest
#   ./emsdk activate latest
#   source ./emsdk_env.sh        # (run this in each new shell)
#
# Then, from the project root:
#
#   bash web/build.sh
#
# Output: web/registration.js  +  web/registration.wasm
# Serve the web/ folder over HTTP (see web/DEPLOY.md) — opening index.html
# directly with file:// will NOT work because browsers block WASM there.
# ---------------------------------------------------------------------------
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"

if ! command -v em++ >/dev/null 2>&1; then
  echo "ERROR: 'em++' not found. Install & activate the Emscripten SDK first"
  echo "       (see the setup notes at the top of this script)."
  exit 1
fi

echo "Compiling main_web.cpp -> WebAssembly ..."
em++ -std=c++11 -O2 "$here/main_web.cpp" \
  -sASYNCIFY \
  -sALLOW_MEMORY_GROWTH=1 \
  -sEXIT_RUNTIME=0 \
  -sENVIRONMENT=web \
  -sEXPORTED_RUNTIME_METHODS=stringToNewUTF8,UTF8ToString \
  -o "$here/registration.js"

echo "Done."
echo "  -> $here/registration.js"
echo "  -> $here/registration.wasm"
echo
echo "Preview locally with:"
echo "  cd \"$here\" && python3 -m http.server 8080"
echo "  then open http://localhost:8080/"
