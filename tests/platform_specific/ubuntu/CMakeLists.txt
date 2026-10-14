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

set(LIBPARAMS_INIT_PARAMS_BASE_NAME initial_params)
add_definitions(-DLIBPARAMS_TEMP_PARAMS_FILE_NAME="temp_params")
add_definitions(-DLIBPARAMS_INIT_PARAMS_FILE_NAME="${LIBPARAMS_INIT_PARAMS_BASE_NAME}")

# The same application is built both ways: `make ubuntu` with the libparams target,
# `make ubuntu_legacy` with the legacy include(libparams.cmake), until that one is removed.
option(LIBPARAMS_TEST_LEGACY_BUILD "Build with the legacy include(libparams.cmake)" OFF)
set(LIBPARAMS_PLATFORM ubuntu)
if(LIBPARAMS_TEST_LEGACY_BUILD)
    include(${ROOT_DIR}/libparams.cmake)
    add_definitions(-DLIBPARAMS_PARAMS_DIR="${CMAKE_CURRENT_BINARY_DIR}")
    add_executable(application
        ${libparamsSrc}
        ${CMAKE_CURRENT_BINARY_DIR}/params.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    )
    target_include_directories(application PRIVATE ${libparamsHeaders})
else()
    add_subdirectory(${ROOT_DIR} libparams)
    include(${ROOT_DIR}/cmake/platform.cmake)
    target_compile_definitions(libparams PRIVATE LIBPARAMS_PARAMS_DIR="${CMAKE_CURRENT_BINARY_DIR}")
    add_executable(application
        ${LIBPARAMS_PLATFORM_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/params.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    )
    target_include_directories(application PRIVATE ${LIBPARAMS_PLATFORM_HEADERS})
    target_link_libraries(application PRIVATE libparams::libparams)
endif()
target_include_directories(application PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

libparams_generate_params(
    OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
    YAML params.yaml
    DEFAULT_PARAMS_DIR ${CMAKE_CURRENT_BINARY_DIR}
    DEFAULT_PARAMS_NAME ${LIBPARAMS_INIT_PARAMS_BASE_NAME}
)
