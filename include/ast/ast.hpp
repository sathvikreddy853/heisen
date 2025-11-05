#ifndef HEISEN_AST_HPP
#define HEISEN_AST_HPP
#include <macros.hpp>

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
//                      Forward Declarations
// ===================================================================

// ---------- Base Nodes ----------
class ASTNode;
class Expr;
class Stmt;
class Decl;
class Type;
class Quantum;

// ---------- Expression Nodes ----------
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

class QuantumStateExpr;
class QuantumStateIdentifier;
class QuantumStateIndexAccess;
class QuantumStateList;

// ---------- Statement Nodes ----------
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
class QuantumStmt;
class ApplyGateStmt;
class MeasureStmt;
class ResetStmt;

// ---------- Declaration Nodes ----------
class VariableDecl;
class ParameterDecl;
class FunctionDecl;
class GateDecl;

// ---------- Type Nodes ----------
class BaseTypeNode;
class ArrayTypeNode;
class FunctionTypeNode; // mentioned in comment

// ---------- Quantum Gate Nodes ----------
class GateNode;
class SimpleGateNode;
class ParametricGateNode;
class CompositeGateNode;
class GateCompositionNode;

// ===================================================================
//
//                       Enum Definitions
//
// ===================================================================

enum struct BaseTypeKind { TYPE_INT, TYPE_FLOAT, TYPE_QUBIT, TYPE_BOOL, TYPE_STRING };

enum struct GateKind {
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

enum struct PrintOperationKind { PRINT, PRINTLN, SCAN };

// ===================================================================
//
//                         Base AST Nodes
//
// ===================================================================

class ASTNode {
    public:
    virtual ~ASTNode () = default;
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
    enum Kind { BaseType, ArrayType };

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

// ---------- Literal Expressions ----------

class LiteralExpr : public Expr {
    public:
    virtual ~LiteralExpr () = default;
};

class IntLiteralExpr : public LiteralExpr {
    private:
    int value;

    public:
    IntLiteralExpr (int v) : value (v) {}
    int getValue () const { return value; }
};

class FloatLiteralExpr : public LiteralExpr {
    private:
    float value;

    public:
    FloatLiteralExpr (float v) : value (v) {}
    float getValue () const { return value; }
};

class BoolLiteralExpr : public LiteralExpr {
    private:
    bool value;

    public:
    BoolLiteralExpr (bool v) : value (v) {}
    bool getValue () const { return value; }
};

class StringLiteralExpr : public LiteralExpr {
    private:
    std::string value;

    public:
    StringLiteralExpr (std::string v) : value (std::move (v)) {}
    const std::string& getValue () const { return value; }
};

// ---------- Identifier Expression ----------

class IdentifierExpr : public Expr {
    private:
    std::string value;

    public:
    IdentifierExpr (std::string v) : value (std::move (v)) {}
    const std::string& getValue () const { return value; }
};

// ---------- Binary/Unary Expressions ----------

class BinaryOpExpr : public Expr {
    private:
    Expr* left;
    Expr* right;
    std::string op;

    public:
    BinaryOpExpr (Expr* left, std::string op, Expr* right)
    : left (left), right (right), op (std::move (op)) {}

    ~BinaryOpExpr () {
        delete left;
        delete right;
    }
    Expr* getLeft () const { return left; }
    Expr* getRight () const { return right; }
    const std::string& getOperator () const { return op; }
};

class UnaryOpExpr : public Expr {
    private:
    std::string op;
    Expr* operand;

    public:
    UnaryOpExpr (std::string op, Expr* operand) : op (std::move (op)), operand (operand) {}

    ~UnaryOpExpr () { delete operand; }
    const std::string& getOperator () const { return op; }
    Expr* getOperand () const { return operand; }
};

// ---------- Call & Access Expressions ----------

class FunctionCallExpr : public Expr {
    private:
    Expr* callee;
    std::vector<Expr*> arguments;

