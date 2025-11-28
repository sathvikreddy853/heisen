BUILD_DIR := ./build
TARGET_PROGRAM := heisen

INCLUDE_DIR := ./include
INCLUDE_SUBDIRS := $(shell find $(INCLUDE_DIR) -type d)
INCLUDE_LIST := $(addprefix -I,$(INCLUDE_SUBDIRS))

SRC_DIR := ./src
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.cpp' ! -path '*/CodeGen/*')

CXX := clang++
CXXFLAGS := -std=c++17 $(INCLUDE_LIST) -I$(BUILD_DIR)

.PHONY: all build yacc lex compile run clean

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

run:
	@printf "[RUN] Running heisen...\n"
	@$(BUILD_DIR)/$(TARGET_PROGRAM)

clean:
	@printf "[CLEAN] Removing build directory\n"
	@rm -rf $(BUILD_DIR)
