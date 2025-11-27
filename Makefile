# ================================================================
# Project Settings
# ================================================================
BUILD_DIR := ./build
TARGET_PROGRAM := heisen

INCLUDE_DIR := ./include
INCLUDE_SUBDIRS := $(shell find $(INCLUDE_DIR) -type d)
INCLUDE_LIST := $(addprefix -I,$(INCLUDE_SUBDIRS))

SRC_DIR := ./src
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
# If you have a main.cpp that builds the compiler, exclude it when building examples
MAIN_SRC := $(SRC_DIR)/main.cpp
SRC_NO_MAIN := $(filter-out $(MAIN_SRC),$(SRC_FILES))

# Examples directory & sources (place your example_usage.cpp here)
EXAMPLES_DIR := ./examples
EXAMPLE_SRCS := $(wildcard $(EXAMPLES_DIR)/*.cpp)
# Example binaries: examples/foo.cpp -> build/foo
EXAMPLE_BINS := $(patsubst $(EXAMPLES_DIR)/%.cpp,$(BUILD_DIR)/%,$(EXAMPLE_SRCS))

LLVM_PREFIX := $(shell brew --prefix llvm 2>/dev/null || echo /usr)
LLVM_CXX    := $(LLVM_PREFIX)/bin/clang++
LLVM_CFLAGS := -I$(LLVM_PREFIX)/include
LLVM_LDFLAGS := -L$(LLVM_PREFIX)/lib -lLLVM

ifeq ($(wildcard $(LLVM_CXX)),)
  CXX := clang++
else
  CXX := $(LLVM_CXX)
endif

CXXFLAGS := -std=c++26 $(LLVM_CFLAGS) $(INCLUDE_LIST) -I$(BUILD_DIR)
LDFLAGS  := $(LLVM_LDFLAGS)

.PHONY: all build yacc lex compile run codegen examples clean

all: build yacc lex compile

build:
	@printf "[MKDIR] Create build directory\n"
	@mkdir -p $(BUILD_DIR)

lex:
	@printf "[BUILD] Constructing Lexer (Flex)\n"
	@flex -o $(BUILD_DIR)/Lexer.yy.cpp src/Lex/Lexer.l

yacc:
	@printf "[BUILD] Constructing Parser (Bison)\n"
	@bison --defines=$(BUILD_DIR)/Parser.tab.hpp -o $(BUILD_DIR)/Parser.tab.cpp src/Parse/Parser.y

grammar:
	@printf "[BUILD] Constructing Grammar (Bison)\n"
	@bison -Wcounterexamples -Wother --update --defines=$(BUILD_DIR)/Parser.tab.hpp -o $(BUILD_DIR)/Parser.tab.cpp src/Parse/Grammar.y

compile:
	@printf "[COMPILE] Building executable: $(TARGET_PROGRAM)\n"
	@$(CXX) $(CXXFLAGS) $(BUILD_DIR)/Parser.tab.cpp $(BUILD_DIR)/Lexer.yy.cpp $(SRC_FILES) $(LDFLAGS) -o $(BUILD_DIR)/$(TARGET_PROGRAM)

codegen: examples

examples: $(EXAMPLE_BINS)
	@printf "[CODEGEN] Built %d example(s) in %s\n" $(words $(EXAMPLE_BINS)) $(BUILD_DIR)

$(BUILD_DIR)/%: $(EXAMPLES_DIR)/%.cpp | build
	@printf "[CODEGEN] Building example: $@\n"
	@$(CXX) $(CXXFLAGS) $(BUILD_DIR)/Parser.tab.cpp $(BUILD_DIR)/Lexer.yy.cpp $(SRC_NO_MAIN) $< $(LDFLAGS) -o $@

run:
	@printf "[RUN] Running heisen...\n"
	@$(BUILD_DIR)/$(TARGET_PROGRAM)

clean:
	@printf "[CLEAN] Removing build directory\n"
	@rm -rf $(BUILD_DIR)
