#include <SemanticAnalyzer.hpp>

namespace Heisen {

void SemanticAnalyzer::initializeBuiltins () {
    // Math functions
    auto* piType = new PrimitiveType (SemanticType::Kind::FLOAT);
    symbolTable.declare ("pi", new Symbol ("pi", piType, Symbol::Kind::VARIABLE, Location ()));

    // Built-in functions
    std::vector<std::string> mathFuncs = { "sqrt", "round", "floor", "ceil", "sin", "cos", "tan", "abs" };
    for (const auto& func : mathFuncs) {
        std::vector<SemanticType*> params;
        params.push_back (new PrimitiveType (SemanticType::Kind::FLOAT));
        PrimitiveType* retType = nullptr;
        if (func == "round" || func == "floor" || func == "ceil") {
            retType  = new PrimitiveType (SemanticType::Kind::INT);
        } else {
            retType  = new PrimitiveType (SemanticType::Kind::FLOAT);
        }
        auto* funcType = new FunctionType (params, retType);
        symbolTable.declare (func, new Symbol (func, funcType, Symbol::Kind::FUNCTION, Location ()));
    }

    // String conversion
    std::vector<SemanticType*> floatParam;
    floatParam.push_back (new PrimitiveType (SemanticType::Kind::FLOAT));
    auto* floatFuncType = new FunctionType (floatParam, new PrimitiveType (SemanticType::Kind::FLOAT));
    symbolTable.declare ("float", new Symbol ("float", floatFuncType, Symbol::Kind::FUNCTION, Location ()));
}

bool SemanticAnalyzer::analyze (std::vector<ASTNode*>& translationUnit) {
    LOG("============ SEM ANALYSIS START=============");
    // First pass: collect all function declarations
    for (auto* node : translationUnit) {
        if (auto* funcDecl = dynamic_cast<FunctionDecl*> (node)) { visitFunctionDecl (funcDecl); }
    }

    // Second pass: analyze function bodies and statements 
    for (auto* node : translationUnit) {
        if (auto* funcDecl = dynamic_cast<FunctionDecl*> (node)) {
            if (funcDecl->getBody ()) {
                currentFunction = funcDecl;
                symbolTable.enterScope ();

                // Add parameters to scope
                for (auto* param : funcDecl->getParameters ()) { visitParameterDecl (param); }

                visitCompoundStmt (funcDecl->getBody ());
                symbolTable.exitScope ();
                currentFunction = nullptr;
            }
        } else if (auto* stmt = dynamic_cast<Stmt*> (node)) {
            if (auto* declStmt = dynamic_cast<DeclarationStmt*> (stmt)) {
                LOG("visitDeclarationStmt");
                visitDeclarationStmt (declStmt);
            } else if (auto* assignStmt = dynamic_cast<AssignmentStmt*> (stmt)) {
                LOG("visitAssignmentStmt");
                visitAssignmentStmt (assignStmt);
            } else if (auto* exprStmt = dynamic_cast<ExpressionStmt*> (stmt)) {
                LOG("visitExpressionStmt");
                visitExpressionStmt (exprStmt);
            } else if (auto* ifStmt = dynamic_cast<IfStmt*> (stmt)) {
                LOG("visitIfStmt");
                visitIfStmt (ifStmt);
            } else if (auto* whileStmt = dynamic_cast<WhileStmt*> (stmt)) {
                LOG("visitWhileStmt");
                visitWhileStmt (whileStmt);
            } else if (auto* doWhileStmt = dynamic_cast<DoWhileStmt*> (stmt)) {
                LOG("visitDoWhileStmt");
                visitDoWhileStmt (doWhileStmt);
            } else if (auto* forStmt = dynamic_cast<ForStmt*> (stmt)) {
                LOG("visitForStmt");
                visitForStmt (forStmt);
            } else if (auto* matchStmt = dynamic_cast<MatchStmt*> (stmt)) {
                LOG("visitMatchStmt");
                visitMatchStmt (matchStmt);
            } else if (auto* compStmt = dynamic_cast<CompoundStmt*> (stmt)) {
                LOG("visitCompoundStmt");
                symbolTable.enterScope ();
                visitCompoundStmt (compStmt);
                symbolTable.exitScope ();
            } else if (auto* applyStmt = dynamic_cast<ApplyGateStmt*> (stmt)) {
                LOG("visitApplyGateStmt");
                visitApplyGateStmt (applyStmt);
            } else if (auto* measureStmt = dynamic_cast<MeasureStmt*> (stmt)) {
                LOG("visitMeasureStmt");
                visitMeasureStmt (measureStmt);
            } else if (auto* resetStmt = dynamic_cast<ResetStmt*> (stmt)) {
                LOG("visitResetStmt");
                visitResetStmt (resetStmt);
            } else {
                LOG("reportError");
                reportError (stmt->loc, "Invalid stmt use in global scope");   
            }
        }
    }

    LOG("============ SEM ANALYSIS END=============");
    return !hasErrors ();

}

SemanticType* SemanticAnalyzer::cloneType (SemanticType* type) {
    if (!type) return nullptr;

    switch (type->getKind ()) {
    case SemanticType::Kind::INT:
    case SemanticType::Kind::FLOAT:
    case SemanticType::Kind::BOOL:
    case SemanticType::Kind::STRING:
    case SemanticType::Kind::QUBIT:
    case SemanticType::Kind::BIT:
    case SemanticType::Kind::VOID:
    case SemanticType::Kind::ERROR: return new PrimitiveType (type->getKind ());

    case SemanticType::Kind::ARRAY: {
        auto* arrType = static_cast<ArrayType*> (type);
        return new ArrayType (cloneType (arrType->getElementType ()), arrType->getDimension ());
    }

    case SemanticType::Kind::FUNCTION: {
        auto* funcType = static_cast<FunctionType*> (type);
        std::vector<SemanticType*> params;
        for (auto* p : funcType->getParamTypes ()) { params.push_back (cloneType (p)); }
        return new FunctionType (params, cloneType (funcType->getReturnType ()));
    }
    }

    return nullptr;
}

SemanticType* SemanticAnalyzer::resolveType (Type* astType) {
    if (!astType) return new PrimitiveType (SemanticType::Kind::VOID);

    if (auto* baseType = dynamic_cast<BaseTypeNode*> (astType)) {
        switch (baseType->getBaseType ()) {
        case BaseTypeKind::INT: return new PrimitiveType (SemanticType::Kind::INT);
        case BaseTypeKind::FLOAT: return new PrimitiveType (SemanticType::Kind::FLOAT);
        case BaseTypeKind::BOOL: return new PrimitiveType (SemanticType::Kind::BOOL);
        case BaseTypeKind::STRING: return new PrimitiveType (SemanticType::Kind::STRING);
        case BaseTypeKind::QUBIT: return new PrimitiveType (SemanticType::Kind::QUBIT);
        case BaseTypeKind::BIT: return new PrimitiveType (SemanticType::Kind::BIT);
        }
    }

    if (auto* arrayType = dynamic_cast<ArrayTypeNode*> (astType)) {
        SemanticType* elemType = resolveType (arrayType->getElementType ());
        int dim                = -1;
        if (arrayType->getDimension ()) {
            auto* dimExpr = arrayType->getDimension ();
            if (auto* intLit = dynamic_cast<IntLiteralExpr*> (dimExpr)) {
                dim = static_cast<int> (intLit->getValue ());
            }
        }
        return new ArrayType (elemType, dim);
    }

    if (auto* funcType = dynamic_cast<FunctionTypeNode*> (astType)) {
        std::vector<SemanticType*> params;
        for (auto* p : funcType->getParamTypes ()) { params.push_back (resolveType (p)); }
        SemanticType* retType = resolveType (funcType->getReturnType ());
        return new FunctionType (params, retType);
    }

    return new PrimitiveType (SemanticType::Kind::ERROR);
}

bool SemanticAnalyzer::isTypeCompatible (SemanticType* expected, SemanticType* actual) {
    if (!expected || !actual) return false;

    if (expected->getKind () == SemanticType::Kind::ERROR || actual->getKind () == SemanticType::Kind::ERROR) {
        return true; // Don't cascade errors
    }

    return expected->equals (actual) || canImplicitlyCast (actual, expected);
}

bool SemanticAnalyzer::canImplicitlyCast (SemanticType* from, SemanticType* to) {
    if (!from || !to) return false;

    // INT -> FLOAT
    if (from->getKind () == SemanticType::Kind::INT && to->getKind () == SemanticType::Kind::FLOAT) {
        return true;
    }

    // BIT -> INT
    if (from->getKind () == SemanticType::Kind::BIT && to->getKind () == SemanticType::Kind::INT) {
        return true;
    }

    return false;
}

bool SemanticAnalyzer::isQubitType (SemanticType* type) {
    if (!type) return false;

    if (type->getKind () == SemanticType::Kind::QUBIT) { return true; }

    if (type->getKind () == SemanticType::Kind::ARRAY) {
        auto* arrType = static_cast<ArrayType*> (type);
        return isQubitType (arrType->getElementType ());
    }

    return false;
}

bool SemanticAnalyzer::canCopyType (SemanticType* type) { return type && type->isCopyable (); }

SemanticType* SemanticAnalyzer::getExprType (Expr* expr) {
    if (!expr) return nullptr;

    auto it = exprTypes.find (expr);
    if (it != exprTypes.end ()) { return it->second; }

    SemanticType* type = nullptr;

    if (auto* binOp = dynamic_cast<BinaryOpExpr*> (expr)) {
        type = visitBinaryOpExpr (binOp);
    } else if (auto* unOp = dynamic_cast<UnaryOpExpr*> (expr)) {
        type = visitUnaryOpExpr (unOp);
    } else if (auto* call = dynamic_cast<FunctionCallExpr*> (expr)) {
        type = visitFunctionCallExpr (call);
    } else if (auto* id = dynamic_cast<IdentifierExpr*> (expr)) {
        type = visitIdentifierExpr (id);
    } else if (auto* idx = dynamic_cast<IndexAccessExpr*> (expr)) {
        type = visitIndexAccessExpr (idx);
    } else if (auto* member = dynamic_cast<MemberAccessExpr*> (expr)) {
        type = visitMemberAccessExpr (member);
    } else if (dynamic_cast<IntLiteralExpr*> (expr)) {
        type = new PrimitiveType (SemanticType::Kind::INT);
    } else if (dynamic_cast<FloatLiteralExpr*> (expr)) {
        type = new PrimitiveType (SemanticType::Kind::FLOAT);
    } else if (dynamic_cast<BoolLiteralExpr*> (expr)) {
        type = new PrimitiveType (SemanticType::Kind::BOOL);
    } else if (dynamic_cast<StringLiteralExpr*> (expr)) {
        type = new PrimitiveType (SemanticType::Kind::STRING);
    } else if (auto* lambda = dynamic_cast<LambdaExpr*> (expr)) {
        type = visitLambdaExpr (lambda);
    } else if (auto* braced = dynamic_cast<BracedInitList*> (expr)) {
        type = visitBracedInitList (braced);
    } else if (auto* qstate = dynamic_cast<QuantumStateList*> (expr)) {
        type = visitQuantumStateList (qstate);
    }

    if (type) { exprTypes[expr] = type; }

    return type;
}

// ===================================================================
//                      Declaration Visitors
// ===================================================================

void SemanticAnalyzer::visitFunctionDecl (FunctionDecl* decl) {
    /* FUTURE: Add name mangling here for function overloading */
    std::string funcName = decl->getFunctionName ()->getValue ();

    // Check if already declared
    if (symbolTable.lookupLocal (funcName)) {
        reportError (decl->loc, "Function '" + funcName + "' already declared");
        return;
    }

    // Resolve parameter types
    std::vector<SemanticType*> paramTypes;
    for (auto* param : decl->getParameters ()) {
        paramTypes.push_back (cloneType (resolveType (param->getType ())));
    }

    // Resolve return type
    SemanticType* retType = resolveType (decl->getReturnType ());

    // Create function type
    auto* funcType = new FunctionType (paramTypes, retType);

    // Add to symbol table
    symbolTable.declare (funcName, new Symbol (funcName, funcType, Symbol::Kind::FUNCTION, decl->loc));
}

void SemanticAnalyzer::visitVariableDecl (VariableDecl* decl) {
    std::string varName = decl->getVariableName ()->getValue ();

    // Check if already declared in current scope
    if (symbolTable.lookupLocal (varName)) {
        reportError (decl->loc, "Variable '" + varName + "' already declared in this scope");
        return;
    }

    SemanticType* declaredType = resolveType (decl->getVariableType ());

    // Check initializer if present
    if (decl->getInitializer ()) {
        SemanticType* initType = getExprType (decl->getInitializer ());

        if (!isTypeCompatible (declaredType, initType)) {
            reportError (decl->loc,
            "Type mismatch: cannot initialize '" + declaredType->toString () + "' with '" +
            initType->toString () + "'");
            delete declaredType;
            return;
        }

        // Special handling for qubit assignments (aliasing)
        if (isQubitType (declaredType)) {
            // Check if initializer is a simple identifier (reference)
            if (auto* idExpr = dynamic_cast<IdentifierExpr*> (decl->getInitializer ())) {
                auto* sym = new Symbol (varName, declaredType, Symbol::Kind::VARIABLE, decl->loc);
                sym->isInitialized = true;
                sym->isQubitAlias  = true;
                symbolTable.declare (varName, sym);
                return;
            } else {
                reportError (decl->loc, "Qubits can only be assigned by reference");
                delete declaredType;
                return;
            }
        }
    }

    // All variables have default initialization:
    // int = 0, float = 0.0, string = "", qubit = |0>, bit = 0, bool = false
    auto* sym = new Symbol (varName, declaredType, Symbol::Kind::VARIABLE, decl->loc);
    symbolTable.declare (varName, sym);
}

void SemanticAnalyzer::visitParameterDecl (ParameterDecl* decl) {
    std::string paramName = decl->getName ()->getValue ();

    if (symbolTable.lookupLocal (paramName)) {
        reportError (decl->loc, "Parameter '" + paramName + "' already declared");
        return;
    }

    SemanticType* paramType = resolveType (decl->getType ());
    auto* sym               = new Symbol (paramName, paramType, Symbol::Kind::PARAMETER, decl->loc);
    sym->isInitialized      = true; // Parameters are always initialized

    // Parameters of qubit type are always references
    if (isQubitType (paramType)) { sym->isQubitAlias = true; }

    symbolTable.declare (paramName, sym);
}

// ===================================================================
//                      Statement Visitors
// ===================================================================

void SemanticAnalyzer::visitCompoundStmt (CompoundStmt* stmt) {
    LOG("=============== COMPOUND STATEMENT START ===============");
    for (auto* s : stmt->getStatements ()) {
        if (auto* declStmt = dynamic_cast<DeclarationStmt*> (s)) {
            LOG("visitDeclarationStmt");
            visitDeclarationStmt (declStmt);
        } else if (auto* assignStmt = dynamic_cast<AssignmentStmt*> (s)) {
            LOG("visitAssignmentStmt");
            visitAssignmentStmt (assignStmt);
        } else if (auto* exprStmt = dynamic_cast<ExpressionStmt*> (s)) {
            LOG("visitExpressionStmt");
            visitExpressionStmt (exprStmt);
        } else if (auto* ifStmt = dynamic_cast<IfStmt*> (s)) {
            LOG("visitIfStmt");
            visitIfStmt (ifStmt);
        } else if (auto* whileStmt = dynamic_cast<WhileStmt*> (s)) {
            LOG("visitWhileStmt");
            visitWhileStmt (whileStmt);
        } else if (auto* doWhileStmt = dynamic_cast<DoWhileStmt*> (s)) {
            LOG("visitDoWhileStmt");
            visitDoWhileStmt (doWhileStmt);
        } else if (auto* forStmt = dynamic_cast<ForStmt*> (s)) {
            LOG("visitForStmt");
            visitForStmt (forStmt);
        } else if (auto* retStmt = dynamic_cast<ReturnStmt*> (s)) {
            visitReturnStmt (retStmt);
        } else if (auto* breakStmt = dynamic_cast<BreakStmt*> (s)) {
             LOG("visitBreakStmt");
            visitBreakStmt (breakStmt);
        } else if (auto* contStmt = dynamic_cast<ContinueStmt*> (s)) {
             LOG("visitContinueStmt");
            visitContinueStmt (contStmt);
        } else if (auto* matchStmt = dynamic_cast<MatchStmt*> (s)) {
            LOG("visitMatchStmt");
            visitMatchStmt (matchStmt);
        } else if (auto* compStmt = dynamic_cast<CompoundStmt*> (s)) {
            LOG("visitCompoundStmt");
            symbolTable.enterScope ();
            visitCompoundStmt (compStmt);
            symbolTable.exitScope ();
        } else if (auto* applyStmt = dynamic_cast<ApplyGateStmt*> (s)) {
            LOG("visitApplyGateStmt");
            visitApplyGateStmt (applyStmt);
        } else if (auto* measureStmt = dynamic_cast<MeasureStmt*> (s)) {
            LOG("visitMeasureStmt");
            visitMeasureStmt (measureStmt);
        } else if (auto* resetStmt = dynamic_cast<ResetStmt*> (s)) {
             LOG("visitResetStmt");
            visitResetStmt (resetStmt);
        }
    }
      LOG("=============== COMPOUND STATEMENT END ===============");
}

void SemanticAnalyzer::visitDeclarationStmt (DeclarationStmt* stmt) {
    for (auto* decl : stmt->getDeclarations ()) { visitVariableDecl (decl); }
}

void SemanticAnalyzer::visitAssignmentStmt (AssignmentStmt* stmt) {
    SemanticType* leftType  = getExprType (stmt->getLeft ());
    SemanticType* rightType = getExprType (stmt->getRight ());

    if (!leftType || !rightType) return;

    std::string op = stmt->getOperator ();

    // Check if left side is assignable
    auto* idExpr = dynamic_cast<IdentifierExpr*> (stmt->getLeft ());
    if (idExpr) {
        auto* sym = symbolTable.lookup (idExpr->getValue ());
        if (sym && isQubitType (sym->type)) {
            // Qubit reassignment - must be by reference
            if (op != "=") {
                reportError (stmt->loc, "Cannot use compound assignment with qubits");
                return;
            }

            if (!dynamic_cast<IdentifierExpr*> (stmt->getRight ())) {
                reportError (stmt->loc, "Qubits can only be assigned by reference");
                return;
            }
        }
    }

    // For simple assignment
    if (op == "=") {
        if (!isTypeCompatible (leftType, rightType)) {
            reportError (stmt->loc,
            "Type mismatch in assignment: cannot assign '" + rightType->toString () + "' to '" +
            leftType->toString () + "'");
        }

        // Check no-cloning for qubits
        if (isQubitType (leftType) && !canCopyType (leftType)) {
            if (!dynamic_cast<IdentifierExpr*> (stmt->getRight ())) {
                reportError (stmt->loc, "Cannot clone quantum state (no-cloning theorem)");
            }
        }
    } else {
        // Compound assignment
        if (isQubitType (leftType)) {
            reportError (stmt->loc, "Cannot use compound assignment with quantum types");
        }
    }
}

void SemanticAnalyzer::visitExpressionStmt (ExpressionStmt* stmt) {
    getExprType (stmt->getExpression ());
}

void SemanticAnalyzer::visitIfStmt (IfStmt* stmt) {
    SemanticType* condType = getExprType (stmt->getCondition ());

    if (condType && condType->getKind () != SemanticType::Kind::BOOL) {
        reportError (stmt->loc, "Condition must be of type bool");
    }

    symbolTable.enterScope ();
    if (auto* thenBlock = dynamic_cast<CompoundStmt*> (stmt->getThenBlock ())) {
        visitCompoundStmt (thenBlock);
    }
    symbolTable.exitScope ();

    if (stmt->getElseBlock ()) {
        symbolTable.enterScope ();
        if (auto* elseBlock = dynamic_cast<CompoundStmt*> (stmt->getElseBlock ())) {
            visitCompoundStmt (elseBlock);
        }
        symbolTable.exitScope ();
    }
}

void SemanticAnalyzer::visitWhileStmt (WhileStmt* stmt) {
    SemanticType* condType = getExprType (stmt->getCondition ());

    if (condType && condType->getKind () != SemanticType::Kind::BOOL) {
        reportError (stmt->loc, "While condition must be of type bool");
    }

    bool wasInLoop = inLoop;
    inLoop         = true;

    symbolTable.enterScope ();
    if (auto* body = dynamic_cast<CompoundStmt*> (stmt->getBody ())) { visitCompoundStmt (body); }
    symbolTable.exitScope ();

    inLoop = wasInLoop;
}

void SemanticAnalyzer::visitDoWhileStmt (DoWhileStmt* stmt) {
    bool wasInLoop = inLoop;
    inLoop         = true;

    symbolTable.enterScope ();
    if (auto* body = dynamic_cast<CompoundStmt*> (stmt->getBody ())) { visitCompoundStmt (body); }
    symbolTable.exitScope ();

    SemanticType* condType = getExprType (stmt->getCondition ());
    if (condType && condType->getKind () != SemanticType::Kind::BOOL) {
        reportError (stmt->loc, "Do-while condition must be of type bool");
    }

    inLoop = wasInLoop;
}

void SemanticAnalyzer::visitForStmt (ForStmt* stmt) {
    symbolTable.enterScope ();

    if (stmt->getInit ()) { visitDeclarationStmt (stmt->getInit ()); }

    if (stmt->getCondition ()) {
        SemanticType* condType = getExprType (stmt->getCondition ());
        if (condType && condType->getKind () != SemanticType::Kind::BOOL) {
            reportError (stmt->loc, "For loop condition must be of type bool");
        }
    }

    if (stmt->getUpdate ()) {
        // The update is a statement (typically an assignment statement)
        if (auto* assignStmt = dynamic_cast<AssignmentStmt*> (stmt->getUpdate ())) {
            visitAssignmentStmt (assignStmt);
        } else if (auto* exprStmt = dynamic_cast<ExpressionStmt*> (stmt->getUpdate ())) {
            visitExpressionStmt (exprStmt);
        }
    }

    bool wasInLoop = inLoop;
    inLoop         = true;

    if (auto* body = dynamic_cast<CompoundStmt*> (stmt->getBody ())) { visitCompoundStmt (body); }

    inLoop = wasInLoop;
    symbolTable.exitScope ();
}

void SemanticAnalyzer::visitReturnStmt (ReturnStmt* stmt) {
    if (!currentFunction) {
        reportError (stmt->loc, "Return statement outside function");
        return;
    }

    SemanticType* expectedType = resolveType (currentFunction->getReturnType ());

    if (stmt->hasReturnValue ()) {
        SemanticType* actualType = getExprType (stmt->getReturnValue ());

        if (!isTypeCompatible (expectedType, actualType)) {
            reportError (stmt->loc,
            "Return type mismatch: expected '" + expectedType->toString () + "', got '" +
            actualType->toString () + "'");
        }
    } else {
        if (expectedType->getKind () != SemanticType::Kind::VOID) {
            reportError (stmt->loc, "Missing return value");
        }
    }

    delete expectedType;
}

void SemanticAnalyzer::visitBreakStmt (BreakStmt* stmt) {
    if (!inLoop) { reportError (stmt->loc, "Break statement outside loop"); }
}

void SemanticAnalyzer::visitContinueStmt (ContinueStmt* stmt) {
    if (!inLoop) { reportError (stmt->loc, "Continue statement outside loop"); }
}

void SemanticAnalyzer::visitMatchStmt (MatchStmt* stmt) {
    SemanticType* matchType = getExprType (stmt->getMatchExpr ());

    for (auto* matchCase : stmt->getCases ()) {
        SemanticType* caseType = getExprType (matchCase->getMatchExpr ());

        if (!isTypeCompatible (matchType, caseType)) {
            reportError (matchCase->loc,
            "Match case type '" + caseType->toString () + "' does not match expression type '" +
            matchType->toString () + "'");
        }

        symbolTable.enterScope ();
        if (auto* body = dynamic_cast<CompoundStmt*> (matchCase->getBody ())) {
            visitCompoundStmt (body);
        }
        symbolTable.exitScope ();
    }
}

} // namespace Heisen

