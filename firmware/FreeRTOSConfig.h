/**
 * FreeRTOS configuration for NeoCDJ-Pro
 *
 * sim   → FreeRTOS POSIX port (GCC_POSIX) — pthreads-based scheduler
 * stm32 → FreeRTOS ARM_CM7 port — Cortex-M7 preemptive scheduler
 */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Scheduler */
#define configUSE_PREEMPTION                     1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  0   /* Required for POSIX port */
#define configUSE_TICKLESS_IDLE                  0
#define configCPU_CLOCK_HZ                       480000000UL  /* STM32H743 @ 480 MHz */
#define configTICK_RATE_HZ                       ((TickType_t)1000)
#define configMAX_PRIORITIES                     10
#define configMINIMAL_STACK_SIZE                 ((StackType_t)4096)  /* bytes (POSIX port) */
#define configMAX_TASK_NAME_LEN                  16
#define configUSE_16_BIT_TICKS                   0
#define configIDLE_SHOULD_YIELD                  1
#define configUSE_TASK_NOTIFICATIONS             1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES    3
#define configUSE_MUTEXES                        1
#define configUSE_RECURSIVE_MUTEXES              1
#define configUSE_COUNTING_SEMAPHORES            1
#define configUSE_TASK_FPU_SUPPORT               1   /* STM32H7 FPU context save */
#define configQUEUE_REGISTRY_SIZE                8
#define configUSE_QUEUE_SETS                     0
#define configUSE_TIME_SLICING                   1
#define configUSE_NEWLIB_REENTRANT               0
#define configENABLE_BACKWARD_COMPATIBILITY      0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS  0

/* Memory */
#define configSUPPORT_STATIC_ALLOCATION          0
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#ifdef PLATFORM_SIM
  #define configTOTAL_HEAP_SIZE                  ((size_t)(8 * 1024 * 1024))  /* 8 MB on PC */
#else
  #define configTOTAL_HEAP_SIZE                  ((size_t)(384 * 1024))       /* 384 KB AXI SRAM */
#endif

/* Hooks */
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCHECK_FOR_STACK_OVERFLOW           0   /* POSIX port doesn't support method 2 */
#define configUSE_MALLOC_FAILED_HOOK             1

/* Run-time stats */
#define configGENERATE_RUN_TIME_STATS            0
#define configUSE_TRACE_FACILITY                 1
#define configUSE_STATS_FORMATTING_FUNCTIONS     1

/* Co-routines: not used */
#define configUSE_CO_ROUTINES                    0

/* Software timers */
#define configUSE_TIMERS                         1
#define configTIMER_TASK_PRIORITY                (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                 16
#define configTIMER_TASK_STACK_DEPTH             (configMINIMAL_STACK_SIZE * 2)

/* POSIX port — use SIGUSR1 for tick (default, avoids SDL conflicts) */
#define configUSE_POSIX_ERRNO                    1

/* Optional API functions */
#define INCLUDE_vTaskPrioritySet                 1
#define INCLUDE_uxTaskPriorityGet                1
#define INCLUDE_vTaskDelete                      1
#define INCLUDE_vTaskSuspend                     1
#define INCLUDE_vTaskDelayUntil                  1
#define INCLUDE_vTaskDelay                       1
#define INCLUDE_xTaskGetCurrentTaskHandle        1
#define INCLUDE_xTaskGetIdleTaskHandle           0
#define INCLUDE_xQueueGetMutexHolder             0
#define INCLUDE_xSemaphoreGetMutexHolder         0
#define INCLUDE_uxTaskGetStackHighWaterMark      1
#define INCLUDE_eTaskGetState                    1
#define INCLUDE_xTimerPendFunctionCall           1
#define INCLUDE_xTaskAbortDelay                  0
#define INCLUDE_xTaskGetHandle                   0
#define INCLUDE_xTaskResumeFromISR               1

/* Cortex-M7: lowest priority for SysTick / PendSV */
#ifdef PLATFORM_STM32
  #define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      15
  #define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5
  #define configPRIO_BITS                               4
  #define configKERNEL_INTERRUPT_PRIORITY \
      (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
  #define configMAX_SYSCALL_INTERRUPT_PRIORITY \
      (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
  /* Map FreeRTOS ISR handlers to CMSIS names */
  #define vPortSVCHandler     SVC_Handler
  #define xPortPendSVHandler  PendSV_Handler
  #define xPortSysTickHandler SysTick_Handler
#endif

/* Assert */
#include <assert.h>
#define configASSERT(x) assert(x)

#endif /* FREERTOS_CONFIG_H */
