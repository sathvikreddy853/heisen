#ifndef HEISEN_AST_HPP
#define HEISEN_AST_HPP

#include <macros.hpp>
#include <string>
#include <vector>
#include <ast_traversal.hpp>
#include<token.hpp>
// Forward declarations
class Expr;
class Stmt;
class Type;
class Decl;
class Quantum;
class GateNode;
class QuantumStateExpr;
class MatchCase;
class VariableDecl;
class ParameterDecl;

// Enums
enum class BaseTypeKind { TYPE_INT, TYPE_FLOAT, TYPE_QUBIT, TYPE_BIT, TYPE_BOOL, TYPE_STRING };

enum class GateKind {
    H, S, T, I, X, Y, Z, RX, RY, RZ,
    CNOT, CZ, SWAP, CSWAP, CCNOT,
    CRX, CRY, CRZ, UNKNOWN, CTRL
};

enum class PrintOperationKind { PRINT, PRINTLN, SCAN };

// Base AST Node
class ASTNode {
public:
    Heisen::Location loc;
    virtual ~ASTNode() = default;
    ASTNode(Heisen::Location l) : loc(l) {}
    ASTNode() {}
    virtual void process(AST_Traversal* Visitor) = 0;
};

// Expression Base
class Expr : public ASTNode {
public:
    Expr() {}
    Expr(Heisen::Location l) : ASTNode(l) {}
    virtual ~Expr() = default;
};

// Statement Base
class Stmt : public ASTNode {
public:
    Stmt(Heisen::Location l) : ASTNode(l) {}
    Stmt() {}
    virtual ~Stmt() = default;
};

// Declaration Base
class Decl : public ASTNode {
public:
    Decl(Heisen::Location l) : ASTNode(l) {}
    Decl() {}
    virtual ~Decl() = default;
};

// Type Base
class Type : public ASTNode {
public:
    enum class Kind { BaseType, ArrayType, FunctionType };

protected:
    Kind kind;

public:
    Type(Kind k) : kind(k) {}
    Kind getKind() const { return kind; }
};

// Quantum Base
class Quantum : public ASTNode {
public:
    Quantum() {}
    Quantum(Heisen::Location l) : ASTNode(l) {}
    virtual ~Quantum() = default;
};

// ===================================================================
//                      Expression Nodes
// ===================================================================

class LiteralExpr : public Expr {
public:
    virtual ~LiteralExpr() = default;
    LiteralExpr() {}
    LiteralExpr(Heisen::Location l) : Expr(l) {}
};

class IntLiteralExpr : public LiteralExpr {
    long long value;
public:
    IntLiteralExpr(long long v, Heisen::Location l) : LiteralExpr(l), value(v) {}
    long long getValue() const { return value; }
    void process(AST_Traversal* v) override {}
};

class FloatLiteralExpr : public LiteralExpr {
    double value;
public:
    FloatLiteralExpr(double v, Heisen::Location l) : LiteralExpr(l), value(v) {}
    double getValue() const { return value; }
    void process(AST_Traversal* v) override {}
};

class BoolLiteralExpr : public LiteralExpr {
    bool value;
public:
    BoolLiteralExpr(bool v, Heisen::Location l) : LiteralExpr(l), value(v) {}
    bool getValue() const { return value; }
    void process(AST_Traversal* v) override {}
};

class StringLiteralExpr : public LiteralExpr {
    std::string value;
public:
    StringLiteralExpr(std::string v, Heisen::Location l) : LiteralExpr(l), value(std::move(v)) {}
    const std::string& getValue() const { return value; }
    void process(AST_Traversal* v) override {}
};

class IdentifierExpr : public Expr {
    std::string value;
public:
    IdentifierExpr(std::string v, Heisen::Location l) : Expr(l), value(std::move(v)) {}
    const std::string& getValue() const { return value; }
    void process(AST_Traversal* v) override {}
};

class BinaryOpExpr : public Expr {
    Expr* left;
    Expr* right;
    std::string op;
public:
    BinaryOpExpr(Expr* l, std::string o, Expr* r, Heisen::Location loc = Heisen::Location()) 
        : Expr(loc), left(l), right(r), op(std::move(o)) {}
    ~BinaryOpExpr() {
        delete left;
        delete right;
    }
    Expr* getLeft() const { return left; }
    Expr* getRight() const { return right; }
    const std::string& getOperator() const { return op; }
    void process(AST_Traversal* v) override {}
};

