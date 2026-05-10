/**
 * sim_main.c — Simulator entry point
 *
 * Architecture (Phase 0 — single-threaded):
 *   main()
 *     └─ SDL_Init
 *     └─ lv_init + lv_port_disp_init (creates SDL2 window)
 *     └─ lv_port_indev_init (mouse + wheel)
 *     └─ app_main_init() (initialises all app subsystems, loads screen)
 *     └─ event loop:
 *           SDL_PollEvent → feeds wheel delta to LVGL indev
 *           lv_timer_handler → renders frame
 *           app_main_tick → polls CDJ controls → event_bus
 *           SDL_Delay(5)
 *
 * FreeRTOS (Phase 1): audio task + USB watcher task will be added here,
 * with lv_port_disp_init() keeping SDL on the main thread.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "app_main.h"
#include "hal.h"

/* ── HAL implementations (sim) ────────────────────────────────────────────── */
#include "mock_usb_msc.h"
#include "mock_controls.h"
#include "mock_audio.h"

/* ── HAL: tick ───────────────────────────────────────────────────────────── */
uint32_t hal_tick_ms(void) {
    return SDL_GetTicks();
}

void hal_delay_ms(uint32_t ms) {
    SDL_Delay(ms);
}

/* ── HAL: display (sim delegates to lv_port_disp) ────────────────────────── */
void hal_display_init(void) {
    lv_port_disp_init();
    lv_port_indev_init();
}

/* ── HAL: audio ──────────────────────────────────────────────────────────── */
void hal_audio_init(const hal_audio_cfg_t *cfg) {
    mock_audio_init(cfg);
}
void hal_audio_write(const void *samples, uint32_t frames) {
    mock_audio_write(samples, frames);
}
uint32_t hal_audio_frames_free(void) {
    return mock_audio_frames_free();
}
void hal_audio_deinit(void) {
    mock_audio_deinit();
}

/* ── HAL: USB / storage ──────────────────────────────────────────────────── */
void hal_usb_init(hal_usb_event_cb_t cb) {
    mock_usb_init(cb);
}
bool hal_usb_is_mounted(void) {
    return mock_usb_is_mounted();
}
const char *hal_usb_mount_path(void) {
    return mock_usb_mount_path();
}
void hal_usb_simulate_insert(void) { mock_usb_insert(); }
void hal_usb_simulate_eject(void)  { mock_usb_eject();  }

/* ── HAL: controls ───────────────────────────────────────────────────────── */
void hal_controls_init(void) {
    mock_controls_init();
}
void hal_controls_poll(hal_controls_state_t *out) {
    mock_controls_poll(out);
}

/* ── HAL: task (Phase 0: no-op — tasks introduced in Phase 1) ────────────── */
void hal_task_create(hal_task_fn_t fn, const char *name,
                     uint32_t stack_bytes, void *arg, int priority) {
    /* Phase 0: run the task function inline (single-threaded sim).
     * Phase 1: replace with pthread_create or FreeRTOS xTaskCreate. */
    (void)name; (void)stack_bytes; (void)priority;
    if (fn) fn(arg);
}
void hal_task_delay_ms(uint32_t ms) {
    SDL_Delay(ms);
}

/* ── Main ────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    /* SDL must be initialised on the main thread (required by macOS Cocoa) */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "[sim] SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    printf("NeoCDJ-Pro Simulator — Phase 0\n");
    printf("SDL version: %d.%d.%d\n",
           SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

    /* LVGL init */
    lv_init();
    lv_tick_set_cb(hal_tick_ms);

    /* Display + input devices (creates the SDL2 window) */
    hal_display_init();

    /* App init (loads main screen, subscribes to events) */
    app_main_init();

    /* ── Main loop ─────────────────────────────────────────────────────── */
    bool running = true;
    while (running) {
        /* Process SDL events:
         *   - SDL_QUIT  → exit
         *   - SDL_MOUSEWHEEL → forward delta to LVGL encoder indev
         *   - All other events update SDL's internal keyboard/mouse state
         *     which lv_port_indev and mock_controls read via GetState APIs */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEWHEEL:
                    /* Feed wheel movement to LVGL's encoder indev */
                    lv_port_indev_sdl_wheel_add(e.wheel.y);
                    break;
                default:
                    break;
            }
        }

        if (!running) break;

        /* LVGL: process timers, run animations, call flush callbacks */
        uint32_t delay_ms = lv_timer_handler();

        /* App: poll CDJ controls and dispatch events */
        app_main_tick();

        /* Sleep for the minimum of LVGL's requested delay and 16 ms (60 fps cap) */
        uint32_t sleep = (delay_ms < 16) ? delay_ms : 16;
        if (sleep > 0) SDL_Delay(sleep);
    }

    printf("[sim] Exiting.\n");
    hal_audio_deinit();
    lv_deinit();
    SDL_Quit();
    return 0;
}
