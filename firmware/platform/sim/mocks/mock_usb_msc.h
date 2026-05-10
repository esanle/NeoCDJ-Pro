#pragma once

#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * mock_usb_msc — simulates USB flash drive hot-plug.
 *
 * "Insertion" = the test_assets/mock_usb/ directory is accessible.
 * "Ejection"  = the directory is treated as unmounted.
 *
 * Toggle with the 'U' key in the simulator (handled by mock_controls.c).
 */

void mock_usb_init(hal_usb_event_cb_t event_cb);

/** Simulate inserting the USB drive (fires HAL_USB_EVENT_INSERTED). */
void mock_usb_insert(void);

/** Simulate ejecting the USB drive (fires HAL_USB_EVENT_REMOVED). */
void mock_usb_eject(void);

/** Toggle insert/eject state. */
void mock_usb_toggle(void);

bool        mock_usb_is_mounted(void);
const char *mock_usb_mount_path(void);

#ifdef __cplusplus
}
#endif
