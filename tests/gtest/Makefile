INC=-I ../../libparams/   	\
    -I ../stm32f103_build     			\
	-I ../../platform_specific/stm32f103/ \
    -I ./

SRC_COMMON=../../platform_specific/stm32f103/flash_driver.c \
		   ../../libparams/rom.c \
		   ../../libparams/storage.c \
		   ../params.c \
		   ../stm32f103_build/main.c
SRC_FLASH_TEST=test_stm32f1xx.cpp

COMMON_LIBS=-lpthread -pthread -lgtest

CPPFLAGS = -Wall -std=c++11 -DCPP_TEST -DSTM32F103xB
COMPILE = g++ $(CPPFLAGS) $(INC) -o shell

all:
	$(COMPILE) $(SRC_COMMON) $(SRC_FLASH_TEST)       $(COMMON_LIBS)
	./shell
