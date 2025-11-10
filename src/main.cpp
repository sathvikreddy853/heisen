#include <macros.hpp>
#include <ast.hpp>
#include <ast-print.hpp>

extern int yydebug;
extern int yyparse();
extern std::vector<ASTNode*> translationUnit;


int main() {
    yydebug = 0;
    yyparse();
    
    Heisen::printTranslationUnit(std::cout, translationUnit);

    // Clean up
    for (auto* node : translationUnit) {
        delete node;
    }
    
    return 0;
}