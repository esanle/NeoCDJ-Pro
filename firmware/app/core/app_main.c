#include "app_main.h"
#include "event_bus.h"
#include "screen_main.h"
#include "hal.h"
#include <stdio.h>

/* Pitch change threshold to suppress noise */
#define PITCH_CHANGE_DEADBAND 0.001f

static hal_controls_state_t s_prev_controls;
static hal_controls_state_t s_curr_controls;

/* ── USB event callback (fires from USB task / mock watcher) ──────────────── */
static void on_usb_event(hal_usb_event_t event) {
    event_bus_post_usb(event);
    /* Screen reacts via its own subscription */
}

/* ── Translate raw controls → event bus posts ────────────────────────────── */
static void dispatch_controls(const hal_controls_state_t *c) {
    /* Rising-edge buttons */
    if (c->buttons_pressed) {
        /* Transport */
        if (c->buttons_pressed & (1u << HAL_BTN_PLAY_PAUSE)) {
            neo_event_t e = { .type = EVT_PLAY };
            event_bus_post(&e);
        }
        if (c->buttons_pressed & (1u << HAL_BTN_CUE)) {
            neo_event_t e = { .type = EVT_CUE_PRESS };
            event_bus_post(&e);
        }
        /* Navigation */
        if (c->buttons_pressed & (1u << HAL_BTN_TRACK_PREV)) {
            neo_event_t e = { .type = EVT_TRACK_PREV };
            event_bus_post(&e);
        }
        if (c->buttons_pressed & (1u << HAL_BTN_TRACK_NEXT)) {
            neo_event_t e = { .type = EVT_TRACK_NEXT };
            event_bus_post(&e);
        }
        /* Loop */
        if (c->buttons_pressed & (1u << HAL_BTN_LOOP_IN))     { neo_event_t e = {.type=EVT_LOOP_IN};     event_bus_post(&e); }
        if (c->buttons_pressed & (1u << HAL_BTN_LOOP_OUT))    { neo_event_t e = {.type=EVT_LOOP_OUT};    event_bus_post(&e); }
        if (c->buttons_pressed & (1u << HAL_BTN_RELOOP))      { neo_event_t e = {.type=EVT_RELOOP};      event_bus_post(&e); }
        if (c->buttons_pressed & (1u << HAL_BTN_LOOP_HALF))   { neo_event_t e = {.type=EVT_LOOP_HALF};   event_bus_post(&e); }
        if (c->buttons_pressed & (1u << HAL_BTN_LOOP_DOUBLE)) { neo_event_t e = {.type=EVT_LOOP_DOUBLE}; event_bus_post(&e); }
        /* Performance */
        if (c->buttons_pressed & (1u << HAL_BTN_SLIP))     { neo_event_t e = {.type=EVT_SLIP_TOGGLE};     event_bus_post(&e); }
        if (c->buttons_pressed & (1u << HAL_BTN_QUANTIZE)) { neo_event_t e = {.type=EVT_QUANTIZE_TOGGLE}; event_bus_post(&e); }
        if (c->buttons_pressed & (1u << HAL_BTN_SYNC))     { neo_event_t e = {.type=EVT_SYNC_TOGGLE};     event_bus_post(&e); }

        /* Hot Cues A–H */
        for (int hc = 0; hc < 8; hc++) {
            if (c->buttons_pressed & (1u << (HAL_BTN_HOT_CUE_A + hc))) {
                neo_event_t e = { .type = EVT_HOT_CUE_PRESS, .data.button_id = HAL_BTN_HOT_CUE_A + hc };
                event_bus_post(&e);
            }
        }
    }

    /* Falling-edge buttons */
    if (c->buttons_released & (1u << HAL_BTN_CUE)) {
        neo_event_t e = { .type = EVT_CUE_RELEASE };
        event_bus_post(&e);
    }

    /* Jog rotation */
    if (c->jog_delta != 0) {
        neo_event_t e = { .type = EVT_JOG_TURN, .data.jog_delta = c->jog_delta };
        event_bus_post(&e);
    }

    /* Jog touch edge */
    if (c->jog_touch != s_prev_controls.jog_touch) {
        neo_event_t e = { .type = EVT_JOG_TOUCH, .data.jog_touch = c->jog_touch };
        event_bus_post(&e);
    }

    /* Pitch slider */
    float dp = c->pitch_position - s_prev_controls.pitch_position;
    if (dp > PITCH_CHANGE_DEADBAND || dp < -PITCH_CHANGE_DEADBAND) {
        neo_event_t e = { .type = EVT_PITCH_CHANGE, .data.pitch_value = c->pitch_position };
        event_bus_post(&e);
    }
}

/* ── Public API ──────────────────────────────────────────────────────────── */

void app_main_init(void) {
    printf("[app] Initialising NeoCDJ-Pro v0.1.0\n");

    event_bus_init();

    hal_usb_init(on_usb_event);
    hal_controls_init();

    /* Audio init deferred to Phase 3 */

    /* Load the main UI screen */
    screen_main_load();

    printf("[app] Init complete. Entering main loop.\n");
}

void app_main_tick(void) {
    hal_controls_poll(&s_curr_controls);
    dispatch_controls(&s_curr_controls);
    s_prev_controls = s_curr_controls;
}
