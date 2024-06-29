# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.15.3)
project(libparams)

if(LIBPARAMS_PLATFORM STREQUAL "stm32f103")
elseif(LIBPARAMS_PLATFORM STREQUAL "stm32g0b1")
elseif(LIBPARAMS_PLATFORM STREQUAL "ubuntu")
else()
  message(SEND_ERROR "LIBPARAMS_PLATFORM is not specified! Options: stm32f103, stm32g0b1, ubuntu.")
endif()

FILE(GLOB libparamsPlatformSpecificSrc
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/flash_driver.*
)

add_library(${PROJECT_NAME} STATIC
  ${CMAKE_CURRENT_LIST_DIR}/libparams/rom.c
  ${CMAKE_CURRENT_LIST_DIR}/libparams/storage.c
  ${libparamsPlatformSpecificSrc}
)
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/libparams/
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/
)