    public:
    FunctionCallExpr (Expr* callee, std::vector<Expr*> args)
    : callee (callee), arguments (std::move (args)) {}

    ~FunctionCallExpr () {
        delete callee;
        for (Expr* arg : arguments) { delete arg; }
    }
    Expr* getCallee () const { return callee; }
    const std::vector<Expr*>& getArgs () const { return arguments; }
    size_t getArgCount () const { return arguments.size (); }
};

class IndexAccessExpr : public Expr {
    private:
    Expr* target;
    Expr* index;

    public:
    IndexAccessExpr (Expr* target, Expr* index) : target (target), index (index) {}

    ~IndexAccessExpr () {
        delete target;
        delete index;
    }
    Expr* getTarget () const { return target; }
    Expr* getIndex () const { return index; }
};

class SliceExpr : public Expr {
    private:
    Expr* start; // Can be nullptr
    Expr* stop;  // Can be nullptr
    Expr* step;  // Can be nullptr
    public:
    SliceExpr (Expr* start, Expr* stop, Expr* step) : start (start), stop (stop), step (step) {}

    ~SliceExpr () {
        if (start) delete start;
        if (stop) delete stop;
        if (step) delete step;
    }
    Expr* getStart () const { return start; }
    Expr* getStop () const { return stop; }
    Expr* getStep () const { return step; }
};

class MemberAccessExpr : public Expr {
    private:
    Expr* target;
    IdentifierExpr* memberName;

    public:
    MemberAccessExpr (Expr* target, IdentifierExpr* memberName)
    : target (target), memberName (memberName) {}

    ~MemberAccessExpr () {
        delete target;
        delete memberName;
    }
    Expr* getTarget () const { return target; }
    IdentifierExpr* getMemberName () const { return memberName; }
};

// ---------- Cast Expression ----------

class CastExpr : public Expr {
    private:
    Type* targetType;
    Expr* expression;

    public:
    CastExpr (Type* type, Expr* expr) : targetType (type), expression (expr) {}

    ~CastExpr () {
        delete targetType;
        delete expression;
    }
    Type* getTargetType () const { return targetType; }
    Expr* getExpression () const { return expression; }
};

// ---------- Quantum State Expressions ----------

class QuantumStateExpr : public Expr {
    public:
    virtual ~QuantumStateExpr () = default;
};

class QuantumStateIdentifier : public QuantumStateExpr {
    private:
    IdentifierExpr* identifier;

    public:
    QuantumStateIdentifier (IdentifierExpr* id) : identifier (id) {}
    ~QuantumStateIdentifier () { delete identifier; }
    IdentifierExpr* getIdentifier () const { return identifier; }
};

class QuantumStateIndexAccess : public QuantumStateExpr {
    private:
    IdentifierExpr* identifier;
    Expr* index;

    public:
    QuantumStateIndexAccess (IdentifierExpr* id, Expr* index) : identifier (id), index (index) {}
    ~QuantumStateIndexAccess () {
        delete identifier;
        delete index;
    }
    IdentifierExpr* getIdentifier () const { return identifier; }
    Expr* getIndex () const { return index; }
};

class QuantumStateList : public QuantumStateExpr {
    private:
    std::vector<QuantumStateExpr*> states;

    public:
    QuantumStateList (std::vector<QuantumStateExpr*> states) : states (std::move (states)) {}
    ~QuantumStateList () {
        for (QuantumStateExpr* state : states) { delete state; }
    }
    const std::vector<QuantumStateExpr*>& getStates () const { return states; }
};

// ===================================================================
//
//                       Statement Nodes
//
// ===================================================================

class CompoundStmt : public Stmt {
    private:
    std::vector<Stmt*> statements;

    public:
    CompoundStmt (std::vector<Stmt*> statements) : statements (std::move (statements)) {}

