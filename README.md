# NeoCDJ-Pro

A modern CDJ-2000-compatible DJ player with Apple-style UI, built on STM32H7 + FreeRTOS + LVGL v9.

## Quick Start (Simulator)

```bash
# macOS
brew install cmake ninja sdl2
./scripts/run-sim.sh

# Ubuntu
sudo apt-get install cmake ninja-build libsdl2-dev
./scripts/run-sim.sh
```

This opens an 800×480 SDL2 window. No hardware required.

## Build System

```bash
cd firmware

# Simulator
cmake --preset sim && cmake --build --preset sim
./build/sim/neo-cdj-sim

# STM32H743 (requires arm-none-eabi-gcc)
cmake --preset stm32 && cmake --build --preset stm32

# Unit tests
cmake --preset tests && cmake --build --preset tests
ctest --test-dir build/tests
```

## Keyboard Controls

| Key | Function |
|-----|----------|
| `Space` | Play / Pause |
| `C` | Cue |
| `1`–`8` | Hot Cues A–H |
| `I` / `O` | Loop In / Out |
| `S` / `Q` / `T` | Slip / Quantize / Sync |
| `↑` / `↓` | Jog CCW / CW |
| `Shift + ↑↓` | Jog with touch (scratch) |
| `+` / `-` | Pitch ±1% |
| `U` | USB insert / eject |
| `←` / `→` | Track Prev / Next |

See [docs/simulation-guide.md](docs/simulation-guide.md) for the full reference.

## Architecture

```
app/         ← 100% cross-platform business logic
platform/
  common/hal.h  ← single interface between app/ and hardware
  sim/          ← SDL2 + mock peripherals
  stm32/        ← STM32H7 HAL implementation
```

See [docs/firmware-architecture.md](docs/firmware-architecture.md).

## Development Milestones

- [x] **Phase 0** — Repository skeleton, CMake dual-target, simulator boots to blank LVGL window
- [ ] **Phase 1** — mock USB + FatFs + Apple-style file browser
- [ ] **Phase 2** — Full LVGL theme + waveform Canvas + keyboard mapping
- [ ] **Phase 3** — Audio decode (FLAC/WAV) + Pitch DSP + SDL2 playback
- [ ] **Phase 4** — Rekordbox ANLZXXXX.DAT parser (Cue, Beat Grid, Waveform)
- [ ] **Phase 5** — Full UI integration + perf optimisation + test coverage

## License

MIT
