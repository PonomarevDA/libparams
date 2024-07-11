# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

BUILD_PATH=build
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_PARAMS_GENERATOR=${BUILD_PATH}/tests/params_generator

all: coverage

create_build_dir:
	mkdir -p ${BUILD_PATH}

coverage:
	cd tests/unit_tests && $(MAKE) -s coverage

unit_tests: clean
	cd tests/unit_tests && $(MAKE) -s unit_tests && cd ..


BUILD_C_GENERATOR=${BUILD_PATH}/tests/params_generator/c
c_generator: clean
	mkdir -p ${BUILD_C_GENERATOR}
	./scripts/params_generate_array_from_yaml.py ${BUILD_PARAMS_GENERATOR} c params \
		tests/params_generator/c/baro.yaml \
		tests/params_generator/c/mag.yaml
	cp ${BUILD_PARAMS_GENERATOR}/params.h ${BUILD_PARAMS_GENERATOR}/params.hpp
	cmake -S tests/params_generator -B ${BUILD_C_GENERATOR}
	cd tests/params_generator && cmake --build ../../${BUILD_C_GENERATOR}
	cd ${BUILD_C_GENERATOR} && ctest --verbose


BUILD_CPP_GENERATOR=${BUILD_PATH}/tests/params_generator/cpp
cpp_generator: clean
	mkdir -p ${BUILD_CPP_GENERATOR}
	./scripts/generate_params.py --out-dir ${BUILD_PARAMS_GENERATOR} -f \
		tests/params_generator/cpp/baro.yaml \
		tests/params_generator/cpp/mag.yaml
	cmake -S tests/params_generator -B ${BUILD_CPP_GENERATOR}
	cd tests/params_generator && cmake --build ../../${BUILD_CPP_GENERATOR}
	cd ${BUILD_CPP_GENERATOR} && ctest --verbose

stm32f103: clean
	mkdir -p build/tests/platform_specific/stm32f103
	cd tests/platform_specific/stm32f103 && make

stm32g0b1: clean
	mkdir -p build/tests/platform_specific/stm32g0b1
	cd tests/platform_specific/stm32g0b1 && make

UBUNTU_BUILD_DIR=${ROOT_DIR}/build/tests/platform_specific/ubuntu
UBUNTU_CMAKE_DIR=${ROOT_DIR}/tests/platform_specific/ubuntu
ubuntu: clean
	mkdir -p ${UBUNTU_BUILD_DIR}
	cd ${UBUNTU_BUILD_DIR} && cmake -S ${UBUNTU_CMAKE_DIR} -B . && $(MAKE) && ./application

cppcheck:
	./scripts/cppcheck.sh

clean:
	rm -rf ${BUILD_PATH}
