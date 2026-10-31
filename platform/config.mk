#
# Platform-specific settings
#

PLATFORM_SOURCE_DIRS := platform/src
PLATFORM_INCLUDE_DIRS := platform/include

# Flash via Blue-Prog.
.PHONY += Blue-Prog
Blue-Prog: $(BUILD_DIR)/$(PROJECT_NAME).hex
	python3 $(ARCH_UPLOADER_DIR)/mik32_upload.py $(BUILD_DIR)/$(PROJECT_NAME).hex \
		--run-openocd --openocd-exec=openocd \
		--openocd-interface=arch/tools/openocd-scripts/interface/ftdi/Blue-Prog.cfg \
		--openocd-target=$(ARCH_UPLOADER_DIR)/openocd-scripts/target/mik32.cfg \
		--adapter-speed=4000

# Flash via J-Link.
.PHONY += jlink
jlink: $(BUILD_DIR)/$(PROJECT_NAME).hex
	python3 $(ARCH_UPLOADER_DIR)/mik32_upload.py $(BUILD_DIR)/$(PROJECT_NAME).hex \
		--run-openocd --openocd-exec=openocd \
		--openocd-interface=arch/tools/openocd-scripts/interface/ftdi/jlink.cfg \
		--openocd-target=$(ARCH_UPLOADER_DIR)/openocd-scripts/target/mik32.cfg \
		--adapter-speed=2000
