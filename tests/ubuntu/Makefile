CRNT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
TESTS_DIR := $(abspath $(dir $(lastword $(CRNT_DIR))))
REPOSITORY_DIR := $(abspath $(dir $(lastword $(TESTS_DIR))))
BUILD_DIR=${REPOSITORY_DIR}/build
EXECUTABLE_PATH=${BUILD_DIR}/shell
TEST_DIR=${CRNT_DIR}

INC =
SRC =

# libparams
INC+=-I ${REPOSITORY_DIR}/libparams/
SRC+=$(wildcard ${REPOSITORY_DIR}/libparams/*.c)

# platform_specific
INC+=-I ${REPOSITORY_DIR}/platform_specific/ubuntu/
SRC+=$(wildcard ${REPOSITORY_DIR}/platform_specific/ubuntu/*.cpp)

# user params
SRC+=${REPOSITORY_DIR}/tests/params.c

COMMON_LIBS=-lpthread -pthread -lgtest
CPPFLAGS = -Wall -Wpedantic -Werror -std=c++11
COMPILE = g++ $(CPPFLAGS) $(INC) -o ${EXECUTABLE_PATH}

all: unit_tests

unit_tests: flash_driver rom storage

create_build_dir:
	mkdir -p ${BUILD_DIR}

coverage: create_build_dir
	$(COMPILE) -fprofile-arcs -ftest-coverage $(SRC) $(TEST_DIR)/test_flash_driver.cpp $(COMMON_LIBS)
	${EXECUTABLE_PATH}

	$(COMPILE) -fprofile-arcs -ftest-coverage $(SRC) $(TEST_DIR)/test_rom.cpp $(COMMON_LIBS)
	${EXECUTABLE_PATH}

	$(COMPILE) -fprofile-arcs -ftest-coverage $(SRC) $(TEST_DIR)/test_storage.cpp $(COMMON_LIBS)
	${EXECUTABLE_PATH}

flash_driver: create_build_dir
	$(COMPILE) $(SRC) $(TEST_DIR)/test_flash_driver.cpp $(COMMON_LIBS)
	${EXECUTABLE_PATH}

rom: create_build_dir
	$(COMPILE) $(SRC) $(TEST_DIR)/test_rom.cpp $(COMMON_LIBS)
	${EXECUTABLE_PATH}

storage: create_build_dir
	$(COMPILE) $(SRC) $(TEST_DIR)/test_storage.cpp $(COMMON_LIBS)
	${EXECUTABLE_PATH}

# clean:
# 	rm -rf ${BUILD_DIR}
