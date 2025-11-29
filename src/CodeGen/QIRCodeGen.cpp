#include "QIRCodeGen.hpp"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FileSystem.h"

QIRCodeGen::QIRCodeGen (const std::string& moduleName)
: currentFunction (nullptr), currentBlock (nullptr), lastValue (nullptr) {
    context = std::make_unique<llvm::LLVMContext> ();
    module  = std::make_unique<llvm::Module> (moduleName, *context);
    builder = std::make_unique<llvm::IRBuilder<>> (*context);

    declareQIRRuntime ();
}

QIRCodeGen::~QIRCodeGen () = default;

void QIRCodeGen::declareQIRRuntime () {
    declareQubitManagement ();
    declareResultManagement ();
    declareQuantumGates ();
    declareArrayHelpers ();
    declareIOFunctions ();
}

void QIRCodeGen::declareQubitManagement () {
    llvm::Type* qubitPtrType = llvm::PointerType::getUnqual (*context);
    llvm::Type* arrayPtrType = llvm::PointerType::getUnqual (*context);

    // __quantum__rt__qubit_allocate
    runtime.qubitAllocate =
    llvm::Function::Create (llvm::FunctionType::get (qubitPtrType, {}, false),
    llvm::Function::ExternalLinkage, "__quantum__rt__qubit_allocate", module.get ());

    // __quantum__rt__qubit_allocate_array(i64)
    runtime.qubitAllocateArray = llvm::Function::Create (
    llvm::FunctionType::get (arrayPtrType, { builder->getInt64Ty () }, false),
    llvm::Function::ExternalLinkage, "__quantum__rt__qubit_allocate_array",
    module.get ());

    // __quantum__rt__qubit_release(%Qubit*)
    runtime.qubitRelease = llvm::Function::Create (
    llvm::FunctionType::get (builder->getVoidTy (), { qubitPtrType }, false),
    llvm::Function::ExternalLinkage, "__quantum__rt__qubit_release", module.get ());

    // __quantum__rt__qubit_release_array(%Array*)
    runtime.qubitReleaseArray = llvm::Function::Create (
    llvm::FunctionType::get (builder->getVoidTy (), { arrayPtrType }, false),
    llvm::Function::ExternalLinkage, "__quantum__rt__qubit_release_array",
    module.get ());
}

void QIRCodeGen::declareResultManagement () {
    llvm::Type* resultPtrType = llvm::PointerType::getUnqual (*context);

    // __quantum__rt__result_get_one
    runtime.resultGetOne =
    llvm::Function::Create (llvm::FunctionType::get (resultPtrType, {}, false),
    llvm::Function::ExternalLinkage, "__quantum__rt__result_get_one", module.get ());

    // __quantum__rt__result_get_zero
    runtime.resultGetZero =
    llvm::Function::Create (llvm::FunctionType::get (resultPtrType, {}, false),
    llvm::Function::ExternalLinkage, "__quantum__rt__result_get_zero", module.get ());

    // __quantum__rt__result_equal(%Result*, %Result*) -> i1
    runtime.resultEqual = llvm::Function::Create (
    llvm::FunctionType::get (builder->getInt1Ty (), { resultPtrType, resultPtrType }, false),
    llvm::Function::ExternalLinkage, "__quantum__rt__result_equal", module.get ());

    // __quantum__rt__bool_record_output(i1, i8*)
    // Used to print boolean results to stdout in a QIR compliant way
    llvm::Type* i8Ptr  = llvm::PointerType::getUnqual (*context);
    runtime.recordBool = llvm::Function::Create (
    llvm::FunctionType::get (builder->getVoidTy (), { builder->getInt1Ty (), i8Ptr }, false),
    llvm::Function::ExternalLinkage, "__quantum__rt__bool_record_output", module.get ());
}

