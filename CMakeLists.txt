# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MIT License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

if(NOT LIBPARAMS_PLATFORM)
  message(SEND_ERROR "LIBPARAMS_PLATFORM is not specified")
endif()

FILE(GLOB libparamsPlatformSpecificSrc
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/flash_driver.*
)

set(libparamsSrc
  ${CMAKE_CURRENT_LIST_DIR}/libparams/rom.c
  ${CMAKE_CURRENT_LIST_DIR}/libparams/storage.c
  ${libparamsPlatformSpecificSrc}
)

set(libparamsHeaders
  ${CMAKE_CURRENT_LIST_DIR}/libparams/
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/
)
