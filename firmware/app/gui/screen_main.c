#include "screen_main.h"
#include "theme_apple.h"
#include "hal.h"
#include <stdio.h>

/* ── Static widget handles ───────────────────────────────────────────────── */
static lv_obj_t *s_usb_label    = NULL;
static lv_obj_t *s_status_bar   = NULL;

/* ── USB event subscription ──────────────────────────────────────────────── */
static void on_usb_event(const neo_event_t *evt, void *ud) {
    LV_UNUSED(ud);
    screen_main_set_usb_status(evt->type == EVT_USB_INSERTED);
}

/* ── Status bar (top strip) ──────────────────────────────────────────────── */
static lv_obj_t *create_status_bar(lv_obj_t *parent) {
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_set_size(bar, LV_PCT(100), 28);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(bar, APPLE_BG_CARD, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_radius(bar, 0, 0);
    lv_obj_set_style_pad_hor(bar, 12, 0);
    lv_obj_set_style_pad_ver(bar, 4, 0);

    lv_obj_t *title = lv_label_create(bar);
    lv_label_set_text(title, "NeoCDJ-Pro");
    lv_obj_set_style_text_color(title, APPLE_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(title, APPLE_FONT_BODY, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 0, 0);

    /* USB indicator (right side of status bar) */
    s_usb_label = lv_label_create(bar);
    lv_label_set_text(s_usb_label, "No USB");
    lv_obj_set_style_text_color(s_usb_label, APPLE_TEXT_TERTIARY, 0);
    lv_obj_set_style_text_font(s_usb_label, APPLE_FONT_CAPTION, 0);
    lv_obj_align(s_usb_label, LV_ALIGN_RIGHT_MID, 0, 0);

    return bar;
}

/* ── Centre splash (Phase 0 placeholder) ────────────────────────────────── */
static void create_splash(lv_obj_t *parent) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, 320, 180);
    lv_obj_center(card);
    theme_apple_apply_card(card);
    lv_obj_set_style_bg_color(card, APPLE_BG_CARD, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    /* Logo text */
    lv_obj_t *logo = lv_label_create(card);
    lv_label_set_text(logo, "NeoCDJ-Pro");
    lv_obj_set_style_text_font(logo, APPLE_FONT_TITLE, 0);
    lv_obj_set_style_text_color(logo, APPLE_TEXT_PRIMARY, 0);
    lv_obj_align(logo, LV_ALIGN_TOP_MID, 0, 12);

    /* Subtitle */
    lv_obj_t *sub = lv_label_create(card);
    lv_label_set_text(sub, "Phase 0 - Simulator Ready");
    lv_obj_set_style_text_font(sub, APPLE_FONT_CAPTION, 0);
    lv_obj_set_style_text_color(sub, APPLE_TEXT_SECONDARY, 0);
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, 0);

    /* Hint */
    lv_obj_t *hint = lv_label_create(card);
    lv_label_set_text(hint, "Press U to simulate USB insert/eject");
    lv_obj_set_style_text_font(hint, APPLE_FONT_CAPTION, 0);
    lv_obj_set_style_text_color(hint, APPLE_TEXT_TERTIARY, 0);
    lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(hint, 270);
    lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -12);
}

