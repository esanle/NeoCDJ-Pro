#pragma once

#include <stdint.h>
#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ── Event types ──────────────────────────────────────────────────────────── */
typedef enum {
    /* USB / Storage */
    EVT_USB_INSERTED = 0,
    EVT_USB_REMOVED,
    EVT_USB_ERROR,

    /* Transport */
    EVT_PLAY,
    EVT_PAUSE,
    EVT_STOP,
    EVT_CUE_PRESS,
    EVT_CUE_RELEASE,

    /* Navigation */
    EVT_TRACK_PREV,
    EVT_TRACK_NEXT,
    EVT_JOG_TURN,       /**< data.jog_delta != 0 */
    EVT_JOG_TOUCH,      /**< data.jog_touch: 1=press, 0=release */
    EVT_PITCH_CHANGE,   /**< data.pitch_value */

    /* Hot Cue */
    EVT_HOT_CUE_PRESS,  /**< data.button_id: HAL_BTN_HOT_CUE_A..H */
    EVT_HOT_CUE_RELEASE,

    /* Loop */
    EVT_LOOP_IN,
    EVT_LOOP_OUT,
    EVT_RELOOP,
    EVT_LOOP_HALF,
    EVT_LOOP_DOUBLE,

    /* Misc */
    EVT_SLIP_TOGGLE,
    EVT_QUANTIZE_TOGGLE,
    EVT_SYNC_TOGGLE,

    EVT_COUNT
} neo_event_type_t;

typedef struct {
    neo_event_type_t type;
    union {
        int32_t  jog_delta;
        uint8_t  jog_touch;
        float    pitch_value;
        uint32_t button_id;
    } data;
} neo_event_t;

/* ── Subscriber callback ─────────────────────────────────────────────────── */
typedef void (*event_handler_t)(const neo_event_t *evt, void *user_data);

/* ── API ──────────────────────────────────────────────────────────────────── */

/** Initialise the event bus. Call once before any tasks start. */
void event_bus_init(void);

/** Subscribe to a specific event type.
 *  @param type      Event to listen for
 *  @param handler   Callback invoked synchronously from event_bus_dispatch()
 *  @param user_data Passed through to the handler
 *  @return subscription handle (pass to event_bus_unsubscribe) */
int event_bus_subscribe(neo_event_type_t type,
                        event_handler_t handler,
                        void *user_data);

/** Unsubscribe using the handle returned by event_bus_subscribe. */
void event_bus_unsubscribe(int handle);

/** Post an event. Safe to call from any task.
 *  Dispatches synchronously to all subscribers on the calling thread. */
void event_bus_post(const neo_event_t *evt);

/** Convenience: post a USB event (called from HAL USB callback). */
void event_bus_post_usb(hal_usb_event_t usb_event);

#ifdef __cplusplus
}
#endif
