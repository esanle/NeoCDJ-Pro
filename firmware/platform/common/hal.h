/**
 * hal.h — NeoCDJ-Pro Platform Abstraction Layer
 *
 * RULE: Every file under app/ MUST only call functions declared here.
 *       Never call SDL2, STM32 HAL, FreeRTOS, or POSIX APIs directly from app/.
 *
 * Each function has two implementations:
 *   firmware/platform/sim/   ← SDL2 + POSIX + mock peripherals
 *   firmware/platform/stm32/ ← STM32H7 HAL + real peripherals
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ════════════════════════════════════════════════════════════════════════════
 * System / Time
 * ════════════════════════════════════════════════════════════════════════════ */

/** Milliseconds since boot. Used as LVGL tick source. */
uint32_t hal_tick_ms(void);

/** Blocking delay (use sparingly; prefer task delays). */
void hal_delay_ms(uint32_t ms);

/* ════════════════════════════════════════════════════════════════════════════
 * Display
 *   sim:   initialises SDL2 window + custom LVGL display driver
 *   stm32: initialises LTDC + DMA2D, registers LVGL display driver
 * ════════════════════════════════════════════════════════════════════════════ */

/** Initialise the display hardware and register the LVGL display driver.
 *  Must be called AFTER lv_init(). */
void hal_display_init(void);

/* ════════════════════════════════════════════════════════════════════════════
 * Audio
 * ════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    uint32_t sample_rate;   /**< 44100 or 96000 Hz */
    uint8_t  channels;      /**< Always 2 (stereo) */
    uint8_t  bits_per_sample; /**< 16 or 32 */
    uint32_t buffer_frames; /**< DMA / SDL buffer size in sample frames */
} hal_audio_cfg_t;

/** Open the audio output device. */
void hal_audio_init(const hal_audio_cfg_t *cfg);

/** Write interleaved PCM samples. Blocks if the output buffer is full.
 *  @param samples  Pointer to interleaved L/R sample data
 *  @param frames   Number of sample frames (not bytes) to write */
void hal_audio_write(const void *samples, uint32_t frames);

/** Returns available space in the output buffer (in frames). */
uint32_t hal_audio_frames_free(void);

/** Close audio device (called on shutdown). */
void hal_audio_deinit(void);

/* ════════════════════════════════════════════════════════════════════════════
 * USB / Storage
 *   sim:   watches test_assets/mock_usb/ — call hal_usb_simulate_insert/eject
 *          from keyboard handler to emulate hot-plug
 *   stm32: USB Host MSC driver with FatFs
 * ════════════════════════════════════════════════════════════════════════════ */

typedef enum {
    HAL_USB_EVENT_INSERTED = 0,
    HAL_USB_EVENT_REMOVED,
    HAL_USB_EVENT_ERROR,
} hal_usb_event_t;

/** Callback type invoked from the USB task on insert/remove. */
typedef void (*hal_usb_event_cb_t)(hal_usb_event_t event);

/** Initialise USB host (or file watcher in sim).
 *  @param cb  Called whenever a USB insert/remove event fires. */
void hal_usb_init(hal_usb_event_cb_t cb);

/** @return true if a USB drive is currently mounted and accessible. */
bool hal_usb_is_mounted(void);

/** @return Mount root path: "/USB" on hardware, "test_assets/mock_usb" on sim. */
const char *hal_usb_mount_path(void);

/* Simulator-only: call these from the keyboard mock to simulate hot-plug. */
#ifdef PLATFORM_SIM
void hal_usb_simulate_insert(void);
void hal_usb_simulate_eject(void);
#endif

/* ════════════════════════════════════════════════════════════════════════════
 * Controls — CDJ-2000 button matrix, Jog Wheel, Pitch Slider
 * ════════════════════════════════════════════════════════════════════════════ */

/** CDJ-2000–identical button IDs. Order must not change (hardware matrix). */
typedef enum {
    /* Transport */
    HAL_BTN_PLAY_PAUSE = 0,
    HAL_BTN_CUE,
    HAL_BTN_SYNC,
    HAL_BTN_MASTER,

    /* Hot Cues A–H */
    HAL_BTN_HOT_CUE_A,
    HAL_BTN_HOT_CUE_B,
    HAL_BTN_HOT_CUE_C,
    HAL_BTN_HOT_CUE_D,
    HAL_BTN_HOT_CUE_E,
    HAL_BTN_HOT_CUE_F,
    HAL_BTN_HOT_CUE_G,
    HAL_BTN_HOT_CUE_H,

    /* Loop */
    HAL_BTN_LOOP_IN,
    HAL_BTN_LOOP_OUT,
    HAL_BTN_RELOOP,
    HAL_BTN_LOOP_HALF,
    HAL_BTN_LOOP_DOUBLE,

    /* Performance */
    HAL_BTN_SLIP,
    HAL_BTN_QUANTIZE,

    /* Browse / Navigate */
    HAL_BTN_TRACK_PREV,
    HAL_BTN_TRACK_NEXT,
    HAL_BTN_BACK,
    HAL_BTN_MENU_ENTER,
    HAL_BTN_NEEDLE_SEARCH,

    /* Storage */
    HAL_BTN_USB_STOP,
    HAL_BTN_EJECT,

    HAL_BTN_COUNT
} hal_button_t;

/** Snapshot of all control inputs for one polling frame. */
typedef struct {
    /** Bitmask: bit N set → button N (hal_button_t) is currently pressed. */
    uint32_t buttons_held;

    /** Bitmask: bit N set → button N was pressed this frame (rising edge). */
    uint32_t buttons_pressed;

    /** Bitmask: bit N set → button N was released this frame (falling edge). */
    uint32_t buttons_released;

    /** Jog wheel rotation this frame, in 1/100 degree encoder ticks.
     *  Positive = clockwise, negative = counter-clockwise.
     *  Zero when jog is not turning. */
    int32_t jog_delta;

    /** 1 = finger on jog top plate (vinyl/scratch mode active), 0 = not touching. */
    uint8_t jog_touch;

    /** Pitch slider position, range −1.0 (−100%) to +1.0 (+100%).
     *  Typical DJ range: ±0.08 (±8%), ±0.16 (±16%). */
    float pitch_position;
} hal_controls_state_t;

/** Initialise control input hardware (GPIO matrix, encoder, ADC). */
void hal_controls_init(void);

/** Read the current control state into @p out.
 *  Call once per frame before dispatching to the app. */
void hal_controls_poll(hal_controls_state_t *out);

/* ════════════════════════════════════════════════════════════════════════════
 * Task management (thin wrapper so app/ never includes FreeRTOS headers)
 * ════════════════════════════════════════════════════════════════════════════ */

typedef void (*hal_task_fn_t)(void *arg);

/** Create an OS task (FreeRTOS on stm32; pthread on sim).
 *  @param stack_bytes  Stack size in bytes. */
void hal_task_create(hal_task_fn_t fn, const char *name,
                     uint32_t stack_bytes, void *arg, int priority);

/** Delay the calling task. Maps to vTaskDelay / usleep. */
void hal_task_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif
