#pragma once

#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * mock_audio — SDL2 audio output backend.
 * Implements hal_audio_* for the simulator.
 */
void mock_audio_init(const hal_audio_cfg_t *cfg);
void mock_audio_write(const void *samples, uint32_t frames);
uint32_t mock_audio_frames_free(void);
void mock_audio_deinit(void);

#ifdef __cplusplus
}
#endif
