/**
 * stm32_hal_impl.c — STM32H7 HAL implementations of all hal.h interfaces
 *
 * Status: Phase 0 stubs — populated when STM32CubeMX project is set up.
 *
 * Hardware targets:
 *   MCU:     STM32H743ZIT6 @ 480 MHz
 *   Display: 800×480 RGB IPS via LTDC + DMA2D
 *   Audio:   SAI1 → PCM5102A DAC (I2S, 44.1/96 kHz, 32-bit)
 *   USB:     USB_OTG_HS as Host MSC
 *   GPIO:    TIM encoder for jog wheel, ADC for pitch slider, GPIO matrix for buttons
 */

#include "hal.h"
/* #include "stm32h7xx_hal.h" */

/* ── Audio ───────────────────────────────────────────────────────────────── */
void hal_audio_init(const hal_audio_cfg_t *cfg) {
    (void)cfg;
    /* TODO:
     *   - Configure SAI1 Block A in master TX mode
     *   - Set up DMA circular buffer (double-buffered, half-complete IRQ)
     *   - Enable I2S master clock for PCM5102A (256× oversampling)
     */
}

void hal_audio_write(const void *samples, uint32_t frames) {
    (void)samples; (void)frames;
    /* TODO: Write to SAI DMA double-buffer */
}

uint32_t hal_audio_frames_free(void) {
    /* TODO: Return DMA buffer free space */
    return 0;
}

void hal_audio_deinit(void) {
    /* TODO: Stop SAI DMA */
}

/* ── USB / Storage ───────────────────────────────────────────────────────── */
void hal_usb_init(hal_usb_event_cb_t cb) {
    (void)cb;
    /* TODO:
     *   - Initialise USB_OTG_HS in Host mode
     *   - Register MSC class driver
     *   - Mount FatFs with exFAT support on MSC LUN 0
     *   - Call cb(HAL_USB_EVENT_INSERTED) when drive enumerated
     *   - Call cb(HAL_USB_EVENT_REMOVED)  on disconnect IRQ
     */
}

bool hal_usb_is_mounted(void) {
    /* TODO: return FatFs mount state */
    return false;
}

const char *hal_usb_mount_path(void) {
    return "0:";    /* FatFs drive 0 */
}

/* ── Controls ────────────────────────────────────────────────────────────── */
void hal_controls_init(void) {
    /* TODO:
     *   - Configure GPIO input matrix (12×8 scanning)
     *   - Configure TIM3 as encoder for jog wheel (optical, 600 PPR)
     *   - Configure ADC1 CH10 for pitch slider (12-bit, DMA)
     *   - Configure EXTI for jog top-plate capacitive touch (INT pin)
     */
}

void hal_controls_poll(hal_controls_state_t *out) {
    (void)out;
    /* TODO:
     *   - Scan button matrix
     *   - Read TIM3 encoder count → jog_delta
     *   - Read ADC1 → pitch_position (normalise to -1.0..+1.0)
     *   - Read capacitive touch INT → jog_touch
     */
}
