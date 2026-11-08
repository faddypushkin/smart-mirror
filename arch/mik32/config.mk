#
# Architecture-specific settings
#

CROSS_COMPILE := ./arch/$(ARCH)/tools/xpack-riscv-none-elf-gcc/bin/riscv-none-elf-

LDSCRIPTS := arch/$(ARCH)/lib/shared/ldscripts

# Select the memory layout; defaults to SPIFI.
MEMORY_LAYOUT ?= spifi
LINKER_SCRIPT := $(LDSCRIPTS)/$(MEMORY_LAYOUT).ld

ARCH_SOURCE_DIRS := arch/$(ARCH)/src \
	arch/$(ARCH)/lib/hal/core/Source \
	arch/$(ARCH)/lib/hal/peripherals/Source \
	arch/$(ARCH)/lib/hal/utilities/Source \
	arch/$(ARCH)/lib/shared/libs \
	arch/$(ARCH)/lib/shared/runtime
ARCH_INCLUDE_DIRS := arch/$(ARCH)/include \
	arch/$(ARCH)/lib/hal/core/Include \
	arch/$(ARCH)/lib/hal/peripherals/Include \
	arch/$(ARCH)/lib/hal/utilities/Include \
	arch/$(ARCH)/lib/shared/include \
	arch/$(ARCH)/lib/shared/libs \
	arch/$(ARCH)/lib/shared/periphery
ARCH_UPLOADER_DIR := arch/$(ARCH)/tools/uploader

DEFINES += MIK32V2
ARCH_FLAGS := -specs=nano.specs -nostartfiles \
	-march=rv32imc_zicsr_zifencei -mabi=ilp32

# Flash via Blue-Prog.
.PHONY += Blue-Prog
Blue-Prog: $(BUILD_DIR)/$(PLAT).hex
	python3 $(ARCH_UPLOADER_DIR)/mik32_upload.py $(BUILD_DIR)/$(PLAT).hex \
		--run-openocd --openocd-exec=openocd \
		--openocd-interface=arch/$(ARCH)/tools/openocd-scripts/interface/ftdi/Blue-Prog.cfg \
		--openocd-target=$(ARCH_UPLOADER_DIR)/openocd-scripts/target/mik32.cfg \
		--adapter-speed=4000

# Flash via J-Link.
.PHONY += jlink
jlink: $(BUILD_DIR)/$(PLAT).hex
	python3 $(ARCH_UPLOADER_DIR)/mik32_upload.py $(BUILD_DIR)/$(PLAT).hex \
		--run-openocd --openocd-exec=openocd \
		--openocd-interface=arch/$(ARCH)tools/openocd-scripts/interface/ftdi/jlink.cfg \
		--openocd-target=$(ARCH_UPLOADER_DIR)/openocd-scripts/target/mik32.cfg \
		--adapter-speed=2000
