#include "lv_port_indev.h"
#include <SDL2/SDL.h>

/* ── Mouse (pointer) ─────────────────────────────────────────────────────── */
static void mouse_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    LV_UNUSED(indev);
    int x, y;
    uint32_t btns = SDL_GetMouseState(&x, &y);
    data->point.x = (lv_coord_t)x;
    data->point.y = (lv_coord_t)y;
    data->state   = (btns & SDL_BUTTON(SDL_BUTTON_LEFT))
                    ? LV_INDEV_STATE_PRESSED
                    : LV_INDEV_STATE_RELEASED;
}

/* ── Mouse wheel (encoder — used to scroll file browser) ────────────────── */
static int32_t s_wheel_diff = 0;   /* accumulated between reads */

void lv_port_indev_sdl_wheel_add(int32_t delta) {
    s_wheel_diff += delta;
}

static void wheel_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    LV_UNUSED(indev);
    data->enc_diff = (int16_t)s_wheel_diff;
    s_wheel_diff   = 0;

    uint32_t btns = SDL_GetMouseState(NULL, NULL);
    data->state   = (btns & SDL_BUTTON(SDL_BUTTON_MIDDLE))
                    ? LV_INDEV_STATE_PRESSED
                    : LV_INDEV_STATE_RELEASED;
}

/* ── Public API ──────────────────────────────────────────────────────────── */
void lv_port_indev_init(void) {
    /* Pointer — drives the LVGL cursor */
    lv_indev_t *mouse = lv_indev_create();
    lv_indev_set_type(mouse, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(mouse, mouse_read_cb);

    /* Encoder — scrolls lists, adjusts values */
    lv_indev_t *wheel = lv_indev_create();
    lv_indev_set_type(wheel, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(wheel, wheel_read_cb);
}