namespace Heisen {

// ===================================================================
//                      Expression Visitors
// ===================================================================

SemanticType* SemanticAnalyzer::visitBinaryOpExpr (BinaryOpExpr* expr) {
    SemanticType* leftType  = getExprType (expr->getLeft ());
    SemanticType* rightType = getExprType (expr->getRight ());

    if (!leftType || !rightType) { return new PrimitiveType (SemanticType::Kind::ERROR); }

    return inferBinaryOpType (expr->getOperator (), leftType, rightType, expr->loc);
}

SemanticType* SemanticAnalyzer::visitUnaryOpExpr (UnaryOpExpr* expr) {
    SemanticType* operandType = getExprType (expr->getOperand ());

    if (!operandType) { return new PrimitiveType (SemanticType::Kind::ERROR); }

    return inferUnaryOpType (expr->getOperator (), operandType, expr->loc);
}

SemanticType* SemanticAnalyzer::visitFunctionCallExpr (FunctionCallExpr* expr) {
    SemanticType* calleeType = getExprType (expr->getCallee ());

    if (!calleeType) { return new PrimitiveType (SemanticType::Kind::ERROR); }

    if (calleeType->getKind () != SemanticType::Kind::FUNCTION) {
        reportError (expr->loc, "Expression is not callable");
        return new PrimitiveType (SemanticType::Kind::ERROR);
    }

    auto* funcType         = static_cast<FunctionType*> (calleeType);
    const auto& paramTypes = funcType->getParamTypes ();
    const auto& args       = expr->getArgs ();

    // Check argument count
    if (args.size () != paramTypes.size ()) {
        reportError (expr->loc,
        "Function expects " + std::to_string (paramTypes.size ()) + " arguments, got " +
        std::to_string (args.size ()));
        return cloneType (funcType->getReturnType ());
    }

    // Check argument types
    for (size_t i = 0; i < args.size (); ++i) {
        SemanticType* argType = getExprType (args[i]);

        if (!argType) continue;

        if (!isTypeCompatible (paramTypes[i], argType)) {
            reportError (expr->loc,
            "Argument " + std::to_string (i + 1) + " type mismatch: expected '" +
            paramTypes[i]->toString () + "', got '" + argType->toString () + "'");
        }

        // Check for qubit passing
        if (isQubitType (paramTypes[i])) {
            // Must pass by reference (identifier only)
            if (!dynamic_cast<IdentifierExpr*> (args[i]) && !dynamic_cast<IndexAccessExpr*> (args[i])) {
                reportError (expr->loc, "Qubits must be passed by reference");
            }
        }
    }

    return cloneType (funcType->getReturnType ());
}

SemanticType* SemanticAnalyzer::visitIdentifierExpr (IdentifierExpr* expr) {
    std::string name = expr->getValue ();
    Symbol* sym      = symbolTable.lookup (name);

    if (!sym) {
        reportError (expr->loc, "Undefined identifier '" + name + "'");
        return new PrimitiveType (SemanticType::Kind::ERROR);
    }

    // All variables are default-initialized, no need to check isInitialized
    return cloneType (sym->type);
}

SemanticType* SemanticAnalyzer::visitIndexAccessExpr (IndexAccessExpr* expr) {
    SemanticType* targetType = getExprType (expr->getTarget ());
    SemanticType* indexType  = getExprType (expr->getIndex ());

    if (!targetType || !indexType) { return new PrimitiveType (SemanticType::Kind::ERROR); }

    // Check if target is indexable
    if (targetType->getKind () != SemanticType::Kind::ARRAY) {
        reportError (expr->loc, "Cannot index non-array type '" + targetType->toString () + "'");
        return new PrimitiveType (SemanticType::Kind::ERROR);
    }

    // Handle slicing
    if (auto* sliceExpr = dynamic_cast<SliceExpr*> (expr->getIndex ())) {
        // Slicing returns the same array type
        return cloneType (targetType);
    }

    // Check index type
    if (indexType->getKind () != SemanticType::Kind::INT) {
        reportError (expr->loc, "Array index must be of type int");
    }

    // Return element type
    auto* arrayType = static_cast<ArrayType*> (targetType);
    return cloneType (arrayType->getElementType ());
}

SemanticType* SemanticAnalyzer::visitMemberAccessExpr (MemberAccessExpr* expr) {
    SemanticType* targetType = getExprType (expr->getTarget ());

    if (!targetType) { return new PrimitiveType (SemanticType::Kind::ERROR); }

    std::string memberName = expr->getMemberName ()->getValue ();

    // Special case for array.size
    if (targetType->getKind () == SemanticType::Kind::ARRAY && memberName == "size") {
        return new PrimitiveType (SemanticType::Kind::INT);
    }

    reportError (expr->loc, "Type '" + targetType->toString () + "' has no member '" + memberName + "'");
    return new PrimitiveType (SemanticType::Kind::ERROR);
}

SemanticType* SemanticAnalyzer::visitLambdaExpr (LambdaExpr* expr) {
    // Enter new scope for lambda
    symbolTable.enterScope ();

    // Add parameters
    std::vector<SemanticType*> paramTypes;
    for (auto* param : expr->getParams ()) {
        visitParameterDecl (param);
        paramTypes.push_back (cloneType (resolveType (param->getType ())));
    }

    // Save and set current function context
    FunctionDecl* savedFunc = currentFunction;
    currentFunction         = nullptr; // Lambda is not a named function

    // Analyze body
    if (expr->getBody ()) { visitCompoundStmt (expr->getBody ()); }

    currentFunction = savedFunc;
    symbolTable.exitScope ();

    // Resolve return type
    SemanticType* retType = expr->getReturnType () ? resolveType (expr->getReturnType ()) :
                                                     new PrimitiveType (SemanticType::Kind::VOID);

    return new FunctionType (paramTypes, retType);
}

SemanticType* SemanticAnalyzer::visitBracedInitList (BracedInitList* expr) {
    const auto& elements = expr->getElements ();

    if (elements.empty ()) {
        // Empty initializer list - type will be inferred from context
        return new PrimitiveType (SemanticType::Kind::ERROR);
    }

    // Infer type from first element
    SemanticType* elemType = getExprType (elements[0]);
    if (!elemType) { return new PrimitiveType (SemanticType::Kind::ERROR); }

    // Check all elements have compatible types
    for (size_t i = 1; i < elements.size (); ++i) {
        SemanticType* currType = getExprType (elements[i]);

        if (!currType || !isTypeCompatible (elemType, currType)) {
            reportError (expr->loc,
            "Inconsistent types in initializer list: expected '" + elemType->toString () +
            "', got '" + currType->toString () + "'");
        }
    }

    return new ArrayType (cloneType (elemType), static_cast<int> (elements.size ()));
}

SemanticType* SemanticAnalyzer::visitQuantumStateList (QuantumStateList* expr) {
    const auto& states = expr->getStates ();

    if (states.empty ()) { return new PrimitiveType (SemanticType::Kind::ERROR); }

    // All states must be qubits
    for (auto* state : states) {
        SemanticType* stateType = getExprType (state);

        if (!stateType || !isQubitType (stateType)) {
            reportError (expr->loc, "Quantum state list can only contain qubits");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }
    }

    return new ArrayType (new PrimitiveType (SemanticType::Kind::QUBIT), static_cast<int> (states.size ()));
}

// ===================================================================
//                      Type Inference for Operators
// ===================================================================

SemanticType*
SemanticAnalyzer::inferBinaryOpType (const std::string& op, SemanticType* left, SemanticType* right, Location loc) {
    auto leftKind  = left->getKind ();
    auto rightKind = right->getKind ();

    // Arithmetic operators: +, -, *, /, %, **
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "**") {
        // Check for quantum types
        if (left->isQuantum () || right->isQuantum ()) {
            reportError (loc, "Cannot use arithmetic operators on quantum types");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }

        // Both must be numeric
        bool leftNumeric = (leftKind == SemanticType::Kind::INT || leftKind == SemanticType::Kind::FLOAT);
        bool rightNumeric = (rightKind == SemanticType::Kind::INT || rightKind == SemanticType::Kind::FLOAT);

        if (!leftNumeric || !rightNumeric) {
            reportError (loc, "Arithmetic operators require numeric types");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }

        // Result is float if either operand is float
        if (leftKind == SemanticType::Kind::FLOAT || rightKind == SemanticType::Kind::FLOAT) {
            return new PrimitiveType (SemanticType::Kind::FLOAT);
        }

        return new PrimitiveType (SemanticType::Kind::INT);
    }

