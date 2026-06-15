cmake_minimum_required(VERSION 3.22)
project(libparams_stm32g0b1 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/stm32_platform_test.cmake)
add_libparams_stm32_platform_test(libparams stm32g0b1 STM32G0B1xx)
