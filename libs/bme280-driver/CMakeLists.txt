cmake_minimum_required(VERSION 3.15)

project(bme280-driver)

add_library(bme280-driver STATIC
    src/bme280-driver.c
)

target_include_directories(bme280-driver PUBLIC
    include
)
