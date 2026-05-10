#include "mock_usb_msc.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* Relative to the binary location (set by CMake POST_BUILD copy) */
#define MOCK_USB_PATH "test_assets/mock_usb"

static hal_usb_event_cb_t s_event_cb  = NULL;
static bool               s_mounted   = false;

void mock_usb_init(hal_usb_event_cb_t event_cb) {
    s_event_cb = event_cb;
    s_mounted  = false;
    printf("[mock_usb] Initialised. Mount path: %s\n", MOCK_USB_PATH);
    printf("[mock_usb] Press 'U' to simulate USB insert/eject.\n");
}

void mock_usb_insert(void) {
    if (s_mounted) return;
    s_mounted = true;
    printf("[mock_usb] USB inserted → %s\n", MOCK_USB_PATH);
    if (s_event_cb) s_event_cb(HAL_USB_EVENT_INSERTED);
}

void mock_usb_eject(void) {
    if (!s_mounted) return;
    s_mounted = false;
    printf("[mock_usb] USB ejected.\n");
    if (s_event_cb) s_event_cb(HAL_USB_EVENT_REMOVED);
}

void mock_usb_toggle(void) {
    if (s_mounted) mock_usb_eject();
    else           mock_usb_insert();
}

bool mock_usb_is_mounted(void) {
    return s_mounted;
}

const char *mock_usb_mount_path(void) {
    return MOCK_USB_PATH;
}
