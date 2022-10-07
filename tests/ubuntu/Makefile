REPOSITORY_PATH=../..
BUILD_PATH=../../build/tests/ubuntu

INC =
SRC =

# libparams
INC+=-I ${REPOSITORY_PATH}/libparams/
SRC+=$(wildcard ${REPOSITORY_PATH}/libparams/*.c)

# platform_specific
INC+=-I ${REPOSITORY_PATH}/platform_specific/ubuntu/
SRC+=$(wildcard ${REPOSITORY_PATH}/platform_specific/ubuntu/*.c)

# user params
SRC+=${REPOSITORY_PATH}/tests/params.c

# target
FLASH_DRIVER_TEST_SRC=test_flash_driver.cpp

COMMON_LIBS=-lpthread -pthread -lgtest
CPPFLAGS = -Wall -std=c++11
COMPILE = g++ $(CPPFLAGS) $(INC) -o shell


all:
	$(COMPILE) $(SRC) $(FLASH_DRIVER_TEST_SRC) $(COMMON_LIBS)
	./shell

clean:
	rm -rf ${BUILD_PATH}