    ~CompoundStmt () {
        for (Stmt* statement : statements) { delete statement; }
    }
    const std::vector<Stmt*>& getStatements () const { return statements; }
};

class DeclarationStmt : public Stmt {
    private:
    std::vector<VariableDecl*> variableDeclarations;

    public:
    DeclarationStmt (std::vector<VariableDecl*> decls) : variableDeclarations (std::move (decls)) {}

    virtual ~DeclarationStmt () {
        for (VariableDecl* decl : variableDeclarations) { delete decl; }
    }

    const std::vector<VariableDecl*>& getDeclarations () const { return variableDeclarations; }
};

class AssignmentStmt : public Stmt {
    private:
    Expr* left;
    std::string op; // Right now a string, thinking of creating a node for this
    Expr* right;

    public:
    AssignmentStmt (Expr* left, std::string op, Expr* right)
    : left (left), op (std::move (op)), right (right) {}

    virtual ~AssignmentStmt () {
        delete left;
        delete right;
    }

    Expr* getLeft () const { return left; }

    const std::string& getOperator () const { return op; }

    Expr* getRight () const { return right; }
};

class ExpressionStmt : public Stmt {
    private:
    Expr* expression;

    public:
    ExpressionStmt (Expr* expr) : expression (expr) {}

    virtual ~ExpressionStmt () { delete expression; }

    Expr* getExpression () const { return expression; }
};

class IfStmt : public Stmt {
    private:
    Expr* condition;
    Stmt* thenBlock;
    Stmt* elseBlock; // Can be nullptr if there is no 'else' or 'else if'

    public:
    IfStmt (Expr* cond, Stmt* thenBlk, Stmt* elseBlk = nullptr)
    : condition (cond), thenBlock (thenBlk), elseBlock (elseBlk) {}

    virtual ~IfStmt () {
        delete condition;
        delete thenBlock;
        if (elseBlock) { delete elseBlock; }
    }

    Expr* getCondition () const { return condition; }
    Stmt* getThenBlock () const { return thenBlock; }
    Stmt* getElseBlock () const { return elseBlock; }
};

class MatchStmt : public Stmt {
    private:
    Expr* matchExpression; // the scrutinee (the expression after 'match')
    std::vector<MatchCase*> matchCases;

    public:
    MatchStmt (Expr* matchExpression, std::vector<MatchCase*> matchCases)
    : matchExpression (matchExpression), matchCases (std::move (matchCases)) {}

    ~MatchStmt () {
        delete matchExpression;
        for (MatchCase* c : matchCases) delete c;
    }

    Expr* getMatchExpression () const { return matchExpression; }
    const std::vector<MatchCase*>& getMatchCases () const { return matchCases; }
    size_t getCaseCount () const { return matchCases.size (); }
};

class LoopStmt : public Stmt {
    public:
    virtual ~LoopStmt () = default;
};

class WhileStmt : public LoopStmt {
    private:
    Expr* condition;
    Stmt* body;

    public:
    WhileStmt (Expr* cond, Stmt* body) : condition (cond), body (body) {}

    virtual ~WhileStmt () {
        delete condition;
        delete body;
    }

    Expr* getCondition () const { return condition; }
    Stmt* getBody () const { return body; }
};

class DoWhileStmt : public LoopStmt {
    private:
    Stmt* body;
    Expr* condition;

    public:
    DoWhileStmt (Stmt* body, Expr* cond) : body (body), condition (cond) {}

    virtual ~DoWhileStmt () {
        delete body;
        delete condition;
    }

    Stmt* getBody () const { return body; }
    Expr* getCondition () const { return condition; }
};

class ForStmt : public LoopStmt {
    private:
    DeclarationStmt* initializer;
    Expr* condition;
    Expr* update;
    Stmt* body;

    public:
    ForStmt (DeclarationStmt* init, Expr* cond, Expr* update, Stmt* body)
    : initializer (init), condition (cond), update (update), body (body) {}

