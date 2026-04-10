# NeoCDJ-Pro System Analysis Document (claude.md)

**Project Codename**: NeoCDJ-Pro  
**Objective**: Develop a **modern, uncompromising, Apple-style** DJ player hardware + firmware from scratch.  
**Core Constraints**:
- All **physical buttons, Jog Wheel, Pitch Slider, and knobs** must be **100% identical in position and function** to the original CDJ-2000 layout (zero learning curve for veteran DJs).
- Extremely **thin chassis** (target thickness ≤ 3.5 cm).
- **Most critical feature**: **Native USB flash drive (U-disk) hot-plug support + exFAT** (microSD compatible as well).
- UI must be **pure Apple style** (iOS-level rounded corners, blur effects, dark mode, San Francisco font, smooth 60 fps animations).
- **Simulation-First** development workflow: **No hardware development board required** for multi-person parallel development, UI debugging, audio DSP testing, or USB simulation.

This file is the **complete system requirements document** for **Claude Code / Cursor / Claude.dev**. Please generate the code repository strictly according to this document.

---

## 1. Project Overview and Positioning

- **Positioning**: A modern player that can directly replace the Pioneer CDJ-2000, while keeping the traditional CDJ look and feel, with a full Apple-style UI upgrade.
- **Non-compromising Features**:
  - Lossless audio support (FLAC / WAV / AIFF 24bit/96kHz)
  - Real-time high-precision Pitch / Tempo / BPM / Beat Grid / Slip / Quantize / Hot Cue / Loop / Memory Cue
  - Full Rekordbox ANLZXXXX.DAT database parsing (Cue, Waveform, Beat Grid)
  - Pro DJ Link (Ethernet sync, WiFi expansion possible later)
  - Ultra-low latency audio processing + high-quality DAC output
- **Hardware Form Factor**: Custom 4-layer ultra-thin PCB + 7-9 inch high-resolution IPS (LTDC driven) + capacitive touch (optional for browsing) + exact CDJ-2000 physical button matrix replication + high-precision optical Jog encoder + front USB-A Host port + CNC aluminum ultra-thin chassis.

---

## 2. Technology Stack (Must Be Strictly Followed)

| Module            | Technology Choice                     | Notes |
|-------------------|---------------------------------------|-------|
| Main MCU          | STM32H7xx (H743/H750/H7R preferred)  | Cortex-M7 600 MHz+ |
| RTOS              | FreeRTOS (official POSIX port for simulator) | Task isolation |
| GUI               | LVGL v9+ + SquareLine Studio          | Apple-style theme |
| Build System      | CMake + Ninja                         | Dual-target build |
| Storage           | USB Host MSC + FatFs + exFAT + microSD | **U-disk hot-plug is core** |
| Audio             | libFLAC + Helix + custom 32x oversampling interpolation + Pitch DSP | M7 FPU + DSP instructions |
| Platform Abstraction | platform/ layer (common / stm32 / sim) | Must be implemented |
| Simulator         | SDL2 + LVGL SDL port + POSIX FreeRTOS | Zero-hardware PC development |

**License**: MIT

---

## 3. Repository Structure (Must Be Created 100% According to This)

