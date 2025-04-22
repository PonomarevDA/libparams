# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.15.3)
project(tests CXX C)

set(UNIT_TESTS_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH TESTS_DIR)
cmake_path(GET TESTS_DIR PARENT_PATH ROOT_DIR)

# Check if the COVERAGE flag is set
option(COVERAGE "Enable coverage flags" OFF)
option(TEST_NUM "Enable test num" OFF)
if(COVERAGE EQUAL 1)
    message(STATUS "Code coverage enabled!")
    add_compile_options(-g -O0 --coverage )
    add_link_options(--coverage)
endif()

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

# include_directories(.)
set(LIBPARAMS_PLATFORM ubuntu)
set(LIBPARAMS_GENERATED_PARAMS_DIR ${TESTS_DIR}/params/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../.. ${CMAKE_BINARY_DIR}/libparams)

function(gen_test app_name test_file)
    add_executable(${app_name}
        ${test_file}
        ${libparamsSrc}
    )

    message(STATUS "${app_name} params dir ${UNIT_TESTS_DIR}")
    target_link_libraries(${app_name} gtest libparams::libparams)
endfunction()

gen_test(flash_driver     ${UNIT_TESTS_DIR}/test_flash_driver.cpp)
gen_test(rom              ${UNIT_TESTS_DIR}/test_rom.cpp)
gen_test(storage          ${UNIT_TESTS_DIR}/test_storage.cpp)
gen_test(yaml_parameters  ${UNIT_TESTS_DIR}/test_yaml_parameters.cpp)
