CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I src
LDFLAGS  := -static

SRC_DIR  := src
BUILD_DIR := build
TARGET   := $(BUILD_DIR)/minic

SRCS := $(SRC_DIR)/main.cpp \
        $(SRC_DIR)/lexer/lexer.cpp \
        $(SRC_DIR)/debug.cpp

OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# ----------------------------------
# Default target
# ----------------------------------
all: $(TARGET)

$(TARGET): $(OBJS)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ----------------------------------
# Clean build artifacts
# ----------------------------------
clean:
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	if exist logs rmdir /s /q logs

# ----------------------------------
# Debug build (with logging enabled)
# ----------------------------------
debug: CXXFLAGS += -DENABLE_DEBUG_LOG
debug: clean $(TARGET)

# ----------------------------------
# Run lexer tests only
# ----------------------------------
test-lexer: $(TARGET)
	python scripts/test_lexer.py

# ----------------------------------
# Run all tests
# ----------------------------------
test: $(TARGET)
	python scripts/run_tests.py

.PHONY: all clean debug test test-lexer
