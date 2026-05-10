# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

NeoCDJ-Pro is a CDJ-2000-compatible DJ player with Apple-style UI, built on STM32H7xx + FreeRTOS + LVGL v9. The defining design constraint is a **simulation-first** workflow: all development runs on macOS/Linux via SDL2 before touching hardware. USB flash drive (U-disk) hot-plug with exFAT is the single highest-priority feature.

## Build Commands

All firmware lives under `firmware/`. Two build targets exist:

```bash
# Simulator (macOS/Linux, requires SDL2)
cmake -B firmware/build/sim -S firmware -DPLATFORM=sim -G Ninja
cmake --build firmware/build/sim
./firmware/build/sim/neo-cdj-sim

# STM32 (requires arm-none-eabi-gcc toolchain)
cmake -B firmware/build/stm32 -S firmware -DPLATFORM=stm32 -G Ninja
cmake --build firmware/build/stm32

# Convenience scripts
./scripts/run-sim.sh
./scripts/flash-stm32.sh
```

Unit tests (under `firmware/tests/`):
```bash
cmake -B firmware/build/tests -S firmware -DPLATFORM=sim -DBUILD_TESTS=ON -G Ninja
cmake --build firmware/build/tests
ctest --test-dir firmware/build/tests
# Run a single test binary directly, e.g.:
./firmware/build/tests/test_rekordbox_parser
```

## Architecture: Platform Abstraction Layer

**The golden rule**: `app/` code may **never** call SDL2 or STM32 HAL directly. All hardware access must go through `platform/common/hal.h`.

```
firmware/
├── platform/
│   ├── common/hal.h        ← single interface all app/ code calls
│   ├── stm32/              ← HAL impl for real hardware
│   └── sim/
│       ├── sdl/            ← SDL2 display + audio backend
│       ├── posix/          ← FreeRTOS POSIX port
│       └── mocks/          ← mock_usb_msc.c, mock_audio.c, mock_controls.c
└── app/                    ← 100% cross-platform business logic
    ├── core/               ← FreeRTOS tasks, event bus
    ├── gui/                ← LVGL screens: browser, waveform, cue_editor
    ├── audio/              ← libFLAC/Helix decode + Pitch DSP
    ← storage/             ← USB MSC + FatFs/exFAT + Rekordbox ANLZ parser
    ├── controls/           ← button matrix, jog wheel, pitch slider
    └── network/            ← Pro DJ Link (future)
```

**USB mock**: `test_assets/mock_usb/` acts as the virtual U-disk. Drop/remove audio files there to simulate hot-plug insert/eject.

## Technology Stack

| Layer | Choice |
|---|---|
| MCU | STM32H7xx (H743/H750/H7R), Cortex-M7 @ 600 MHz+ |
| RTOS | FreeRTOS (POSIX port in simulator) |
| GUI | LVGL v9+ (submodule in `third_party/`) |
| Build | CMake + Ninja, `-DPLATFORM=sim\|stm32` |
| Storage | USB Host MSC + FatFs + exFAT |
| Audio | libFLAC + Helix MP3 + custom 32x oversampling Pitch DSP |
| Simulator | SDL2 + LVGL SDL driver |

Third-party libraries are git submodules under `firmware/third_party/` (LVGL, FreeRTOS, FatFs, libFLAC).

## UI Rules (LVGL Apple Style)

- Dark mode only; frosted-glass blur panels; SF Pro font (or closest available); all corner radii ≥ 12px.
- Waveform rendered via LVGL Canvas — must sustain 60 fps with CPU < 40% (verify with `LV_USE_PERF_MONITOR`).
- Three-panel main layout: left file browser (Finder-style), center waveform + transport, right Hot Cue panel.
- Use latest LVGL v9 style APIs — do not use deprecated v8 style calls.

## Development Milestones (implement in order)

- **Phase 0** — CMake dual-target skeleton + `hal.h` stubs + simulator opens blank LVGL window
- **Phase 1** — mock USB + FatFs mount + basic Apple-style file browser
- **Phase 2** — Full LVGL Apple theme + waveform Canvas + keyboard/mouse CDJ-2000 mapping
- **Phase 3** — Audio decode (FLAC/WAV) + Pitch DSP + SDL2 real-time playback
- **Phase 4** — Rekordbox ANLZXXXX.DAT parser (Cue points, Beat Grid, Waveform data)
- **Phase 5** — Full UI integration + perf optimization + unit test coverage

## Key Constraints

- Physical button layout must be **pixel-identical** to CDJ-2000 — no rearranging for convenience.
- Chassis target ≤ 3.5 cm thick — affects PCB stackup choices in `hardware/`.
- When adding any new file under `app/`, also update `firmware/CMakeLists.txt` and note any keyboard mapping additions in `docs/simulation-guide.md`.
- CI (`.github/workflows/`) must build both `sim` and `stm32` targets in parallel on every push.
