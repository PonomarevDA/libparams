# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

CRNT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
TESTS_DIR := $(abspath $(dir $(lastword $(CRNT_DIR))))
REPO_DIR := $(abspath $(dir $(lastword $(TESTS_DIR))))
BUILD_DIR=${REPO_DIR}/build

all: unit_tests

unit_tests: flash_driver rom storage yaml_parameters

create_build_dir:
	mkdir -p ${BUILD_DIR}

coverage: create_build_dir
	cd ${BUILD_DIR} && cmake -DCOVERAGE=1 ${CRNT_DIR} && make
	cd ${BUILD_DIR} && ./storage
	cd ${BUILD_DIR} && ./rom
	cd ${BUILD_DIR} && ./flash_driver
	cd ${BUILD_DIR} && ./yaml_parameters

	echo "\n1. Storage:"
	cd ${BUILD_DIR} && gcov ${BUILD_DIR}/CMakeFiles/storage.dir${REPO_DIR}/libparams/storage.c.gcda

	echo "\n2. ROM:"
	cd ${BUILD_DIR} && gcov ${BUILD_DIR}/CMakeFiles/rom.dir${REPO_DIR}/libparams/rom.c.gcda

	echo "\n3. Ubuntu flash driver:"
	cd ${BUILD_DIR} && gcov ${BUILD_DIR}/CMakeFiles/flash_driver.dir${REPO_DIR}/platform_specific/ubuntu/flash_driver.cpp.gcda

	echo "\n4. Yaml Parameters:"
	cd ${BUILD_DIR} && gcov ${BUILD_DIR}/CMakeFiles/yaml_parameters.dir${REPO_DIR}/platform_specific/ubuntu/YamlParameters.cpp.gcda

flash_driver: create_build_dir
	cd ${BUILD_DIR} && cmake ${CRNT_DIR} && make && ./flash_driver

rom: create_build_dir
	cd ${BUILD_DIR} && cmake ${CRNT_DIR} && make && ./rom

storage: create_build_dir
	cd ${BUILD_DIR} && cmake ${CRNT_DIR} && make && ./storage

yaml_parameters: create_build_dir
	cd ${BUILD_DIR} && cmake ${CRNT_DIR} && make && ./yaml_parameters