void QIRCodeGen::declareQuantumGates () {
    llvm::Type* qubitPtrType  = llvm::PointerType::getUnqual (*context);
    llvm::Type* resultPtrType = llvm::PointerType::getUnqual (*context);
    llvm::Type* voidTy        = builder->getVoidTy ();
    llvm::Type* doubleTy      = builder->getDoubleTy ();

    auto create1QGate = [&] (const std::string& name) {
        return llvm::Function::Create (
        llvm::FunctionType::get (voidTy, { qubitPtrType }, false),
        llvm::Function::ExternalLinkage, name, module.get ());
    };

    runtime.qisX     = create1QGate ("__quantum__qis__x__body");
    runtime.qisY     = create1QGate ("__quantum__qis__y__body");
    runtime.qisZ     = create1QGate ("__quantum__qis__z__body");
    runtime.qisH     = create1QGate ("__quantum__qis__h__body");
    runtime.qisS     = create1QGate ("__quantum__qis__s__body");
    runtime.qisT     = create1QGate ("__quantum__qis__t__body");
    runtime.qisReset = create1QGate ("__quantum__qis__reset__body");

    llvm::FunctionType* rotType =
    llvm::FunctionType::get (voidTy, { doubleTy, qubitPtrType }, false);
    runtime.qisRx = llvm::Function::Create (rotType,
    llvm::Function::ExternalLinkage, "__quantum__qis__rx__body", module.get ());
    runtime.qisRy = llvm::Function::Create (rotType,
    llvm::Function::ExternalLinkage, "__quantum__qis__ry__body", module.get ());
    runtime.qisRz = llvm::Function::Create (rotType,
    llvm::Function::ExternalLinkage, "__quantum__qis__rz__body", module.get ());

    llvm::FunctionType* twoQType =
    llvm::FunctionType::get (voidTy, { qubitPtrType, qubitPtrType }, false);
    runtime.qisCNOT = llvm::Function::Create (twoQType,
    llvm::Function::ExternalLinkage, "__quantum__qis__cnot__body", module.get ());
    runtime.qisCZ   = llvm::Function::Create (twoQType,
      llvm::Function::ExternalLinkage, "__quantum__qis__cz__body", module.get ());
    runtime.qisSWAP = llvm::Function::Create (twoQType,
    llvm::Function::ExternalLinkage, "__quantum__qis__swap__body", module.get ());

    llvm::FunctionType* threeQType = llvm::FunctionType::get (
    voidTy, { qubitPtrType, qubitPtrType, qubitPtrType }, false);
    runtime.qisCCNOT = llvm::Function::Create (threeQType,
    llvm::Function::ExternalLinkage, "__quantum__qis__ccnot__body", module.get ());

    llvm::FunctionType* measType =
    llvm::FunctionType::get (resultPtrType, { qubitPtrType }, false);
    runtime.qisMeasure = llvm::Function::Create (measType,
    llvm::Function::ExternalLinkage, "__quantum__qis__m__body", module.get ());
}

void QIRCodeGen::declareArrayHelpers () {
    // i8* @__quantum__rt__array_get_element_ptr_1d(%Array*, i64)
    llvm::Type* arrayPtr = llvm::PointerType::getUnqual (*context);
    llvm::Type* i8Ptr    = llvm::PointerType::getUnqual (*context); // i8*

    runtime.arrayGetElement = llvm::Function::Create (
    llvm::FunctionType::get (i8Ptr, { arrayPtr, builder->getInt64Ty () }, false),
    llvm::Function::ExternalLinkage, "__quantum__rt__array_get_element_ptr_1d",
    module.get ());
}

void QIRCodeGen::declareIOFunctions () {
    // printf for debugging
    llvm::Type* i8PtrType = llvm::PointerType::getUnqual (*context);
    llvm::FunctionType* printfType =
    llvm::FunctionType::get (builder->getInt32Ty (), { i8PtrType }, true);

    module->getOrInsertFunction ("printf", printfType);
}

void QIRCodeGen::generateCode (const std::vector<ASTNode*>& nodes) {
    llvm::FunctionType* mainType =
    llvm::FunctionType::get (builder->getVoidTy (), {}, false);
    llvm::Function* mainFunc = llvm::Function::Create (
    mainType, llvm::Function::ExternalLinkage, "main", module.get ());

    mainFunc->addFnAttr ("EntryPoint");

    llvm::BasicBlock* entry = llvm::BasicBlock::Create (*context, "entry", mainFunc);
    builder->SetInsertPoint (entry);

    currentFunction = mainFunc;
    currentBlock    = entry;

    for (auto* node : nodes) {
        if (auto* funcDecl = dynamic_cast<FunctionDecl*> (node)) {
            // CASE A: It's a Function Definition
            // We cannot define a function *inside* another function in LLVM.
            // We must pause 'main' generation, generate the new function, then
            // resume 'main'. a. Save current spot in 'main'
            llvm::BasicBlock* mainInsertBlock = builder->GetInsertBlock ();

            // b. Generate the helper function
            // (The visitor will change 'currentFunction' to the new func, then reset it)
            funcDecl->visit (this);

            // c. Restore 'main' context
            currentFunction = mainFunc;
            builder->SetInsertPoint (mainInsertBlock);

        } else {
            // CASE B: Top-Level Statement/Expression/Variable
            // Generate this code directly into the body of 'main'
            node->visit (this);
        }
    }

    if (!builder->GetInsertBlock ()->getTerminator ()) {
        builder->CreateRetVoid ();
    }

    currentFunction = nullptr;
}

