#ifndef HEISEN_SEMANTIC_ANALYZER_HPP
#define HEISEN_SEMANTIC_ANALYZER_HPP

#include <ast.hpp>
#include <ast_traversal.hpp>
#include <token.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>

namespace Heisen {

// ===================================================================
//                      Type System
// ===================================================================

class SemanticType {
public:
    enum class Kind {
        INT, FLOAT, BOOL, STRING,
        QUBIT, BIT,
        ARRAY, FUNCTION,
        VOID, ERROR
    };

protected:
    Kind kind;

public:
    SemanticType(Kind k) : kind(k) {}
    virtual ~SemanticType() = default;
    Kind getKind() const { return kind; }
    virtual std::string toString() const = 0;
    virtual bool equals(const SemanticType* other) const = 0;
    virtual bool isQuantum() const { return false; }
    virtual bool isCopyable() const { return true; }
};

class PrimitiveType : public SemanticType {
public:
    PrimitiveType(Kind k) : SemanticType(k) {}
    
    std::string toString() const override {
        switch (kind) {
            case Kind::INT: return "int";
            case Kind::FLOAT: return "float";
            case Kind::BOOL: return "bool";
            case Kind::STRING: return "string";
            case Kind::QUBIT: return "qubit";
            case Kind::BIT: return "bit";
            case Kind::VOID: return "void";
            case Kind::ERROR: return "<error>";
            default: return "<unknown>";
        }
    }
    
    bool equals(const SemanticType* other) const override {
        return other && other->getKind() == kind;
    }
    
    bool isQuantum() const override {
        return kind == Kind::QUBIT || kind == Kind::BIT;
    }
    
    bool isCopyable() const override {
        // Qubits cannot be copied (no-cloning theorem)
        return kind != Kind::QUBIT;
    }
};

class ArrayType : public SemanticType {
    SemanticType* elementType;
    int dimension; // -1 for unknown size
    
public:
    ArrayType(SemanticType* elem, int dim = -1) 
        : SemanticType(Kind::ARRAY), elementType(elem), dimension(dim) {}
    
    ~ArrayType() { delete elementType; }
    
    SemanticType* getElementType() const { return elementType; }
    int getDimension() const { return dimension; }
    
    std::string toString() const override {
        std::string dimStr = (dimension >= 0) ? "[" + std::to_string(dimension) + "]" : "[]";
        return elementType->toString() + dimStr;
    }
    
    bool equals(const SemanticType* other) const override {
        if (!other || other->getKind() != Kind::ARRAY) return false;
        auto* otherArr = static_cast<const ArrayType*>(other);
        return elementType->equals(otherArr->elementType);
    }
    
    bool isQuantum() const override {
        return elementType->isQuantum();
    }
    
    bool isCopyable() const override {
        return elementType->isCopyable();
    }
};

class FunctionType : public SemanticType {
    std::vector<SemanticType*> paramTypes;
    SemanticType* returnType;
    
public:
    FunctionType(std::vector<SemanticType*> params, SemanticType* ret)
        : SemanticType(Kind::FUNCTION), paramTypes(std::move(params)), returnType(ret) {}
    
    ~FunctionType() {
        for (auto* p : paramTypes) delete p;
        delete returnType;
    }
    
    const std::vector<SemanticType*>& getParamTypes() const { return paramTypes; }
    SemanticType* getReturnType() const { return returnType; }
    
    std::string toString() const override {
        std::string result = "(";
        for (size_t i = 0; i < paramTypes.size(); ++i) {
            if (i > 0) result += ", ";
            result += paramTypes[i]->toString();
        }
        result += ") => ";
        result += returnType ? returnType->toString() : "()";
        return result;
    }
    
