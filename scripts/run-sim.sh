#!/usr/bin/env bash
# run-sim.sh — Configure, build, and launch the NeoCDJ-Pro simulator
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$SCRIPT_DIR/.."
FIRMWARE="$ROOT/firmware"
BUILD_DIR="$FIRMWARE/build/sim"

# ── Check dependencies ──────────────────────────────────────────────────────
check_cmd() {
    if ! command -v "$1" &>/dev/null; then
        echo "ERROR: '$1' not found. Install it and re-run."
        echo "  macOS:   brew install $2"
        echo "  Ubuntu:  sudo apt-get install $3"
        exit 1
    fi
}
check_cmd cmake  cmake   cmake
check_cmd ninja  ninja   ninja-build
check_cmd sdl2-config sdl2 libsdl2-dev 2>/dev/null || true

echo "─── NeoCDJ-Pro Simulator ───────────────────────────────────────────────"
echo "Build dir: $BUILD_DIR"

# ── Configure ───────────────────────────────────────────────────────────────
if [[ ! -f "$BUILD_DIR/build.ninja" ]]; then
    echo "Configuring..."
    cmake -B "$BUILD_DIR" -S "$FIRMWARE" \
          -DPLATFORM=sim \
          -DCMAKE_BUILD_TYPE=Debug \
          -DCMAKE_OSX_ARCHITECTURES=arm64 \
          -G Ninja
fi

# ── Build ────────────────────────────────────────────────────────────────────
echo "Building..."
cmake --build "$BUILD_DIR" -- -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu)"

# ── Run ──────────────────────────────────────────────────────────────────────
echo "Launching simulator..."
echo ""
cd "$BUILD_DIR"
exec ./neo-cdj-sim "$@"
