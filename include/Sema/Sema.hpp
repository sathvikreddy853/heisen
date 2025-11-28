#ifndef HEISEN_SEMANTIC_ANALYZER_HPP
#define HEISEN_SEMANTIC_ANALYZER_HPP

#include "AST/AST.hpp"
#include "AST/ASTTraversal.hpp"
#include "Macros.hpp"
#include "Symbol.hpp"
#include "Sema/SymbolTable.hpp"
#include "Sema/Scope.hpp"
#include "Sema/SemaType.hpp"

namespace Heisen {

class SemanticError {
    public:
    Location loc;
    std::string message;

    SemanticError (Location l, std::string msg) : loc (l), message (std::move (msg)) {}

    std::string toString () const {
        return "Semantic Error at line " + std::to_string (loc.line) + ", column " +
        std::to_string (loc.column) + ": " + message;
    }
};

class SemanticAnalyzer : public AST_Traversal {
    SymbolTable symbolTable;
    std::vector<SemanticError> errors;

    // Context tracking
    FunctionDecl* currentFunction;
    bool inLoop;
    bool inQuantumContext;

    // Type cache for expressions
    std::map<Expr*, SemanticType*> exprTypes;

    public:
    SemanticAnalyzer () : currentFunction (nullptr), inLoop (false), inQuantumContext (false) {
        // Initialize built-in functions
        initializeBuiltins ();
    }

    ~SemanticAnalyzer () {
        for (auto& pair : exprTypes) { delete pair.second; }
    }

    // Main analysis entry point
    bool analyze (std::vector<ASTNode*>& translationUnit);

    // Error reporting
    void reportError (Location loc, const std::string& message) {
        errors.emplace_back (loc, message);
    }

    bool hasErrors () const { return !errors.empty (); }

    const std::vector<SemanticError>& getErrors () const { return errors; }

    void printErrors () const {
        for (const auto& error : errors) { std::cerr << error.toString () << std::endl; }
    }

    // Type checking utilities
    SemanticType* getExprType (Expr* expr);
    bool isTypeCompatible (SemanticType* expected, SemanticType* actual);
    bool canImplicitlyCast (SemanticType* from, SemanticType* to);
    SemanticType* resolveType (Type* astType);

    // Quantum-specific checks
    bool isQubitType (SemanticType* type);
    bool canCopyType (SemanticType* type);
    void checkQubitUsage (Expr* expr, Location loc);
    void checkGateApplication (GateNode* gate, Expr* target, Location loc);

    //helpers
    int  getGateQubitRequirement(GateKind kind);
    int countQubitsInTarget(Expr* target);
    int countGatesInNode(GateNode* gate);
    std::string getGateName(GateNode* gate);
    void validateGateParameters(GateNode* gate, Location loc);

    private:
    void initializeBuiltins ();

    // Helper methods
    SemanticType* cloneType (SemanticType* type);
    std::string formatType (SemanticType* type);

    // Visit methods for declarations
    void visitFunctionDecl (FunctionDecl* decl);
    void visitVariableDecl (VariableDecl* decl);
    void visitParameterDecl (ParameterDecl* decl);

    // Visit methods for statements
    void visitCompoundStmt (CompoundStmt* stmt);
    void visitDeclarationStmt (DeclarationStmt* stmt);
    void visitAssignmentStmt (AssignmentStmt* stmt);
    void visitExpressionStmt (ExpressionStmt* stmt);
    void visitIfStmt (IfStmt* stmt);
    void visitWhileStmt (WhileStmt* stmt);
    void visitDoWhileStmt (DoWhileStmt* stmt);
    void visitForStmt (ForStmt* stmt);
    void visitReturnStmt (ReturnStmt* stmt);
    void visitBreakStmt (BreakStmt* stmt);
    void visitContinueStmt (ContinueStmt* stmt);
    void visitMatchStmt (MatchStmt* stmt);

    // Visit methods for quantum statements
    void visitApplyGateStmt (ApplyGateStmt* stmt);
    void visitMeasureStmt (MeasureStmt* stmt);
    void visitResetStmt (ResetStmt* stmt);

    // Visit methods for expressions
    SemanticType* visitBinaryOpExpr (BinaryOpExpr* expr);
    SemanticType* visitUnaryOpExpr (UnaryOpExpr* expr);
    SemanticType* visitFunctionCallExpr (FunctionCallExpr* expr);
    SemanticType* visitIdentifierExpr (IdentifierExpr* expr);
    SemanticType* visitIndexAccessExpr (IndexAccessExpr* expr);
    SemanticType* visitMemberAccessExpr (MemberAccessExpr* expr);
    SemanticType* visitLambdaExpr (LambdaExpr* expr);
    SemanticType* visitIntLiteralExpr (IntLiteralExpr* expr);
    SemanticType* visitFloatLiteralExpr (FloatLiteralExpr* expr);
    SemanticType* visitBoolLiteralExpr (BoolLiteralExpr* expr);
    SemanticType* visitStringLiteralExpr (StringLiteralExpr* expr);
    SemanticType* visitBracedInitList (BracedInitList* expr);
    SemanticType* visitQuantumStateList (QuantumStateList* expr);

    // Type inference for operators
    SemanticType* inferBinaryOpType (const std::string& op, SemanticType* left, SemanticType* right, Location loc);

    SemanticType* inferUnaryOpType (const std::string& op, SemanticType* operand, Location loc);
};

} // namespace Heisen

#endif // HEISEN_SEMANTIC_ANALYZER_HPP