    bool equals(const SemanticType* other) const override {
        if (!other || other->getKind() != Kind::FUNCTION) return false;
        auto* otherFunc = static_cast<const FunctionType*>(other);
        
        if (paramTypes.size() != otherFunc->paramTypes.size()) return false;
        
        for (size_t i = 0; i < paramTypes.size(); ++i) {
            if (!paramTypes[i]->equals(otherFunc->paramTypes[i])) return false;
        }
        
        if (returnType && otherFunc->returnType) {
            return returnType->equals(otherFunc->returnType);
        }
        return (!returnType && !otherFunc->returnType);
    }
};

// ===================================================================
//                      Symbol Table
// ===================================================================

struct Symbol {
    enum class Kind { VARIABLE, FUNCTION, PARAMETER };
    
    std::string name;
    SemanticType* type;
    Kind kind;
    bool isInitialized;
    bool isQubitAlias; // For tracking qubit references
    Location loc;
    
    Symbol(std::string n, SemanticType* t, Kind k, Location l)
        : name(std::move(n)), type(t), kind(k), isInitialized(false), 
          isQubitAlias(false), loc(l) {}
};

class Scope {
    std::unordered_map<std::string, Symbol*> symbols;
    Scope* parent;
    
public:
    Scope(Scope* p = nullptr) : parent(p) {}
    
    ~Scope() {
        for (auto& pair : symbols) {
            delete pair.second->type;
            delete pair.second;
        }
    }
    
    bool declare(const std::string& name, Symbol* symbol) {
        if (symbols.find(name) != symbols.end()) {
            return false; // Already declared in this scope
        }
        symbols[name] = symbol;
        return true;
    }
    
    Symbol* lookup(const std::string& name) {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return it->second;
        }
        if (parent) {
            return parent->lookup(name);
        }
        return nullptr;
    }
    
    Symbol* lookupLocal(const std::string& name) {
        auto it = symbols.find(name);
        return (it != symbols.end()) ? it->second : nullptr;
    }
    
    Scope* getParent() const { return parent; }
};

class SymbolTable {
    Scope* currentScope;
    std::vector<Scope*> allScopes;
    
public:
    SymbolTable() {
        currentScope = new Scope();
        allScopes.push_back(currentScope);
    }
    
    ~SymbolTable() {
        for (auto* scope : allScopes) {
            delete scope;
        }
    }
    
    void enterScope() {
        Scope* newScope = new Scope(currentScope);
        allScopes.push_back(newScope);
        currentScope = newScope;
    }
    
    void exitScope() {
        if (currentScope->getParent()) {
            currentScope = currentScope->getParent();
        }
    }
    
    bool declare(const std::string& name, Symbol* symbol) {
        return currentScope->declare(name, symbol);
    }
    
    Symbol* lookup(const std::string& name) {
        return currentScope->lookup(name);
    }
    
    Symbol* lookupLocal(const std::string& name) {
        return currentScope->lookupLocal(name);
    }
    
    Scope* getCurrentScope() const { return currentScope; }
};

// ===================================================================
//                      Semantic Error Handling
// ===================================================================

class SemanticError {
public:
    Location loc;
    std::string message;
    
    SemanticError(Location l, std::string msg) 
        : loc(l), message(std::move(msg)) {}
    
    std::string toString() const {
        return "Semantic Error at line " + std::to_string(loc.line) + 
               ", column " + std::to_string(loc.column) + ": " + message;
    }
};

// ===================================================================
//                      Semantic Analyzer
// ===================================================================

class SemanticAnalyzer : public AST_Traversal {
    SymbolTable symbolTable;
    std::vector<SemanticError> errors;
    
    // Context tracking
    FunctionDecl* currentFunction;
    bool inLoop;
    bool inQuantumContext;
    
    // Type cache for expressions
    std::unordered_map<Expr*, SemanticType*> exprTypes;
    
public:
    SemanticAnalyzer() 
        : currentFunction(nullptr), inLoop(false), inQuantumContext(false) {
        // Initialize built-in functions
        initializeBuiltins();
    }
    