void QIRCodeGen::visit (FunctionDecl* node) {
    std::string funcName = node->getFunctionName ()->getValue ();

    std::vector<llvm::Type*> paramTypes;
    for (auto* param : node->getParameters ()) {
        paramTypes.push_back (getLLVMType (param->getType ()));
    }

    llvm::Type* returnType = getLLVMType (node->getReturnType ());
    llvm::FunctionType* funcType = llvm::FunctionType::get (returnType, paramTypes, false);

    llvm::Function* func = llvm::Function::Create (
    funcType, llvm::Function::ExternalLinkage, funcName, module.get ());

    if (funcName == "main" || funcName == "Main") {
        func->addFnAttr ("EntryPoint");
    }

    // 4. Register in Function Table
    functionTable[funcName] = func;

    // 5. Generate Body (if it exists)
    if (node->getBody ()) {
        currentFunction = func;

        // Create entry block
        llvm::BasicBlock* entry = llvm::BasicBlock::Create (*context, "entry", func);
        builder->SetInsertPoint (entry);
        currentBlock = entry;

        // Handle Parameters (create local variables for them)
        auto paramIter = node->getParameters ().begin ();
        for (auto& arg : func->args ()) {
            std::string paramName = (*paramIter)->getName ()->getValue ();
            arg.setName (paramName);

            // Allocate space for the parameter
            llvm::AllocaInst* alloca =
            builder->CreateAlloca (arg.getType (), nullptr, paramName);
            builder->CreateStore (&arg, alloca);

            // Register symbol
            bool isQuantum = arg.getType ()->isPointerTy ();
            SymbolInfo info{ alloca, arg.getType (), isQuantum, false, 0 };
            declareSymbol (paramName, info);

            ++paramIter;
        }

        // Visit the function body
        node->getBody ()->visit (this);

        // Ensure the block is terminated (add 'ret void' if missing)
        if (!currentBlock->getTerminator ()) {
            if (returnType->isVoidTy ()) {
                builder->CreateRetVoid ();
            } else {
                builder->CreateRet (llvm::Constant::getNullValue (returnType));
            }
        }

        currentFunction = nullptr;
    }
}

void QIRCodeGen::visit (VariableDecl* node) {
    std::string varName = node->getVariableName ()->getValue ();

    bool isQuantum = false;
    bool isArray   = false;
    int arraySize  = 0;

    if (auto* base = dynamic_cast<BaseTypeNode*> (node->getVariableType ())) {
        if (base->getBaseType () == BaseTypeKind::QUBIT) isQuantum = true;
    } else if (auto* arr = dynamic_cast<ArrayTypeNode*> (node->getVariableType ())) {
        isArray = true;
        if (auto* baseElem = dynamic_cast<BaseTypeNode*> (arr->getElementType ())) {
            if (baseElem->getBaseType () == BaseTypeKind::QUBIT)
                isQuantum = true;
        }

        if (arr->getDimension ()) {
            arr->getDimension ()->visit (this);
            if (auto* cInt = llvm::dyn_cast<llvm::ConstantInt> (lastValue)) {
                arraySize = cInt->getSExtValue ();
            }
        }
    }

    llvm::Value* storage = nullptr;
    llvm::Type* llvmType = getLLVMType (node->getVariableType ());

    if (currentFunction) {
        if (isQuantum) {
            if (isArray) {
                storage = allocateQubitArray (arraySize);
            } else {
                storage = allocateQubit ();
            }

            // For quantum resources, we map the name directly to the resource pointer,
            // NOT an alloca that holds the pointer (simplifies access).
            // But to be consistent with assignment logic, let's use alloca to hold the pointer.
            llvm::AllocaInst* ptrStorage =
            builder->CreateAlloca (llvmType, nullptr, varName);
            builder->CreateStore (storage, ptrStorage);
            storage = ptrStorage;
        } else {
            storage = builder->CreateAlloca (llvmType, nullptr, varName);
            if (node->getInitializer ()) {
                node->getInitializer ()->visit (this);
                if (lastValue) builder->CreateStore (lastValue, storage);
            }
        }
    } else {
        std::cerr << "Global variables not fully supported in this snippet.\n";
        return;
    }

    SymbolInfo info{ storage, llvmType, isQuantum, isArray, arraySize };
    declareSymbol (varName, info);
}

