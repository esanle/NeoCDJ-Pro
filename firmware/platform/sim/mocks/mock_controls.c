#include "mock_controls.h"
#include "mock_usb_msc.h"
#include "lv_port_indev.h"   /* for lv_port_indev_sdl_wheel_add() */
#include <SDL2/SDL.h>
#include <string.h>
#include <stdbool.h>

/* Jog sensitivity: pixels of mouse drag per encoder tick */
#define JOG_DRAG_SENSITIVITY  3
/* Keyboard jog step per key repeat */
#define JOG_KEY_STEP          50

/* Pitch step per keypress (±0.01 = 1%) */
#define PITCH_KEY_STEP        0.01f
#define PITCH_MIN            -1.0f
#define PITCH_MAX             1.0f

static const uint8_t *s_keys_now  = NULL;
static uint8_t        s_keys_prev[SDL_NUM_SCANCODES];
static float          s_pitch     = 0.0f;
static int            s_mouse_drag_x_last = 0;
static bool           s_mouse_dragging    = false;

/* USB toggle debounce */
static bool s_usb_key_was_pressed = false;

void mock_controls_init(void) {
    s_keys_now = SDL_GetKeyboardState(NULL);
    memset(s_keys_prev, 0, sizeof(s_keys_prev));
    s_pitch = 0.0f;
    printf("[mock_controls] Keyboard mapping active. See docs/simulation-guide.md\n");
}

/* Helper: true on rising edge (key just pressed this frame) */
static inline bool key_pressed(SDL_Scancode sc) {
    return s_keys_now[sc] && !s_keys_prev[sc];
}
static inline bool key_held(SDL_Scancode sc) {
    return s_keys_now[sc];
}

