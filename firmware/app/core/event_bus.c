#include "event_bus.h"
#include <string.h>
#include <stdio.h>

#define MAX_SUBSCRIBERS 32

typedef struct {
    bool             active;
    neo_event_type_t type;
    event_handler_t  handler;
    void            *user_data;
} subscriber_t;

static subscriber_t s_subs[MAX_SUBSCRIBERS];

void event_bus_init(void) {
    memset(s_subs, 0, sizeof(s_subs));
}

int event_bus_subscribe(neo_event_type_t type, event_handler_t handler, void *user_data) {
    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        if (!s_subs[i].active) {
            s_subs[i].active    = true;
            s_subs[i].type      = type;
            s_subs[i].handler   = handler;
            s_subs[i].user_data = user_data;
            return i;
        }
    }
    fprintf(stderr, "[event_bus] No free subscriber slots!\n");
    return -1;
}

void event_bus_unsubscribe(int handle) {
    if (handle >= 0 && handle < MAX_SUBSCRIBERS) {
        s_subs[handle].active = false;
    }
}

void event_bus_post(const neo_event_t *evt) {
    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        if (s_subs[i].active && s_subs[i].type == evt->type) {
            s_subs[i].handler(evt, s_subs[i].user_data);
        }
    }
}

void event_bus_post_usb(hal_usb_event_t usb_event) {
    neo_event_t evt = {0};
    switch (usb_event) {
        case HAL_USB_EVENT_INSERTED: evt.type = EVT_USB_INSERTED; break;
        case HAL_USB_EVENT_REMOVED:  evt.type = EVT_USB_REMOVED;  break;
        case HAL_USB_EVENT_ERROR:    evt.type = EVT_USB_ERROR;    break;
        default: return;
    }
    event_bus_post(&evt);
}
