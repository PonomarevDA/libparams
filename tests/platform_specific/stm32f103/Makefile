TARGET = libparams
DEBUG = 1
OPT = -Og
ROOT_DIR = ../../..
BUILD_DIR = ${ROOT_DIR}/build/tests/stm32f103_build

######################################
# source
######################################
C_SOURCES =                     	\
${ROOT_DIR}/libparams/rom.c             	\
${ROOT_DIR}/platform_specific/stm32f103/flash_driver.c       \
${ROOT_DIR}/libparams/storage.c           \
../../params/params.c                         \
main.c               				\
application.c

# C includes
C_INCLUDES =    \
-I${ROOT_DIR}/libparams/	\
-I${ROOT_DIR}/platform_specific/stm32f103/ \
-I./ \
-I../../ 


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
SZ = $(PREFIX)size
HEX = $(PREFIX)objcopy -O ihex
BIN = $(PREFIX)objcopy -O binary -S
 
#######################################
# CFLAGS
#######################################
CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# C defines
C_DEFS = -DUSE_HAL_DRIVER -DSTM32F103xB

# compile gcc flags
CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -Wpedantic -Werror -fdata-sections -ffunction-sections -Wcast-align -MMD -MP -MF"$(@:%.o=%.d)"
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


#######################################
# LDFLAGS
#######################################

# libraries
LIBS = -lc -lm -lnosys
LDFLAGS = $(MCU) -specs=nano.specs -specs=nosys.specs -Wall -Wpedantic -Werror $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
