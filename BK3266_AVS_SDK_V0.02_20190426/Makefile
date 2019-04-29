
#pre_check:
ifeq (,$(wildcard ./out/current_build.mk))
$(error please execute 'build.sh target config' or 'build.sh target' first from root directory)
endif
include ./out/current_build.mk

ifeq ($(BUILD_PLATFORM), actions)
BUILD_CMD=sh ./build/build_actions.sh
BUILD_ALL_OPS=$(BUILD_TARGET) $(BUILD_INI_FILE) normal
BUILD_BUILD_OPS=$(BUILD_TARGET) $(BUILD_INI_FILE) build
BUILD_RELEASE_OPS=$(BUILD_TARGET) $(BUILD_INI_FILE) release
BUILD_LIB_OPS=lib_linkplay linkplay_demo.ini other
BUILD_CLEAN_OPS=$(BUILD_TARGET) $(BUILD_INI_FILE) clean
BUILD_CLEAN_TARGET_OPS=$(BUILD_TARGET) $(BUILD_INI_FILE) clean_target
BUILD_CLEAN_ALL_OPS=$(BUILD_TARGET) $(BUILD_INI_FILE) clean_all
endif
#ifeq
#endif

.PHONY : all clean clean_target build
all:
	@echo "BUILD_TARGET: $(BUILD_TARGET)"
	@echo "BUILD_INI_FILE: $(BUILD_INI_FILE)"
	@echo "BUILD_PLATFORM: $(BUILD_PLATFORM)"
	@echo "BUILD_TYPE: $(BUILD_TYPE)"
	$(BUILD_CMD) $(BUILD_ALL_OPS)

build:
	$(BUILD_CMD) $(BUILD_BUILD_OPS)

release:
	$(BUILD_CMD) $(BUILD_RELEASE_OPS)

lib_linkplay:
	$(BUILD_CMD) $(BUILD_LIB_OPS)

clean:
	$(BUILD_CMD) $(BUILD_CLEAN_OPS)

clean_target:
	$(BUILD_CMD) $(BUILD_CLEAN_TARGET_OPS)

clean_all:
	$(BUILD_CMD) $(BUILD_CLEAN_ALL_OPS)


