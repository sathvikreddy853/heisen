#ifndef HEISEN_AST_TRAVERSAL_HPP
#define HEISEN_AST_TRAVERSAL_HPP

#include <Macros.hpp>

// Forward declarations for all AST node types
class ASTNode;
class Expr;
class Stmt;
class Decl;
class Type;
class Quantum;
class GateNode;

// Expression nodes
class LiteralExpr;
class IntLiteralExpr;
class FloatLiteralExpr;
class BoolLiteralExpr;
class StringLiteralExpr;
class IdentifierExpr;
class BinaryOpExpr;
class UnaryOpExpr;
class FunctionCallExpr;
class IndexAccessExpr;
class SliceExpr;
class MemberAccessExpr;
class CastExpr;
class LambdaExpr;
class BracedInitList;

// Quantum state expressions
class QuantumStateExpr;
class QuantumStateIdentifier;
class QuantumStateIndexAccess;
class QuantumStateList;

// Declaration nodes
class VariableDecl;
class ParameterDecl;
class FunctionDecl;
class GateDecl;

// Statement nodes
class CompoundStmt;
class DeclarationStmt;
class AssignmentStmt;
class ExpressionStmt;
class IfStmt;
class MatchStmt;
class MatchCase;
class LoopStmt;
class WhileStmt;
class DoWhileStmt;
class ForStmt;
class JumpStmt;
class ContinueStmt;
class BreakStmt;
class ReturnStmt;
class PrintStmt;

// Quantum statement nodes
class QuantumStmt;
class ApplyGateStmt;
class MeasureStmt;
class ResetStmt;

// Gate nodes
class SimpleGateNode;
class ParametricGateNode;
class CompositeGateNode;
class GateCompositionNode;

// Type nodes
class BaseTypeNode;
class ArrayTypeNode;
class FunctionTypeNode;

/**
 * @brief Abstract base class for AST traversal (Visitor pattern)
 * 
 * This class provides a visitor interface for traversing the AST.
 * Subclasses can implement specific traversal behaviors by overriding
 * the visit methods they need.
 * 
 * Default implementations provide no-op behavior, so you only need to
 * override the methods relevant to your traversal.
 */
class AST_Traversal {
public:
    virtual ~AST_Traversal() = default;

    // ===================================================================
    //                      Expression Visitors
    // ===================================================================
    
    virtual void visit(IntLiteralExpr* node) {}
    virtual void visit(FloatLiteralExpr* node) {}
    virtual void visit(BoolLiteralExpr* node) {}
    virtual void visit(StringLiteralExpr* node) {}
    virtual void visit(IdentifierExpr* node) {}
    virtual void visit(BinaryOpExpr* node) {}
    virtual void visit(UnaryOpExpr* node) {}
    virtual void visit(FunctionCallExpr* node) {}
    virtual void visit(IndexAccessExpr* node) {}
    virtual void visit(SliceExpr* node) {}
    virtual void visit(MemberAccessExpr* node) {}
    virtual void visit(CastExpr* node) {}
    virtual void visit(LambdaExpr* node) {}
    virtual void visit(BracedInitList* node) {}
    
    // Quantum state expressions
    virtual void visit(QuantumStateIdentifier* node) {}
    virtual void visit(QuantumStateIndexAccess* node) {}
    virtual void visit(QuantumStateList* node) {}
    
    // ===================================================================
    //                      Declaration Visitors
    // ===================================================================
    
    virtual void visit(VariableDecl* node) {}
    virtual void visit(ParameterDecl* node) {}
    virtual void visit(FunctionDecl* node) {}
    virtual void visit(GateDecl* node) {}
    
    // ===================================================================
    //                      Statement Visitors
    // ===================================================================
    
    virtual void visit(CompoundStmt* node) {}
    virtual void visit(DeclarationStmt* node) {}
    virtual void visit(AssignmentStmt* node) {}
    virtual void visit(ExpressionStmt* node) {}
    virtual void visit(IfStmt* node) {}
    virtual void visit(MatchStmt* node) {}
    virtual void visit(MatchCase* node) {}
    virtual void visit(WhileStmt* node) {}
    virtual void visit(DoWhileStmt* node) {}
    virtual void visit(ForStmt* node) {}
    virtual void visit(ContinueStmt* node) {}
    virtual void visit(BreakStmt* node) {}
    virtual void visit(ReturnStmt* node) {}
    virtual void visit(PrintStmt* node) {}
    
