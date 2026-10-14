# Copyright (c) 2023-2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

# Legacy build, planned to be deprecated: the application compiles the library sources itself.
# Use add_subdirectory() and libparams::libparams instead, see README "2.2. Build".
# This file keeps libparamsSrc, libparamsHeaders and the older variables for existing projects.

cmake_minimum_required(VERSION 3.22)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/platform.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/generate_params.cmake)

set(libparamsPlatformSpecificSrc ${LIBPARAMS_PLATFORM_SOURCES})
set(libparamsPlatformSpecificHeaders ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/)
set(libparamsExtraPlatformHeaders)
if(LIBPARAMS_PLATFORM STREQUAL "stm32h753xx")
  list(APPEND libparamsExtraPlatformHeaders
    ${CMAKE_CURRENT_LIST_DIR}/platform_specific/stm32h753xx/stm32h753xx_spifram/
  )
endif()

set(libparamsSrc
  ${CMAKE_CURRENT_LIST_DIR}/src/rom.c
  ${CMAKE_CURRENT_LIST_DIR}/src/storage.c
  ${LIBPARAMS_PLATFORM_SOURCES}
)

set(libparamsHeaders
  ${CMAKE_CURRENT_LIST_DIR}/include/libparams/
  ${LIBPARAMS_PLATFORM_HEADERS}
)
