/**
 * NeoCDJ-Pro LVGL v9 Configuration
 * Tuned for: 800×480 IPS + Apple-style dark UI + SDL2 simulator
 */
#if 1  /* set to 0 to disable this file */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* ── Color ─────────────────────────────────────────────────────────────────── */
#define LV_COLOR_DEPTH 32           /* ARGB8888 — matches SDL SDL_PIXELFORMAT_ARGB8888 */

/* ── Memory ────────────────────────────────────────────────────────────────── */
#define LV_MEM_SIZE          (512U * 1024U)   /* 512 KB for sim; STM32 DTCM tunes this */
#define LV_MEM_POOL_INCLUDE  <stdlib.h>
#define LV_MEM_POOL_ALLOC    malloc
#define LV_MEM_POOL_FREE     free

/* ── Tick ───────────────────────────────────────────────────────────────────── */
/* Provided via lv_tick_set_cb() — do NOT use built-in tick */
#define LV_TICK_CUSTOM 0

/* ── Log ───────────────────────────────────────────────────────────────────── */
#define LV_USE_LOG      1
#define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF   1

/* ── Performance monitors ──────────────────────────────────────────────────── */
#define LV_USE_PERF_MONITOR 1
#define LV_USE_MEM_MONITOR  1

/* ── Assert ────────────────────────────────────────────────────────────────── */
#define LV_USE_ASSERT_NULL         1
#define LV_USE_ASSERT_MALLOC       1
#define LV_USE_ASSERT_STYLE        0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ          0

/* ── Display ───────────────────────────────────────────────────────────────── */
#define LV_USE_SDL 0    /* We use our own lv_port_disp.c for full SDL event control */

/* ── Fonts ─────────────────────────────────────────────────────────────────── */
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_36 1
#define LV_FONT_MONTSERRAT_48 1

#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* ── Widgets ───────────────────────────────────────────────────────────────── */
#define LV_USE_ANIMIMG   1
#define LV_USE_ARC       1
#define LV_USE_BAR       1
#define LV_USE_BTN       1
#define LV_USE_BTNMATRIX 1
#define LV_USE_CALENDAR  0
#define LV_USE_CANVAS    1    /* Waveform rendering */
#define LV_USE_CHART     0
#define LV_USE_CHECKBOX  1
#define LV_USE_DROPDOWN  1
#define LV_USE_IMG       1
#define LV_USE_IMGBTN    0
#define LV_USE_KEYBOARD  0
#define LV_USE_LABEL     1
#define LV_USE_LED       0
#define LV_USE_LINE      1
#define LV_USE_LIST      1
#define LV_USE_MENU      1
#define LV_USE_METER     0
#define LV_USE_MSGBOX    1
#define LV_USE_ROLLER    0
#define LV_USE_SCALE     1
#define LV_USE_SLIDER    1
#define LV_USE_SPAN      1
#define LV_USE_SPINBOX   0
#define LV_USE_SPINNER   1
#define LV_USE_SWITCH    1
#define LV_USE_TABLE     1
#define LV_USE_TABVIEW   1
#define LV_USE_TEXTAREA  1
#define LV_USE_TILEVIEW  1
#define LV_USE_WIN       0

/* ── Layouts ───────────────────────────────────────────────────────────────── */
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/* ── Animation ─────────────────────────────────────────────────────────────── */
#define LV_USE_ANIM 1

/* ── Image decoders ────────────────────────────────────────────────────────── */
#define LV_USE_BMP  0
#define LV_USE_PNG  0
#define LV_USE_SJPG 0
#define LV_USE_GIF  0

/* ── Misc ───────────────────────────────────────────────────────────────────── */
#define LV_USE_FS_POSIX   1     /* File access via POSIX fopen/fread (sim) */
#define LV_FS_POSIX_LETTER 'P'  /* e.g. P:/path/to/file */

#define LV_USE_SNAPSHOT   0
#define LV_USE_SYSMON     0
#define LV_USE_PROFILER   0
#define LV_USE_MONKEY     0
#define LV_USE_GRIDNAV    0
#define LV_USE_FRAGMENT   0
#define LV_USE_OBSERVER   1    /* For data binding in future screens */
#define LV_USE_IME_PINYIN 0
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/* ── Theme ──────────────────────────────────────────────────────────────────── */
#define LV_USE_THEME_DEFAULT 1
#define LV_USE_THEME_SIMPLE  0
#define LV_USE_THEME_MONO    0

/* ── Input devices ──────────────────────────────────────────────────────────── */
#define LV_INDEV_DEF_READ_PERIOD   16    /* ms between indev reads (≈60 fps) */
#define LV_INDEV_DEF_DRAG_LIMIT    10
#define LV_INDEV_DEF_DRAG_THROW    10
#define LV_INDEV_DEF_LONG_PRESS_TIME  400
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME 100
#define LV_INDEV_DEF_GESTURE_LIMIT   50
#define LV_INDEV_DEF_GESTURE_MIN_VELOCITY 3

/* ── Display driver ─────────────────────────────────────────────────────────── */
#define LV_DEF_REFR_PERIOD     16    /* ms — 60 fps target */

/* ── Object features ────────────────────────────────────────────────────────── */
#define LV_USE_OBJ_PROPERTY 1
#define LV_USE_OBJ_NAME     0

#endif /* LV_CONF_H */
#endif /* "if 1" at the top */
