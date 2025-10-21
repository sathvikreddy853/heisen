BUILD_DIR := ./build
TARGET_PROGRAM := feyn

INCLUDE_DIR := ./include 
INCLUDE_SUBDIRS := $(shell find $(INCLUDE_DIR) -type d)
INCLUDE_LIST := $(addprefix -I,$(INCLUDE_SUBDIRS))

# view:
# 	@echo "$(INCLUDE_LIST)"
# 	@echo "$(INCLUDE_DIR)"
# 	@echo "$(INCLUDE_SUBDIRS)"
# 	@echo "clang++ -I./build $(INCLUDE_LIST) build/parser.tab.cpp build/lexer.yy.cpp -o ./build/feyn"

.PHONY: all yacc lex compile

all: build yacc lex compile

build:
	mkdir -p build

run: 
	$(BUILD_DIR)/$(TARGET_PROGRAM)

compile:
	clang++ -I$(BUILD_DIR) $(INCLUDE_LIST) -I$(BUILD_DIR)/parser.tab.hpp $(BUILD_DIR)/parser.tab.cpp $(BUILD_DIR)/lexer.yy.cpp -o $(BUILD_DIR)/$(TARGET_PROGRAM)

lex:
	flex -o $(BUILD_DIR)/lexer.yy.cpp src/lexer/lexer.l

yacc: 
	bison -Wcounterexamples -Wother --update --defines=$(BUILD_DIR)/parser.tab.hpp -o $(BUILD_DIR)/parser.tab.cpp src/parser/parser.y

clean:
	rm -rf $(BUILD_DIR)