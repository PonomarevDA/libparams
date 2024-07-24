# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.15.3)
project(libparams_ubuntu_target)

# Path variables
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH PLATFORM_SPECIFIC_DIR)
cmake_path(GET PLATFORM_SPECIFIC_DIR PARENT_PATH TESTS_DIR)
cmake_path(GET TESTS_DIR PARENT_PATH ROOT_DIR)

# Build options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall") # -Wextra -pedantic -Werror
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Libraries
set(LIBPARAMS_PLATFORM ubuntu)
include(${ROOT_DIR}/CMakeLists.txt)

set(LIBPARAMS_INIT_PARAMS_BASE_NAME initial_params)
add_definitions(-DLIBPARAMS_PARAMS_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DLIBPARAMS_TEMP_PARAMS_FILE_NAME="temp_params")
add_definitions(-DLIBPARAMS_INIT_PARAMS_FILE_NAME="${LIBPARAMS_INIT_PARAMS_BASE_NAME}")

add_executable(application
    ${libparamsSrc}
    ${CMAKE_CURRENT_BINARY_DIR}/params.cpp
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

execute_process(
    COMMAND ${ROOT_DIR}/scripts/generate_params.py --out-dir . -f ${CMAKE_CURRENT_LIST_DIR}/params.yaml
    RESULT_VARIABLE ret
)
if(NOT ret EQUAL 0)
    message( FATAL_ERROR "Params Generator has been failed. Abort.")
endif()

execute_process(
    COMMAND ${ROOT_DIR}/scripts/generate_default_params.py --out-dir ${CMAKE_CURRENT_BINARY_DIR} -f ${CMAKE_CURRENT_BINARY_DIR}/params.cpp
    --out-file-name ${LIBPARAMS_INIT_PARAMS_BASE_NAME}
    RESULT_VARIABLE ret
)
if(NOT ret EQUAL 0)
    message( FATAL_ERROR "Default Params Generator has been failed. Abort.")
endif()

include_directories(
    ${libparamsHeaders}
    ${CMAKE_CURRENT_BINARY_DIR}
)
