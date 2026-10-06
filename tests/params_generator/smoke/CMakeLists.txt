# Copyright (c) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.22)
project(libparams_generator_smoke C CXX)
set(CMAKE_CXX_STANDARD 17)

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH GENERATOR_TESTS_DIR)
cmake_path(GET GENERATOR_TESTS_DIR PARENT_PATH TESTS_DIR)
cmake_path(GET TESTS_DIR PARENT_PATH ROOT_DIR)

set(LIBPARAMS_PLATFORM ubuntu)
include(${ROOT_DIR}/libparams.cmake)
enable_testing()

# YAML -> generated tables and SITL default values -> load, save and load again.
function(add_smoke_test target language main source_ext)
  set(out_dir ${CMAKE_CURRENT_BINARY_DIR}/${target}_generated)
  libparams_generate_params(
    OUT_DIR ${out_dir}
    LANGUAGE ${language}
    YAML params.yaml
    DEFAULT_PARAMS_DIR ${out_dir}/params
    DOCS ${out_dir}/params.md
  )
  add_executable(${target} ${main} ${out_dir}/params.${source_ext} ${libparamsSrc})
  target_include_directories(${target} PRIVATE ${libparamsHeaders} ${out_dir})
  target_compile_definitions(${target} PRIVATE LIBPARAMS_PARAMS_DIR="${out_dir}/params")
  add_test(NAME ${target} COMMAND ${target})
endfunction()

add_smoke_test(smoke_c c main.c c)
add_smoke_test(smoke_cpp c++ main.cpp cpp)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_test(NAME smoke_invalid_yaml
  COMMAND ${Python3_EXECUTABLE} ${ROOT_DIR}/scripts/generate_params.py
          --out-dir ${CMAKE_CURRENT_BINARY_DIR}/invalid
          -f ${CMAKE_CURRENT_LIST_DIR}/invalid.yaml
)
set_tests_properties(smoke_invalid_yaml PROPERTIES
  PASS_REGULAR_EXPRESSION "invalid.yaml: node.id: missing min, max"
)
