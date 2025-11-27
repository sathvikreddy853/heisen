#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <Parser.tab.hpp>
#include <AST.hpp>
#include <SemanticAnalyzer.hpp>


extern FILE* yyin;
extern void yyrestart(FILE*);
extern int yyparse();
extern std::vector<ASTNode*> translationUnit;


inline bool parseSourceForTests(const std::string& code) {
    translationUnit.clear();

    FILE* f = fmemopen((void*)code.c_str(), code.size(), "r");
    if (!f) {
        std::cerr << "[TestS] fmemopen() failed\n";
        return false;
    }

    yyin = f;
    yyrestart(f);
    int parseResult = yyparse(); 
    fclose(f);

    return parseResult == 0;
}

inline bool sema_ok(const std::string& code) {
    if (!parseSourceForTests(code)) return false; // parse failed => treat as not ok

    Heisen::SemanticAnalyzer analyzer;
    bool success = analyzer.analyze(translationUnit);

    for (auto* n : translationUnit) delete n;
    translationUnit.clear();
    return success && !analyzer.hasErrors();
}

inline bool sema_fail(const std::string& code) {

    if (!parseSourceForTests(code)) return true;

    Heisen::SemanticAnalyzer analyzer;
    bool success = analyzer.analyze(translationUnit);
    bool hasErr = analyzer.hasErrors();

    for (auto* n : translationUnit) delete n;
    translationUnit.clear();

    return !success || hasErr;
}

inline bool sema_fail_contains(const std::string& code, const std::string& substr) {
    if (!parseSourceForTests(code)) return true;

    Heisen::SemanticAnalyzer analyzer;
    analyzer.analyze(translationUnit);
    bool found = false;
    for (const auto& e : analyzer.getErrors()) {
        if (e.message.find(substr) != std::string::npos) { found = true; break; }
    }

    for (auto* n : translationUnit) delete n;
    translationUnit.clear();

    return found;
}
