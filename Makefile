BUILD_DIR := build

# ----------------------------------
# CMake wrapper targets (macOS)
# ----------------------------------
all:
	cmake --preset llvm-debug
	cmake --build --preset build-debug

debug:
	cmake --preset llvm-debug -DENABLE_DEBUG_LOG=ON
	cmake --build --preset build-debug

test-lexer: all
	cmake --build $(BUILD_DIR) --target test-lexer

test: all
	cmake --build $(BUILD_DIR) --target test

clean:
	rm -rf $(BUILD_DIR) logs

.PHONY: all clean debug test test-lexer
