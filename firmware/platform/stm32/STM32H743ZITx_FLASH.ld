/*
 * STM32H743ZITx_FLASH.ld — Linker script for STM32H743ZI (2 MB Flash, 1 MB RAM)
 *
 * Memory map:
 *   FLASH: 0x08000000 (2 MB) — code + read-only data
 *   DTCM:  0x20000000 (128 KB) — FreeRTOS heap + audio DMA buffers (fast, DMA-capable)
 *   AXI:   0x24000000 (512 KB) — LVGL draw buffers + BSS
 *   SRAM1: 0x30000000 (128 KB) — USB stack + FatFs buffers
 *   SRAM2: 0x30020000 (128 KB) — available
 *   SRAM3: 0x30040000 (32  KB) — available
 *   SRAM4: 0x38000000 (64  KB) — available (power-domain D3)
 *   BKPRAM: 0x38800000 (4 KB) — backup registers (RTC, power state)
 */

MEMORY {
    FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
    DTCM   (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
    AXI    (xrw) : ORIGIN = 0x24000000, LENGTH = 512K
    SRAM1  (xrw) : ORIGIN = 0x30000000, LENGTH = 128K
    SRAM2  (xrw) : ORIGIN = 0x30020000, LENGTH = 128K
    SRAM3  (xrw) : ORIGIN = 0x30040000, LENGTH = 32K
    SRAM4  (xrw) : ORIGIN = 0x38000000, LENGTH = 64K
    BKPRAM (xrw) : ORIGIN = 0x38800000, LENGTH = 4K
}

/* Entry point */
ENTRY(Reset_Handler)

_stack_size = 8K;
_heap_size  = 16K;

SECTIONS {
    /* Vector table + code */
    .isr_vector : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    .text : {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
    .ARM : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH

    .preinit_array : {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } >FLASH

    .init_array : {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } >FLASH

    .fini_array : {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } >FLASH

    /* Initialised data — loaded from FLASH, copied to AXI SRAM at startup */
    _sidata = LOADADDR(.data);
    .data : {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >AXI AT >FLASH

    /* BSS — zero-initialised in AXI SRAM */
    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >AXI

    /* LVGL draw buffers in AXI SRAM (fast, uncached via MPU if needed) */
    .lvgl_buf (NOLOAD) : {
        . = ALIGN(32);
        *(.lvgl_buf)
    } >AXI

    /* FreeRTOS heap in DTCM (fastest access for task stacks) */
    .freertos_heap (NOLOAD) : {
        . = ALIGN(4);
        *(.freertos_heap)
    } >DTCM

    /* Stack at top of DTCM */
    ._user_heap_stack : {
        . = ALIGN(8);
        PROVIDE(end = .);
        PROVIDE(_end = .);
        . = . + _heap_size;
        . = . + _stack_size;
        . = ALIGN(8);
    } >DTCM

    /* USB / FatFs buffers in SRAM1 (accessible from USB HS DMA) */
    .usb_buf (NOLOAD) : {
        . = ALIGN(4);
        *(.usb_buf)
    } >SRAM1
}
