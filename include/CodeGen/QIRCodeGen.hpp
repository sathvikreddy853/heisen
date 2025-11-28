#ifndef HEISEN_QIR_CODEGEN_HPP
#define HEISEN_QIR_CODEGEN_HPP

#include "AST/AST.hpp"
#include "AST/ASTTraversal.hpp"
#include "Macros.hpp"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

/**
 * @brief QIR Code Generator for Heisen Language
 * * Generates Microsoft QIR (Quantum Intermediate Representation) compliant LLVM IR.
 */
class QIRCodeGen : public RecursiveASTVisitor {
public:
    QIRCodeGen(const std::string& moduleName);
    ~QIRCodeGen();

    // Main entry point
    void generateCode(const std::vector<ASTNode*>& nodes);
    
    // Output methods
    void dump() const;
    void writeToFile(const std::string& filename);
    std::string toString() const;
    
    // Verification
    bool verify(std::string& errorMsg);

    // Visitor methods
    void visit(IntLiteralExpr* node) override;
    void visit(FloatLiteralExpr* node) override;
    void visit(BoolLiteralExpr* node) override;
    void visit(StringLiteralExpr* node) override;
    void visit(IdentifierExpr* node) override;
    void visit(BinaryOpExpr* node) override;
    void visit(UnaryOpExpr* node) override;
    void visit(FunctionCallExpr* node) override;
    void visit(IndexAccessExpr* node) override;
    void visit(CastExpr* node) override;
    void visit(LambdaExpr* node) override;
    void visit(BracedInitList* node) override;
    
    void visit(VariableDecl* node) override;
    void visit(FunctionDecl* node) override;
    
    void visit(CompoundStmt* node) override;
    void visit(DeclarationStmt* node) override;
    void visit(AssignmentStmt* node) override;
    void visit(ExpressionStmt* node) override;
    void visit(IfStmt* node) override;
    void visit(WhileStmt* node) override;
    void visit(DoWhileStmt* node) override;
    void visit(ForStmt* node) override;
    void visit(ReturnStmt* node) override;
    void visit(BreakStmt* node) override;
    void visit(ContinueStmt* node) override;
    void visit(PrintStmt* node) override;
    
    void visit(ApplyGateStmt* node) override;
    void visit(MeasureStmt* node) override;
    void visit(ResetStmt* node) override;
    
    void visit(SimpleGateNode* node) override;
    void visit(ParametricGateNode* node) override;
    void visit(TensoredGateNode* node) override;
    void visit(GateCompositionNode* node) override;

private:
    // LLVM core components
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    
    // Symbol tables
    struct SymbolInfo {
        llvm::Value* value;     // The AllocaInst or GlobalVariable
        llvm::Type* type;       // The LLVM Type
        bool isQuantum;         // Is this a Qubit/Qubit array?
        bool isArray;
        int arraySize;
    };
    
    std::map<std::string, SymbolInfo> symbolTable;
    std::map<std::string, llvm::Function*> functionTable;
    
    // Context
    llvm::Function* currentFunction;
    llvm::BasicBlock* currentBlock;
    llvm::Value* lastValue;
    
    struct LoopContext {
        llvm::BasicBlock* continueBlock;
        llvm::BasicBlock* breakBlock;
    };
    std::stack<LoopContext> loopStack;
    
    // QIR Runtime function definitions
    struct QIRRuntime {
        // Qubit management
        llvm::Function* qubitAllocate;
        llvm::Function* qubitAllocateArray;
        llvm::Function* qubitRelease;
        llvm::Function* qubitReleaseArray;
        
        // Result management
        llvm::Function* resultGetOne;
        llvm::Function* resultGetZero;
        llvm::Function* resultEqual;
        
        // Output / Recording (Standard QIR)
        llvm::Function* recordBool;
        
        // Gates
        llvm::Function* qisX;
        llvm::Function* qisY;
        llvm::Function* qisZ;
        llvm::Function* qisH;
        llvm::Function* qisS;
        llvm::Function* qisT;
        llvm::Function* qisRx;
        llvm::Function* qisRy;
        llvm::Function* qisRz;
        llvm::Function* qisCNOT;
        llvm::Function* qisCZ;
        llvm::Function* qisSWAP;
        llvm::Function* qisCCNOT;
        
        // Measurement
        llvm::Function* qisMeasure; // "m" instruction
        llvm::Function* qisReset;
        
        // Helpers
        llvm::Function* arrayGetElement;
    } runtime;
    
    // Helper methods
    void declareQIRRuntime();
    void declareQubitManagement();
    void declareResultManagement();
    void declareQuantumGates();
    void declareArrayHelpers();
    void declareIOFunctions();
    
    llvm::Type* getLLVMType(Type* type);
    
    void enterScope();
    void exitScope();
    bool declareSymbol(const std::string& name, const SymbolInfo& info);
    SymbolInfo* lookupSymbol(const std::string& name);
    
    // Quantum operations
    llvm::Value* allocateQubit();
    llvm::Value* allocateQubitArray(int size);
    void releaseQubit(llvm::Value* qubit);
    void releaseQubitArray(llvm::Value* qubits, int size);
    
    llvm::Value* applyGate(GateKind gate, llvm::Value* target);
    llvm::Value* applyGate(GateKind gate, llvm::Value* target, const std::vector<llvm::Value*>& params);
    
    llvm::Value* measureQubit(llvm::Value* qubit);
    void resetQubit(llvm::Value* qubit);
    
    // Expression evaluation helpers
    llvm::Value* createBinaryOp(const std::string& op, llvm::Value* left, llvm::Value* right);
    llvm::Value* createUnaryOp(const std::string& op, llvm::Value* operand);
    llvm::Value* createComparison(const std::string& op, llvm::Value* left, llvm::Value* right);
    
    llvm::BasicBlock* createBasicBlock(const std::string& name);
    int nameCounter = 0;
};

#endif // HEISEN_QIR_CODEGEN_HPP
