.PHONY: all yacc lex compile

all: build lex compile

build:
	mkdir -p build

run: 
	./build/feyn

compile:
	clang++ -I./build build/lexer.yy.cpp -o ./build/feyn

lex:
	flex -o build/lexer.yy.cpp src/lexer/lexer.l

yacc: 
	bison --defines=build/parser.tab.hpp -o build/parser.tab.cpp src/parser/parser.y

clean:
	rm -rf build