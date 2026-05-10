#include "lv_port_disp.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

/* ── Render buffer: 1/10 of the screen → ~115 KB per buffer @32 bpp ──────── */
#define BUF_LINES    (DISP_HEIGHT / 10)
#define BUF_BYTES    (DISP_WIDTH * BUF_LINES * 4)   /* 4 bytes/px = ARGB8888 */

static SDL_Window   *s_window   = NULL;
static SDL_Renderer *s_renderer = NULL;
static SDL_Texture  *s_texture  = NULL;

static uint8_t s_buf1[BUF_BYTES];
static uint8_t s_buf2[BUF_BYTES];

/* ── LVGL flush callback ─────────────────────────────────────────────────── */
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    int32_t x = area->x1;
    int32_t y = area->y1;
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    SDL_Rect rect = { (int)x, (int)y, (int)w, (int)h };

    /*
     * LVGL v9 @LV_COLOR_DEPTH=32 → pixel format ARGB8888:
     * bytes in memory = [B][G][R][A] (little-endian) = SDL_PIXELFORMAT_BGRA32
     * which SDL maps to SDL_PIXELFORMAT_ARGB8888 on little-endian hosts.
     */
    if (SDL_UpdateTexture(s_texture, &rect, px_map, w * 4) != 0) {
        fprintf(stderr, "[disp] SDL_UpdateTexture: %s\n", SDL_GetError());
    }

    if (lv_display_flush_is_last(disp)) {
        SDL_RenderClear(s_renderer);
        SDL_RenderCopy(s_renderer, s_texture, NULL, NULL);
        SDL_RenderPresent(s_renderer);
    }

    lv_display_flush_ready(disp);
}

/* ── Public API ──────────────────────────────────────────────────────────── */

lv_display_t *lv_port_disp_init(void) {
    /* Create SDL window */
    s_window = SDL_CreateWindow(
        "NeoCDJ-Pro Simulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        DISP_WIDTH, DISP_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!s_window) {
        fprintf(stderr, "[disp] SDL_CreateWindow: %s\n", SDL_GetError());
        exit(1);
    }

    /* Renderer — prefer hardware acceleration */
    s_renderer = SDL_CreateRenderer(
        s_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!s_renderer) {
        /* Fallback to software renderer */
        s_renderer = SDL_CreateRenderer(s_window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!s_renderer) {
        fprintf(stderr, "[disp] SDL_CreateRenderer: %s\n", SDL_GetError());
        exit(1);
    }

    /* Streaming texture — updated each flush */
    s_texture = SDL_CreateTexture(
        s_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        DISP_WIDTH, DISP_HEIGHT
    );
    if (!s_texture) {
        fprintf(stderr, "[disp] SDL_CreateTexture: %s\n", SDL_GetError());
        exit(1);
    }

    /* Register LVGL display */
    lv_display_t *disp = lv_display_create(DISP_WIDTH, DISP_HEIGHT);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, s_buf1, s_buf2, BUF_BYTES,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    printf("[disp] SDL2 window %dx%d (HW accel: %s)\n",
           DISP_WIDTH, DISP_HEIGHT,
           (SDL_GetRendererInfo(s_renderer, &(SDL_RendererInfo){}) == 0) ? "yes" : "n/a");

    return disp;
}

void lv_port_disp_present(void) {
    /* Explicit present is handled inside flush_cb on last flush.
     * This function is a no-op but kept for future full-screen render mode. */
    (void)0;
}