```bash
NeoCDJ-Pro/
├── README.md
├── LICENSE
├── .github/workflows/              # CI: sim + stm32 builds in parallel
├── docs/
│   ├── simulation-guide.md         # Keyboard mapping, mock usage
│   ├── firmware-architecture.md    # System architecture diagram
│   ├── ui-design-apple.md          # Apple style specification
│   └── usb-support-guide.md
├── hardware/                       # KiCad, 3D models, BOM (hardware only)
├── ui-design/                      # SquareLine Studio .slp files
├── scripts/
│   ├── run-sim.sh
│   └── flash-stm32.sh
├── firmware/                       # Core (focus area)
│   ├── CMakeLists.txt
│   ├── CMakePresets.json
│   ├── platform/
│   │   ├── common/                 # hal.h interface definitions
│   │   ├── stm32/                  # Real hardware implementation
│   │   └── sim/                    # PC simulator
│   │       ├── sdl/
│   │       ├── posix/
│   │       └── mocks/              # mock_usb_msc.c, mock_audio.c, mock_controls.c etc.
│   ├── app/                        # 100% cross-platform business logic
│   │   ├── core/
│   │   ├── gui/                    # LVGL screens (browser, waveform, cue_editor)
│   │   ├── audio/
│   │   ├── storage/                # USB + Rekordbox parser
│   │   ├── controls/
│   │   └── network/
│   ├── tests/                      # Unit tests (audio_dsp, rekordbox, usb_hotplug)
│   ├── third_party/                # submodules: LVGL, FreeRTOS, FatFs, libFLAC
│   └── build/
├── tools/
└── CHANGELOG.md
```
---

## 4. Simulation-First Development Mode (Must Be Implemented First)

- **Dual Build Targets**:
  - `neo-cdj-sim` (PC simulator, SDL2-based)
  - `neo-cdj-stm32` (real hardware)
- **CMake Option**: `-DPLATFORM=sim|stm32`
- **Simulator Features**:
  - LVGL + SDL2 window with real-time Apple-style UI
  - Keyboard/mouse fully emulates all CDJ-2000 buttons + Jog Wheel (mouse drag) + Pitch Slider
  - mock USB: PC folder `test_assets/mock_usb/` simulates U-disk hot-plug
  - SDL2 audio output (hear Pitch/Slip effects in real time)
  - FreeRTOS POSIX port (task scheduling identical to hardware)
- **Debugging**:
  - VSCode + CMake Tools + C/C++ extension
  - Built-in LV_USE_PERF_MONITOR + LV_USE_MEM_MONITOR
  - SquareLine Studio export → instant preview in simulator

---

## 5. UI Design Requirements (Apple Style)

- Overall aesthetic: Dark mode, heavy use of rounded corners, frosted glass blur, SF Pro font, fluid animations
- Main screen:
  - Left browser (Finder-like: album grid + list + search)
  - Large central waveform (dual-channel color, real-time scrolling, Cue points highlighted)
  - Right-side info + Hot Cue panel
- All animations at 60 fps (LVGL Canvas waveform rendering)
- Performance requirement: Perf Monitor CPU < 40%

---

## 6. Feature Priority (USB Is Highest Priority)

1. **U-disk Support** (Most Critical)
   - USB Host MSC + exFAT hot-plug
   - Auto-scan Rekordbox folder + ANLZ files
   - Auto-pause and show “USB removed” on unplug
2. Exact CDJ-2000 button mapping
3. Full Rekordbox parser
4. Real-time audio DSP (Pitch/BPM/Slip/Beat Grid)
5. Apple-style waveform + browser
6. Pro DJ Link (later)

---

## 7. Development Milestones (Claude Should Generate in This Order)

**Phase 0**: Repository skeleton + CMake dual-target + platform abstraction layer + simulator boots to blank LVGL window  
**Phase 1**: mock USB + FatFs + basic Apple-style file browser  
**Phase 2**: LVGL Apple theme + waveform Canvas + keyboard mapping  
**Phase 3**: Audio decoding + Pitch DSP + SDL2 real-time playback  
**Phase 4**: Rekordbox ANLZ parser + Cue/Loop support  
**Phase 5**: Full UI integration + performance optimization + test cases  

---

## 8. Claude Development Instructions

1. **Before generating any code**, always read this file first.
2. **Strictly follow the repository structure** — do not create extra folders arbitrarily.
3. **All app/ modules must go through `platform/common/hal.h` interfaces** — never call STM32 HAL or SDL directly.
4. **U-disk mock** must support hot-plug (folder switch = insert/eject simulation).
5. **UI must be Apple style** — use latest LVGL style APIs.
6. **When generating code**, also provide the corresponding `CMakeLists.txt` changes and `simulation-guide.md` updates.
