REPOSITORY_PATH=../..
BUILD_PATH=../../build/tests/ubuntu

INC =
SRC =

# libparams
INC+=-I ${REPOSITORY_PATH}/libparams/
SRC+=$(wildcard ${REPOSITORY_PATH}/libparams/*.c)

# platform_specific
INC+=-I ${REPOSITORY_PATH}/platform_specific/ubuntu/
SRC+=$(wildcard ${REPOSITORY_PATH}/platform_specific/ubuntu/*.cpp)

# user params
SRC+=${REPOSITORY_PATH}/tests/params.c

COMMON_LIBS=-lpthread -pthread -lgtest
CPPFLAGS = -Wall -Wpedantic -Werror -std=c++11
COMPILE = g++ $(CPPFLAGS) $(INC) -o shell

all: flash_driver rom storage

flash_driver:
	$(COMPILE) $(SRC) test_flash_driver.cpp $(COMMON_LIBS)
	./shell

rom:
	$(COMPILE) $(SRC) test_rom.cpp $(COMMON_LIBS)
	./shell

storage:
	$(COMPILE) $(SRC) test_storage.cpp $(COMMON_LIBS)
	./shell

clean:
	rm -rf ${BUILD_PATH}