    // ===================================================================
    //                      Quantum Statement Visitors
    // ===================================================================
    
    virtual void visit(ApplyGateStmt* node) {}
    virtual void visit(MeasureStmt* node) {}
    virtual void visit(ResetStmt* node) {}
    
    // ===================================================================
    //                      Gate Visitors
    // ===================================================================
    
    virtual void visit(SimpleGateNode* node) {}
    virtual void visit(ParametricGateNode* node) {}
    virtual void visit(CompositeGateNode* node) {}
    virtual void visit(GateCompositionNode* node) {}
    
    // ===================================================================
    //                      Type Visitors
    // ===================================================================
    
    virtual void visit(BaseTypeNode* node) {}
    virtual void visit(ArrayTypeNode* node) {}
    virtual void visit(FunctionTypeNode* node) {}
};

/**
 * @brief Recursive AST traversal visitor
 * 
 * This class provides a default recursive traversal that visits all
 * nodes in the AST tree. Subclasses can override specific visit methods
 * to perform actions at particular nodes while maintaining the recursive
 * traversal for other nodes.
 * 
 * This is useful for passes that need to examine or transform the entire
 * AST, such as:
 * - Code generation
 * - Optimization passes
 * - Pretty printing
 * - AST transformation
 */
class RecursiveASTVisitor : public AST_Traversal {
public:
    virtual ~RecursiveASTVisitor() = default;

    // ===================================================================
    //                      Expression Visitors
    // ===================================================================
    
    void visit(BinaryOpExpr* node) override;
    void visit(UnaryOpExpr* node) override;
    void visit(FunctionCallExpr* node) override;
    void visit(IndexAccessExpr* node) override;
    void visit(SliceExpr* node) override;
    void visit(MemberAccessExpr* node) override;
    void visit(CastExpr* node) override;
    void visit(LambdaExpr* node) override;
    void visit(BracedInitList* node) override;
    void visit(QuantumStateList* node) override;
    
    // ===================================================================
    //                      Declaration Visitors
    // ===================================================================
    
    void visit(VariableDecl* node) override;
    void visit(ParameterDecl* node) override;
    void visit(FunctionDecl* node) override;
    void visit(GateDecl* node) override;
    
    // ===================================================================
    //                      Statement Visitors
    // ===================================================================
    
    void visit(CompoundStmt* node) override;
    void visit(DeclarationStmt* node) override;
    void visit(AssignmentStmt* node) override;
    void visit(ExpressionStmt* node) override;
    void visit(IfStmt* node) override;
    void visit(MatchStmt* node) override;
    void visit(MatchCase* node) override;
    void visit(WhileStmt* node) override;
    void visit(DoWhileStmt* node) override;
    void visit(ForStmt* node) override;
    void visit(ReturnStmt* node) override;
    
    // ===================================================================
    //                      Quantum Statement Visitors
    // ===================================================================
    
    void visit(ApplyGateStmt* node) override;
    void visit(MeasureStmt* node) override;
    void visit(ResetStmt* node) override;
    
    // ===================================================================
    //                      Gate Visitors
    // ===================================================================
    
    void visit(SimpleGateNode* node) override;
    void visit(ParametricGateNode* node) override;
    void visit(CompositeGateNode* node) override;
    void visit(GateCompositionNode* node) override;
    
    // ===================================================================
    //                      Type Visitors
    // ===================================================================
    
    void visit(ArrayTypeNode* node) override;
    void visit(FunctionTypeNode* node) override;

protected:
    // Helper method for visiting expressions (handles nullptr)
    void visitExpr(Expr* expr);
    
    // Helper method for visiting statements (handles nullptr)
    void visitStmt(Stmt* stmt);
    
    // Helper method for visiting declarations (handles nullptr)
    void visitDecl(Decl* decl);
    
    // Helper method for visiting types (handles nullptr)
    void visitType(Type* type);
    
    // Helper method for visiting gates (handles nullptr)
    void visitGate(GateNode* gate);
};
/**
 * @brief AST printer for debugging
 * 
 * This visitor prints a formatted representation of the AST to stdout,
 * useful for debugging the parser and understanding the AST structure.
 */

