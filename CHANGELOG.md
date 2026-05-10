# Changelog

## [Unreleased] — Phase 0

### Added
- CMake dual-target build system (`-DPLATFORM=sim|stm32`) with presets
- `platform/common/hal.h` — full platform abstraction interface covering display, audio, USB, controls, and task management
- `platform/sim/` — SDL2 display port, mouse/wheel input port, mock USB (folder-based hot-plug), mock controls (keyboard→CDJ mapping), mock audio (SDL2 ring-buffer output)
- `platform/stm32/` — STM32H743 HAL implementation stubs + linker script
- `app/core/event_bus` — lightweight pub/sub event bus (no FreeRTOS dependency)
- `app/gui/theme_apple` — LVGL v9 Apple-style dark theme (ARGB colours, rounded corners, Montserrat fonts)
- `app/gui/screen_main` — Phase 0 main screen: status bar, three-panel skeleton, splash overlay
- FreeRTOS configuration (`FreeRTOSConfig.h`) for both POSIX and ARM CM7 ports
- GitHub Actions CI: parallel sim + stm32 builds + unit tests
- `scripts/run-sim.sh` and `scripts/flash-stm32.sh`
- `docs/simulation-guide.md` — keyboard mapping reference
- `docs/firmware-architecture.md` — layer diagram + event flow