    virtual ~ForStmt () {
        if (initializer) { delete initializer; }
        if (condition) { delete condition; }
        if (update) { delete update; }
        delete body;
    }

    DeclarationStmt* getInitializer () const { return initializer; }
    Expr* getCondition () const { return condition; }
    Expr* getUpdate () const { return update; }
    Stmt* getBody () const { return body; }
};

class JumpStmt : public Stmt {
    public:
    virtual ~JumpStmt () = default;
};

class ContinueStmt : public JumpStmt {
    public:
    virtual ~ContinueStmt () = default;
};

class BreakStmt : public JumpStmt {
    public:
    virtual ~BreakStmt () = default;
};

class ReturnStmt : public JumpStmt {
    private:
    Expr* returnValue; // Can be nullptr for a void return

    public:
    ReturnStmt (Expr* val = nullptr) : returnValue (val) {}

    virtual ~ReturnStmt () {
        if (returnValue) { delete returnValue; }
    }

    Expr* getReturnValue () const { return returnValue; }

    bool hasReturnValue () const { return returnValue != nullptr; }
};

class PrintStmt : public Stmt {
    private:
    PrintOperationKind kind;
    Expr* argument;

    public:
    PrintStmt (PrintOperationKind opKind, Expr* arg) : kind (opKind), argument (arg) {}
    virtual ~PrintStmt () { delete argument; }

    PrintOperationKind getKind () const { return kind; }

    Expr* getArgument () const { return argument; }
};


class QuantumStmt : public Stmt {
public:
    virtual ~QuantumStmt() = default;
};

class ApplyGateStmt : public QuantumStmt {
private:
    GateNode* gate;
    QuantumStateExpr* target;
public:
    ApplyGateStmt(GateNode* g, QuantumStateExpr* t)
        : gate(g), target(t) {}
    
    ~ApplyGateStmt() { 
        delete gate; 
        delete target; 
    }
    
    GateNode* getGate() const { return gate; }
    QuantumStateExpr* getTarget() const { return target; }
};

class MeasureStmt : public QuantumStmt {
private:
    QuantumStateExpr* source;  // The state to measure
    QuantumStateExpr* target;  // Where to store the measurement result
public:
    MeasureStmt(QuantumStateExpr* src, QuantumStateExpr* tgt)
        : source(src), target(tgt) {}
    
    ~MeasureStmt() { 
        delete source; 
        delete target; 
    }
    
    QuantumStateExpr* getSource() const { return source; }
    QuantumStateExpr* getTarget() const { return target; }
};

class ResetStmt : public QuantumStmt {
private:
    QuantumStateExpr* target;  // The quantum state to reset
public:
    ResetStmt(QuantumStateExpr* tgt)
        : target(tgt) {}
    
    ~ResetStmt() { 
        delete target; 
    }
    
    QuantumStateExpr* getTarget() const { return target; }
};

// class LabelStmt : public Stmt {
// private:
//     IdentifierExpr* label;
//     Stmt* statement;

// public:
//     LabelStmt(IdentifierExpr* label, Stmt* statement)
//         : label(label), statement(statement) {}

//     virtual ~LabelStmt() {
//         delete label;
//         delete statement;
//     }

//     IdentifierExpr* getLabel() const { return label; }

//     Stmt* getStatement() const { return statement; }
// };

class MatchCase : public ASTNode {
    private:
    Expr* matchCase; // The expression to match (can be nullptr for 'default')
    Stmt* body;      // The statement or block to execute

    public:
    MatchCase (Expr* matchCase, Stmt* body) : matchCase (matchCase), body (body) {}

    virtual ~MatchCase () {
        if (matchCase) { delete matchCase; }
        delete body;
    }

    Expr* getCase () const { return matchCase; }
    Stmt* getBody () const { return body; }
};

// ===================================================================
//
//                      Declaration Nodes
//
// ===================================================================

class VariableDecl : public Decl {
    private:
    IdentifierExpr* variableName;
    Type* variableType;
    Expr* initializer; // Can be nullptr
    public:
    VariableDecl (IdentifierExpr* name, Type* type, Expr* init)
    : variableName (name), variableType (type), initializer (init) {}

