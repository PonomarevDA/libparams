# Copyright (c) 2023-2025 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.22)

if(LIBPARAMS_PLATFORM STREQUAL "stm32f103")
elseif(LIBPARAMS_PLATFORM STREQUAL "stm32g0b1")
elseif(LIBPARAMS_PLATFORM STREQUAL "stm32h753xx")
elseif(LIBPARAMS_PLATFORM STREQUAL "ubuntu")
else()
  message(SEND_ERROR "LIBPARAMS_PLATFORM is not specified! Options: stm32f103, stm32g0b1, stm32h753xx, ubuntu.")
endif()

FILE(GLOB libparamsPlatformSpecificSrc
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/*.c*
)

if(LIBPARAMS_PLATFORM STREQUAL "stm32h753xx")
  FILE(GLOB libparamsStm32h753xxSpiFramSrc
    ${CMAKE_CURRENT_LIST_DIR}/platform_specific/stm32h753xx/stm32h753xx_spifram/*.c*
  )
  list(APPEND libparamsPlatformSpecificSrc ${libparamsStm32h753xxSpiFramSrc})
endif()

FILE(GLOB libparamsPlatformSpecificHeaders
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/
)

set(libparamsExtraPlatformHeaders)
if(LIBPARAMS_PLATFORM STREQUAL "stm32h753xx")
  list(APPEND libparamsExtraPlatformHeaders
    ${CMAKE_CURRENT_LIST_DIR}/platform_specific/stm32h753xx/stm32h753xx_spifram/
  )
endif()

set(libparamsSrc
  ${CMAKE_CURRENT_LIST_DIR}/src/rom.c
  ${CMAKE_CURRENT_LIST_DIR}/src/storage.c
  ${libparamsPlatformSpecificSrc}
)

set(libparamsHeaders
  ${CMAKE_CURRENT_LIST_DIR}/include/libparams/
  ${CMAKE_CURRENT_LIST_DIR}/platform_specific/${LIBPARAMS_PLATFORM}/
  ${libparamsPlatformSpecificHeaders}
  ${libparamsExtraPlatformHeaders}
)

# Generate the parameter tables from YAML files at configure time:
#
# libparams_generate_params(
#     OUT_DIR <dir>                 # <OUT_FILE_NAME>.cpp/.hpp, or .c/.h for LANGUAGE c
#     YAML <file>...                # parameters in index order
#     [LANGUAGE c++|c]              # default: c++
#     [OUT_FILE_NAME <name>]        # default: params
#     [DEFAULT_PARAMS_DIR <dir>]    # SITL: also write <DEFAULT_PARAMS_NAME>_<page>.yaml
#     [DEFAULT_PARAMS_NAME <name>]  # default: default_params
#     [DOCS <file>]                 # markdown table of the parameters
# )
#
# Relative YAML and DOCS paths are relative to the current source dir, relative output
# dirs to the current binary dir. CMake re-configures when a YAML file changes.
function(libparams_generate_params)
  cmake_parse_arguments(ARG ""
    "OUT_DIR;LANGUAGE;OUT_FILE_NAME;DEFAULT_PARAMS_DIR;DEFAULT_PARAMS_NAME;DOCS"
    "YAML" ${ARGN})
  if(NOT ARG_OUT_DIR OR NOT ARG_YAML)
    message(FATAL_ERROR "libparams_generate_params: OUT_DIR and YAML are required.")
  endif()
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  set(scripts_dir ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts)

  set(yaml_files)
  foreach(yaml_file ${ARG_YAML})
    get_filename_component(yaml_file ${yaml_file} ABSOLUTE)
    list(APPEND yaml_files ${yaml_file})
  endforeach()
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${yaml_files})

  get_filename_component(out_dir ${ARG_OUT_DIR} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})
  set(args --out-dir ${out_dir} -f ${yaml_files})
  if(ARG_LANGUAGE)
    list(APPEND args --language ${ARG_LANGUAGE})
  endif()
  if(ARG_OUT_FILE_NAME)
    list(APPEND args --out-file-name ${ARG_OUT_FILE_NAME})
  endif()
  if(ARG_DEFAULT_PARAMS_DIR)
    get_filename_component(default_params_dir ${ARG_DEFAULT_PARAMS_DIR} ABSOLUTE
                           BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})
    list(APPEND args --default-params-dir ${default_params_dir})
  endif()
  if(ARG_DEFAULT_PARAMS_NAME)
    list(APPEND args --default-params-name ${ARG_DEFAULT_PARAMS_NAME})
  endif()
  execute_process(
    COMMAND ${Python3_EXECUTABLE} ${scripts_dir}/generate_params.py ${args}
    COMMAND_ERROR_IS_FATAL ANY
  )

  if(ARG_DOCS)
    get_filename_component(docs ${ARG_DOCS} ABSOLUTE)
    execute_process(
      COMMAND ${Python3_EXECUTABLE} ${scripts_dir}/generate_docs.py --output ${docs} ${yaml_files}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endfunction()
