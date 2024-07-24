# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.15.3)
project(tests CXX C)

set(UNIT_TESTS_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH TESTS_DIR)
cmake_path(GET TESTS_DIR PARENT_PATH ROOT_DIR)

if(COVERAGE)
    message(STATUS "Code coverage enabled!")
    add_compile_options(-g -O0 --coverage )
    add_link_options(--coverage)
endif()

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

include_directories(.)
set(LIBPARAMS_PLATFORM ubuntu)
include(${ROOT_DIR}/CMakeLists.txt)

if(YAML_PARAMETERS)
    set(LIBPARAMS_PARAMS_BASE_NAME initial_params)
    add_definitions(-DLIBPARAMS_INITIAL_PARAMS_DIR="${CMAKE_CURRENT_BINARY_DIR}")
    add_definitions(-DLIBPARAMS_INITIAL_PARAMS_FILE="${CMAKE_CURRENT_BINARY_DIR}/${LIBPARAMS_PARAMS_BASE_NAME}.yml")
    execute_process(
        COMMAND ${ROOT_DIR}/scripts/generate_random_params.py 
        --out-dir ${CMAKE_CURRENT_BINARY_DIR}
        RESULT_VARIABLE ret
    )
    execute_process(
        COMMAND ${ROOT_DIR}/scripts/generate_default_params.py --out-dir ${CMAKE_CURRENT_BINARY_DIR} -f ${CMAKE_CURRENT_BINARY_DIR}/params.cpp
        --out-file-name ${LIBPARAMS_PARAMS_BASE_NAME}
        RESULT_VARIABLE ret
    )
    if(NOT ret EQUAL 0)
        message( FATAL_ERROR "Default Params Generator has been failed. Abort.")
    endif()
endif()


function(gen_test app_name test_file)
    # Create the executable target
    add_executable(${app_name}
        ${test_file}
        ${libparamsSrc}
    )

    # Conditional source file based on app_name
    if(app_name STREQUAL "yaml_parameters")
        target_sources(${app_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/params.cpp)
        target_include_directories(${app_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    else()
        target_sources(${app_name} PRIVATE ${TESTS_DIR}/params.c)
        target_include_directories(${app_name} PRIVATE ${TESTS_DIR})
    endif()

    # Link libraries to the target
    target_link_libraries(${app_name} gtest)
endfunction()


gen_test(flash_driver     ${UNIT_TESTS_DIR}/test_flash_driver.cpp)
gen_test(rom              ${UNIT_TESTS_DIR}/test_rom.cpp)
gen_test(storage          ${UNIT_TESTS_DIR}/test_storage.cpp)
gen_test(yaml_parameters  ${UNIT_TESTS_DIR}/test_yaml_parameters.cpp)
