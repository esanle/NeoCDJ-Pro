cmake_minimum_required(VERSION 3.22)

# Unit tests — compiled as part of the sim build
# Run: ctest --test-dir firmware/build/tests

include(CTest)

# Shared test helpers
set(TEST_INCLUDES
    ${CMAKE_SOURCE_DIR}/platform/common
    ${CMAKE_SOURCE_DIR}/app/core
    ${CMAKE_SOURCE_DIR}/app/gui
)

# ── test_event_bus ─────────────────────────────────────────────────────────
add_executable(test_event_bus
    test_event_bus.c
    ${CMAKE_SOURCE_DIR}/app/core/event_bus.c
)
target_include_directories(test_event_bus PRIVATE ${TEST_INCLUDES})
target_compile_definitions(test_event_bus PRIVATE PLATFORM_SIM=1)
target_link_libraries(test_event_bus PRIVATE m)
add_test(NAME event_bus COMMAND test_event_bus)
