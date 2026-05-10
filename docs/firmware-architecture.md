# Firmware Architecture

## Layer Diagram

```
┌────────────────────────────────────────────────────────────────────────────┐
│                           app/  (cross-platform)                           │
│                                                                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │  core/       │  │  gui/        │  │  audio/      │  │  storage/    │  │
│  │  app_main.c  │  │  screen_main │  │  decoder     │  │  usb_browser │  │
│  │  event_bus.c │  │  theme_apple │  │  pitch_dsp   │  │  rekordbox   │  │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  │
│         │                 │                  │                  │          │
│         └─────────────────┴──────────────────┴──────────────────┘          │
│                                    │                                        │
│                            hal.h (interface)                               │
└────────────────────────────────────┬───────────────────────────────────────┘
                                     │
              ┌──────────────────────┴────────────────────────┐
              │  platform/                                    │
              │                                               │
   ┌──────────┴─────────────┐              ┌──────────────────┴──────────┐
   │  sim/                  │              │  stm32/                     │
   │  sim_main.c            │              │  stm32_main.c               │
   │  sdl/lv_port_disp.c    │              │  stm32_hal_impl.c           │
   │  sdl/lv_port_indev.c   │              │  (LTDC, SAI, USB HS, GPIO)  │
   │  mocks/mock_usb_msc.c  │              │                             │
   │  mocks/mock_controls.c │              │                             │
   │  mocks/mock_audio.c    │              │                             │
   └────────────────────────┘              └─────────────────────────────┘
```

## Key Invariant

`app/` code compiles identically for both targets. The only platform-conditional code lives in `platform/sim/` and `platform/stm32/`. This is enforced by the CMake build: `app/` sources are never given `-DPLATFORM_SIM` or `-DPLATFORM_STM32` include dirs.

## Event Bus

All inter-module communication goes through `event_bus.c`. No module calls another module's functions directly — it posts an event and any subscriber reacts.

```
Controls poll → app_main_tick → event_bus_post(EVT_JOG_TURN)
                                          │
                             ┌────────────┘
                             │
                     screen_main (subscriber) → updates waveform position
                     audio     (subscriber) → scratch DSP
```

## FreeRTOS Task Map (Phase 1+)

| Task       | Priority | Stack   | Notes |
|------------|----------|---------|-------|
| GUI        | 2        | 16 KB   | lv_timer_handler + app_main_tick |
| Audio      | 3        | 8 KB    | Decode + DSP + SAI DMA feed |
| USB/FatFs  | 2        | 4 KB    | MSC mount + directory scan |
| Net (P5+)  | 1        | 4 KB    | Pro DJ Link Ethernet |

Phase 0 uses a single-threaded main loop (no scheduler) for simulator compatibility.

## Display Pipeline

```
LVGL render → flush_cb (partial band)
   ↓
sim:   SDL_UpdateTexture → SDL_RenderPresent  (software or Metal/GL)
stm32: DMA2D → LTDC framebuffer → TFT panel  (zero-copy)
```

## USB Hot-Plug Sequence

```
hardware:                  sim (mock):
USB inserted               press 'U' key
     │                          │
USB_OTG IRQ              mock_usb_toggle()
     │                          │
hal_usb_init cb          HAL_USB_EVENT_INSERTED
     │                          │
event_bus_post(EVT_USB_INSERTED)
     │
screen_main → "USB Connected" label
storage task → scan PIONEER/ folder → populate browser
```
