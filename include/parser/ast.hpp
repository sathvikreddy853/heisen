#ifndef HEISEN_AST_HPP
#define HEISEN_AST_HPP

#include <macros.hpp>
#include <string>
#include <vector>
#include <ast_traversal.hpp>

// ===================================================================
//
//                  Abstract Syntax Tree (AST) Hierarchy
//
// ===================================================================
//
// ASTNode (Base node for all)
// ├── Expr (Base for all expressions)
// │   ├── LiteralExpr (Abstract base for literals)
// │   │   ├── IntLiteralExpr (INT_LITERAL)
// │   │   ├── FloatLiteralExpr (FLOAT_LITERAL)
// │   │   ├── StringLiteralExpr (STRING_LITERAL)
// │   │   └── BoolLiteralExpr (TRUE, FALSE)
// │   ├── IdentifierExpr (IDENTIFIER)
// │   ├── BinaryOpExpr (+, -, *, /, AND, OR, etc.)
// │   ├── UnaryOpExpr (+, -, !, NOT)
// │   ├── FunctionCallExpr (expr(expr_list))
// │   ├── IndexAccessExpr (expr[index_expr])
// │   ├── SliceExpr (start:stop:step)
// │   ├── MemberAccessExpr (expr.IDENTIFIER)
// │   ├── CastExpr (CAST<type>(expr))
// │   └── QuantumStateExpr (Represents 'quantum_state')
// │       ├── QuantumStateIdentifier (IDENTIFIER)
// │       ├── QuantumStateIndexAccess (IDENTIFIER[index_expr])
// │       └── QuantumStateList ([quantum_state_list])
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
// │
// ├── Decl (Base for top-level declarations and variable definitions)
// │   ├── FunctionDecl (FUNC ident(params) [: type] { ... })
// │   ├── GateDecl (GATE { APPLY: ... })
// │   ├── VariableDecl (From 'variable_declarations', holds idents, type, init)
// │   └── ParameterDecl (IDENTIFIER: type, used in FunctionDecl)
// │
// ├── Type (Base for all type-specifier nodes)
// │   ├── BaseTypeNode (INT, FLOAT, QUBIT, etc.)
// │   └── ArrayTypeNode (type[dimension_expr])
// |   └── FunctionTypeNode
// │
// └── Quantum (Base for quantum-specific syntax nodes)
//     └── GateNode (Abstract base for 'quantum_gate')
//         ├── SimpleGateNode (GATE_H, GATE_X, etc.)
//         ├── ParametricGateNode (GATE_RX(expr))
//         ├── CompositeGateNode ([gate_list])
//         └── GateCompositionNode (gate @ gate)
//

// ===================================================================
//
//                       Enum Definitions
//
// ===================================================================





// ===================================================================
//
//                      Forward Declarations
//
// ===================================================================

// --- Forward declare only where truly necessary ---
class Expr;
class Stmt;
class Type;
class Decl;
class Quantum;
class GateNode;
class QuantumStateExpr;
class MatchCase;
class VariableDecl;

// ===================================================================
//
//                       Enum Definitions
//
// ===================================================================

enum class BaseTypeKind { TYPE_INT, TYPE_FLOAT, TYPE_QUBIT, TYPE_BOOL, TYPE_STRING };

enum class GateKind {
    H,
    S,
    T,
    I,
    X,
    Y,
    Z,
    RX,
    RY,
    RZ,
    CNOT,
    CZ,
    SWAP,
    CSWAP,
    CCNOT,
    CRX,
    CRY,
    CRZ,
    UNKNOWN,
    CTRL
};

enum class PrintOperationKind { PRINT, PRINTLN, SCAN };

// ===================================================================
//
//                         Base AST Nodes
//
// ===================================================================

class ASTNode {
    public:
    virtual ~ASTNode () = default;
    virtual void process(AST_Traversal* Visitor) = 0;
};

class Expr : public ASTNode {
    public:
    virtual ~Expr () = default;
};

class Stmt : public ASTNode {
    public:
    virtual ~Stmt () = default;
};

class Decl : public ASTNode {
    public:
    virtual ~Decl () = default;
};

class Type : public ASTNode {
    public:
    enum class Kind { BaseType, ArrayType };

    protected:
    Kind kind;

    public:
    Type (Kind k) : kind (k) {}
    Kind getKind () const { return kind; }
};

class Quantum : public ASTNode {
    public:
    virtual ~Quantum () = default;
};

// ===================================================================
//
//                      Expression Nodes
//
// ===================================================================

class LiteralExpr : public Expr {
    public:
    virtual ~LiteralExpr () = default;
};

class IntLiteralExpr : public LiteralExpr {
    int value;

