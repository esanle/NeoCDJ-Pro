#include "mock_audio.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RING_BUFFER_FRAMES  8192

static SDL_AudioDeviceID s_dev          = 0;
static uint32_t          s_bytes_per_frame = 0;
static uint8_t          *s_ring_buf     = NULL;
static volatile int      s_write_pos    = 0;
static volatile int      s_read_pos     = 0;
static int               s_ring_capacity = 0;

/* SDL audio callback: called by SDL from its audio thread */
static void sdl_audio_cb(void *userdata, uint8_t *stream, int len) {
    (void)userdata;
    int available = (s_write_pos - s_read_pos + s_ring_capacity) % s_ring_capacity;
    int to_copy   = (len < available) ? len : available;

    if (to_copy > 0) {
        /* Wrap-around ring buffer read */
        int first  = s_ring_capacity - s_read_pos;
        if (first >= to_copy) {
            memcpy(stream, s_ring_buf + s_read_pos, to_copy);
        } else {
            memcpy(stream, s_ring_buf + s_read_pos, first);
            memcpy(stream + first, s_ring_buf, to_copy - first);
        }
        s_read_pos = (s_read_pos + to_copy) % s_ring_capacity;
    }

    /* Zero-pad if underrun */
    if (to_copy < len) {
        memset(stream + to_copy, 0, len - to_copy);
    }
}

void mock_audio_init(const hal_audio_cfg_t *cfg) {
    s_bytes_per_frame = cfg->channels * (cfg->bits_per_sample / 8);
    s_ring_capacity   = RING_BUFFER_FRAMES * s_bytes_per_frame;
    s_ring_buf        = (uint8_t *)malloc(s_ring_capacity);
    if (!s_ring_buf) {
        fprintf(stderr, "[mock_audio] OOM\n");
        return;
    }
    memset(s_ring_buf, 0, s_ring_capacity);
    s_write_pos = 0;
    s_read_pos  = 0;

    SDL_AudioSpec want = {
        .freq     = (int)cfg->sample_rate,
        .format   = (cfg->bits_per_sample == 16) ? AUDIO_S16LSB : AUDIO_S32LSB,
        .channels = cfg->channels,
        .samples  = (uint16_t)cfg->buffer_frames,
        .callback = sdl_audio_cb,
        .userdata = NULL,
    };
    SDL_AudioSpec got;

    s_dev = SDL_OpenAudioDevice(NULL, 0, &want, &got, 0);
    if (s_dev == 0) {
        fprintf(stderr, "[mock_audio] SDL_OpenAudioDevice: %s\n", SDL_GetError());
        return;
    }

    SDL_PauseAudioDevice(s_dev, 0);  /* start playback */
    printf("[mock_audio] SDL2 audio: %u Hz, %uch, %u-bit\n",
           got.freq, got.channels, got.format == AUDIO_S16LSB ? 16 : 32);
}

void mock_audio_write(const void *samples, uint32_t frames) {
    if (!s_ring_buf || s_dev == 0) return;
    uint32_t bytes = frames * s_bytes_per_frame;

    /* Spin-wait if ring buffer is full (blocking write semantics) */
    while (1) {
        int free_bytes = (s_read_pos - s_write_pos - 1 + s_ring_capacity) % s_ring_capacity;
        if ((uint32_t)free_bytes >= bytes) break;
        SDL_Delay(1);
    }

    const uint8_t *src = (const uint8_t *)samples;
    int first = s_ring_capacity - s_write_pos;
    if ((uint32_t)first >= bytes) {
        memcpy(s_ring_buf + s_write_pos, src, bytes);
    } else {
        memcpy(s_ring_buf + s_write_pos, src, first);
        memcpy(s_ring_buf, src + first, bytes - first);
    }
    s_write_pos = (s_write_pos + bytes) % s_ring_capacity;
}

uint32_t mock_audio_frames_free(void) {
    if (!s_ring_buf) return 0;
    int free_bytes = (s_read_pos - s_write_pos - 1 + s_ring_capacity) % s_ring_capacity;
    return (uint32_t)free_bytes / s_bytes_per_frame;
}

void mock_audio_deinit(void) {
    if (s_dev) {
        SDL_CloseAudioDevice(s_dev);
        s_dev = 0;
    }
    free(s_ring_buf);
    s_ring_buf = NULL;
}
