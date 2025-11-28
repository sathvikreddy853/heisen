#include "Sema/Sema.hpp"

namespace Heisen {

void SemanticAnalyzer::visitCompoundStmt (CompoundStmt* stmt) {
    for (auto* s : stmt->getStatements ()) {
        if (auto* declStmt = dynamic_cast<DeclarationStmt*> (s)) {
            visitDeclarationStmt (declStmt);
        } else if (auto* assignStmt = dynamic_cast<AssignmentStmt*> (s)) {
            visitAssignmentStmt (assignStmt);
        } else if (auto* exprStmt = dynamic_cast<ExpressionStmt*> (s)) {
            visitExpressionStmt (exprStmt);
        } else if (auto* ifStmt = dynamic_cast<IfStmt*> (s)) {
            visitIfStmt (ifStmt);
        } else if (auto* whileStmt = dynamic_cast<WhileStmt*> (s)) {
            visitWhileStmt (whileStmt);
        } else if (auto* doWhileStmt = dynamic_cast<DoWhileStmt*> (s)) {
            visitDoWhileStmt (doWhileStmt);
        } else if (auto* forStmt = dynamic_cast<ForStmt*> (s)) {
            visitForStmt (forStmt);
        } else if (auto* retStmt = dynamic_cast<ReturnStmt*> (s)) {
            visitReturnStmt (retStmt);
        } else if (auto* breakStmt = dynamic_cast<BreakStmt*> (s)) {
            visitBreakStmt (breakStmt);
        } else if (auto* contStmt = dynamic_cast<ContinueStmt*> (s)) {
            visitContinueStmt (contStmt);
        } else if (auto* matchStmt = dynamic_cast<MatchStmt*> (s)) {
            visitMatchStmt (matchStmt);
        } else if (auto* compStmt = dynamic_cast<CompoundStmt*> (s)) {
            symbolTable.enterScope ();
            visitCompoundStmt (compStmt);
            symbolTable.exitScope ();
        } else if (auto* applyStmt = dynamic_cast<ApplyGateStmt*> (s)) {
            visitApplyGateStmt (applyStmt);
        } else if (auto* measureStmt = dynamic_cast<MeasureStmt*> (s)) {
            visitMeasureStmt (measureStmt);
        } else if (auto* resetStmt = dynamic_cast<ResetStmt*> (s)) {
            visitResetStmt (resetStmt);
        }
    }
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
            "Type mismatch in assignment: cannot assign '" +
            rightType->toString () + "' to '" + leftType->toString () + "'");
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
    if (auto* body = dynamic_cast<CompoundStmt*> (stmt->getBody ())) {
        visitCompoundStmt (body);
    }
    symbolTable.exitScope ();

    inLoop = wasInLoop;
}

void SemanticAnalyzer::visitDoWhileStmt (DoWhileStmt* stmt) {
    bool wasInLoop = inLoop;
    inLoop         = true;

    symbolTable.enterScope ();
    if (auto* body = dynamic_cast<CompoundStmt*> (stmt->getBody ())) {
        visitCompoundStmt (body);
    }
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

    if (auto* body = dynamic_cast<CompoundStmt*> (stmt->getBody ())) {
        visitCompoundStmt (body);
    }

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
            "Return type mismatch: expected '" + expectedType->toString () +
            "', got '" + actualType->toString () + "'");
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
            "Match case type '" + caseType->toString () +
            "' does not match expression type '" + matchType->toString () + "'");
        }

        symbolTable.enterScope ();
        if (auto* body = dynamic_cast<CompoundStmt*> (matchCase->getBody ())) {
            visitCompoundStmt (body);
        }
        symbolTable.exitScope ();
    }
}

} // namespace Heisen
