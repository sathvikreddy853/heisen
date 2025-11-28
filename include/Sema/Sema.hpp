#ifndef HEISEN_SEMANTIC_ANALYZER_HPP
#define HEISEN_SEMANTIC_ANALYZER_HPP

#include "AST/AST.hpp"
#include "AST/ASTTraversal.hpp"
#include "Macros.hpp"
#include "Sema/Scope.hpp"
#include "Sema/SemaType.hpp"
#include "Sema/Symbol.hpp"
#include "Sema/SymbolTable.hpp"

namespace Heisen {

class SemanticError {
    public:
    Location loc;
    std::string message;

    SemanticError (Location l, std::string msg)
    : loc (l), message (std::move (msg)) {
    }

    std::string toString () const {
        return "Semantic Error at line " + std::to_string (loc.line) +
        ", column " + std::to_string (loc.column) + ": " + message;
    }

    void printClangStyle (const std::string& sourceFile = "") const {
        if (!loc.filename.empty ()) {
            std::cerr << loc.filename;
        } else if (!sourceFile.empty ()) {
            std::cerr << sourceFile;
        } else {
            std::cerr << "<input>";
        }
        std::cerr << ":" << loc.line << ":" << loc.column << ": ";
        std::cerr << "\033[1;31merror:\033[0m " << message << std::endl;

        // Try to read and display the source line
        std::string filename = !loc.filename.empty () ? loc.filename : sourceFile;
        if (!filename.empty ()) {
            std::ifstream file (filename);
            if (file.is_open ()) {
                std::string line;
                int currentLine = 0;
                while (std::getline (file, line)) {
                    currentLine++;
                    if (currentLine == loc.line) {
                        // Print the source line
                        std::cerr << line << std::endl;

                        // Print caret indicator (^) at the error column
                        if (loc.column > 0) {
                            std::cerr << std::string (loc.column - 1, ' ')
                                      << "\033[1;32m^\033[0m" << std::endl;
                        }
                        break;
                    }
                }
                file.close ();
            }
        }
    }
};

class SemanticAnalyzer : public AST_Traversal {
    SymbolTable symbolTable;
    std::vector<SemanticError> errors;
    std::string sourceFilename; // Track source file for error reporting

    // Context tracking
    FunctionDecl* currentFunction;
    bool inLoop;
    bool inQuantumContext;

    // Type cache for expressions
    std::map<Expr*, SemanticType*> exprTypes;

    public:
    SemanticAnalyzer ()
    : currentFunction (nullptr), inLoop (false), inQuantumContext (false) {
        // Initialize built-in functions
        initializeBuiltins ();
    }

    ~SemanticAnalyzer () {
        for (auto& pair : exprTypes) { delete pair.second; }
    }

    void setSourceFilename (const std::string& filename) {
        sourceFilename = filename;
    }

    bool analyze (std::vector<ASTNode*>& translationUnit);

    void reportError (Location loc, const std::string& message) {
        errors.emplace_back (loc, message);
    }

    bool hasErrors () const {
        return !errors.empty ();
    }

    const std::vector<SemanticError>& getErrors () const {
        return errors;
    }

    void printErrors () const {
        for (const auto& error : errors) {
            error.printClangStyle (sourceFilename);
        }
    }

    const SymbolTable& getSymbolTable () const {
        return symbolTable;
    }

    SemanticType* getExprType (Expr* expr);
    bool isTypeCompatible (SemanticType* expected, SemanticType* actual);
    bool canImplicitlyCast (SemanticType* from, SemanticType* to);
    SemanticType* resolveType (Type* astType);

    bool isQubitType (SemanticType* type);
    bool canCopyType (SemanticType* type);
    void checkQubitUsage (Expr* expr, Location loc);
    void checkGateApplication (GateNode* gate, Expr* target, Location loc);

    int getGateQubitRequirement (GateKind kind);
    int countQubitsInTarget (Expr* target);
    int countGatesInNode (GateNode* gate);
    std::string getGateName (GateNode* gate);
    void validateGateParameters (GateNode* gate, Location loc);

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
    SemanticType* inferBinaryOpType (const std::string& op,
    SemanticType* left,
    SemanticType* right,
    Location loc);

    SemanticType* inferUnaryOpType (const std::string& op, SemanticType* operand, Location loc);
};

} // namespace Heisen

#endif // HEISEN_SEMANTIC_ANALYZER_HPP
