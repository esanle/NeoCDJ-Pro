/**
 * test_event_bus.c — Unit tests for the event bus
 *
 * Build: part of the "tests" cmake preset
 * Run:   ./build/tests/test_event_bus
 */
#include "event_bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Stub hal.h dependencies so tests don't need the full platform */
uint32_t hal_tick_ms(void)          { return 0; }
void     hal_delay_ms(uint32_t ms)  { (void)ms; }

/* ── Test infrastructure ─────────────────────────────────────────────────── */
static int s_pass = 0;
static int s_fail = 0;

#define TEST(name) do { \
    printf("  %-40s ", name); \
    fflush(stdout); \
} while(0)

#define PASS() do { printf("PASS\n"); s_pass++; } while(0)
#define FAIL(msg) do { printf("FAIL — %s\n", msg); s_fail++; } while(0)
#define EXPECT(cond) do { if (cond) PASS(); else FAIL(#cond); } while(0)

/* ── Test cases ──────────────────────────────────────────────────────────── */

static int s_call_count = 0;
static neo_event_t s_last_event;

static void test_handler(const neo_event_t *evt, void *user_data) {
    s_call_count++;
    s_last_event = *evt;
    (void)user_data;
}

/* Handlers for user_data and USB tests */
static void *s_received_ud = NULL;
static void handler_with_ud(const neo_event_t *evt, void *ud) {
    s_received_ud = ud;
    (void)evt;
}

static neo_event_type_t s_last_usb_type = EVT_COUNT;
static void usb_handler(const neo_event_t *evt, void *ud) {
    s_last_usb_type = evt->type;
    (void)ud;
}

static void test_subscribe_and_fire(void) {
    TEST("subscribe + post fires handler");
    event_bus_init();
    s_call_count = 0;

    int h = event_bus_subscribe(EVT_PLAY, test_handler, NULL);
    neo_event_t e = { .type = EVT_PLAY };
    event_bus_post(&e);

    EXPECT(s_call_count == 1 && s_last_event.type == EVT_PLAY);
    event_bus_unsubscribe(h);
}

static void test_unsubscribe(void) {
    TEST("unsubscribe stops delivery");
    event_bus_init();
    s_call_count = 0;

    int h = event_bus_subscribe(EVT_PLAY, test_handler, NULL);
    event_bus_unsubscribe(h);

    neo_event_t e = { .type = EVT_PLAY };
    event_bus_post(&e);
    EXPECT(s_call_count == 0);
}

static void test_wrong_type_not_fired(void) {
    TEST("handler not called for different event type");
    event_bus_init();
    s_call_count = 0;

    int h = event_bus_subscribe(EVT_PLAY, test_handler, NULL);
    neo_event_t e = { .type = EVT_PAUSE };
    event_bus_post(&e);
    EXPECT(s_call_count == 0);
    event_bus_unsubscribe(h);
}

static void test_multiple_subscribers(void) {
    TEST("multiple subscribers all receive the event");
    event_bus_init();
    s_call_count = 0;

    int h1 = event_bus_subscribe(EVT_USB_INSERTED, test_handler, NULL);
    int h2 = event_bus_subscribe(EVT_USB_INSERTED, test_handler, NULL);
    int h3 = event_bus_subscribe(EVT_USB_INSERTED, test_handler, NULL);
    neo_event_t e = { .type = EVT_USB_INSERTED };
    event_bus_post(&e);
    EXPECT(s_call_count == 3);
    event_bus_unsubscribe(h1);
    event_bus_unsubscribe(h2);
    event_bus_unsubscribe(h3);
}

static void test_user_data_passthrough(void) {
    TEST("user_data is passed to handler");
    event_bus_init();
    s_received_ud = NULL;

    int sentinel = 42;
    int h = event_bus_subscribe(EVT_JOG_TURN, handler_with_ud, &sentinel);
    neo_event_t e = { .type = EVT_JOG_TURN };
    event_bus_post(&e);
    EXPECT(s_received_ud == &sentinel);
    event_bus_unsubscribe(h);
}

static void test_usb_helper(void) {
    TEST("event_bus_post_usb maps correctly");
    event_bus_init();
    s_last_usb_type = EVT_COUNT;

    event_bus_subscribe(EVT_USB_INSERTED, usb_handler, NULL);
    event_bus_subscribe(EVT_USB_REMOVED,  usb_handler, NULL);

    event_bus_post_usb(HAL_USB_EVENT_INSERTED);
    EXPECT(s_last_usb_type == EVT_USB_INSERTED);
    event_bus_post_usb(HAL_USB_EVENT_REMOVED);
    EXPECT(s_last_usb_type == EVT_USB_REMOVED);
}

static void test_jog_delta_data(void) {
    TEST("jog_delta data survives post");
    event_bus_init();
    s_last_event.data.jog_delta = 0;

    int h = event_bus_subscribe(EVT_JOG_TURN, test_handler, NULL);
    neo_event_t e = { .type = EVT_JOG_TURN, .data.jog_delta = -1234 };
    event_bus_post(&e);
    EXPECT(s_last_event.data.jog_delta == -1234);
    event_bus_unsubscribe(h);
}

/* ── Main ────────────────────────────────────────────────────────────────── */
int main(void) {
    printf("=== event_bus unit tests ===\n");
    test_subscribe_and_fire();
    test_unsubscribe();
    test_wrong_type_not_fired();
    test_multiple_subscribers();
    test_user_data_passthrough();
    test_usb_helper();
    test_jog_delta_data();

    printf("\n%d passed, %d failed\n", s_pass, s_fail);
    return s_fail ? 1 : 0;
}