void QIRCodeGen::visit (IndexAccessExpr* node) {
    node->getTarget ()->visit (this);
    llvm::Value* arrayPtr = lastValue;

    node->getIndex ()->visit (this);
    llvm::Value* idx = lastValue;

    if (!arrayPtr || !idx) {
        lastValue = nullptr;
        return;
    }

    llvm::Value* rawSlotPtr =
    builder->CreateCall (runtime.arrayGetElement, { arrayPtr, idx });

    // We need to know if this is an array of Qubits or standard types.
    // For now, assuming Qubit Array based on usage context is tricky in AST visitor
    // without full type propagation.
    // HOWEVER, QIR Qubits are opaque pointers (%Qubit*).
    // The array stores these pointers.
    // So rawSlotPtr points to a %Qubit*. Effectively, it is a %Qubit**.

    // Cast i8* to %Qubit** (pointer to opaque pointer)
    llvm::Type* qubitPtrTy = llvm::PointerType::getUnqual (*context); // %Qubit*
    llvm::Type* qubitSlotTy = llvm::PointerType::getUnqual (
    *context); // %Qubit** (opaque pointers in LLVM 15+ are just ptr)

    // In opaque pointer world (LLVM 15+), bitcasts are implicit, but for safety/older LLVM:
    llvm::Value* typedSlotPtr = builder->CreateBitCast (rawSlotPtr, qubitSlotTy);

    // LOAD the actual Qubit (%Qubit*) from the slot
    lastValue = builder->CreateLoad (qubitPtrTy, typedSlotPtr);
}

void QIRCodeGen::visit (ApplyGateStmt* node) {
    // 1. Get Target Qubit
    // If target is "q[0]", IndexAccessExpr visitor will execute:
    //   -> Get Array Pointer
    //   -> Get Slot Pointer
    //   -> LOAD Qubit*
    // Resulting 'lastValue' is %Qubit*
    node->getTarget ()->visit (this);
    llvm::Value* targetQubit = lastValue;

    if (!targetQubit) {
        std::cerr << "Error: Invalid gate target.\n";
        return;
    }

    // 2. Apply Gate
    if (auto* simple = dynamic_cast<SimpleGateNode*> (node->getGate ())) {
        applyGate (simple->getGateKind (), targetQubit);
    } else if (auto* param = dynamic_cast<ParametricGateNode*> (node->getGate ())) {
        std::vector<llvm::Value*> args;
        for (auto* p : param->getParams ()) {
            p->visit (this);
            args.push_back (lastValue);
        }
        applyGate (param->getGateKind (), targetQubit, args);
    }
}

void QIRCodeGen::visit (MeasureStmt* node) {
    // 1. Get Source Qubit
    node->getSource ()->visit (this);
    llvm::Value* qubit = lastValue;

    if (!qubit) return;

    // 2. Measure (Returns %Result*)
    llvm::Value* resultPtr = measureQubit (qubit);

    // 3. Store result in boolean variable (if target exists)
    if (node->getTarget ()) {
        auto* targetId = dynamic_cast<IdentifierExpr*> (node->getTarget ());
        if (targetId) {
            auto* sym = lookupSymbol (targetId->getValue ());
            if (sym) {
                // Check if result == One
                llvm::Value* oneObj = builder->CreateCall (runtime.resultGetOne);
                llvm::Value* isEqual =
                builder->CreateCall (runtime.resultEqual, { resultPtr, oneObj });

                // Store boolean
                builder->CreateStore (isEqual, sym->value);
            }
        }
    }
}

void QIRCodeGen::visit (ResetStmt* node) {
    node->getTarget ()->visit (this);
    llvm::Value* qubit = lastValue;

    if (!qubit) {
        std::cerr << "Error: Invalid reset target.\n";
        return;
    }

    builder->CreateCall (runtime.qisReset, { qubit });
}

void QIRCodeGen::visit (PrintStmt* node) {
    // QIR compliant printing for results is usually `__quantum__rt__bool_record_output`
    // But for general printing, we use printf.

    if (node->getArgument ()) {
        node->getArgument ()->visit (this);
        llvm::Value* val = lastValue;

        if (val) {
            // Check if we are printing a boolean from a measurement
            if (val->getType ()->isIntegerTy (1)) {
                // Use QIR output recording for booleans (standard way to see output in runner)
                llvm::Value* nullPtr = llvm::ConstantPointerNull::get (
                llvm::PointerType::getUnqual (*context));
                builder->CreateCall (runtime.recordBool, { val, nullPtr });
            } else {
                // Fallback to printf for other types
                llvm::Function* printfF = module->getFunction ("printf");
                llvm::Value* fmt;
                if (val->getType ()->isIntegerTy ())
                    fmt = builder->CreateGlobalString ("%lld\n");
                else if (val->getType ()->isDoubleTy ())
                    fmt = builder->CreateGlobalString ("%f\n");
                else
                    fmt = builder->CreateGlobalString ("%s\n");

                builder->CreateCall (printfF, { fmt, val });
            }
        }
    }
}

llvm::Value* QIRCodeGen::allocateQubit () {
    return builder->CreateCall (runtime.qubitAllocate);
}

