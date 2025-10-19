BUILD_DIR := ./build
TARGET_PROGRAM := feyn
INCLUDE_DIR := ./include 
INCLUDE_SUBDIRS := $(shell find $(INCLUDE_DIR) -type d)


# view:
# 	@printf "$(INCLUDE_DIR)"
# 	@printf "$(INCLUDE_SUBDIRS)\n"
# 	@printf "clang++ -I./build -I$(INCLUDE_SUBDIRS) build/parser.tab.cpp build/lexer.yy.cpp -o ./build/feyn"

.PHONY: all yacc lex compile

all: build yacc lex compile

build:
	mkdir -p build

run: 
	$(BUILD)/$(TARGET_PROGRAM)

compile:
	clang++ -I$(BUILD_DIR) -I$(INCLUDE_SUBDIRS) build/parser.tab.hpp build/lexer.yy.cpp -o $(BUILD_DIR)/$(TARGET_PROGRAM)

lex:
	flex -o $(BUILD_DIR)/lexer.yy.cpp src/lexer/lexer.l

yacc: 
	bison --defines=$(BUILD_DIR)/parser.tab.hpp -o $(BUILD_DIR)/parser.tab.cpp src/parser/parser.y

clean:
	rm -rf $(BUILD_DIR)