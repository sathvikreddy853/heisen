#include "Sema/Sema.hpp"

namespace Heisen {

void SemanticAnalyzer::visitApplyGateStmt (ApplyGateStmt* stmt) {
    checkGateApplication (stmt->getGate (), stmt->getTarget (), stmt->loc);
}

void SemanticAnalyzer::checkGateApplication (GateNode* gate, Expr* target, Location loc) {
    SemanticType* targetType = getExprType (target);

    if (!targetType) return;

    // Target must be qubit or qubit array
    if (!isQubitType (targetType)) {
        reportError (loc, "Gate can only be applied to qubits");
        return;
    }

    // Additional validation for specific gates could be added here
    // For example, CNOT requires 2 qubits, etc.
}

void SemanticAnalyzer::visitMeasureStmt (MeasureStmt* stmt) {
    SemanticType* srcType = getExprType (stmt->getSource ());
    SemanticType* tgtType = getExprType (stmt->getTarget ());

    if (!srcType || !tgtType) return;

    // Source must be qubit or qubit array
    if (!isQubitType (srcType)) {
        reportError (stmt->loc, "Measurement source must be a qubit");
        return;
    }

    // Target must be bit or bit array
    if (tgtType->getKind () != SemanticType::Kind::BIT) {
        if (tgtType->getKind () == SemanticType::Kind::ARRAY) {
            auto* arrType = static_cast<ArrayType*> (tgtType);
            if (arrType->getElementType ()->getKind () != SemanticType::Kind::BIT) {
                reportError (stmt->loc, "Measurement target must be a bit");
                return;
            }
        } else {
            reportError (stmt->loc, "Measurement target must be a bit");
            return;
        }
    }

    // Check dimension compatibility for arrays
    if (srcType->getKind () == SemanticType::Kind::ARRAY && tgtType->getKind () == SemanticType::Kind::ARRAY) {
        auto* srcArr = static_cast<ArrayType*> (srcType);
        auto* tgtArr = static_cast<ArrayType*> (tgtType);

        if (srcArr->getDimension () != tgtArr->getDimension () && srcArr->getDimension () != -1 &&
        tgtArr->getDimension () != -1) {
            reportError (stmt->loc, "Array dimension mismatch in measurement");
        }
    }
}

void SemanticAnalyzer::visitResetStmt (ResetStmt* stmt) {
    SemanticType* targetType = getExprType (stmt->getTarget ());

    if (!targetType) return;

    if (!isQubitType (targetType)) {
        reportError (stmt->loc, "Reset can only be applied to qubits");
    }
}

}