class UnaryOpExpr : public Expr {
    std::string op;
    Expr* operand;
public:
    UnaryOpExpr(std::string o, Expr* e, Heisen::Location loc = Heisen::Location()) 
        : Expr(loc), op(std::move(o)), operand(e) {}
    ~UnaryOpExpr() { delete operand; }
    const std::string& getOperator() const { return op; }
    Expr* getOperand() const { return operand; }
    void process(AST_Traversal* v) override {}
};

class FunctionCallExpr : public Expr {
    Expr* callee;
    std::vector<Expr*> args;
public:
    FunctionCallExpr(Expr* c, std::vector<Expr*> a, Heisen::Location loc = Heisen::Location()) 
        : Expr(loc), callee(c), args(std::move(a)) {}
    ~FunctionCallExpr() {
        delete callee;
        for (auto* a : args) delete a;
    }
    Expr* getCallee() const { return callee; }
    const std::vector<Expr*>& getArgs() const { return args; }
    void process(AST_Traversal* v) override {}
};

class IndexAccessExpr : public Expr {
    Expr* target;
    Expr* index;
public:
    IndexAccessExpr(Expr* t, Expr* i, Heisen::Location loc = Heisen::Location()) 
        : Expr(loc), target(t), index(i) {}
    ~IndexAccessExpr() {
        delete target;
        delete index;
    }
    Expr* getTarget() const { return target; }
    Expr* getIndex() const { return index; }
    void process(AST_Traversal* v) override {}
};

class SliceExpr : public Expr {
    Expr* start;
    Expr* stop;
    Expr* step;
public:
    SliceExpr(Expr* s, Expr* e, Expr* st, Heisen::Location loc = Heisen::Location()) 
        : Expr(loc), start(s), stop(e), step(st) {}
    ~SliceExpr() {
        delete start;
        delete stop;
        delete step;
    }
    Expr* getStart() const { return start; }
    Expr* getStop() const { return stop; }
    Expr* getStep() const { return step; }
    void process(AST_Traversal* v) override {}
};

class MemberAccessExpr : public Expr {
    Expr* target;
    IdentifierExpr* member;
public:
    MemberAccessExpr(Expr* t, IdentifierExpr* m, Heisen::Location loc = Heisen::Location()) 
        : Expr(loc), target(t), member(m) {}
    ~MemberAccessExpr() {
        delete target;
        delete member;
    }
    Expr* getTarget() const { return target; }
    IdentifierExpr* getMemberName() const { return member; }
    void process(AST_Traversal* v) override {}
};

class CastExpr : public Expr {
    Type* targetType;
    Expr* expr;
public:
    CastExpr(Type* t, Expr* e, Heisen::Location loc = Heisen::Location()) 
        : Expr(loc), targetType(t), expr(e) {}
    ~CastExpr() {
        delete targetType;
        delete expr;
    }
    Type* getTargetType() const { return targetType; }
    Expr* getExpression() const { return expr; }
    void process(AST_Traversal* v) override {}
};

class LambdaExpr : public Expr {
    std::vector<ParameterDecl*> params;
    Type* returnType;
    CompoundStmt* body;
public:
    LambdaExpr(std::vector<ParameterDecl*> p, Type* rt, CompoundStmt* b, Heisen::Location loc = Heisen::Location())
        : Expr(loc), params(std::move(p)), returnType(rt), body(b) {}
    ~LambdaExpr();
    const std::vector<ParameterDecl*>& getParams() const { return params; }
    Type* getReturnType() const { return returnType; }
    CompoundStmt* getBody() const { return body; }
    void process(AST_Traversal* v) override {}
};

class BracedInitList : public Expr {
    std::vector<Expr*> elements;
public:
    BracedInitList(std::vector<Expr*> e, Heisen::Location loc = Heisen::Location()) 
        : Expr(loc), elements(std::move(e)) {}
    ~BracedInitList() {
        for (auto* e : elements) delete e;
    }
    const std::vector<Expr*>& getElements() const { return elements; }
    void process(AST_Traversal* v) override {}
};