/* ── Three-panel layout skeleton (populated in later phases) ─────────────── */
static void create_panel_skeleton(lv_obj_t *parent) {
    /* Left panel — browser (Phase 1) */
    lv_obj_t *left = lv_obj_create(parent);
    lv_obj_set_size(left, 220, LV_PCT(100));
    lv_obj_align(left, LV_ALIGN_LEFT_MID, 0, 0);
    theme_apple_apply_card(left);
    lv_obj_set_style_radius(left, 0, 0);
    lv_obj_set_style_bg_color(left, APPLE_BG_CARD, 0);
    lv_obj_clear_flag(left, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *left_lbl = lv_label_create(left);
    lv_label_set_text(left_lbl, "Browser\n(Phase 1)");
    lv_obj_set_style_text_color(left_lbl, APPLE_TEXT_TERTIARY, 0);
    lv_obj_set_style_text_font(left_lbl, APPLE_FONT_CAPTION, 0);
    lv_obj_set_style_text_align(left_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(left_lbl);

    /* Centre panel — waveform (Phase 2) */
    lv_obj_t *centre = lv_obj_create(parent);
    lv_obj_set_size(centre, LV_PCT(100) - 220 - 200, LV_PCT(100));
    lv_obj_align(centre, LV_ALIGN_CENTER, 10, 0);
    lv_obj_set_style_bg_color(centre, APPLE_BG_DARK, 0);
    lv_obj_set_style_bg_opa(centre, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(centre, 0, 0);
    lv_obj_set_style_radius(centre, 0, 0);
    lv_obj_clear_flag(centre, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *centre_lbl = lv_label_create(centre);
    lv_label_set_text(centre_lbl, "Waveform\n(Phase 2)");
    lv_obj_set_style_text_color(centre_lbl, APPLE_TEXT_TERTIARY, 0);
    lv_obj_set_style_text_font(centre_lbl, APPLE_FONT_CAPTION, 0);
    lv_obj_set_style_text_align(centre_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(centre_lbl);

    /* Right panel — hot cues / info (Phase 2) */
    lv_obj_t *right = lv_obj_create(parent);
    lv_obj_set_size(right, 200, LV_PCT(100));
    lv_obj_align(right, LV_ALIGN_RIGHT_MID, 0, 0);
    theme_apple_apply_card(right);
    lv_obj_set_style_radius(right, 0, 0);
    lv_obj_set_style_bg_color(right, APPLE_BG_CARD, 0);
    lv_obj_clear_flag(right, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *right_lbl = lv_label_create(right);
    lv_label_set_text(right_lbl, "Hot Cues\n(Phase 2)");
    lv_obj_set_style_text_color(right_lbl, APPLE_TEXT_TERTIARY, 0);
    lv_obj_set_style_text_font(right_lbl, APPLE_FONT_CAPTION, 0);
    lv_obj_set_style_text_align(right_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(right_lbl);
}

/* ── Public API ──────────────────────────────────────────────────────────── */

void screen_main_load(void) {
    lv_obj_t *scr = lv_screen_active();

    /* Root background */
    lv_obj_set_style_bg_color(scr, APPLE_BG_DARK, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scr, 0, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    /* Status bar */
    s_status_bar = create_status_bar(scr);

    /* Content area below status bar */
    lv_obj_t *content = lv_obj_create(scr);
    lv_obj_set_pos(content, 0, 28);
    lv_obj_set_size(content, LV_PCT(100), LV_PCT(100) - 28);
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_set_style_pad_all(content, 0, 0);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);

    /* Three-panel skeleton */
    create_panel_skeleton(content);

    /* Phase-0 splash overlay */
    create_splash(scr);

    /* Subscribe to USB events */
    event_bus_subscribe(EVT_USB_INSERTED, on_usb_event, NULL);
    event_bus_subscribe(EVT_USB_REMOVED,  on_usb_event, NULL);

    printf("[screen_main] UI loaded. Display: %dx%d\n",
           lv_display_get_horizontal_resolution(lv_display_get_default()),
           lv_display_get_vertical_resolution(lv_display_get_default()));
}

void screen_main_set_usb_status(bool connected) {
    if (!s_usb_label) return;
    if (connected) {
        lv_label_set_text(s_usb_label, "USB Connected");
        lv_obj_set_style_text_color(s_usb_label, APPLE_ACCENT_GREEN, 0);
    } else {
        lv_label_set_text(s_usb_label, "No USB");
        lv_obj_set_style_text_color(s_usb_label, APPLE_TEXT_TERTIARY, 0);
    }
}
