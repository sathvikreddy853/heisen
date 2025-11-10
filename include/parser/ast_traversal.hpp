#ifndef HEISEN_AST_VISITOR_HPP
#define HEISEN_AST_VISITOR_HPP

/* Expression Classes */
/* LiteralExpr */
class IntLiteralExpr;
class FloatLiteralExpr;
class StringLiteralExpr;
class BoolLiteralExpr;

class IdentifierExpr;
class BinaryOpExpr;
class UnaryOpExpr;
class FunctionCallExpr;
class IndexAccessExpr;
class SliceExpr;
class MemberAccessExpr;
class CastExpr;

/* Quantum State Expr */
class QuantumStateIdentifier;
class QuantumStateIndexAccess;
class QuantumStateList;

/* Statements */
class CompoundStmt;
class DeclarationStmt;
class AssignmentStmt;
class ExpressionStmt;
class IfStmt;
class MatchStmt;
class MatchCase;
class WhileStmt;
class DoWhileStmt;
class ForStmt;
class ContinueStmt;
class BreakStmt;
class ReturnStmt;
class PrintStmt;
class ApplyGateStmt;
class MeasureStmt;
class ResetStmt;

/* Declarations */
class VariableDecl;
class ParameterDecl;
class FunctionDecl;
class GateDecl;

/* Types */
class BaseTypeNode;
class ArrayTypeNode;

/* Quantum Gate Nodes */
class SimpleGateNode;
class ParametricGateNode;
class CompositeGateNode;
class GateCompositionNode;

class AST_Traversal {
    public:
    virtual ~AST_Traversal () = default;

    /*Expressions*/
    virtual void visit (IntLiteralExpr* node)          = 0;
    virtual void visit (FloatLiteralExpr* node)        = 0;
    virtual void visit (BoolLiteralExpr* node)         = 0;
    virtual void visit (StringLiteralExpr* node)       = 0;
    virtual void visit (IdentifierExpr* node)          = 0;
    virtual void visit (BinaryOpExpr* node)            = 0;
    virtual void visit (UnaryOpExpr* node)             = 0;
    virtual void visit (FunctionCallExpr* node)        = 0;
    virtual void visit (IndexAccessExpr* node)         = 0;
    virtual void visit (SliceExpr* node)               = 0;
    virtual void visit (MemberAccessExpr* node)        = 0;
    virtual void visit (CastExpr* node)                = 0;
    virtual void visit (QuantumStateIdentifier* node)  = 0;
    virtual void visit (QuantumStateIndexAccess* node) = 0;
    virtual void visit (QuantumStateList* node)        = 0;

    /* Statements */
    virtual void visit (CompoundStmt* node)    = 0;
    virtual void visit (DeclarationStmt* node) = 0;
    virtual void visit (AssignmentStmt* node)  = 0;
    virtual void visit (ExpressionStmt* node)  = 0;
    virtual void visit (IfStmt* node)          = 0;
    virtual void visit (MatchStmt* node)       = 0;
    virtual void visit (MatchCase* node)       = 0;
    virtual void visit (WhileStmt* node)       = 0;
    virtual void visit (DoWhileStmt* node)     = 0;
    virtual void visit (ForStmt* node)         = 0;
    virtual void visit (ContinueStmt* node)    = 0;
    virtual void visit (BreakStmt* node)       = 0;
    virtual void visit (ReturnStmt* node)      = 0;
    virtual void visit (PrintStmt* node)       = 0;
    virtual void visit (ApplyGateStmt* node)   = 0;
    virtual void visit (MeasureStmt* node)     = 0;
    virtual void visit (ResetStmt* node)       = 0;

    /*Declarations*/
    virtual void visit (VariableDecl* node)  = 0;
    virtual void visit (ParameterDecl* node) = 0;
    virtual void visit (FunctionDecl* node)  = 0;
    virtual void visit (GateDecl* node)      = 0;

    /*Types*/
    virtual void visit (BaseTypeNode* node)  = 0;
    virtual void visit (ArrayTypeNode* node) = 0;

    /*Quantum Gate nodes*/
    virtual void visit (SimpleGateNode* node)      = 0;
    virtual void visit (ParametricGateNode* node)  = 0;
    virtual void visit (CompositeGateNode* node)   = 0;
    virtual void visit (GateCompositionNode* node) = 0;
};


#endif