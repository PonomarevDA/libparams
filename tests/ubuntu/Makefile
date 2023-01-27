REPOSITORY_PATH=../..
BUILD_PATH=.
EXECUTABLE_PATH=${BUILD_PATH}/shell

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
COMPILE = g++ $(CPPFLAGS) $(INC) -o ${EXECUTABLE_PATH}

all: flash_driver rom storage

create_build_dir:
	mkdir -p ${BUILD_PATH}

flash_driver: create_build_dir
	$(COMPILE) $(SRC) test_flash_driver.cpp $(COMMON_LIBS)
	./${EXECUTABLE_PATH}

rom: create_build_dir
	$(COMPILE) $(SRC) test_rom.cpp $(COMMON_LIBS)
	./${EXECUTABLE_PATH}

storage: create_build_dir
	$(COMPILE) $(SRC) test_storage.cpp $(COMMON_LIBS)
	./${EXECUTABLE_PATH}

clean:
	rm -rf ${BUILD_PATH}