void mock_controls_poll(hal_controls_state_t *out) {
    memset(out, 0, sizeof(*out));

    /* Snapshot keyboard state (SDL_PollEvent must have been called first) */
    /* s_keys_now points to SDL's internal array, always current */

    /* ── Buttons: held state ─────────────────────────────────────────────── */
    if (key_held(SDL_SCANCODE_SPACE))       out->buttons_held |= (1u << HAL_BTN_PLAY_PAUSE);
    if (key_held(SDL_SCANCODE_C))           out->buttons_held |= (1u << HAL_BTN_CUE);
    if (key_held(SDL_SCANCODE_1))           out->buttons_held |= (1u << HAL_BTN_HOT_CUE_A);
    if (key_held(SDL_SCANCODE_2))           out->buttons_held |= (1u << HAL_BTN_HOT_CUE_B);
    if (key_held(SDL_SCANCODE_3))           out->buttons_held |= (1u << HAL_BTN_HOT_CUE_C);
    if (key_held(SDL_SCANCODE_4))           out->buttons_held |= (1u << HAL_BTN_HOT_CUE_D);
    if (key_held(SDL_SCANCODE_5))           out->buttons_held |= (1u << HAL_BTN_HOT_CUE_E);
    if (key_held(SDL_SCANCODE_6))           out->buttons_held |= (1u << HAL_BTN_HOT_CUE_F);
    if (key_held(SDL_SCANCODE_7))           out->buttons_held |= (1u << HAL_BTN_HOT_CUE_G);
    if (key_held(SDL_SCANCODE_8))           out->buttons_held |= (1u << HAL_BTN_HOT_CUE_H);
    if (key_held(SDL_SCANCODE_I))           out->buttons_held |= (1u << HAL_BTN_LOOP_IN);
    if (key_held(SDL_SCANCODE_O))           out->buttons_held |= (1u << HAL_BTN_LOOP_OUT);
    if (key_held(SDL_SCANCODE_R))           out->buttons_held |= (1u << HAL_BTN_RELOOP);
    if (key_held(SDL_SCANCODE_LEFTBRACKET)) out->buttons_held |= (1u << HAL_BTN_LOOP_HALF);
    if (key_held(SDL_SCANCODE_RIGHTBRACKET))out->buttons_held |= (1u << HAL_BTN_LOOP_DOUBLE);
    if (key_held(SDL_SCANCODE_S))           out->buttons_held |= (1u << HAL_BTN_SLIP);
    if (key_held(SDL_SCANCODE_Q))           out->buttons_held |= (1u << HAL_BTN_QUANTIZE);
    if (key_held(SDL_SCANCODE_T))           out->buttons_held |= (1u << HAL_BTN_SYNC);
    if (key_held(SDL_SCANCODE_LEFT))        out->buttons_held |= (1u << HAL_BTN_TRACK_PREV);
    if (key_held(SDL_SCANCODE_RIGHT))       out->buttons_held |= (1u << HAL_BTN_TRACK_NEXT);
    if (key_held(SDL_SCANCODE_ESCAPE))      out->buttons_held |= (1u << HAL_BTN_BACK);
    if (key_held(SDL_SCANCODE_RETURN))      out->buttons_held |= (1u << HAL_BTN_MENU_ENTER);

    /* ── Rising / falling edges ──────────────────────────────────────────── */
    out->buttons_pressed  = out->buttons_held & ~(
        (s_keys_prev[SDL_SCANCODE_SPACE]        ? (1u<<HAL_BTN_PLAY_PAUSE)   : 0) |
        (s_keys_prev[SDL_SCANCODE_C]            ? (1u<<HAL_BTN_CUE)          : 0) |
        (s_keys_prev[SDL_SCANCODE_1]            ? (1u<<HAL_BTN_HOT_CUE_A)   : 0) |
        (s_keys_prev[SDL_SCANCODE_2]            ? (1u<<HAL_BTN_HOT_CUE_B)   : 0) |
        (s_keys_prev[SDL_SCANCODE_3]            ? (1u<<HAL_BTN_HOT_CUE_C)   : 0) |
        (s_keys_prev[SDL_SCANCODE_4]            ? (1u<<HAL_BTN_HOT_CUE_D)   : 0) |
        (s_keys_prev[SDL_SCANCODE_5]            ? (1u<<HAL_BTN_HOT_CUE_E)   : 0) |
        (s_keys_prev[SDL_SCANCODE_6]            ? (1u<<HAL_BTN_HOT_CUE_F)   : 0) |
        (s_keys_prev[SDL_SCANCODE_7]            ? (1u<<HAL_BTN_HOT_CUE_G)   : 0) |
        (s_keys_prev[SDL_SCANCODE_8]            ? (1u<<HAL_BTN_HOT_CUE_H)   : 0) |
        (s_keys_prev[SDL_SCANCODE_I]            ? (1u<<HAL_BTN_LOOP_IN)     : 0) |
        (s_keys_prev[SDL_SCANCODE_O]            ? (1u<<HAL_BTN_LOOP_OUT)    : 0) |
        (s_keys_prev[SDL_SCANCODE_R]            ? (1u<<HAL_BTN_RELOOP)      : 0) |
        (s_keys_prev[SDL_SCANCODE_LEFTBRACKET]  ? (1u<<HAL_BTN_LOOP_HALF)   : 0) |
        (s_keys_prev[SDL_SCANCODE_RIGHTBRACKET] ? (1u<<HAL_BTN_LOOP_DOUBLE) : 0) |
        (s_keys_prev[SDL_SCANCODE_S]            ? (1u<<HAL_BTN_SLIP)        : 0) |
        (s_keys_prev[SDL_SCANCODE_Q]            ? (1u<<HAL_BTN_QUANTIZE)    : 0) |
        (s_keys_prev[SDL_SCANCODE_T]            ? (1u<<HAL_BTN_SYNC)        : 0) |
        (s_keys_prev[SDL_SCANCODE_LEFT]         ? (1u<<HAL_BTN_TRACK_PREV)  : 0) |
        (s_keys_prev[SDL_SCANCODE_RIGHT]        ? (1u<<HAL_BTN_TRACK_NEXT)  : 0) |
        (s_keys_prev[SDL_SCANCODE_ESCAPE]       ? (1u<<HAL_BTN_BACK)        : 0) |
        (s_keys_prev[SDL_SCANCODE_RETURN]       ? (1u<<HAL_BTN_MENU_ENTER)  : 0)
    );

    /* released = was held last frame, not held now */
    uint32_t prev_held =
        (s_keys_prev[SDL_SCANCODE_SPACE]  ? (1u<<HAL_BTN_PLAY_PAUSE) : 0) |
        (s_keys_prev[SDL_SCANCODE_C]      ? (1u<<HAL_BTN_CUE)        : 0);
    /* (add more if needed for release-sensitive buttons) */
    out->buttons_released = prev_held & ~out->buttons_held;

    /* ── USB toggle (single-shot on U press) ────────────────────────────── */
    bool usb_now = key_held(SDL_SCANCODE_U);
    if (usb_now && !s_usb_key_was_pressed) {
        mock_usb_toggle();
    }
    s_usb_key_was_pressed = usb_now;

    /* ── Pitch slider (+ / - keys) ──────────────────────────────────────── */
    if (key_pressed(SDL_SCANCODE_EQUALS) || key_pressed(SDL_SCANCODE_KP_PLUS)) {
        s_pitch += PITCH_KEY_STEP;
        if (s_pitch > PITCH_MAX) s_pitch = PITCH_MAX;
    }
    if (key_pressed(SDL_SCANCODE_MINUS) || key_pressed(SDL_SCANCODE_KP_MINUS)) {
        s_pitch -= PITCH_KEY_STEP;
        if (s_pitch < PITCH_MIN) s_pitch = PITCH_MIN;
    }
    out->pitch_position = s_pitch;

    /* ── Jog — keyboard (UP/DOWN without Shift = no touch) ─────────────── */
    bool shift = key_held(SDL_SCANCODE_LSHIFT) || key_held(SDL_SCANCODE_RSHIFT);
    if (key_held(SDL_SCANCODE_UP)) {
        out->jog_delta = -JOG_KEY_STEP;
        out->jog_touch = shift ? 1 : 0;
    } else if (key_held(SDL_SCANCODE_DOWN)) {
        out->jog_delta = +JOG_KEY_STEP;
        out->jog_touch = shift ? 1 : 0;
    }

    /* ── Jog — mouse drag (left button = touch + scratch) ──────────────── */
    int mx, my;
    uint32_t mbtns = SDL_GetMouseState(&mx, &my);
    if (mbtns & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        if (!s_mouse_dragging) {
            s_mouse_drag_x_last = mx;
            s_mouse_dragging    = true;
        }
        int dx = mx - s_mouse_drag_x_last;
        if (dx != 0) {
            out->jog_delta     = dx * JOG_DRAG_SENSITIVITY;
            out->jog_touch     = 1;
            s_mouse_drag_x_last = mx;
        }
        out->jog_touch = 1;
    } else {
        s_mouse_dragging = false;
    }

    /* ── Update previous key state ──────────────────────────────────────── */
    SDL_memcpy(s_keys_prev, s_keys_now, SDL_NUM_SCANCODES);
}
