# Copyright (c) 2023-2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

# The flash driver of LIBPARAMS_PLATFORM, compiled by the application: a microcontroller driver
# includes the application's HAL (main.h, spi.h), so it can't live in the libparams target.
# Sets LIBPARAMS_PLATFORM_SOURCES and LIBPARAMS_PLATFORM_HEADERS.

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH libparams_root_dir)

if(NOT LIBPARAMS_PLATFORM MATCHES "^(stm32f103|stm32g0b1|stm32h753xx|ubuntu)$")
  message(SEND_ERROR "LIBPARAMS_PLATFORM is not specified! Options: stm32f103, stm32g0b1, stm32h753xx, ubuntu.")
endif()

FILE(GLOB LIBPARAMS_PLATFORM_SOURCES ${libparams_root_dir}/platform_specific/${LIBPARAMS_PLATFORM}/*.c*)
set(LIBPARAMS_PLATFORM_HEADERS ${libparams_root_dir}/platform_specific/${LIBPARAMS_PLATFORM}/)

if(LIBPARAMS_PLATFORM STREQUAL "stm32h753xx")
  FILE(GLOB libparams_spifram_sources
    ${libparams_root_dir}/platform_specific/stm32h753xx/stm32h753xx_spifram/*.c*
  )
  list(APPEND LIBPARAMS_PLATFORM_SOURCES ${libparams_spifram_sources})
  list(APPEND LIBPARAMS_PLATFORM_HEADERS
    ${libparams_root_dir}/platform_specific/stm32h753xx/stm32h753xx_spifram/
  )
endif()
