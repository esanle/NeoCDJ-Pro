#pragma once

#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * mock_controls — maps keyboard/mouse to CDJ-2000 controls.
 *
 * Keyboard mapping (see docs/simulation-guide.md for full table):
 *
 *  SPACE       → Play/Pause
 *  C           → Cue
 *  1-8         → Hot Cue A-H
 *  I / O       → Loop In / Loop Out
 *  R           → Re-Loop
 *  [  / ]      → Loop Half / Loop Double
 *  S           → Slip
 *  Q           → Quantize
 *  T           → Sync
 *  LEFT / RIGHT → Track Prev / Next
 *  UP / DOWN   → Jog CCW / CW  (no touch)
 *  Shift+UP/DN → Jog touch + scratch
 *  +  / -      → Pitch +0.01 / -0.01
 *  U           → USB insert/eject toggle
 *  Escape      → Back
 *  Enter       → Menu/Select
 *
 * Mouse:
 *  Drag left/right on the jog area (centre screen) → jog rotation with touch
 */

void mock_controls_init(void);
void mock_controls_poll(hal_controls_state_t *out);

#ifdef __cplusplus
}
#endif