// ===================================================================
//                      Quantum State Expressions
// ===================================================================

class QuantumStateExpr : public Expr {
public:
    QuantumStateExpr() {}
    QuantumStateExpr(Heisen::Location l) : Expr(l) {}
    virtual ~QuantumStateExpr() = default;
};

class QuantumStateIdentifier : public QuantumStateExpr {
    IdentifierExpr* identifier;
public:
    QuantumStateIdentifier(IdentifierExpr* id, Heisen::Location loc = Heisen::Location()) 
        : QuantumStateExpr(loc), identifier(id) {}
    ~QuantumStateIdentifier() { delete identifier; }
    IdentifierExpr* getIdentifier() const { return identifier; }
    void process(AST_Traversal* v) override {}
};

class QuantumStateIndexAccess : public QuantumStateExpr {
    IdentifierExpr* identifier;
    Expr* index;
public:
    QuantumStateIndexAccess(IdentifierExpr* id, Expr* idx, Heisen::Location loc = Heisen::Location()) 
        : QuantumStateExpr(loc), identifier(id), index(idx) {}
    ~QuantumStateIndexAccess() {
        delete identifier;
        delete index;
    }
    IdentifierExpr* getIdentifier() const { return identifier; }
    Expr* getIndex() const { return index; }
    void process(AST_Traversal* v) override {}
};

class QuantumStateList : public QuantumStateExpr {
    std::vector<Expr*> states;
public:
    QuantumStateList(std::vector<Expr*> s, Heisen::Location loc = Heisen::Location()) 
        : QuantumStateExpr(loc), states(std::move(s)) {}
    ~QuantumStateList() {
        for (auto* s : states) delete s;
    }
    const std::vector<Expr*>& getStates() const { return states; }
    void process(AST_Traversal* v) override {}
};

// ===================================================================
//                      Declaration Nodes
// ===================================================================

class VariableDecl : public Decl {
    IdentifierExpr* name;
    Type* type;
    Expr* init;
public:
    VariableDecl(IdentifierExpr* n, Type* t, Expr* i, Heisen::Location l) 
        : Decl(l), name(n), type(t), init(i) {}
    ~VariableDecl() {
        delete name;
        delete type;
        delete init;
    }
    IdentifierExpr* getVariableName() const { return name; }
    Type* getVariableType() const { return type; }
    Expr* getInitializer() const { return init; }
    void process(AST_Traversal* v) override {}
};

class ParameterDecl : public Decl {
    IdentifierExpr* name;
    Type* type;
public:
    ParameterDecl(IdentifierExpr* n, Type* t, Heisen::Location l) 
        : Decl(l), name(n), type(t) {}
    ~ParameterDecl() {
        delete name;
        delete type;
    }
    IdentifierExpr* getName() const { return name; }
    Type* getType() const { return type; }
    void process(AST_Traversal* v) override {}
};

// ===================================================================
//                      Statement Nodes
// ===================================================================

class CompoundStmt : public Stmt {
    std::vector<Stmt*> stmts;
public:
    CompoundStmt(std::vector<Stmt*> s, Heisen::Location loc = Heisen::Location()) 
        : Stmt(loc), stmts(std::move(s)) {}
    ~CompoundStmt() {
        for (auto* st : stmts) delete st;
    }
    const std::vector<Stmt*>& getStatements() const { return stmts; }
    void process(AST_Traversal* v) override {}
};

class DeclarationStmt : public Stmt {
    std::vector<VariableDecl*> decls;
public:
    DeclarationStmt(std::vector<VariableDecl*> d, Heisen::Location loc = Heisen::Location()) 
        : Stmt(loc), decls(std::move(d)) {}
    ~DeclarationStmt() {
        for (auto* d : decls) delete d;
    }
    const std::vector<VariableDecl*>& getDeclarations() const { return decls; }
    void process(AST_Traversal* v) override {}
};

class AssignmentStmt : public Stmt {
    Expr* left;
    std::string op;
    Expr* right;
public:
    AssignmentStmt(Expr* l, std::string o, Expr* r, Heisen::Location loc) 
        : Stmt(loc), left(l), op(std::move(o)), right(r) {}
    ~AssignmentStmt() {
        delete left;
        delete right;
    }
    Expr* getLeft() const { return left; }
    const std::string& getOperator() const { return op; }
    Expr* getRight() const { return right; }
    void process(AST_Traversal* v) override {}
};