    ~SemanticAnalyzer() {
        for (auto& pair : exprTypes) {
            delete pair.second;
        }
    }
    
    // Main analysis entry point
    bool analyze(std::vector<ASTNode*>& translationUnit);
    
    // Error reporting
    void reportError(Location loc, const std::string& message) {
        errors.emplace_back(loc, message);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    
    const std::vector<SemanticError>& getErrors() const { return errors; }
    
    void printErrors() const {
        for (const auto& error : errors) {
            std::cerr << error.toString() << std::endl;
        }
    }
    
    // Type checking utilities
    SemanticType* getExprType(Expr* expr);
    bool isTypeCompatible(SemanticType* expected, SemanticType* actual);
    bool canImplicitlyCast(SemanticType* from, SemanticType* to);
    SemanticType* resolveType(Type* astType);
    
    // Quantum-specific checks
    bool isQubitType(SemanticType* type);
    bool canCopyType(SemanticType* type);
    void checkQubitUsage(Expr* expr, Location loc);
    void checkGateApplication(GateNode* gate, Expr* target, Location loc);
    
private:
    void initializeBuiltins();
    
    // Helper methods
    SemanticType* cloneType(SemanticType* type);
    std::string formatType(SemanticType* type);
    
    // Visit methods for declarations
    void visitFunctionDecl(FunctionDecl* decl);
    void visitVariableDecl(VariableDecl* decl);
    void visitParameterDecl(ParameterDecl* decl);
    
    // Visit methods for statements
    void visitCompoundStmt(CompoundStmt* stmt);
    void visitDeclarationStmt(DeclarationStmt* stmt);
    void visitAssignmentStmt(AssignmentStmt* stmt);
    void visitExpressionStmt(ExpressionStmt* stmt);
    void visitIfStmt(IfStmt* stmt);
    void visitWhileStmt(WhileStmt* stmt);
    void visitDoWhileStmt(DoWhileStmt* stmt);
    void visitForStmt(ForStmt* stmt);
    void visitReturnStmt(ReturnStmt* stmt);
    void visitBreakStmt(BreakStmt* stmt);
    void visitContinueStmt(ContinueStmt* stmt);
    void visitMatchStmt(MatchStmt* stmt);
    
    // Visit methods for quantum statements
    void visitApplyGateStmt(ApplyGateStmt* stmt);
    void visitMeasureStmt(MeasureStmt* stmt);
    void visitResetStmt(ResetStmt* stmt);
    
    // Visit methods for expressions
    SemanticType* visitBinaryOpExpr(BinaryOpExpr* expr);
    SemanticType* visitUnaryOpExpr(UnaryOpExpr* expr);
    SemanticType* visitFunctionCallExpr(FunctionCallExpr* expr);
    SemanticType* visitIdentifierExpr(IdentifierExpr* expr);
    SemanticType* visitIndexAccessExpr(IndexAccessExpr* expr);
    SemanticType* visitMemberAccessExpr(MemberAccessExpr* expr);
    SemanticType* visitLambdaExpr(LambdaExpr* expr);
    SemanticType* visitIntLiteralExpr(IntLiteralExpr* expr);
    SemanticType* visitFloatLiteralExpr(FloatLiteralExpr* expr);
    SemanticType* visitBoolLiteralExpr(BoolLiteralExpr* expr);
    SemanticType* visitStringLiteralExpr(StringLiteralExpr* expr);
    SemanticType* visitBracedInitList(BracedInitList* expr);
    SemanticType* visitQuantumStateList(QuantumStateList* expr);
    
    // Type inference for operators
    SemanticType* inferBinaryOpType(const std::string& op, 
                                     SemanticType* left, 
                                     SemanticType* right,
                                     Location loc);
    
    SemanticType* inferUnaryOpType(const std::string& op, 
                                    SemanticType* operand,
                                    Location loc);
};

} // namespace Heisen

#endif // HEISEN_SEMANTIC_ANALYZER_HPP