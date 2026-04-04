BUILD_DIR := build

ifneq ($(filter debug,$(MAKECMDGOALS)),)
TEST_PREREQS := build
else
TEST_PREREQS := configure build
endif

# ----------------------------------
# CMake wrapper targets (macOS)
# ----------------------------------
all: configure build

configure:
	cmake --preset llvm-debug

build:
	cmake --build --preset build-debug

debug: configure-debug build

configure-debug:
	cmake --preset llvm-debug -DENABLE_DEBUG_LOG=ON

test-lexer: $(TEST_PREREQS)
	cmake --build $(BUILD_DIR) --target test-lexer

test-parser: $(TEST_PREREQS)
	cmake --build $(BUILD_DIR) --target test-parser

test: $(TEST_PREREQS)
	cmake --build $(BUILD_DIR) --target test

clean:
	rm -rf $(BUILD_DIR) logs

.PHONY: all configure configure-debug build clean debug test test-lexer test-parser