class ExpressionStmt : public Stmt {
    Expr* expr;
public:
    ExpressionStmt(Expr* e, Heisen::Location loc = Heisen::Location()) 
        : Stmt(loc), expr(e) {}
    ~ExpressionStmt() { delete expr; }
    Expr* getExpression() const { return expr; }
    void process(AST_Traversal* v) override {}
};

class IfStmt : public Stmt {
    Expr* cond;
    Stmt* thenBlk;
    Stmt* elseBlk;
public:
    IfStmt(Expr* c, Stmt* t, Stmt* e, Heisen::Location loc = Heisen::Location()) 
        : Stmt(loc), cond(c), thenBlk(t), elseBlk(e) {}
    ~IfStmt() {
        delete cond;
        delete thenBlk;
        if (elseBlk) delete elseBlk;
    }
    Expr* getCondition() const { return cond; }
    Stmt* getThenBlock() const { return thenBlk; }
    Stmt* getElseBlock() const { return elseBlk; }
    void process(AST_Traversal* v) override {}
};

class MatchCase : public ASTNode {
    Expr* matchExpr;
    Stmt* body;
public:
    MatchCase(Expr* e, Stmt* b, Heisen::Location loc = Heisen::Location()) 
        : ASTNode(loc), matchExpr(e), body(b) {}
    ~MatchCase() {
        delete matchExpr;
        delete body;
    }
    Expr* getMatchExpr() const { return matchExpr; }
    Stmt* getBody() const { return body; }
    void process(AST_Traversal* v) override {}
};

class MatchStmt : public Stmt {
    Expr* matchExpr;
    std::vector<MatchCase*> cases;
public:
    MatchStmt(Expr* e, std::vector<MatchCase*> c, Heisen::Location loc = Heisen::Location()) 
        : Stmt(loc), matchExpr(e), cases(std::move(c)) {}
    ~MatchStmt() {
        delete matchExpr;
        for (auto* cs : cases) delete cs;
    }
    Expr* getMatchExpr() const { return matchExpr; }
    const std::vector<MatchCase*>& getCases() const { return cases; }
    void process(AST_Traversal* v) override {}
};

class LoopStmt : public Stmt {
public:
    LoopStmt() {}
    LoopStmt(Heisen::Location l) : Stmt(l) {}
    virtual ~LoopStmt() = default;
};

class WhileStmt : public LoopStmt {
    Expr* cond;
    Stmt* body;
public:
    WhileStmt(Expr* c, Stmt* b, Heisen::Location loc = Heisen::Location()) 
        : LoopStmt(loc), cond(c), body(b) {}
    ~WhileStmt() {
        delete cond;
        delete body;
    }
    Expr* getCondition() const { return cond; }
    Stmt* getBody() const { return body; }
    void process(AST_Traversal* v) override {}
};

class DoWhileStmt : public LoopStmt {
    Stmt* body;
    Expr* cond;
public:
    DoWhileStmt(Stmt* b, Expr* c, Heisen::Location loc = Heisen::Location()) 
        : LoopStmt(loc), body(b), cond(c) {}
    ~DoWhileStmt() {
        delete body;
        delete cond;
    }
    Stmt* getBody() const { return body; }
    Expr* getCondition() const { return cond; }
    void process(AST_Traversal* v) override {}
};

class ForStmt : public LoopStmt {
    DeclarationStmt* init;
    Expr* cond;
    Expr* update;
    Stmt* body;
public:
    ForStmt(DeclarationStmt* i, Expr* c, Expr* u, Stmt* b, Heisen::Location loc = Heisen::Location())
        : LoopStmt(loc), init(i), cond(c), update(u), body(b) {}
    ~ForStmt() {
        delete init;
        delete cond;
        delete update;
        delete body;
    }
    DeclarationStmt* getInit() const { return init; }
    Expr* getCondition() const { return cond; }
    Expr* getUpdate() const { return update; }
    Stmt* getBody() const { return body; }
    void process(AST_Traversal* v) override {}
};

class JumpStmt : public Stmt {
public:
    JumpStmt() {}
    JumpStmt(Heisen::Location l) : Stmt(l) {}
    virtual ~JumpStmt() = default;
};

