cmake_minimum_required(VERSION 3.15)
project(${PROJECT_NAME})
project(ili9341)

add_library(${PROJECT_NAME} STATIC
    "src/ili9341-driver.c"
    "src/ili9341-display.c"
    "src/ili9341-front.c"
    "src/front-jetbrains.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "include"
)