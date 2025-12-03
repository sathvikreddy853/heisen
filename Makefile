CXX := clang++
CXXFLAGS := -std=c++23 -Wall -Wextra -Iinclude
SRC_DIR := src
BUILD_DIR := build
TARGET := heisen

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@printf "[COMPILE] Creating target program $(TARGET)\n"
	@$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@printf "[COMPILE] Compiling $<\n"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR):
	@printf "[MKDIR] Creating directory $(BUILD_DIR)\n"
	@mkdir -p $@

clean:
	@printf "[CLEAN] Removing all the build files\n"
	@rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