llvm::Value* QIRCodeGen::allocateQubitArray (int size) {
    return builder->CreateCall (runtime.qubitAllocateArray,
    { llvm::ConstantInt::get (builder->getInt64Ty (), size) });
}

llvm::Value* QIRCodeGen::measureQubit (llvm::Value* qubit) {
    return builder->CreateCall (runtime.qisMeasure, { qubit });
}

llvm::Value* QIRCodeGen::applyGate (GateKind gate, llvm::Value* target) {
    llvm::Function* f = nullptr;
    switch (gate) {
    case GateKind::X: f = runtime.qisX; break;
    case GateKind::Y: f = runtime.qisY; break;
    case GateKind::Z: f = runtime.qisZ; break;
    case GateKind::H: f = runtime.qisH; break;
    case GateKind::S: f = runtime.qisS; break;
    case GateKind::T: f = runtime.qisT; break;
    default: return nullptr;
    }
    return builder->CreateCall (f, { target });
}

llvm::Value* QIRCodeGen::applyGate (GateKind gate,
llvm::Value* target,
const std::vector<llvm::Value*>& params) {
    llvm::Function* f = nullptr;
    switch (gate) {
    case GateKind::RX: f = runtime.qisRx; break;
    case GateKind::RY: f = runtime.qisRy; break;
    case GateKind::RZ: f = runtime.qisRz; break;
    default: return nullptr;
    }
    if (params.empty ()) return nullptr;
    return builder->CreateCall (f, { params[0], target });
}

// Basic type mapping
llvm::Type* QIRCodeGen::getLLVMType (Type* type) {
    if (!type) return builder->getVoidTy ();
    if (auto* b = dynamic_cast<BaseTypeNode*> (type)) {
        switch (b->getBaseType ()) {
        case BaseTypeKind::INT: return builder->getInt64Ty ();
        case BaseTypeKind::FLOAT: return builder->getDoubleTy ();
        case BaseTypeKind::BOOL: return builder->getInt1Ty ();
        // Qubits are passed as pointers
        case BaseTypeKind::QUBIT:
            return llvm::PointerType::getUnqual (*context);
        default: return builder->getVoidTy ();
        }
    }
    // Arrays are opaque pointers in QIR
    if (dynamic_cast<ArrayTypeNode*> (type)) {
        return llvm::PointerType::getUnqual (*context);
    }
    return builder->getVoidTy ();
}

// Basic symbol table
void QIRCodeGen::enterScope () { /* TODO: Implement stack */
}
void QIRCodeGen::exitScope () { /* TODO: Implement stack */
}

bool QIRCodeGen::declareSymbol (const std::string& name, const SymbolInfo& info) {
    symbolTable[name] = info;
    return true;
}

QIRCodeGen::SymbolInfo* QIRCodeGen::lookupSymbol (const std::string& name) {
    if (symbolTable.count (name)) return &symbolTable[name];
    return nullptr;
}

// Visitors for other expressions (boilerplate kept simple for brevity)
void QIRCodeGen::visit (IntLiteralExpr* n) {
    lastValue = llvm::ConstantInt::get (builder->getInt64Ty (), n->getValue ());
}
void QIRCodeGen::visit (FloatLiteralExpr* n) {
    lastValue = llvm::ConstantFP::get (builder->getDoubleTy (), n->getValue ());
}
void QIRCodeGen::visit (BoolLiteralExpr* n) {
    lastValue = llvm::ConstantInt::get (builder->getInt1Ty (), n->getValue ());
}
void QIRCodeGen::visit (StringLiteralExpr* n) {
    lastValue = builder->CreateGlobalString (n->getValue ());
}

void QIRCodeGen::visit (IdentifierExpr* node) {
    auto* sym = lookupSymbol (node->getValue ());
    if (sym) {
        // Always load the value from the alloca
        lastValue = builder->CreateLoad (sym->type, sym->value);
    }
}

void QIRCodeGen::visit (BinaryOpExpr* node) {
    node->getLeft ()->visit (this);
    llvm::Value* lhs = lastValue;

    node->getRight ()->visit (this);
    llvm::Value* rhs = lastValue;

    if (!lhs || !rhs) {
        std::cerr << "Error: Invalid operands in binary expression.\n";
        lastValue = nullptr;
        return;
    }

    // Use helper to generate specific LLVM instruction (Add, Sub, Mul, etc.)
    lastValue = createBinaryOp (node->getOperator (), lhs, rhs);
}

void QIRCodeGen::visit (UnaryOpExpr* node) {
    node->getOperand ()->visit (this);
    llvm::Value* operand = lastValue;

    if (!operand) return;

    lastValue = createUnaryOp (node->getOperator (), operand);
}

