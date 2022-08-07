INC=-I ../../libparams/   	\
    -I ../       			\
    -I ./

SRC_COMMON=../../libparams/flash_stm32.c ../../libparams/rom.c ../../libparams/storage.c ../params.c ../stm32f1xx_hal.c
SRC_FLASH_TEST=test_stm32f1xx.cpp

COMMON_LIBS=-lpthread -pthread -lgtest

CPPFLAGS = -Wall -std=c++11 -DCPP_TEST
COMPILE = g++ $(CPPFLAGS) $(INC) -o shell

all:
	$(COMPILE) $(SRC_COMMON) $(SRC_FLASH_TEST)       $(COMMON_LIBS)
	./shell
