#
# Platform-specific settings
#

PLATFORM_SOURCE_DIRS := platform/src
PLATFORM_INCLUDE_DIRS := platform/include

.PHONY: flash

# Build the image before programming the target.
flash: $(BUILD_DIR)/$(PROJECT_NAME).hex
	python3 $(ARCH_UPLOADER_DIR)/mik32_upload.py $(BUILD_DIR)/$(PROJECT_NAME).hex \
		--run-openocd --openocd-exec=openocd \
		--openocd-interface=arch/tools/openocd-scripts/interface/ftdi/Blue-Prog.cfg \
		--openocd-target=$(ARCH_UPLOADER_DIR)/openocd-scripts/target/mik32.cfg \
		--adapter-speed=4000
