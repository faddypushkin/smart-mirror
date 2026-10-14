PROJECT_NAME := smart-mirror
BUILD_DIR    := build

include platform/config.mk
include arch/config.mk

CC	= $(CROSS_COMPILE)gcc
AS	= $(CROSS_COMPILE)gcc
LD	= $(CROSS_COMPILE)gcc
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
SIZE	= $(CROSS_COMPILE)size

# Source and header search paths.
SOURCE_DIRS	:= src $(ARCH_SOURCE_DIRS) $(PLATFORM_SOURCE_DIRS)
INCLUDE_DIRS	:= include $(ARCH_INCLUDE_DIRS) $(PLATFORM_INCLUDE_DIRS)

# Find source files, excluding directories named tools.
SOURCES		:= $(shell find -L $(SOURCE_DIRS) -type d -name tools -prune -o \
						  -type f 2>/dev/null)
C_SOURCES	:= $(filter %.c,$(SOURCES))
ASM_SOURCES	:= $(filter %.S,$(SOURCES))

# Map source files to their object files under BUILD_DIR.
C_OBJECTS	:= $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS	:= $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
OBJECTS		:= $(C_OBJECTS) $(ASM_OBJECTS)

# Compiler and linker options.
INCLUDE_FLAGS	:= $(patsubst %,-I%,$(INCLUDE_DIRS))
DEFINE_FLAGS	:= $(patsubst %,-D%,$(DEFINES))
ASFLAGS := $(DEFINE_FLAGS) $(ARCH_FLAGS) $(INCLUDE_FLAGS)
CFLAGS  := $(DEFINE_FLAGS) $(ARCH_FLAGS) $(INCLUDE_FLAGS) \
	-Os -flto \
	-MMD -MP \
	-fplan9-extensions \
	-fstrict-volatile-bitfields \
	-fno-strict-aliasing \
	-fno-common \
	-ffunction-sections -fdata-sections \
	-ffreestanding
LDFLAGS := $(ARCH_FLAGS) \
	-Os -flto \
	-Wl,-L,$(LDSCRIPTS) \
	-Wl,-T,$(LINKER_SCRIPT) \
	-Wl,-Map,$(BUILD_DIR)/$(PROJECT_NAME).map \
	-Wl,--gc-sections

# Use all online processors for parallel builds; fall back to one job if getconf fails.
MAKEFLAGS += -j$(shell getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)

# Default action: build all.
.PHONY: all clean
.DEFAULT_GOAL := all

all: $(BUILD_DIR)/$(PROJECT_NAME).hex $(BUILD_DIR)/$(PROJECT_NAME).bin

$(BUILD_DIR)/$(PROJECT_NAME).elf: $(sort $(OBJECTS))
	@mkdir -p $(@D)
	@echo "  ELF	$@"
	@$(LD) $^ $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(PROJECT_NAME).hex: $(BUILD_DIR)/$(PROJECT_NAME).elf
	@echo "  HEX	$@"
	@$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/$(PROJECT_NAME).bin: $(BUILD_DIR)/$(PROJECT_NAME).elf
	@echo "  BIN	$@"
	@$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo "  CC	$<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(@D)
	@echo "  AS	$<"
	@$(AS) $(ASFLAGS) -D__ASSEMBLY__=1 -c $< -o $@

# Include compiler-generated header dependencies when available.
-include $(OBJECTS:.o=.d)

clean:
	rm -rf $(BUILD_DIR)
