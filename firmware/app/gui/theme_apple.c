#include "theme_apple.h"
#include "src/widgets/button/lv_button.h"

/* ── Custom LVGL theme callback ──────────────────────────────────────────── */
static void apple_theme_apply_cb(lv_theme_t *th, lv_obj_t *obj) {
    LV_UNUSED(th);

    /* Base style for ALL objects: dark background */
    static lv_style_t style_base;
    static bool style_base_init = false;
    if (!style_base_init) {
        lv_style_init(&style_base);
        lv_style_set_bg_color(&style_base, APPLE_BG_DARK);
        lv_style_set_bg_opa(&style_base, LV_OPA_COVER);
        lv_style_set_text_color(&style_base, APPLE_TEXT_PRIMARY);
        lv_style_set_text_font(&style_base, APPLE_FONT_BODY);
        lv_style_set_border_width(&style_base, 0);
        lv_style_set_pad_all(&style_base, 0);
        style_base_init = true;
    }

    /* Screen */
    if (lv_obj_check_type(obj, &lv_obj_class)) {
        lv_obj_add_style(obj, &style_base, 0);
    }

    /* Labels */
    if (lv_obj_check_type(obj, &lv_label_class)) {
        static lv_style_t style_label;
        static bool style_label_init = false;
        if (!style_label_init) {
            lv_style_init(&style_label);
            lv_style_set_text_color(&style_label, APPLE_TEXT_PRIMARY);
            lv_style_set_text_font(&style_label, APPLE_FONT_BODY);
            style_label_init = true;
        }
        lv_obj_add_style(obj, &style_label, 0);
    }

    /* Buttons */
    if (lv_obj_check_type(obj, &lv_button_class)) {
        static lv_style_t style_btn;
        static lv_style_t style_btn_pressed;
        static bool style_btn_init = false;
        if (!style_btn_init) {
            lv_style_init(&style_btn);
            lv_style_set_bg_color(&style_btn, APPLE_BG_ELEVATED);
            lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
            lv_style_set_radius(&style_btn, APPLE_RADIUS_MEDIUM);
            lv_style_set_border_width(&style_btn, 0);
            lv_style_set_shadow_width(&style_btn, 0);
            lv_style_set_text_color(&style_btn, APPLE_TEXT_PRIMARY);
            lv_style_set_pad_hor(&style_btn, 16);
            lv_style_set_pad_ver(&style_btn, 8);

            lv_style_init(&style_btn_pressed);
            lv_style_set_bg_color(&style_btn_pressed, APPLE_BG_GROUPED);
            lv_style_set_transform_scale(&style_btn_pressed, 245);

            style_btn_init = true;
        }
        lv_obj_add_style(obj, &style_btn, 0);
        lv_obj_add_style(obj, &style_btn_pressed, LV_STATE_PRESSED);
    }

    /* Containers / panels */
    if (lv_obj_check_type(obj, &lv_obj_class) &&
        lv_obj_get_parent(obj) != NULL) {
        lv_obj_add_style(obj, &style_base, 0);
    }
}

void theme_apple_init(void) {
    lv_theme_default_init(lv_display_get_default(),
                          APPLE_ACCENT_BLUE,
                          APPLE_ACCENT_BLUE,
                          true,
                          APPLE_FONT_BODY);

    lv_theme_t *default_theme = lv_theme_default_get();
    lv_theme_set_apply_cb(default_theme, apple_theme_apply_cb);
}

void theme_apple_apply_card(lv_obj_t *obj) {
    static lv_style_t style_card;
    static bool init = false;
    if (!init) {
        lv_style_init(&style_card);
        lv_style_set_bg_color(&style_card, APPLE_BG_CARD);
        lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
        lv_style_set_radius(&style_card, APPLE_RADIUS_LARGE);
        lv_style_set_border_width(&style_card, 0);
        lv_style_set_shadow_width(&style_card, 20);
        lv_style_set_shadow_color(&style_card, lv_color_hex(0x000000));
        lv_style_set_shadow_opa(&style_card, LV_OPA_40);
        lv_style_set_shadow_offset_y(&style_card, 4);
        lv_style_set_pad_all(&style_card, 12);
        init = true;
    }
    lv_obj_add_style(obj, &style_card, 0);
}

void theme_apple_apply_pill_btn(lv_obj_t *btn, lv_color_t accent_color) {
    lv_obj_set_style_bg_color(btn, accent_color, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(btn, APPLE_RADIUS_PILL, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_text_color(btn, APPLE_TEXT_PRIMARY, 0);
    lv_obj_set_style_bg_color(btn, lv_color_darken(accent_color, LV_OPA_30), LV_STATE_PRESSED);
}
