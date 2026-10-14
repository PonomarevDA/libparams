# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.10)
project(tests VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH TESTS_DIR)
cmake_path(GET TESTS_DIR PARENT_PATH LIBPARAMS_ROOT_DIR)
set(PARAMS_PATH ${LIBPARAMS_ROOT_DIR}/build/tests/params_generator)

set(LIBPARAMS_PLATFORM ubuntu)
add_subdirectory(${LIBPARAMS_ROOT_DIR} libparams)
include(${LIBPARAMS_ROOT_DIR}/cmake/platform.cmake)
target_compile_definitions(libparams PRIVATE LIBPARAMS_PARAMS_DIR="${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${PARAMS_PATH})

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

FILE(GLOB GENERATED_PARAMS ${PARAMS_PATH}/*.c*)

add_executable(${PROJECT_NAME} test.cpp ${GENERATED_PARAMS} ${LIBPARAMS_PLATFORM_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBPARAMS_PLATFORM_HEADERS})
target_link_libraries(${PROJECT_NAME} libparams::libparams GTest::GTest GTest::Main)
gtest_discover_tests(${PROJECT_NAME})
