#include "Sema/Sema.hpp"

namespace Heisen {

void SemanticAnalyzer::initializeBuiltins () {
    // Math functions
    auto* piType = new PrimitiveType (SemanticType::Kind::FLOAT);
    symbolTable.declare (
    "pi", new Symbol ("pi", piType, Symbol::Kind::VARIABLE, Location ()));

    // Built-in functions
    std::vector<std::string> mathFuncs = { "sqrt", "round", "floor", "ceil",
        "sin", "cos", "tan", "abs" };
    for (const auto& func : mathFuncs) {
        std::vector<SemanticType*> params;
        params.push_back (new PrimitiveType (SemanticType::Kind::FLOAT));
        PrimitiveType* retType = nullptr;
        if (func == "round" || func == "floor" || func == "ceil") {
            retType = new PrimitiveType (SemanticType::Kind::INT);
        } else {
            retType = new PrimitiveType (SemanticType::Kind::FLOAT);
        }
        auto* funcType = new FunctionType (params, retType);
        symbolTable.declare (
        func, new Symbol (func, funcType, Symbol::Kind::FUNCTION, Location ()));
    }

    // String conversion
    std::vector<SemanticType*> floatParam;
    floatParam.push_back (new PrimitiveType (SemanticType::Kind::FLOAT));
    auto* floatFuncType =
    new FunctionType (floatParam, new PrimitiveType (SemanticType::Kind::FLOAT));
    symbolTable.declare ("float",
    new Symbol ("float", floatFuncType, Symbol::Kind::FUNCTION, Location ()));
}

bool SemanticAnalyzer::analyze (std::vector<ASTNode*>& translationUnit) {
    LOG ("============ SEM ANALYSIS START=============");
    // First pass: collect all function declarations
    for (auto* node : translationUnit) {
        if (auto* funcDecl = dynamic_cast<FunctionDecl*> (node)) {
            visitFunctionDecl (funcDecl);
        }
    }

    // Second pass: analyze function bodies and statements
    for (auto* node : translationUnit) {
        if (auto* funcDecl = dynamic_cast<FunctionDecl*> (node)) {
            if (funcDecl->getBody ()) {
                currentFunction = funcDecl;
                symbolTable.enterScope ();

                // Add parameters to scope
                for (auto* param : funcDecl->getParameters ()) {
                    visitParameterDecl (param);
                }

                visitCompoundStmt (funcDecl->getBody ());
                symbolTable.exitScope ();
                currentFunction = nullptr;
            }
        } else if (auto* stmt = dynamic_cast<Stmt*> (node)) {
            if (auto* declStmt = dynamic_cast<DeclarationStmt*> (stmt)) {
                LOG ("visitDeclarationStmt");
                visitDeclarationStmt (declStmt);
            } else if (auto* assignStmt = dynamic_cast<AssignmentStmt*> (stmt)) {
                LOG ("visitAssignmentStmt");
                visitAssignmentStmt (assignStmt);
            } else if (auto* exprStmt = dynamic_cast<ExpressionStmt*> (stmt)) {
                LOG ("visitExpressionStmt");
                visitExpressionStmt (exprStmt);
            } else if (auto* ifStmt = dynamic_cast<IfStmt*> (stmt)) {
                LOG ("visitIfStmt");
                visitIfStmt (ifStmt);
            } else if (auto* whileStmt = dynamic_cast<WhileStmt*> (stmt)) {
                LOG ("visitWhileStmt");
                visitWhileStmt (whileStmt);
            } else if (auto* doWhileStmt = dynamic_cast<DoWhileStmt*> (stmt)) {
                LOG ("visitDoWhileStmt");
                visitDoWhileStmt (doWhileStmt);
            } else if (auto* forStmt = dynamic_cast<ForStmt*> (stmt)) {
                LOG ("visitForStmt");
                visitForStmt (forStmt);
            } else if (auto* matchStmt = dynamic_cast<MatchStmt*> (stmt)) {
                LOG ("visitMatchStmt");
                visitMatchStmt (matchStmt);
            } else if (auto* compStmt = dynamic_cast<CompoundStmt*> (stmt)) {
                LOG ("visitCompoundStmt");
                symbolTable.enterScope ();
                visitCompoundStmt (compStmt);
                symbolTable.exitScope ();
            } else if (auto* applyStmt = dynamic_cast<ApplyGateStmt*> (stmt)) {
                LOG ("visitApplyGateStmt");
                visitApplyGateStmt (applyStmt);
            } else if (auto* measureStmt = dynamic_cast<MeasureStmt*> (stmt)) {
                LOG ("visitMeasureStmt");
                visitMeasureStmt (measureStmt);
            } else if (auto* resetStmt = dynamic_cast<ResetStmt*> (stmt)) {
                LOG ("visitResetStmt");
                visitResetStmt (resetStmt);
            } else {
                LOG ("reportError");
                reportError (stmt->loc, "Invalid stmt use in global scope");
            }
        }
    }

    LOG ("============ SEM ANALYSIS END=============");
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
        return new ArrayType (
        cloneType (arrType->getElementType ()), arrType->getDimension ());
    }