    ~VariableDecl () {
        delete variableName;
        delete variableType;
        delete initializer;
    }

    IdentifierExpr* getVariableName () const { return variableName; }
    Type* getVariableType () const { return variableType; }
    Expr* getInitializer () const { return initializer; }
};

class ParameterDecl : public Decl {
    private:
    IdentifierExpr* parameterName;
    Type* parameterType;

    public:
    ParameterDecl (IdentifierExpr* parameterName, Type* parameterType)
    : parameterName (parameterName), parameterType (parameterType) {}

    ~ParameterDecl () {
        delete parameterName;
        delete parameterType;
    }
    IdentifierExpr* getParameterName () const { return parameterName; }
    Type* getParameterType () const { return parameterType; }
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
};
class GateDecl : public Decl {
private:
    GateNode* gateComposition;  // The gate composition defined in APPLY block
    
public:
    GateDecl(GateNode* composition)
        : gateComposition(composition) {}

    ~GateDecl() {
        delete gateComposition;
    }
    
    GateNode* getGateComposition() const { return gateComposition; }
};


// ===================================================================
//
//                          Type Nodes
//
// ===================================================================

class BaseTypeNode : public Type {
    private:
    BaseTypeKind baseType;

    public:
    BaseTypeNode (BaseTypeKind t) : Type (Type::BaseType), baseType (t) {}
    BaseTypeKind getBaseType () const { return baseType; }
};

class ArrayTypeNode : public Type {
    private:
    Type* elementType;
    Expr* dimension; // Dimension expression (e.g., the '5' in int[5])
    public:
    ArrayTypeNode (Type* elemType, Expr* dim)
    : Type (Type::ArrayType), elementType (elemType), dimension (dim) {}

    ~ArrayTypeNode () {
        delete elementType;
        if (dimension) { delete dimension; }
    }
    Type* getElementType () const { return elementType; }
    Expr* getDimension () const { return dimension; }
};

// ===================================================================
//
//                       Quantum Gate Nodes
//
// ===================================================================

class GateNode : public Quantum {
    public:
    virtual ~GateNode () = default;
};

class SimpleGateNode : public GateNode {
    private:
    GateKind gateKind;

    public:
    SimpleGateNode (GateKind kind) : gateKind (kind) {}
    GateKind getGateKind () const { return gateKind; }
};

class ParametricGateNode : public GateNode {
    private:
    GateKind gateKind;
    std::vector<Expr*> params;

    public:
    ParametricGateNode (GateKind kind, std::vector<Expr*> params)
    : gateKind (kind), params (std::move (params)) {}

    ~ParametricGateNode () {
        for (Expr* expr : params) delete expr;
    }
    GateKind getGateKind () const { return gateKind; }
    const std::vector<Expr*>& getParams () const { return params; }
    size_t getParamCount () const { return params.size (); }
};

class CompositeGateNode : public GateNode {
    private:
    std::vector<GateNode*> gates;

    public:
    CompositeGateNode (std::vector<GateNode*> gates) : gates (std::move (gates)) {}

    ~CompositeGateNode () {
        for (GateNode* g : gates) delete g;
    }
    const std::vector<GateNode*>& getGates () const { return gates; }
};

class GateCompositionNode : public GateNode {
    private:
    GateNode* leftGate;
    GateNode* rightGate;

    public:
    GateCompositionNode (GateNode* left, GateNode* right) : leftGate (left), rightGate (right) {}

    ~GateCompositionNode () {
        delete leftGate;
        delete rightGate;
    }

    GateNode* getLeftGate () const { return leftGate; }
    GateNode* getRightGate () const { return rightGate; }
};

#endif // HEISEN_AST_HPP