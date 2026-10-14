#
# Architecture-specific settings
#

CROSS_COMPILE := ./arch/tools/xpack-riscv-none-elf-gcc/bin/riscv-none-elf-

LDSCRIPTS := arch/lib/shared/ldscripts

# Select the memory layout; defaults to SPIFI.
MEMORY_LAYOUT ?= spifi
LINKER_SCRIPT := $(LDSCRIPTS)/$(MEMORY_LAYOUT).ld

ARCH_SOURCE_DIRS := arch/src \
	arch/lib/hal/core/Source \
	arch/lib/hal/peripherals/Source \
	arch/lib/hal/utilities/Source \
	arch/lib/shared/libs \
	arch/lib/shared/runtime
ARCH_INCLUDE_DIRS := arch/include \
	arch/lib/hal/core/Include \
	arch/lib/hal/peripherals/Include \
	arch/lib/hal/utilities/Include \
	arch/lib/shared/include \
	arch/lib/shared/libs \
	arch/lib/shared/periphery
ARCH_UPLOADER_DIR := arch/tools/uploader

DEFINES += MIK32V2
ARCH_FLAGS := -specs=nano.specs -nostartfiles \
	-march=rv32imc_zicsr_zifencei -mabi=ilp32
