#pragma once

#include "lvgl.h"
#include "event_bus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Build and load the main DJ screen.
 *
 * Phase 0: Dark background + logo label + USB status indicator.
 * Phase 1: Left browser panel is populated with mock USB contents.
 * Phase 2: Centre waveform Canvas + right Hot Cue panel.
 */
void screen_main_load(void);

/** Update the USB status indicator (called from EVT_USB_INSERTED/REMOVED). */
void screen_main_set_usb_status(bool connected);

#ifdef __cplusplus
}
#endif