void QIRCodeGen::visit (FunctionCallExpr* node) {
    auto* id = dynamic_cast<IdentifierExpr*> (node->getCallee ());
    if (!id) {
        std::cerr << "Error: Indirect function calls not supported yet.\n";
        return;
    }
    std::string funcName = id->getValue ();

    llvm::Function* func = module->getFunction (funcName);
    if (!func) {
        std::cerr << "Error: Undefined function '" << funcName << "'\n";
        return;
    }

    std::vector<llvm::Value*> args;
    const auto& argExprs = node->getArgs ();

    if (func->arg_size () != argExprs.size () && !func->isVarArg ()) {
        std::cerr << "Error: Argument count mismatch for '" << funcName << "'\n";
        return;
    }

    for (size_t i = 0; i < argExprs.size (); ++i) {
        argExprs[i]->visit (this);

        if (!lastValue) return;

        if (i < func->arg_size ()) {
            llvm::Type* paramType = func->getArg (i)->getType ();
            if (lastValue->getType () != paramType) {
                if (lastValue->getType ()->isIntegerTy () && paramType->isDoubleTy ()) {
                    lastValue = builder->CreateSIToFP (lastValue, paramType);
                } else if (lastValue->getType ()->isDoubleTy () && paramType->isIntegerTy ()) {
                    lastValue = builder->CreateFPToSI (lastValue, paramType);
                }
            }
        }
        args.push_back (lastValue);
    }

    lastValue = builder->CreateCall (func, args);
}

void QIRCodeGen::visit (CastExpr* node) {
    node->getExpression ()->visit (this);
    llvm::Value* val = lastValue;
    if (!val) return;

    llvm::Type* destTy = getLLVMType (node->getTargetType ());

    // Integer -> Float
    if (val->getType ()->isIntegerTy () && destTy->isDoubleTy ()) {
        lastValue = builder->CreateSIToFP (val, destTy, "cast_si_fp");
    }
    // Float -> Integer
    else if (val->getType ()->isDoubleTy () && destTy->isIntegerTy ()) {
        lastValue = builder->CreateFPToSI (val, destTy, "cast_fp_si");
    }
    // Integer -> Integer (Resize)
    else if (val->getType ()->isIntegerTy () && destTy->isIntegerTy ()) {
        lastValue = builder->CreateIntCast (val, destTy, true, "cast_int");
    }
    // Fallback: BitCast (careful, usually only for pointers)
    else {
        lastValue = builder->CreateBitCast (val, destTy, "bitcast");
    }
}

void QIRCodeGen::visit (LambdaExpr* n) {
    std::cerr
    << "Warning: Lambda expressions are not yet supported in QIR backend.\n";
    lastValue = nullptr;
}

void QIRCodeGen::visit (BracedInitList* n) {
    // Currently treating as a temporary list for vector creation
    // A full implementation would need to know the target type (Array, Struct, etc.)
    // For now, we visit elements but don't aggregate them into an LLVM value.
    for (auto* elem : n->getElements ()) { elem->visit (this); }
    lastValue = nullptr;
}

void QIRCodeGen::visit (CompoundStmt* n) {
    for (auto s : n->getStatements ()) s->visit (this);
}

void QIRCodeGen::visit (DeclarationStmt* n) {
    for (auto d : n->getDeclarations ()) d->visit (this);
}

void QIRCodeGen::visit (AssignmentStmt* n) {
    // Simplified assignment
    auto* id = dynamic_cast<IdentifierExpr*> (n->getLeft ());
    if (id) {
        auto* sym = lookupSymbol (id->getValue ());
        n->getRight ()->visit (this);
        if (sym && lastValue) builder->CreateStore (lastValue, sym->value);
    }
}

void QIRCodeGen::visit (ExpressionStmt* node) {
    if (node->getExpression ()) { node->getExpression ()->visit (this); }
}

void QIRCodeGen::visit (IfStmt* node) {
    node->getCondition ()->visit (this);
    llvm::Value* cond = lastValue;

    if (!cond) return;

    // Ensure condition is i1 (boolean)
    if (!cond->getType ()->isIntegerTy (1)) {
        cond = builder->CreateICmpNE (
        cond, llvm::Constant::getNullValue (cond->getType ()), "to_bool");
    }

    llvm::Function* func = builder->GetInsertBlock ()->getParent ();

    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create (*context, "if.then", func);
    llvm::BasicBlock* elseBB =
    node->getElseBlock () ? llvm::BasicBlock::Create (*context, "if.else", func) : nullptr;
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create (*context, "if.end", func);

    builder->CreateCondBr (cond, thenBB, elseBB ? elseBB : mergeBB);

    builder->SetInsertPoint (thenBB);
    node->getThenBlock ()->visit (this);

    if (!builder->GetInsertBlock ()->getTerminator ()) {
        builder->CreateBr (mergeBB);
    }

    if (elseBB) {
        builder->SetInsertPoint (elseBB);
        node->getElseBlock ()->visit (this);

        if (!builder->GetInsertBlock ()->getTerminator ()) {
            builder->CreateBr (mergeBB);
        }
    }

    builder->SetInsertPoint (mergeBB);
}