class ContinueStmt : public JumpStmt {
public:
    ContinueStmt() {}
    ContinueStmt(Heisen::Location l) : JumpStmt(l) {}
    void process(AST_Traversal* v) override {}
};

class BreakStmt : public JumpStmt {
public:
    BreakStmt() {}
    BreakStmt(Heisen::Location l) : JumpStmt(l) {}
    void process(AST_Traversal* v) override {}
};

class ReturnStmt : public JumpStmt {
    Expr* value;
public:
    ReturnStmt(Heisen::Location l) : JumpStmt(l), value(nullptr) {}
    ReturnStmt(Expr* v, Heisen::Location l) : JumpStmt(l), value(v) {}
    ~ReturnStmt() { if (value) delete value; }
    Expr* getReturnValue() const { return value; }
    bool hasReturnValue() const { return value != nullptr; }
    void process(AST_Traversal* v) override {}
};

class PrintStmt : public Stmt {
    PrintOperationKind kind;
    Expr* arg;
public:
    PrintStmt(PrintOperationKind k, Expr* a, Heisen::Location l) 
        : Stmt(l), kind(k), arg(a) {}
    ~PrintStmt() { if (arg) delete arg; }
    Expr* getArgument() const { return arg; }
    PrintOperationKind getKind() const { return kind; }
    void process(AST_Traversal* v) override {}
};

// ===================================================================
//                    Quantum + Gate Nodes
// ===================================================================

class GateNode : public Quantum {
public:
    GateNode() {}
    GateNode(Heisen::Location l) : Quantum(l) {}
    virtual ~GateNode() = default;
};

class SimpleGateNode : public GateNode {
    GateKind kind;
public:
    SimpleGateNode(GateKind k, Heisen::Location loc = Heisen::Location()) 
        : GateNode(loc), kind(k) {}
    GateKind getGateKind() const { return kind; }
    void process(AST_Traversal* v) override {}
};

class ParametricGateNode : public GateNode {
    GateKind kind;
    std::vector<Expr*> params;
public:
    ParametricGateNode(GateKind k, std::vector<Expr*> p, Heisen::Location loc = Heisen::Location()) 
        : GateNode(loc), kind(k), params(std::move(p)) {}
    ~ParametricGateNode() {
        for (auto* e : params) delete e;
    }
    GateKind getGateKind() const { return kind; }
    const std::vector<Expr*>& getParams() const { return params; }
    void process(AST_Traversal* v) override {}
};

class CompositeGateNode : public GateNode {
    std::vector<GateNode*> gates;
public:
    CompositeGateNode(std::vector<GateNode*> g, Heisen::Location loc = Heisen::Location()) 
        : GateNode(loc), gates(std::move(g)) {}
    ~CompositeGateNode() {
        for (auto* g : gates) delete g;
    }
    const std::vector<GateNode*>& getGates() const { return gates; }
    void process(AST_Traversal* v) override {}
};

class GateCompositionNode : public GateNode {
    GateNode* left;
    GateNode* right;
public:
    GateCompositionNode(GateNode* l, GateNode* r, Heisen::Location loc = Heisen::Location()) 
        : GateNode(loc), left(l), right(r) {}
    ~GateCompositionNode() {
        delete left;
        delete right;
    }
    GateNode* getLeft() const { return left; }
    GateNode* getRight() const { return right; }
    void process(AST_Traversal* v) override {}
};

// ===================================================================
//                    Quantum Statement Nodes
// ===================================================================

class QuantumStmt : public Stmt {
public:
    QuantumStmt() {}
    QuantumStmt(Heisen::Location l) : Stmt(l) {}
    virtual ~QuantumStmt() = default;
};

class ApplyGateStmt : public QuantumStmt {
    GateNode* gate;
    Expr* target;
public:
    ApplyGateStmt(GateNode* g, Expr* t, Heisen::Location loc = Heisen::Location()) 
        : QuantumStmt(loc), gate(g), target(t) {}
    ~ApplyGateStmt() {
        delete gate;
        delete target;
    }
    GateNode* getGate() const { return gate; }
    Expr* getTarget() const { return target; }
    void process(AST_Traversal* v) override {}
};