    public:
    IntLiteralExpr (int v) : value (v) {}
    int getValue () const { return value; }
    void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class FloatLiteralExpr : public LiteralExpr {
    float value;

    public:
    FloatLiteralExpr (float v) : value (v) {}
    float getValue () const { return value; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class BoolLiteralExpr : public LiteralExpr {
    bool value;

    public:
    BoolLiteralExpr (bool v) : value (v) {}
    bool getValue () const { return value; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class StringLiteralExpr : public LiteralExpr {
    std::string value;

    public:
    StringLiteralExpr (std::string v) : value (std::move (v)) {}
    const std::string& getValue () const { return value; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class IdentifierExpr : public Expr {
    std::string value;

    public:
    IdentifierExpr (std::string v) : value (std::move (v)) {}
    const std::string& getValue () const { return value; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class BinaryOpExpr : public Expr {
    Expr* left;
    Expr* right;
    std::string op;

    public:
    BinaryOpExpr (Expr* l, std::string o, Expr* r) : left (l), right (r), op (std::move (o)) {}
    ~BinaryOpExpr () {
        delete left;
        delete right;
    }
    Expr* getLeft () const { return left; }
    Expr* getRight () const { return right; }
    const std::string& getOperator () const { return op; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class UnaryOpExpr : public Expr {
    std::string op;
    Expr* operand;

    public:
    UnaryOpExpr (std::string o, Expr* e) : op (std::move (o)), operand (e) {}
    ~UnaryOpExpr () { delete operand; }
    const std::string& getOperator () const { return op; }
    Expr* getOperand () const { return operand; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class FunctionCallExpr : public Expr {
    Expr* callee;
    std::vector<Expr*> args;

    public:
    FunctionCallExpr (Expr* c, std::vector<Expr*> a) : callee (c), args (std::move (a)) {}
    ~FunctionCallExpr () {
        delete callee;
        for (auto* a : args) delete a;
    }
    Expr* getCallee () const { return callee; }
    const std::vector<Expr*>& getArgs () const { return args; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class IndexAccessExpr : public Expr {
    Expr* target;
    Expr* index;

    public:
    IndexAccessExpr (Expr* t, Expr* i) : target (t), index (i) {}
    ~IndexAccessExpr () {
        delete target;
        delete index;
    }
    Expr* getTarget () const { return target; }
    Expr* getIndex () const { return index; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class SliceExpr : public Expr {
    Expr* start;
    Expr* stop;
    Expr* step;

    public:
    SliceExpr (Expr* s, Expr* e, Expr* st) : start (s), stop (e), step (st) {}
    ~SliceExpr () {
        delete start;
        delete stop;
        delete step;
    }
    Expr* getStart () const { return start; }
    Expr* getStop () const { return stop; }
    Expr* getStep () const { return step; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class MemberAccessExpr : public Expr {
    Expr* target;
    IdentifierExpr* member;

    public:
    MemberAccessExpr (Expr* t, IdentifierExpr* m) : target (t), member (m) {}
    ~MemberAccessExpr () {
        delete target;
        delete member;
    }
    Expr* getTarget () const { return target; }
    IdentifierExpr* getMemberName () const { return member; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class CastExpr : public Expr {
    Type* targetType;
    Expr* expr;

    public:
    CastExpr (Type* t, Expr* e) : targetType (t), expr (e) {}
    ~CastExpr () {
        delete targetType;
        delete expr;
    }
    Type* getTargetType () const { return targetType; }
    Expr* getExpression () const { return expr; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

// QuantumStateExpr must be fully defined before any Stmt uses it
class QuantumStateExpr : public Expr {
    public:
    virtual ~QuantumStateExpr () = default;
};

class QuantumStateIdentifier : public QuantumStateExpr {
    IdentifierExpr* identifier;

    public:
    QuantumStateIdentifier (IdentifierExpr* id) : identifier (id) {}
    ~QuantumStateIdentifier () { delete identifier; }
    IdentifierExpr* getIdentifier () const { return identifier; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class QuantumStateIndexAccess : public QuantumStateExpr {
    IdentifierExpr* identifier;
    Expr* index;

    public:
    QuantumStateIndexAccess (IdentifierExpr* id, Expr* idx) : identifier (id), index (idx) {}
    ~QuantumStateIndexAccess () {
        delete identifier;
        delete index;
    }
    IdentifierExpr* getIdentifier () const { return identifier; }
    Expr* getIndex () const { return index; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class QuantumStateList : public QuantumStateExpr {
    std::vector<QuantumStateExpr*> states;

    public:
    QuantumStateList (std::vector<QuantumStateExpr*> s) : states (std::move (s)) {}
    ~QuantumStateList () {
        for (auto* s : states) delete s;
    }
    const std::vector<QuantumStateExpr*>& getStates () const { return states; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

// ===================================================================
//
//                       Declaration Nodes
//
// ===================================================================

class VariableDecl : public Decl {
    IdentifierExpr* name;
    Type* type;
    Expr* init;

    public:
    VariableDecl (IdentifierExpr* n, Type* t, Expr* i) : name (n), type (t), init (i) {}
    ~VariableDecl () {
        delete name;
        delete type;
        delete init;
    }
    IdentifierExpr* getVariableName () const { return name; }
    Type* getVariableType () const { return type; }
    Expr* getInitializer () const { return init; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

// Needed before DeclarationStmt
class MatchCase; // forward already fine
class GateNode;  // forward already fine

// ===================================================================
//
//                       Statement Nodes
//
// ===================================================================

class CompoundStmt : public Stmt {
    std::vector<Stmt*> stmts;

    public:
    CompoundStmt (std::vector<Stmt*> s) : stmts (std::move (s)) {}
    ~CompoundStmt () {
        for (auto* st : stmts) delete st;
    }
    const std::vector<Stmt*>& getStatements () const { return stmts; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class DeclarationStmt : public Stmt {
    std::vector<VariableDecl*> decls;

    public:
    DeclarationStmt (std::vector<VariableDecl*> d) : decls (std::move (d)) {}
    ~DeclarationStmt () {
        for (auto* d : decls) delete d;
    }
    const std::vector<VariableDecl*>& getDeclarations () const { return decls; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class AssignmentStmt : public Stmt {
    Expr* left;
    std::string op;
    Expr* right;

    public:
    AssignmentStmt (Expr* l, std::string o, Expr* r) : left (l), op (std::move (o)), right (r) {}
    ~AssignmentStmt () {
        delete left;
        delete right;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class ExpressionStmt : public Stmt {
    Expr* expr;

    public:
    ExpressionStmt (Expr* e) : expr (e) {}
    ~ExpressionStmt () { delete expr; }
    Expr* getExpression () const { return expr; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class IfStmt : public Stmt {
    Expr* cond;
    Stmt* thenBlk;
    Stmt* elseBlk;

    public:
    IfStmt (Expr* c, Stmt* t, Stmt* e = nullptr) : cond (c), thenBlk (t), elseBlk (e) {}
    ~IfStmt () {
        delete cond;
        delete thenBlk;
        delete elseBlk;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class MatchCase : public ASTNode {
    Expr* matchExpr;
    Stmt* body;

    public:
    MatchCase (Expr* e, Stmt* b) : matchExpr (e), body (b) {}
    ~MatchCase () {
        delete matchExpr;
        delete body;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class MatchStmt : public Stmt {
    Expr* matchExpr;
    std::vector<MatchCase*> cases;

    public:
    MatchStmt (Expr* e, std::vector<MatchCase*> c) : matchExpr (e), cases (std::move (c)) {}
    ~MatchStmt () {
        delete matchExpr;
        for (auto* cs : cases) delete cs;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class LoopStmt : public Stmt {
    public:
    virtual ~LoopStmt () = default;
};

class WhileStmt : public LoopStmt {
    Expr* cond;
    Stmt* body;

    public:
    WhileStmt (Expr* c, Stmt* b) : cond (c), body (b) {}
    ~WhileStmt () {
        delete cond;
        delete body;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class DoWhileStmt : public LoopStmt {
    Stmt* body;
    Expr* cond;

    public:
    DoWhileStmt (Stmt* b, Expr* c) : body (b), cond (c) {}
    ~DoWhileStmt () {
        delete body;
        delete cond;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class ForStmt : public LoopStmt {
    DeclarationStmt* init;
    Expr* cond;
    Expr* update;
    Stmt* body;

    public:
    ForStmt (DeclarationStmt* i, Expr* c, Expr* u, Stmt* b)
    : init (i), cond (c), update (u), body (b) {}
    ~ForStmt () {
        delete init;
        delete cond;
        delete update;
        delete body;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class JumpStmt : public Stmt {
    public:
    virtual ~JumpStmt () = default;
};
class ContinueStmt : public JumpStmt {};
class BreakStmt : public JumpStmt {};

class ReturnStmt : public JumpStmt {
    Expr* value;

    public:
    ReturnStmt (Expr* v = nullptr) : value (v) {}
    ~ReturnStmt () { delete value; }
    Expr* getReturnValue () const { return value; }
    bool hasReturnValue () const { return value != nullptr; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class PrintStmt : public Stmt {
    PrintOperationKind kind;
    Expr* arg;

    public:
    PrintStmt (PrintOperationKind k, Expr* a) : kind (k), arg (a) {}
    ~PrintStmt () { delete arg; }
    Expr* getArgument () const { return arg; }
    PrintOperationKind getKind () const { return kind; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

// ===================================================================
//
//                    Quantum + Gate Nodes
//
// ===================================================================

class GateNode : public Quantum {
    public:
    virtual ~GateNode () = default;
};

class SimpleGateNode : public GateNode {
    GateKind kind;

    public:
    SimpleGateNode (GateKind k) : kind (k) {}
    GateKind getGateKind () const { return kind; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class ParametricGateNode : public GateNode {
    GateKind kind;
    std::vector<Expr*> params;

    public:
    ParametricGateNode (GateKind k, std::vector<Expr*> p) : kind (k), params (std::move (p)) {}
    ~ParametricGateNode () {
        for (auto* e : params) delete e;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class CompositeGateNode : public GateNode {
    std::vector<GateNode*> gates;

    public:
    CompositeGateNode (std::vector<GateNode*> g) : gates (std::move (g)) {}
    ~CompositeGateNode () {
        for (auto* g : gates) delete g;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class GateCompositionNode : public GateNode {
    GateNode* left;
    GateNode* right;

    public:
    GateCompositionNode (GateNode* l, GateNode* r) : left (l), right (r) {}
    ~GateCompositionNode () {
        delete left;
        delete right;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

// ===================================================================
//
//                    Quantum Statement Nodes
//
// ===================================================================

class QuantumStmt : public Stmt {
    public:
    virtual ~QuantumStmt () = default;
};

class ApplyGateStmt : public QuantumStmt {
    GateNode* gate;
    QuantumStateExpr* target;

    public:
    ApplyGateStmt (GateNode* g, QuantumStateExpr* t) : gate (g), target (t) {}
    ~ApplyGateStmt () {
        delete gate;
        delete target;
    }
};

class MeasureStmt : public QuantumStmt {
    QuantumStateExpr* src;
    QuantumStateExpr* tgt;

    public:
    MeasureStmt (QuantumStateExpr* s, QuantumStateExpr* t) : src (s), tgt (t) {}
    ~MeasureStmt () {
        delete src;
        delete tgt;
    }
};

class ResetStmt : public QuantumStmt {
    QuantumStateExpr* tgt;

    public:
    ResetStmt (QuantumStateExpr* t) : tgt (t) {}
    ~ResetStmt () { delete tgt; }
};

// ===================================================================
//
//                     Remaining Declaration Nodes
//
// ===================================================================

class ParameterDecl : public Decl {
    IdentifierExpr* name;
    Type* type;

    public:
    ParameterDecl (IdentifierExpr* n, Type* t) : name (n), type (t) {}
    ~ParameterDecl () {
        delete name;
        delete type;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class FunctionDecl : public Decl {
private:
    IdentifierExpr* functionName;
    std::vector<ParameterDecl*> parameters;
    Type* returnType; // Can be nullptr
    CompoundStmt* body;
    
public:
    // Constructor
    FunctionDecl(IdentifierExpr* functionName, 
                 std::vector<ParameterDecl*> parameters, 
                 Type* returnType, 
                 CompoundStmt* body)
        : functionName(functionName), 
          parameters(std::move(parameters)), 
          returnType(returnType), 
          body(body) {}

    // Destructor
    ~FunctionDecl() {
        delete functionName;
        for (ParameterDecl* param : parameters) {
            delete param;
        }
        if (returnType) {
            delete returnType;
        }
        if (body) {
            delete body;
        }
    }
    
    // Getters (const)
    IdentifierExpr* getFunctionName() const { return functionName; }
    const std::vector<ParameterDecl*>& getParameters() const { return parameters; }
    Type* getReturnType() const { return returnType; }
    CompoundStmt* getBody() const { return body; }
    
    // Setters (for grammar actions)
    void setReturnType(Type* type) { 
        if (returnType) {
            delete returnType;
        }
        returnType = type; 
    }
    
    void setBody(CompoundStmt* newBody) { 
        if (body) {
            delete body;
        }
        body = newBody; 
    }
    void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};
class GateDecl : public Decl {
    GateNode* gate;

    public:
    GateDecl (GateNode* g) : gate (g) {}
    ~GateDecl () { delete gate; }
};

// ===================================================================
//
//                          Type Nodes
//
// ===================================================================

class BaseTypeNode : public Type {
    BaseTypeKind baseType;

    public:
    BaseTypeNode (BaseTypeKind t) : Type (Type::Kind::BaseType), baseType (t) {}
    BaseTypeKind getBaseType () const { return baseType; }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

class ArrayTypeNode : public Type {
    Type* elementType;
    Expr* dimension;

    public:
    ArrayTypeNode (Type* e, Expr* d) : Type (Type::Kind::ArrayType), elementType (e), dimension (d) {}
    ~ArrayTypeNode () {
        delete elementType;
        delete dimension;
    }
     void process(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

#endif // HEISEN_AST_HPP