void QIRCodeGen::visit (WhileStmt* node) {
    llvm::Function* func = builder->GetInsertBlock ()->getParent ();

    llvm::BasicBlock* condBB = llvm::BasicBlock::Create (*context, "while.cond", func);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create (*context, "while.body", func);
    llvm::BasicBlock* endBB = llvm::BasicBlock::Create (*context, "while.end", func);

    // Jump to condition
    builder->CreateBr (condBB);

    // 1. Condition
    builder->SetInsertPoint (condBB);
    node->getCondition ()->visit (this);
    llvm::Value* cond = lastValue;
    // Cast to bool if needed
    if (!cond->getType ()->isIntegerTy (1)) {
        cond = builder->CreateICmpNE (
        cond, llvm::Constant::getNullValue (cond->getType ()));
    }
    builder->CreateCondBr (cond, bodyBB, endBB);

    // Push loop context for Break/Continue
    loopStack.push ({ condBB, endBB });

    // 2. Body
    builder->SetInsertPoint (bodyBB);
    node->getBody ()->visit (this);
    if (!builder->GetInsertBlock ()->getTerminator ()) {
        builder->CreateBr (condBB);
    }

    loopStack.pop ();

    // 3. End
    builder->SetInsertPoint (endBB);
}

void QIRCodeGen::visit (ForStmt* node) {
    llvm::Function* func = builder->GetInsertBlock ()->getParent ();

    if (node->getInit ()) {
        enterScope ();
        node->getInit ()->visit (this);
    }

    llvm::BasicBlock* condBB = llvm::BasicBlock::Create (*context, "for.cond", func);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create (*context, "for.body", func);
    llvm::BasicBlock* incBB = llvm::BasicBlock::Create (*context, "for.inc", func);
    llvm::BasicBlock* endBB = llvm::BasicBlock::Create (*context, "for.end", func);

    builder->CreateBr (condBB);

    // 2. Condition
    builder->SetInsertPoint (condBB);
    if (node->getCondition ()) {
        node->getCondition ()->visit (this);
        llvm::Value* cond = lastValue;
        if (!cond->getType ()->isIntegerTy (1)) {
            cond = builder->CreateICmpNE (
            cond, llvm::Constant::getNullValue (cond->getType ()));
        }
        builder->CreateCondBr (cond, bodyBB, endBB);
    } else {
        // Infinite loop if no condition
        builder->CreateBr (bodyBB);
    }

    // Push loop context (Continue goes to Increment, Break goes to End)
    loopStack.push ({ incBB, endBB });

    builder->SetInsertPoint (bodyBB);
    node->getBody ()->visit (this);
    if (!builder->GetInsertBlock ()->getTerminator ()) {
        builder->CreateBr (incBB);
    }

    builder->SetInsertPoint (incBB);
    if (node->getUpdate ()) { node->getUpdate ()->visit (this); }
    builder->CreateBr (condBB);

    loopStack.pop ();
    builder->SetInsertPoint (endBB);

    if (node->getInit ()) exitScope ();
}

void QIRCodeGen::visit (DoWhileStmt* node) {
    llvm::Function* func = builder->GetInsertBlock ()->getParent ();

    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create (*context, "dowhile.body", func);
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create (*context, "dowhile.cond", func);
    llvm::BasicBlock* endBB = llvm::BasicBlock::Create (*context, "dowhile.end", func);

    builder->CreateBr (bodyBB);

    loopStack.push ({ condBB, endBB });

    builder->SetInsertPoint (bodyBB);
    node->getBody ()->visit (this);
    if (!builder->GetInsertBlock ()->getTerminator ()) {
        builder->CreateBr (condBB);
    }

    builder->SetInsertPoint (condBB);
    node->getCondition ()->visit (this);
    llvm::Value* cond = lastValue;
    if (!cond->getType ()->isIntegerTy (1)) {
        cond = builder->CreateICmpNE (
        cond, llvm::Constant::getNullValue (cond->getType ()));
    }
    builder->CreateCondBr (cond, bodyBB, endBB);

    loopStack.pop ();
    builder->SetInsertPoint (endBB);
}

