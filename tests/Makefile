TARGET = libparams
DEBUG = 1
OPT = -Og
BUILD_DIR = build

######################################
# source
######################################
C_SOURCES =  \
../flash.c \
../flash_driver.c \
../storage.c \
main.c


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

# C includes
C_INCLUDES =  	\
-I../			\
-I./

# compile gcc flags
CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -Wcast-align -MMD -MP -MF"$(@:%.o=%.d)"
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


#######################################
# LDFLAGS
#######################################

# libraries
LIBS = -lc -lm -lnosys
LDFLAGS = $(MCU) -specs=nano.specs -specs=nosys.specs -Wall $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

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