    // Comparison operators: ==, !=, <, >, <=, >=
    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
        if (!isTypeCompatible (left, right)) {
            reportError (loc,
            "Cannot compare incompatible types '" + left->toString () + "' and '" + right->toString () + "'");
        }

        // Qubits cannot be compared
        if (left->isQuantum () || right->isQuantum ()) {
            reportError (loc, "Cannot compare quantum types");
        }

        return new PrimitiveType (SemanticType::Kind::BOOL);
    }

    // Logical operators: and, or
    if (op == "and" || op == "or") {
        if (leftKind != SemanticType::Kind::BOOL) {
            reportError (loc, "Left operand of '" + op + "' must be bool");
        }
        if (rightKind != SemanticType::Kind::BOOL) {
            reportError (loc, "Right operand of '" + op + "' must be bool");
        }
        return new PrimitiveType (SemanticType::Kind::BOOL);
    }

    // Bitwise operators: &, |, ^, <<, >>
    if (op == "&" || op == "|" || op == "^" || op == "<<" || op == ">>") {
        if (leftKind != SemanticType::Kind::INT && leftKind != SemanticType::Kind::BIT) {
            reportError (loc, "Bitwise operators require int or bit types");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }
        if (rightKind != SemanticType::Kind::INT && rightKind != SemanticType::Kind::BIT) {
            reportError (loc, "Bitwise operators require int or bit types");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }

        // Result is int unless both are bit
        if (leftKind == SemanticType::Kind::BIT && rightKind == SemanticType::Kind::BIT) {
            return new PrimitiveType (SemanticType::Kind::BIT);
        }
        return new PrimitiveType (SemanticType::Kind::INT);
    }

    reportError (loc, "Unknown binary operator '" + op + "'");
    return new PrimitiveType (SemanticType::Kind::ERROR);
}