void QIRCodeGen::visit (BreakStmt* node) {
    if (loopStack.empty ()) {
        std::cerr << "Error: 'break' statement not within loop.\n";
        return;
    }
    builder->CreateBr (loopStack.top ().breakBlock);

    // Create a new unreachable block so LLVM builder doesn't complain
    // if we try to add instructions after the break in the same block.
    llvm::BasicBlock* nextBB = llvm::BasicBlock::Create (
    *context, "after.break", builder->GetInsertBlock ()->getParent ());
    builder->SetInsertPoint (nextBB);
}

void QIRCodeGen::visit (ContinueStmt* node) {
    if (loopStack.empty ()) {
        std::cerr << "Error: 'continue' statement not within loop.\n";
        return;
    }
    builder->CreateBr (loopStack.top ().continueBlock);

    llvm::BasicBlock* nextBB = llvm::BasicBlock::Create (
    *context, "after.continue", builder->GetInsertBlock ()->getParent ());
    builder->SetInsertPoint (nextBB);
}

void QIRCodeGen::visit (ReturnStmt* node) {
    if (node->hasReturnValue ()) {
        node->getReturnValue ()->visit (this);
        if (lastValue) { builder->CreateRet (lastValue); }
    } else {
        builder->CreateRetVoid ();
    }
}

void QIRCodeGen::visit (SimpleGateNode* n) {
}
void QIRCodeGen::visit (ParametricGateNode* n) {
}
void QIRCodeGen::visit (TensoredGateNode* n) {
}
void QIRCodeGen::visit (GateCompositionNode* n) {
}

llvm::Value*
QIRCodeGen::createBinaryOp (const std::string& op, llvm::Value* lhs, llvm::Value* rhs) {
    bool isFP = lhs->getType ()->isDoubleTy () || rhs->getType ()->isDoubleTy ();

    if (op == "+") {
        return isFP ? builder->CreateFAdd (lhs, rhs) : builder->CreateAdd (lhs, rhs);
    } else if (op == "-") {
        return isFP ? builder->CreateFSub (lhs, rhs) : builder->CreateSub (lhs, rhs);
    } else if (op == "*") {
        return isFP ? builder->CreateFMul (lhs, rhs) : builder->CreateMul (lhs, rhs);
    } else if (op == "/") {
        return isFP ? builder->CreateFDiv (lhs, rhs) : builder->CreateSDiv (lhs, rhs);
    } else if (op == "%") {
        return builder->CreateSRem (lhs, rhs);
    }

    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
        return createComparison (op, lhs, rhs);
    }

    if (op == "&&") return builder->CreateAnd (lhs, rhs);
    if (op == "||") return builder->CreateOr (lhs, rhs);

    return nullptr;
}

llvm::Value* QIRCodeGen::createUnaryOp (const std::string& op, llvm::Value* operand) {
    if (op == "-") {
        return operand->getType ()->isDoubleTy () ? builder->CreateFNeg (operand) :
                                                    builder->CreateNeg (operand);
    }
    if (op == "!") { return builder->CreateNot (operand); }
    return nullptr;
}

llvm::Value*
QIRCodeGen::createComparison (const std::string& op, llvm::Value* lhs, llvm::Value* rhs) {
    bool isFP = lhs->getType ()->isDoubleTy ();

    if (isFP) {
        if (op == "==") return builder->CreateFCmpOEQ (lhs, rhs);
        if (op == "!=") return builder->CreateFCmpONE (lhs, rhs);
        if (op == "<") return builder->CreateFCmpOLT (lhs, rhs);
        if (op == "<=") return builder->CreateFCmpOLE (lhs, rhs);
        if (op == ">") return builder->CreateFCmpOGT (lhs, rhs);
        if (op == ">=") return builder->CreateFCmpOGE (lhs, rhs);
    } else {
        if (op == "==") return builder->CreateICmpEQ (lhs, rhs);
        if (op == "!=") return builder->CreateICmpNE (lhs, rhs);
        if (op == "<") return builder->CreateICmpSLT (lhs, rhs);
        if (op == "<=") return builder->CreateICmpSLE (lhs, rhs);
        if (op == ">") return builder->CreateICmpSGT (lhs, rhs);
        if (op == ">=") return builder->CreateICmpSGE (lhs, rhs);
    }
    return nullptr;
}

// Output
void QIRCodeGen::writeToFile (const std::string& filename) {
    std::error_code EC;
    llvm::raw_fd_ostream dest (filename, EC, llvm::sys::fs::OF_None);
    if (!EC) module->print (dest, nullptr);
}
bool QIRCodeGen::verify (std::string& err) {
    llvm::raw_string_ostream os (err);
    return !llvm::verifyModule (*module, &os);
}
