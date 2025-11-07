#ifndef HEISEN_SEMANTIC_ANALYZER_HPP
#define HEISEN_SEMANTIC_ANALYZER_HPP

#include <ast.hpp>
#include <ast_traversal.hpp>
#include <iostream>


class semanticAnalyzer ::public AST_Traversal {
    public:
    semanticAnalyzer ()          = default;
    virtual ~SemanticAnalyzer () = default;

    // Helper for visiting lists of nodes
    // template <typename T>
    // void visit_list(const std::vector<T*>& list) {
    //     for (T* node : list) {
    //         if (node) node->accept(this);
    //     }
    // }
    

    void visit (IntLiteralExpr * node) override;
    void visit (FloatLiteralExpr * node) override;
    void visit (BoolLiteralExpr * node) override;
    void visit (StringLiteralExpr * node) override;
    void visit (IdentifierExpr * node) override;
    void visit (BinaryOpExpr * node) override;
    void visit (UnaryOpExpr * node) override;
    void visit (FunctionCallExpr * node) override;
    void visit (IndexAccessExpr * node) override;
    void visit (SliceExpr * node) override;
    void visit (MemberAccessExpr * node) override;
    void visit (CastExpr * node) override;
    void visit (QuantumStateIdentifier * node) override;
    void visit (QuantumStateIndexAccess * node) override;
    void visit (QuantumStateList * node) override;

    // --- Statements ---
    void visit (CompoundStmt * node) override;
    void visit (DeclarationStmt * node) override;
    void visit (AssignmentStmt * node) override;
    void visit (ExpressionStmt * node) override;
    void visit (IfStmt * node) override;
    void visit (MatchStmt * node) override;
    void visit (MatchCase * node) override;
    void visit (WhileStmt * node) override;
    void visit (DoWhileStmt * node) override;
    void visit (ForStmt * node) override;
    void visit (ContinueStmt * node) override;
    void visit (BreakStmt * node) override;
    void visit (ReturnStmt * node) override;
    void visit (PrintStmt * node) override;
    void visit (ApplyGateStmt * node) override;
    void visit (MeasureStmt * node) override;
    void visit (ResetStmt * node) override;

    // --- Declarations ---
    void visit (VariableDecl * node) override;
    void visit (ParameterDecl * node) override;
    void visit (FunctionDecl * node) override;
    void visit (GateDecl * node) override;

    // --- Types ---
    void visit (BaseTypeNode * node) override;
    void visit (ArrayTypeNode * node) override;

    // --- Quantum Gate Nodes ---
    void visit (SimpleGateNode * node) override;
    void visit (ParametricGateNode * node) override;
    void visit (CompositeGateNode * node) override;
    void visit (GateCompositionNode * node) override;
}


#endif