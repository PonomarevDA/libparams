# Copyright (c) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

# The portable core as the libparams::libparams target, see README "2.2. Build".
# The flash driver comes from cmake/platform.cmake and the tables from libparams_generate_params().
cmake_minimum_required(VERSION 3.22)
project(libparams LANGUAGES C)

add_library(libparams STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/rom.c
    ${CMAKE_CURRENT_LIST_DIR}/src/storage.c
)
add_library(libparams::libparams ALIAS libparams)
target_include_directories(libparams PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/libparams)
target_compile_features(libparams PUBLIC c_std_11)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/generate_params.cmake)
