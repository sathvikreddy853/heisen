#include <symbol_table.hpp>
#include <semantic_analyzer.hpp>
#include <iostream>



/* these are not semantic checks but for debugging  all literals  remove after testing */
void SemanticAnalyzer::visit(IntLiteralExpr* node) {
    std::cout << "In the IntLiteral " << node->getValue() << "\n";
}
void SemanticAnalyzer::visit(FloatLiteralExpr* node) {
   std::cout << "Visiting Float: " << node->getValue() << "\n";
}
/* not declared in grammar but there in ast (bool literal) */
void SemanticAnalyzer::visit(BoolLiteralExpr* node) {
    std::cout << "Visiting Bool: " << node->getValue() << "\n";
  
}
void SemanticAnalyzer::visit(StringLiteralExpr* node) {
    std::cout << "Visiting String: " << node->getValue() << "\n";
}
