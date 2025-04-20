# Copyright (c) 2023-2025 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.22)

if(LIBPARAMS_PLATFORM STREQUAL "stm32f103")
elseif(LIBPARAMS_PLATFORM STREQUAL "stm32g0b1")
elseif(LIBPARAMS_PLATFORM STREQUAL "ubuntu")
else()
  message(SEND_ERROR "LIBPARAMS_PLATFORM is not specified! Options: stm32f103, stm32g0b1, ubuntu.")
endif()

FILE(GLOB libparamsPlatformSpecificSrc
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/*.c*
)

FILE(GLOB libparamsPlatformSpecificHeaders
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/
)

set(libparamsSrc
  ${CMAKE_CURRENT_LIST_DIR}/src/rom.c
  ${CMAKE_CURRENT_LIST_DIR}/src/storage.c
  ${libparamsPlatformSpecificSrc}
)

set(libparamsHeaders
  ${CMAKE_CURRENT_LIST_DIR}/include/libparams/
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/
  ${libparamsPlatformSpecificHeaders}
)
