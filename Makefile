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
	./scripts/generate_params.py --out-dir ${BUILD_PARAMS_GENERATOR} --language c -f \
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

BUILD_GENERATOR_SMOKE=${BUILD_PATH}/tests/params_generator/smoke
generator_smoke:
	cmake -S tests/params_generator/smoke -B ${BUILD_GENERATOR_SMOKE}
	cmake --build ${BUILD_GENERATOR_SMOKE}
	cd ${BUILD_GENERATOR_SMOKE} && ctest --output-on-failure

stm32f103: clean
	cmake -S tests/platform_specific/stm32f103 -B build/tests/platform_specific/stm32f103 \
		-DCMAKE_TOOLCHAIN_FILE=${ROOT_DIR}/tests/platform_specific/cmake/arm-none-eabi-toolchain.cmake
	cmake --build build/tests/platform_specific/stm32f103

stm32g0b1: clean
	cmake -S tests/platform_specific/stm32g0b1 -B build/tests/platform_specific/stm32g0b1 \
		-DCMAKE_TOOLCHAIN_FILE=${ROOT_DIR}/tests/platform_specific/cmake/arm-none-eabi-toolchain.cmake
	cmake --build build/tests/platform_specific/stm32g0b1

stm32h753xx: clean
	cmake -S tests/platform_specific/stm32h753xx -B build/tests/platform_specific/stm32h753xx \
		-DCMAKE_TOOLCHAIN_FILE=${ROOT_DIR}/tests/platform_specific/cmake/arm-none-eabi-toolchain.cmake
	cmake --build build/tests/platform_specific/stm32h753xx

UBUNTU_BUILD_DIR=${ROOT_DIR}/build/tests/platform_specific/ubuntu
UBUNTU_CMAKE_DIR=${ROOT_DIR}/tests/platform_specific/ubuntu
ubuntu: clean
	mkdir -p ${UBUNTU_BUILD_DIR}
	cmake -S ${UBUNTU_CMAKE_DIR} -B ${UBUNTU_BUILD_DIR}
	cmake --build ${UBUNTU_BUILD_DIR}
	${UBUNTU_BUILD_DIR}/application

# The same application through the legacy include(libparams.cmake), until it is removed.
UBUNTU_LEGACY_BUILD_DIR=${ROOT_DIR}/build/tests/platform_specific/ubuntu_legacy
ubuntu_legacy: clean
	mkdir -p ${UBUNTU_LEGACY_BUILD_DIR}
	cmake -S ${UBUNTU_CMAKE_DIR} -B ${UBUNTU_LEGACY_BUILD_DIR} -DLIBPARAMS_TEST_LEGACY_BUILD=ON
	cmake --build ${UBUNTU_LEGACY_BUILD_DIR}
	${UBUNTU_LEGACY_BUILD_DIR}/application

# The core alone, as a downstream project gets it with add_subdirectory().
library: clean
	cmake -S ${ROOT_DIR} -B ${BUILD_PATH}/library
	cmake --build ${BUILD_PATH}/library


FOOTPRINT_DIR=${BUILD_PATH}/footprint
footprint:
	mkdir -p ${FOOTPRINT_DIR}
	@for cpu in cortex-m0plus cortex-m3 cortex-m7; do \
		for src in rom storage; do \
			arm-none-eabi-gcc -std=c11 -mcpu=$$cpu -mthumb -Os -ffunction-sections -fdata-sections \
				-Iinclude/libparams -c src/$$src.c -o ${FOOTPRINT_DIR}/$$src-$$cpu.o || exit 1; \
		done; \
		echo "$$cpu:"; \
		arm-none-eabi-size -t ${FOOTPRINT_DIR}/rom-$$cpu.o ${FOOTPRINT_DIR}/storage-$$cpu.o; \
	done


code_style: astyle cpplint crlf cppcheck pylint
astyle:
	./scripts/code_style/check_astyle.py src/ include/ platform_specific/ --astylerc scripts/code_style/astylerc
cpplint:
	cpplint src/*.c include/libparams/* platform_specific/*/*.c platform_specific/*/*.h
crlf:
	./scripts/code_style/check_crlf.py
cppcheck:
	./scripts/code_style/cppcheck.sh
pylint:
	pylint --persistent=n scripts/code_style

clean:
	rm -rf ${BUILD_PATH}
