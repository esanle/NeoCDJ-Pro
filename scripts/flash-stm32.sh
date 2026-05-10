#!/usr/bin/env bash
# flash-stm32.sh — Build and flash firmware to STM32H743 via ST-Link
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$SCRIPT_DIR/.."
FIRMWARE="$ROOT/firmware"
BUILD_DIR="$FIRMWARE/build/stm32"
ELF="$BUILD_DIR/neo-cdj-stm32"
BIN="$BUILD_DIR/neo-cdj-stm32.bin"

check_cmd() {
    if ! command -v "$1" &>/dev/null; then
        echo "ERROR: '$1' not found. $2"
        exit 1
    fi
}
check_cmd arm-none-eabi-gcc  "Install arm-none-eabi toolchain."
check_cmd cmake               "Install cmake."
check_cmd ninja               "Install ninja."

echo "─── NeoCDJ-Pro STM32 Flash ─────────────────────────────────────────────"
echo "Build dir: $BUILD_DIR"

# ── Configure ────────────────────────────────────────────────────────────────
if [[ ! -f "$BUILD_DIR/build.ninja" ]]; then
    echo "Configuring..."
    cmake -B "$BUILD_DIR" -S "$FIRMWARE" \
          -DPLATFORM=stm32 \
          -DCMAKE_BUILD_TYPE=Release \
          -G Ninja
fi

# ── Build ────────────────────────────────────────────────────────────────────
echo "Building..."
cmake --build "$BUILD_DIR" -- -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu)"

arm-none-eabi-size "$ELF"

# ── Flash ────────────────────────────────────────────────────────────────────
if command -v STM32_Programmer_CLI &>/dev/null; then
    echo "Flashing via STM32CubeProgrammer..."
    STM32_Programmer_CLI -c port=SWD -w "$BIN" 0x08000000 -v -rst
elif command -v openocd &>/dev/null; then
    echo "Flashing via OpenOCD..."
    openocd \
        -f interface/stlink.cfg \
        -f target/stm32h7x.cfg \
        -c "program $ELF verify reset exit"
else
    echo "WARNING: No flashing tool found. Binary is at:"
    echo "  $BIN"
    echo "Flash manually using STM32CubeProgrammer or OpenOCD."
fi
