cmake_minimum_required(VERSION 3.15)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include(pico_sdk_import.cmake)

project(04-bme)

pico_sdk_init()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/led-task
    ${CMAKE_CURRENT_SOURCE_DIR}/stdio-task
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/protocol/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/bme280-driver/include
)

add_executable(${PROJECT_NAME}
    main.c
    led-task/led-task.c
    stdio-task/stdio-task.c
    ../libs/protocol/src/protocol-task.c
    ../libs/bme280-driver/src/bme280-driver.c
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_i2c
)

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})