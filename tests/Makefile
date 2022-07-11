TARGET = libparams
DEBUG = 1
OPT = -Og
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
../flash.c \
../storage.c \
main.c


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
CPP = $(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)g++ -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
AS = $(PREFIX)g++ -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F103xB

# C includes
C_INCLUDES =  \
-I../	\
-I./

# compile gcc flags
CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif
CFLAGS += -Wcast-align
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -specs=nosys.specs -Wall $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

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
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)
