#pragma once
#include <string>
#include <cstdio>
#include <Parser.tab.hpp>


    int yylex();
    extern FILE* yyin;
    void yyrestart(FILE*);
    extern YYSTYPE yylval;   


inline int run_lexer(const std::string& input) {
    FILE* f = fmemopen((void*)input.c_str(), input.size(), "r");
    yyin = f;
    yyrestart(yyin);
    int tok = yylex();
    fclose(f);
    return tok;
}