class ASTPrinter : public RecursiveASTVisitor {
    int indentLevel;
    bool useColor;
    
    void indent();
    void printNode(const std::string& nodeType, const std::string& details = "");
    
public:
    ASTPrinter(bool color = true) : indentLevel(0), useColor(color) {}
    
    // Override visitor methods to print node information
    void visit(IntLiteralExpr* node) override;
    void visit(FloatLiteralExpr* node) override;
    void visit(BoolLiteralExpr* node) override;
    void visit(StringLiteralExpr* node) override;
    void visit(IdentifierExpr* node) override;
    void visit(BinaryOpExpr* node) override;
    void visit(UnaryOpExpr* node) override;
    void visit(FunctionCallExpr* node) override;
    void visit(VariableDecl* node) override;
    void visit(FunctionDecl* node) override;
    void visit(CompoundStmt* node) override;
    void visit(IfStmt* node) override;
    void visit(WhileStmt* node) override;
    void visit(ForStmt* node) override;
    void visit(ReturnStmt* node) override;
    void visit(ApplyGateStmt* node) override;
    void visit(MeasureStmt* node) override;
    void visit(SimpleGateNode* node) override;
    void visit(ParametricGateNode* node) override;
    void visit(GateCompositionNode* node) override;
    
    // Utility method to print entire AST
    void print(ASTNode* root);
};

/**
 * @brief AST node counter for statistics
 * 
 * Counts different types of nodes in the AST for analysis and optimization
 * decisions.
 */

class ASTStatistics : public RecursiveASTVisitor {
public:
    struct Stats {
        int totalNodes = 0;
        int expressions = 0;
        int statements = 0;
        int declarations = 0;
        int functions = 0;
        int loops = 0;
        int quantumOps = 0;
        int gateApplications = 0;
        int measurements = 0;
    };
    
private:
    Stats stats;
    
public:
    // Override specific node visits to count them
    void visit(FunctionDecl* node) override { 
        stats.functions++; 
        stats.declarations++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node); 
    }
    
    void visit(VariableDecl* node) override {
        stats.declarations++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node);
    }
    
    void visit(GateDecl* node) override {
        stats.declarations++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node);
    }
    
    void visit(WhileStmt* node) override { 
        stats.loops++; 
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node); 
    }
    
    void visit(DoWhileStmt* node) override { 
        stats.loops++; 
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node); 
    }
    
    void visit(ForStmt* node) override { 
        stats.loops++; 
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node); 
    }
    
    void visit(ApplyGateStmt* node) override { 
        stats.quantumOps++; 
        stats.gateApplications++; 
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node); 
    }
    
    void visit(MeasureStmt* node) override { 
        stats.quantumOps++; 
        stats.measurements++; 
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node); 
    }
    
    void visit(ResetStmt* node) override { 
        stats.quantumOps++; 
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node); 
    }
    
    void visit(BinaryOpExpr* node) override {
        stats.expressions++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node);
    }
    
    void visit(UnaryOpExpr* node) override {
        stats.expressions++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node);
    }
    
    void visit(IntLiteralExpr* node) override {
        stats.expressions++;
        stats.totalNodes++;
    }
    
    void visit(FloatLiteralExpr* node) override {
        stats.expressions++;
        stats.totalNodes++;
    }
    
    void visit(IdentifierExpr* node) override {
        stats.expressions++;
        stats.totalNodes++;
    }
    
    void visit(IfStmt* node) override {
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node);
    }
    
    void visit(CompoundStmt* node) override {
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node);
    }
    
    void visit(AssignmentStmt* node) override {
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node);
    }
    
    void visit(ExpressionStmt* node) override {
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node);
    }
    
    void visit(ReturnStmt* node) override {
        stats.statements++;
        stats.totalNodes++;
        RecursiveASTVisitor::visit(node);
    }
    
    // Public helper to analyze a node (implementation in .cpp file)
    void analyzeNode(ASTNode* node);
    
    const Stats& getStats() const { return stats; }
    void reset() { stats = Stats(); }
    
    void printStats() const;
};

#endif // HEISEN_AST_TRAVERSAL_HPP