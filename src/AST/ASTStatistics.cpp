#include "AST.hpp"
#include "ASTTraversal.hpp"


void ASTStatistics::analyzeNode (ASTNode* node) {
    if (!node) return;

    node->visit (this);
}

void ASTStatistics::printStats () const {
    std::cout << std::setw (25) << std::left
              << "Total Nodes:" << stats.totalNodes << std::endl;
    std::cout << std::setw (25) << std::left
              << "Expressions:" << stats.expressions << std::endl;
    std::cout << std::setw (25) << std::left
              << "Statements:" << stats.statements << std::endl;
    std::cout << std::setw (25) << std::left
              << "Declarations:" << stats.declarations << std::endl;
    std::cout << std::setw (25) << std::left << "Functions:" << stats.functions
              << std::endl;
    std::cout << std::setw (25) << std::left << "Loops:" << stats.loops << std::endl;
    std::cout << std::setw (25) << std::left
              << "Quantum Operations:" << stats.quantumOps << std::endl;
    std::cout << std::setw (25) << std::left
              << "  Gate Applications:" << stats.gateApplications << std::endl;
    std::cout << std::setw (25) << std::left
              << "  Measurements:" << stats.measurements << std::endl;
}
