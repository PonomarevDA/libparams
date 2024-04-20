# Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.15.3)
project(tests CXX C)

set(UNIT_TESTS_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH TESTS_DIR)
cmake_path(GET TESTS_DIR PARENT_PATH ROOT_DIR)
cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
include(${ROOT_DIR}/CMakeLists.txt)

add_definitions(-DLIBPARAMS_PARAMS_DIR="${TESTS_DIR}/params")

function(gen_test app_name test_file)
    # Create the executable target
    add_executable(${app_name}
    ${test_file}
    ${libparamsSrc})

    target_include_directories(${app_name} PUBLIC ${libparamsHeaders})

    # Conditional source file based on app_name
    message(STATUS "${app_name} params dir ${UNIT_TESTS_DIR}")
    target_sources(${app_name} PRIVATE ${TESTS_DIR}/params/params.c)
    target_include_directories(${app_name} PRIVATE ${TESTS_DIR}/params/)

    # Link libraries to the target
    target_link_libraries(${app_name} gtest)
endfunction()

gen_test(flash_driver     ${UNIT_TESTS_DIR}/test_flash_driver.cpp)
gen_test(rom              ${UNIT_TESTS_DIR}/test_rom.cpp)
gen_test(storage          ${UNIT_TESTS_DIR}/test_storage.cpp)
gen_test(yaml_parameters  ${UNIT_TESTS_DIR}/test_yaml_parameters.cpp)
