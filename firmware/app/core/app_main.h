#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise all app subsystems and load the main UI screen.
 * Called once by the platform entry point (sim_main.c / stm32_main.c)
 * after lv_init() and hal_display_init() have been called.
 *
 * On stm32: also creates FreeRTOS tasks then returns so the caller
 *           can invoke vTaskStartScheduler().
 * On sim:   runs setup inline (no scheduler needed for Phase 0).
 */
void app_main_init(void);

/**
 * Called every frame from the main loop / GUI task.
 * Polls controls and dispatches input events to the event bus.
 */
void app_main_tick(void);

#ifdef __cplusplus
}
#endif
