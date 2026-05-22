cmake_minimum_required(VERSION 3.15)
set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)
include(pico_sdk_import.cmake)
project(05_display)
pico_sdk_init()

# ПРАВИЛЬНЫЙ ПУТЬ к библиотеке (на уровень выше, в папку libs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs/ili9341-driver 
                 ${CMAKE_CURRENT_BINARY_DIR}/ili9341-driver)

add_executable(${PROJECT_NAME}
    main.c
    stdio-task/stdio-task.c
    protocol-task/protocol-task.c
    led-task/led-task.c
    mem-task/mem-task.c
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_spi
    ili9341
)

pico_set_linker_script(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/memmap_rp2040.ld)
pico_add_extra_outputs(${PROJECT_NAME})
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)