SemanticType* SemanticAnalyzer::inferUnaryOpType (const std::string& op, SemanticType* operand, Location loc) {
    auto kind = operand->getKind ();

    // Arithmetic negation: -, +
    if (op == "-" || op == "+") {
        if (operand->isQuantum ()) {
            reportError (loc, "Cannot use arithmetic operators on quantum types");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }

        if (kind != SemanticType::Kind::INT && kind != SemanticType::Kind::FLOAT) {
            reportError (loc, "Unary '" + op + "' requires numeric type");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }

        return cloneType (operand);
    }

    // Logical negation: not
    if (op == "not") {
        if (kind != SemanticType::Kind::BOOL) {
            reportError (loc, "Logical 'not' requires bool type");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }
        return new PrimitiveType (SemanticType::Kind::BOOL);
    }

    // Bitwise negation: !
    if (op == "!") {
        if (kind != SemanticType::Kind::INT && kind != SemanticType::Kind::BIT) {
            reportError (loc, "Bitwise '!' requires int or bit type");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }
        return cloneType (operand);
    }

    reportError (loc, "Unknown unary operator '" + op + "'");
    return new PrimitiveType (SemanticType::Kind::ERROR);
}

void SemanticAnalyzer::checkQubitUsage (Expr* expr, Location loc) {
    SemanticType* type = getExprType (expr);

    if (!type) return;

    if (isQubitType (type)) {
        // Ensure qubit is accessed by reference only
        if (!dynamic_cast<IdentifierExpr*> (expr) && !dynamic_cast<IndexAccessExpr*> (expr)) {
            reportError (loc, "Invalid qubit usage - must be accessed by reference");
        }
    }
}

} // namespace Heisen