    case SemanticType::Kind::FUNCTION: {
        auto* funcType = static_cast<FunctionType*> (type);
        std::vector<SemanticType*> params;
        for (auto* p : funcType->getParamTypes ()) {
            params.push_back (cloneType (p));
        }
        return new FunctionType (params, cloneType (funcType->getReturnType ()));
    }
    }

    return nullptr;
}

SemanticType* SemanticAnalyzer::resolveType (Type* astType) {
    if (!astType) return new PrimitiveType (SemanticType::Kind::VOID);

    if (auto* baseType = dynamic_cast<BaseTypeNode*> (astType)) {
        switch (baseType->getBaseType ()) {
        case BaseTypeKind::INT:
            return new PrimitiveType (SemanticType::Kind::INT);
        case BaseTypeKind::FLOAT:
            return new PrimitiveType (SemanticType::Kind::FLOAT);
        case BaseTypeKind::BOOL:
            return new PrimitiveType (SemanticType::Kind::BOOL);
        case BaseTypeKind::STRING:
            return new PrimitiveType (SemanticType::Kind::STRING);
        case BaseTypeKind::QUBIT:
            return new PrimitiveType (SemanticType::Kind::QUBIT);
        case BaseTypeKind::BIT:
            return new PrimitiveType (SemanticType::Kind::BIT);
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
        for (auto* p : funcType->getParamTypes ()) {
            params.push_back (resolveType (p));
        }
        SemanticType* retType = resolveType (funcType->getReturnType ());
        return new FunctionType (params, retType);
    }

    return new PrimitiveType (SemanticType::Kind::ERROR);
}

bool SemanticAnalyzer::isTypeCompatible (SemanticType* expected, SemanticType* actual) {
    if (!expected || !actual) return false;

    if (expected->getKind () == SemanticType::Kind::ERROR ||
    actual->getKind () == SemanticType::Kind::ERROR) {
        return true; // Don't cascade errors
    }

    return expected->equals (actual) || canImplicitlyCast (actual, expected);
}

bool SemanticAnalyzer::canImplicitlyCast (SemanticType* from, SemanticType* to) {
    if (!from || !to) return false;

    // INT -> FLOAT
    if (from->getKind () == SemanticType::Kind::INT &&
    to->getKind () == SemanticType::Kind::FLOAT) {
        return true;
    }

    // BIT -> INT
    if (from->getKind () == SemanticType::Kind::BIT &&
    to->getKind () == SemanticType::Kind::INT) {
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

bool SemanticAnalyzer::canCopyType (SemanticType* type) {
    return type && type->isCopyable ();
}

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

} // namespace Heisen