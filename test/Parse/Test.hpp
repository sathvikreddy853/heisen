#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include <Parser.tab.hpp>
#include <AST.hpp>

extern std::vector<ASTNode*> translationUnit;
extern int yyparse();
extern FILE* yyin;
extern void yyrestart(FILE*);

inline void parseSource(const std::string& code) {
    translationUnit.clear();
    FILE* f = fmemopen((void*)code.c_str(), code.size(), "r");
    yyin = f;
    yyrestart(f);
    yyparse();
    fclose(f);
}

inline Expr* parseExpr(const std::string& code) {
    parseSource(code);
    auto* stmt = dynamic_cast<ExpressionStmt*>(translationUnit[0]);
    return stmt ? stmt->getExpression() : nullptr;
}

inline Stmt* parseStmt(const std::string& code) {
    parseSource(code);
    return dynamic_cast<Stmt*>(translationUnit[0]);
}


inline Decl* parseDecl(const std::string& code) {
    parseSource(code);
    return dynamic_cast<Decl*>(translationUnit[0]);
}

inline Expr* argOf(FunctionCallExpr* call, size_t i) {
    return call->getArgs().at(i);
}

inline bool parse_string(const std::string& code) {
    translationUnit.clear();

    FILE* f = fmemopen((void*)code.c_str(), code.size(), "r");
    if (!f) return false;
    yyin = f;
    yyrestart(f);
    int result = yyparse();    
    fclose(f);
    return result == 0;
}