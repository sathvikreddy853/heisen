BUILD_DIR := ./build
TARGET_PROGRAM := heisen

INCLUDE_DIR := ./include 
INCLUDE_SUBDIRS := $(shell find $(INCLUDE_DIR) -type d)
INCLUDE_LIST := $(addprefix -I,$(INCLUDE_SUBDIRS))

SRC_DIR := ./src
SRC_SUBDIRS := $(shell find $(SRC_DIR) -type d)
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.cpp')


SRC_NO_MAIN := $(shell find $(SRC_DIR) -type f -name '*.cpp' ! -name 'main.cpp')

# view:
# 	@echo "$(INCLUDE_LIST)"
# 	@echo "$(INCLUDE_DIR)"
# 	@echo "$(INCLUDE_SUBDIRS)"
# 	@echo "clang++ -I./build $(INCLUDE_LIST) build/Parser.tab.cpp build/Lexer.yy.cpp -o ./build/feyn"
# 	@echo "$(SRC_FILES)"

.PHONY: all yacc grammar lex compile

all: build yacc lex compile

build:
	@printf "[MKDIR] Create build directory\n"
	@mkdir -p build

run:
	@printf "[RUN] Running heisen..\n" 
	@$(BUILD_DIR)/$(TARGET_PROGRAM)

compile:
	@printf "[COMPILE] Compile to generate heisen\n"
	@clang++ -std=c++23 -I$(BUILD_DIR) $(INCLUDE_LIST) -I$(BUILD_DIR)/Parser.tab.hpp $(BUILD_DIR)/Parser.tab.cpp $(BUILD_DIR)/Lexer.yy.cpp $(SRC_FILES) -o $(BUILD_DIR)/$(TARGET_PROGRAM)

lex:
	@printf "[BUILD] Constructing Lexer files from Flex\n"
	@flex -o $(BUILD_DIR)/Lexer.yy.cpp src/Lex/Lexer.l

yacc:
	@printf "[BUILD] Constructing Parser files from Bison\n"
	@bison --defines=$(BUILD_DIR)/Parser.tab.hpp -o $(BUILD_DIR)/Parser.tab.cpp src/Parse/Parser.y
# 	bison -Wcounterexamples -Wother --update --defines=$(BUILD_DIR)/Parser.tab.hpp -o $(BUILD_DIR)/Parser.tab.cpp src/Parse/Parser.y

grammar:
	@printf "[BUILD] Constructing Parser files from Bison\n"
	@bison -Wcounterexamples -Wother --update --defines=$(BUILD_DIR)/Parser.tab.hpp -o $(BUILD_DIR)/Parser.tab.cpp src/Parse/Grammar.y
# 	bison --defines=$(BUILD_DIR)/Parser.tab.hpp -o $(BUILD_DIR)/Parser.tab.cpp src/Parse/Grammar.y
GTEST_LIBS := -lgtest -lgtest_main -pthread

#single test file
test: build yacc lex
ifndef FILE
	$(error Please specify FILE=path/to/test.cpp, e.g. make test FILE=test/Lex/test_arithmetic_ops.cpp)
endif

	@printf "[TEST] Building test: $(FILE)\n"

	clang++ -std=c++20 \
		$(INCLUDE_LIST) \
		$(TEST_INCLUDE_LIST) \
		-I$(BUILD_DIR) \
		$(BUILD_DIR)/Parser.tab.cpp \
		$(BUILD_DIR)/Lexer.yy.cpp \
		$(SRC_NO_MAIN) \
		$(FILE) \
		$(GTEST_LIBS) \
		-o $(BUILD_DIR)/single_test

	@./$(BUILD_DIR)/single_test



clean:
	@printf "[CLEAN] Cleaning up build files\n"
	@rm -rf $(BUILD_DIR)
