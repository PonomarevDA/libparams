# Copyright (c) 2025 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.22)
project(libparams VERSION 1.0 LANGUAGES CXX C)

if(LIBPARAMS_PLATFORM STREQUAL "stm32f103")
elseif(LIBPARAMS_PLATFORM STREQUAL "stm32g0b1")
elseif(LIBPARAMS_PLATFORM STREQUAL "ubuntu")
else()
  message(SEND_ERROR "LIBPARAMS_PLATFORM is not specified! Options: stm32f103, stm32g0b1, ubuntu.")
endif()

execute_process(
  COMMAND ${ROOT_DIR}/scripts/generate_default_params.py
          --out-dir ${CMAKE_CURRENT_BINARY_DIR}/params
          -f ${TESTS_DIR}/params/params.c
          --out-file-name "init_params"
  RESULT_VARIABLE ret
)
if(NOT ret EQUAL 0)
  message( FATAL_ERROR "Default Params Generator has been failed. Abort.")
endif()
set(LIBPARAMS_PARAMS_DIR="${CMAKE_CURRENT_BINARY_DIR}/params")
add_definitions(-DLIBPARAMS_PARAMS_DIR="${CMAKE_CURRENT_BINARY_DIR}/params")

FILE(GLOB libparamsPlatformSpecificSrc
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/*.c*
)

FILE(GLOB libparamsGeneratedParams
  ${LIBPARAMS_GENERATED_PARAMS_DIR}/*.c*
  ${LIBPARAMS_GENERATED_PARAMS_DIR}/*.cpp*
)

add_library(${PROJECT_NAME} STATIC
  src/rom.c
  src/storage.c
  ${libparamsPlatformSpecificSrc}
  ${libparamsGeneratedParams}
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${LIBPARAMS_GENERATED_PARAMS_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${LIBPARAMS_GENERATED_PARAMS_DIR}>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)