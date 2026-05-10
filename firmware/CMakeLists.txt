cmake_minimum_required(VERSION 3.22)

# ── Platform selection ─────────────────────────────────────────────────────────
if(NOT DEFINED PLATFORM)
    set(PLATFORM "sim" CACHE STRING "Target platform: sim or stm32")
endif()
message(STATUS "Building for PLATFORM=${PLATFORM}")

# STM32 must set the toolchain BEFORE project()
if(PLATFORM STREQUAL "stm32")
    set(CMAKE_SYSTEM_NAME  Generic)
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    find_program(ARM_GCC arm-none-eabi-gcc REQUIRED)
    set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
    set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
    set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
    set(CMAKE_SIZE         arm-none-eabi-size)
endif()

project(neo-cdj VERSION 0.1.0 LANGUAGES C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# ── LVGL v9 ───────────────────────────────────────────────────────────────────
# Point LVGL at our lv_conf.h before it's fetched
set(LV_CONF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h" CACHE STRING "" FORCE)
add_compile_definitions(LV_CONF_PATH="${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h")

# Disable LVGL examples and demos (they have build issues on non-ARM hosts)
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON CACHE BOOL "" FORCE)
set(LV_CONF_BUILD_DISABLE_DEMOS    ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG        v9.2.2
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(lvgl)

# LVGL globs *.S unconditionally — strip ARM assembly for non-ARM sim build
if(PLATFORM STREQUAL "sim")
    get_target_property(_lvgl_srcs lvgl SOURCES)
    list(FILTER _lvgl_srcs EXCLUDE REGEX "\\.S$")
    set_target_properties(lvgl PROPERTIES SOURCES "${_lvgl_srcs}")
endif()

# ── FreeRTOS-Kernel (stm32 only for Phase 0; sim uses plain main loop) ─────────
if(PLATFORM STREQUAL "stm32")
    # freertos_config target must exist before MakeAvailable
    add_library(freertos_config INTERFACE)
    target_include_directories(freertos_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

    set(FREERTOS_PORT  GCC_ARM_CM7_MPU CACHE STRING "" FORCE)
    set(FREERTOS_HEAP  4               CACHE STRING "" FORCE)

    FetchContent_Declare(
        FreeRTOS-Kernel
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG        V11.1.0
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(FreeRTOS-Kernel)
endif()

# ── Common app sources (100% cross-platform) ──────────────────────────────────
set(APP_SOURCES
    app/core/app_main.c
    app/core/event_bus.c
    app/gui/screen_main.c
    app/gui/theme_apple.c
)

# ── Common include dirs ────────────────────────────────────────────────────────
set(COMMON_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    platform/common
    app/core
    app/gui
    app/audio
    app/storage
    app/controls
    app/network
)

# ══════════════════════════════════════════════════════════════════════════════
# SIMULATOR TARGET
# ══════════════════════════════════════════════════════════════════════════════
if(PLATFORM STREQUAL "sim")
    find_package(SDL2 REQUIRED)

    set(SIM_SOURCES
        ${APP_SOURCES}
        platform/sim/sim_main.c
        platform/sim/sdl/lv_port_disp.c
        platform/sim/sdl/lv_port_indev.c
        platform/sim/mocks/mock_usb_msc.c
        platform/sim/mocks/mock_controls.c
        platform/sim/mocks/mock_audio.c
    )

    add_executable(neo-cdj-sim ${SIM_SOURCES})

    target_include_directories(neo-cdj-sim PRIVATE
        ${COMMON_INCLUDES}
        platform/sim
        platform/sim/sdl
        platform/sim/mocks
    )

    target_compile_definitions(neo-cdj-sim PRIVATE
        PLATFORM_SIM=1
        LV_CONF_INCLUDE_SIMPLE=1
    )

    target_compile_options(neo-cdj-sim PRIVATE
        -Wall -Wextra -Wno-unused-parameter
        -g -O0
    )

    target_link_libraries(neo-cdj-sim PRIVATE
        lvgl
        SDL2::SDL2
        m
    )

    # Copy mock USB assets next to the binary
    add_custom_command(TARGET neo-cdj-sim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:neo-cdj-sim>/test_assets/mock_usb
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/../test_assets/mock_usb
            $<TARGET_FILE_DIR:neo-cdj-sim>/test_assets/mock_usb
        COMMENT "Copying mock USB assets"
    )

# ══════════════════════════════════════════════════════════════════════════════
# STM32 TARGET
# ══════════════════════════════════════════════════════════════════════════════
elseif(PLATFORM STREQUAL "stm32")
    set(STM32_MCU_FLAGS
        -mcpu=cortex-m7
        -mthumb
        -mfpu=fpv5-d16
        -mfloat-abi=hard
    )
    set(STM32_COMMON_FLAGS
        ${STM32_MCU_FLAGS}
        -fdata-sections
        -ffunction-sections
        -fno-exceptions
    )

    set(STM32_SOURCES
        ${APP_SOURCES}
        platform/stm32/stm32_main.c
        platform/stm32/stm32_hal_impl.c
    )

    add_executable(neo-cdj-stm32 ${STM32_SOURCES})

    target_include_directories(neo-cdj-stm32 PRIVATE
        ${COMMON_INCLUDES}
        platform/stm32
    )

    target_compile_definitions(neo-cdj-stm32 PRIVATE
        PLATFORM_STM32=1
        STM32H743xx
        USE_HAL_DRIVER
        LV_CONF_INCLUDE_SIMPLE=1
    )

    target_compile_options(neo-cdj-stm32 PRIVATE
        ${STM32_COMMON_FLAGS}
        -Wall -Wextra -Wno-unused-parameter
        -Os -g
    )

    target_link_options(neo-cdj-stm32 PRIVATE
        ${STM32_MCU_FLAGS}
        -T${CMAKE_CURRENT_SOURCE_DIR}/platform/stm32/STM32H743ZITx_FLASH.ld
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_BINARY_DIR}/neo-cdj-stm32.map
        --specs=nosys.specs
        --specs=nano.specs
    )

    target_link_libraries(neo-cdj-stm32 PRIVATE
        lvgl
        freertos_kernel
        m
    )

    # Generate .hex and .bin
    add_custom_command(TARGET neo-cdj-stm32 POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:neo-cdj-stm32> neo-cdj-stm32.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:neo-cdj-stm32> neo-cdj-stm32.bin
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:neo-cdj-stm32>
        COMMENT "Generating HEX and BIN"
    )

else()
    message(FATAL_ERROR "Unknown PLATFORM '${PLATFORM}'. Use -DPLATFORM=sim or -DPLATFORM=stm32")
endif()

# ── Tests ──────────────────────────────────────────────────────────────────────
if(PLATFORM STREQUAL "sim" AND BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
