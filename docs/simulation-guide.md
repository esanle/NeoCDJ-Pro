# Simulation Guide

The NeoCDJ-Pro simulator runs on macOS or Linux without any hardware. It renders the full Apple-style UI at 800×480 via SDL2.

## Quick Start

```bash
# macOS
brew install cmake ninja sdl2
./scripts/run-sim.sh

# Ubuntu / Debian
sudo apt-get install cmake ninja-build libsdl2-dev
./scripts/run-sim.sh
```

The first run fetches LVGL from GitHub (~15 MB). Subsequent builds are incremental.

## Keyboard Mapping

All keyboard mappings replicate the CDJ-2000 physical layout.

### Transport

| Key       | CDJ Button    |
|-----------|---------------|
| `Space`   | Play / Pause  |
| `C`       | Cue           |
| `T`       | Sync          |
| `M`       | Master        |

### Hot Cues

| Key | Hot Cue |
|-----|---------|
| `1` | A (Red) |
| `2` | B (Orange) |
| `3` | C (Yellow) |
| `4` | D (Purple) |
| `5` | E |
| `6` | F |
| `7` | G |
| `8` | H |

### Loop

| Key   | Function    |
|-------|-------------|
| `I`   | Loop In     |
| `O`   | Loop Out    |
| `R`   | Re-Loop     |
| `[`   | Loop Half   |
| `]`   | Loop Double |

### Performance

| Key | Function        |
|-----|-----------------|
| `S` | Slip Mode       |
| `Q` | Quantize        |

### Navigation

| Key          | Function        |
|--------------|-----------------|
| `← / →`      | Track Prev/Next |
| `Esc`        | Back            |
| `Enter`      | Menu / Select   |

### Jog Wheel

| Input                       | Function                    |
|-----------------------------|-----------------------------|
| `↑` / `↓`                   | Jog CCW / CW (no touch)     |
| `Shift + ↑` / `↓`           | Jog with touch (scratch)    |
| Right-click + drag left/right | Jog with touch (scratch)  |
| Mouse wheel                 | Scroll file browser         |

### Pitch Slider

| Key       | Function          |
|-----------|-------------------|
| `+` / `=` | Pitch +1% (up)    |
| `-`       | Pitch -1% (down)  |

### USB Simulation

| Key | Action |
|-----|--------|
| `U` | Toggle USB insert / eject |

USB insert maps `test_assets/mock_usb/` as the virtual drive root.  
Drop FLAC/WAV/MP3 files there and press `U` to simulate inserting the drive.

```
test_assets/
└── mock_usb/
    ├── PIONEER/
    │   └── USBANLZ/        ← Rekordbox analysis files
    ├── 01 - Track One.flac
    ├── 02 - Track Two.wav
    └── ...
```

## Debug Overlays

LVGL's built-in monitors are enabled in `lv_conf.h`:

- **FPS + CPU** (top-left corner) — `LV_USE_PERF_MONITOR 1`
- **Heap usage** (top-right corner) — `LV_USE_MEM_MONITOR 1`

Target: CPU < 40%, stable 60 fps.

## Build Options

```bash
# Release build (faster rendering)
cmake -B firmware/build/sim-release -S firmware \
      -DPLATFORM=sim -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build firmware/build/sim-release
./firmware/build/sim-release/neo-cdj-sim

# Run unit tests
cmake -B firmware/build/tests -S firmware \
      -DPLATFORM=sim -DBUILD_TESTS=ON -G Ninja
cmake --build firmware/build/tests
ctest --test-dir firmware/build/tests
```

## VS Code Integration

Install the **CMake Tools** extension and select the `sim` configure preset. F5 launches and attaches the debugger directly to the simulator.

Recommended `.vscode/launch.json`:
```json
{
  "configurations": [{
    "name": "NeoCDJ Sim (Debug)",
    "type": "cppdbg",
    "request": "launch",
    "program": "${workspaceFolder}/firmware/build/sim/neo-cdj-sim",
    "cwd": "${workspaceFolder}/firmware/build/sim",
    "MIMode": "lldb"
  }]
}
```
