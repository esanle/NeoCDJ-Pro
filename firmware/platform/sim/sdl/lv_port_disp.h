#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DISP_WIDTH  800
#define DISP_HEIGHT 480

/**
 * Create an SDL2 window and register it as the LVGL display.
 * Must be called after lv_init() and from the same thread that called SDL_Init().
 * @return the created lv_display_t (also becomes the default display)
 */
lv_display_t *lv_port_disp_init(void);

/** Call once per frame to present the rendered frame to the SDL window. */
void lv_port_disp_present(void);

#ifdef __cplusplus
}
#endif