class MeasureStmt : public QuantumStmt {
    Expr* src;
    Expr* tgt;
public:
    MeasureStmt(Expr* s, Expr* t, Heisen::Location loc = Heisen::Location()) 
        : QuantumStmt(loc), src(s), tgt(t) {}
    ~MeasureStmt() {
        delete src;
        delete tgt;
    }
    Expr* getSource() const { return src; }
    Expr* getTarget() const { return tgt; }
    void process(AST_Traversal* v) override {}
};

class ResetStmt : public QuantumStmt {
    Expr* tgt;
public:
    ResetStmt(Expr* t, Heisen::Location loc = Heisen::Location()) 
        : QuantumStmt(loc), tgt(t) {}
    ~ResetStmt() { delete tgt; }
    Expr* getTarget() const { return tgt; }
    void process(AST_Traversal* v) override {}
};

// ===================================================================
//                     Remaining Declaration Nodes
// ===================================================================

class FunctionDecl : public Decl {
    IdentifierExpr* functionName;
    std::vector<ParameterDecl*> parameters;
    Type* returnType;
    CompoundStmt* body;
public:
    FunctionDecl(IdentifierExpr* functionName, 
                 std::vector<ParameterDecl*> parameters, 
                 Type* returnType, 
                 CompoundStmt* body, 
                 Heisen::Location l)
        : Decl(l), functionName(functionName), parameters(std::move(parameters)), 
          returnType(returnType), body(body) {}
    ~FunctionDecl() {
        delete functionName;
        for (ParameterDecl* param : parameters) delete param;
        if (returnType) delete returnType;
        if (body) delete body;
    }
    IdentifierExpr* getFunctionName() const { return functionName; }
    const std::vector<ParameterDecl*>& getParameters() const { return parameters; }
    Type* getReturnType() const { return returnType; }
    CompoundStmt* getBody() const { return body; }
    void setReturnType(Type* type) { 
        if (returnType) delete returnType;
        returnType = type; 
    }
    void setBody(CompoundStmt* newBody) { 
        if (body) delete body;
        body = newBody; 
    }
    void process(AST_Traversal* v) override {}
};

class GateDecl : public Decl {
    GateNode* gate;
public:
    GateDecl(GateNode* g, Heisen::Location l) : Decl(l), gate(g) {}
    ~GateDecl() { delete gate; }
    GateNode* getGate() const { return gate; }
    void process(AST_Traversal* v) override {}
};

// ===================================================================
//                          Type Nodes
// ===================================================================

class BaseTypeNode : public Type {
    BaseTypeKind baseType;
public:
    BaseTypeNode(BaseTypeKind t, Heisen::Location loc = Heisen::Location()) 
        : Type(Type::Kind::BaseType), baseType(t) {}
    BaseTypeKind getBaseType() const { return baseType; }
    void process(AST_Traversal* v) override {}
};

class ArrayTypeNode : public Type {
    Type* elementType;
    Expr* dimension;
public:
    ArrayTypeNode(Type* e, Expr* d, Heisen::Location loc = Heisen::Location()) 
        : Type(Type::Kind::ArrayType), elementType(e), dimension(d) {}
    ~ArrayTypeNode() {
        delete elementType;
        if (dimension) delete dimension;
    }
    Type* getElementType() const { return elementType; }
    Expr* getDimension() const { return dimension; }
    void process(AST_Traversal* v) override {}
};

class FunctionTypeNode : public Type {
    std::vector<Type*> paramTypes;
    Type* returnType;
public:
    FunctionTypeNode(std::vector<Type*> p, Type* r, Heisen::Location loc = Heisen::Location())
        : Type(Type::Kind::FunctionType), paramTypes(std::move(p)), returnType(r) {}
    ~FunctionTypeNode() {
        for (auto* t : paramTypes) delete t;
        if (returnType) delete returnType;
    }
    const std::vector<Type*>& getParamTypes() const { return paramTypes; }
    Type* getReturnType() const { return returnType; }
    void process(AST_Traversal* v) override {}
};

// LambdaExpr destructor definition (needed after CompoundStmt is defined)
inline LambdaExpr::~LambdaExpr() {
    for (auto* p : params) delete p;
    if (returnType) delete returnType;
    if (body) delete body;
}

#endif // HEISEN_AST_HPP