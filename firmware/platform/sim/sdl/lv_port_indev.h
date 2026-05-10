#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register mouse and mouse-wheel as LVGL pointer/encoder input devices.
 * Call after lv_port_disp_init().
 */
void lv_port_indev_init(void);

/** Feed mouse-wheel delta from SDL event loop into the encoder indev. */
void lv_port_indev_sdl_wheel_add(int32_t delta);

#ifdef __cplusplus
}
#endif
