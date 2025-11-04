#pragma once
#ifndef HEISEN_AST_HPP
#define HEISEN_AST_HPP
#include "macros.hpp"

// ASTNode (Base node for all)
//
// ├── Expr (Base for all expressions)
// │   ├── LiteralExpr (Abstract base for literals)
// │   │   ├── IntLiteralExpr (INT_LITERAL)
// │   │   ├── FloatLiteralExpr (FLOAT_LITERAL)
// │   │   ├── StringLiteralExpr (STRING_LITERAL)
// │   │   └── BoolLiteralExpr (TRUE, FALSE)
// │   ├── IdentifierExpr (IDENTIFIER)
// │   ├── BinaryOpExpr (+, -, *, /, ::, AND, OR, etc.)
// │   ├── UnaryOpExpr (+, -, !, NOT)
// │   ├── FunctionCallExpr (expr(expr_list))
// │   ├── IndexAccessExpr (expr[index_expr])
// │   ├── SliceExpr (start:stop:step)
// │   ├── MemberAccessExpr (expr.IDENTIFIER)
// │   ├── CastExpr (CAST<type>(expr))
// │   └── QuantumStateExpr (Represents 'quantum_state')
// │       ├── QStateIdentifier (IDENTIFIER)
// │       ├── QStateIndexAccess (IDENTIFIER[index_expr])
// │       └── QStateList ([quantum_state_list])
// │
// ├── Stmt (Base for all statements)
// │   ├── CompoundStmt ({ statement_list })
// │   ├── DeclarationStmt (LET variable_declarations)
// │   ├── AssignmentStmt (IDENTIFIER assign_op expr)
// │   ├── ExpressionStmt (expression;)
// │   ├── IfStmt (IF/ELIF/ELSE chain)
// │   ├── MatchStmt (MATCH (expr) { match_list })
// │   │   └── MatchCase (expr => compound_stmt)
// │   ├── LoopStmt (Abstract base for loops)
// │   │   ├── WhileStmt (WHILE (cond) stmt)
// │   │   ├── DoWhileStmt (DO stmt WHILE (cond))
// │   │   └── ForStmt (FOR (init; cond; update) stmt)
// │   ├── JumpStmt (Abstract base for jumps)
// │   │   ├── ContinueStmt (CONTINUE)
// │   │   ├── BreakStmt (BREAK)
// │   │   └── ReturnStmt (RETURN [expr])
// │   ├── PrintStmt (PRINT, PRINTLN, SCAN)
// │   ├── QuantumStmt (Abstract base for quantum ops)
// │   │   ├── ApplyGateStmt (gate_composition @ quantum_state)
// │   │   ├── MeasureStmt (MEASURE_OP state => state)
// │   │   └── ResetStmt (RESET_OP state)
// │   └── LabelStmt (IDENTIFIER: statement)
// │
// ├── Decl (Base for top-level declarations and variable definitions)
// │   ├── FunctionDecl (FUNC ident(params) [: type] { ... })
// │   ├── GateDecl (GATE { APPLY: ... })
// │   ├── ImportDecl (IMPORT scoped_identifier)
// │   ├── VariableDecl (From 'variable_declarations', holds idents, type, init)
// │   └── ParameterDecl (IDENTIFIER: type, used in FunctionDecl)
// │
// ├── Type (Base for all type-specifier nodes)
// │   ├── BaseTypeNode (INT, FLOAT, QUBIT, etc.)
// │   └── ArrayTypeNode (type[index_expr])
// │
// └── Quantum (Base for quantum-specific syntax nodes)
//     ├── GateNode (Abstract base for 'quantum_gate')
//     │   ├── SimpleGateNode (GATE_H, GATE_X, etc.)
//     │   ├── ParametricGateNode (GATE_RX(expr))
//     │   └── CompositeGateNode ([gate_list])
//     └── GateCompositionNode (gate @ gate)

// ================= Base Nodes =================

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class Expr : public ASTNode {
public:
    virtual ~Expr() = default;
};

// ================= Literal Nodes =================

class LiteralExpr : public Expr {
public:
    virtual ~LiteralExpr() = default;
};

class IntLiteral : public LiteralExpr {
private:
    int value;
public:
    IntLiteral(int v) : value(v) {}

    int getValue() const { return value; }
};

class FloatLiteral : public LiteralExpr {
private:
    float value;
public:
    FloatLiteral(float v) : value(v) {}

    float getValue() const { return value; }
};

class BoolLiteral : public LiteralExpr {
private:
    bool value;
public:
    BoolLiteral(bool v) : value(v) {}

    bool getValue() const { return value; }
};

class StringLiteral : public LiteralExpr {
private:
    std::string value;
public:
    StringLiteral(std::string v) : value(std::move(v)) {}

};

// ================= Identifier =================

class IdentifierExpr : public Expr {
private:
    std::string value;
public:
    IdentifierExpr(std::string v)
        : value(std::move(v)) {}

    const std::string& getValue() const { return value; }
};

// ================= Binary Expression =================

class BinaryExpr : public Expr {
private:
    Expr* left;
    Expr* right;
    std::string op;
public:
    BinaryExpr(Expr* left, Expr* right, std::string op)
        : left(left), right(right), op(std::move(op)) {}

    Expr* getLeft() const { return left; }
    Expr* getRight() const { return right; }
    const std::string& getOperator() const { return op; }
};

// ================= Unary Expression =================

class UnaryExpr : public Expr {
private:
    std::string op;
    Expr* operand;
public:
    UnaryExpr(std::string op, Expr* operand)
        : op(std::move(op)), operand(operand) {}

    const std::string& getOperator() const { return op; }
    Expr* getOperand() const { return operand; }
};

// ================= Call Expression =================

class CallExpr : public Expr {
private:
    Expr* callee;
    std::vector<Expr*> arguments;
public:
    CallExpr(Expr* callee, std::vector<Expr*> args)
        : callee(callee), arguments(std::move(args)) {}

    Expr* getCallee() const { return callee; }
    const std::vector<Expr*>& getArgs() const { return arguments; }
    size_t getArgCount() const { return arguments.size(); }
};

#endif // HEISEN_AST_HPP