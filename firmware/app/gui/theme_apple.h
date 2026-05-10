#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ── NeoCDJ Apple-style colour palette ──────────────────────────────────── */
#define APPLE_BG_DARK        lv_color_hex(0x0A0A0A)   /* Near-black background */
#define APPLE_BG_CARD        lv_color_hex(0x1C1C1E)   /* Card / panel surface   */
#define APPLE_BG_ELEVATED    lv_color_hex(0x2C2C2E)   /* Elevated surface       */
#define APPLE_BG_GROUPED     lv_color_hex(0x3A3A3C)   /* Grouped element bg     */

#define APPLE_TEXT_PRIMARY   lv_color_hex(0xFFFFFF)   /* Primary text (white)   */
#define APPLE_TEXT_SECONDARY lv_color_hex(0xAEAEB2)   /* Secondary text (grey)  */
#define APPLE_TEXT_TERTIARY  lv_color_hex(0x636366)   /* Disabled / hint text   */

#define APPLE_ACCENT_BLUE    lv_color_hex(0x0A84FF)   /* iOS blue accent        */
#define APPLE_ACCENT_GREEN   lv_color_hex(0x30D158)   /* Play / active indicator*/
#define APPLE_ACCENT_RED     lv_color_hex(0xFF453A)   /* Cue point / hot cue A  */
#define APPLE_ACCENT_ORANGE  lv_color_hex(0xFF9F0A)   /* Hot cue B / loop       */
#define APPLE_ACCENT_YELLOW  lv_color_hex(0xFFD60A)   /* Hot cue C              */
#define APPLE_ACCENT_CYAN    lv_color_hex(0x5AC8FA)   /* Waveform high freq     */
#define APPLE_ACCENT_PURPLE  lv_color_hex(0xBF5AF2)   /* Hot cue D              */
#define APPLE_SEPARATOR      lv_color_hex(0x38383A)   /* Hairline separator     */

/* ── Corner radii ────────────────────────────────────────────────────────── */
#define APPLE_RADIUS_SMALL   8
#define APPLE_RADIUS_MEDIUM  12
#define APPLE_RADIUS_LARGE   16
#define APPLE_RADIUS_PILL    999   /* Fully rounded / pill shape */

/* ── Font shortcuts ──────────────────────────────────────────────────────── */
#define APPLE_FONT_TITLE     &lv_font_montserrat_24
#define APPLE_FONT_BODY      &lv_font_montserrat_14
#define APPLE_FONT_CAPTION   &lv_font_montserrat_12
#define APPLE_FONT_BPM       &lv_font_montserrat_32
#define APPLE_FONT_TIMECODE  &lv_font_montserrat_36

/* ── Initialise the LVGL default theme with Apple-style overrides ─────────
 *   Call once after lv_init() and before creating any objects.          */
void theme_apple_init(void);

/** Apply Apple-style card appearance to an arbitrary container obj. */
void theme_apple_apply_card(lv_obj_t *obj);

/** Apply pill-style button style (used for transport buttons). */
void theme_apple_apply_pill_btn(lv_obj_t *btn, lv_color_t accent_color);

#ifdef __cplusplus
}
#endif
