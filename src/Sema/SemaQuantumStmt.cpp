#include "Sema/Sema.hpp"

namespace Heisen {

void SemanticAnalyzer::visitApplyGateStmt (ApplyGateStmt* stmt) {
    checkGateApplication (stmt->getGate (), stmt->getTarget (), stmt->loc);
}

// Helper function to count qubits in a target expression
// Helper function to count qubits in a target expression
int SemanticAnalyzer::countQubitsInTarget(Expr* target) {
    if (!target) return 0;
    
    SemanticType* targetType = getExprType(target);
    if (!targetType) return 0;
    
    // Single qubit
    if (targetType->getKind() == SemanticType::Kind::QUBIT) {
        return 1;
    }
    
    // Array of qubits or tensored state
    if (targetType->getKind() == SemanticType::Kind::ARRAY) {
        auto* arrType = static_cast<ArrayType*>(targetType);
        if (isQubitType(arrType->getElementType())) {
            int dim = arrType->getDimension();
            // If dimension is known, return it; otherwise check if it's a QuantumStateList
            if (dim > 0) {
                return dim;
            }
        }
    }
    
    // Check if it's a QuantumStateList (tensored state like [q0, q1, q2])
    if (auto* qStateList = dynamic_cast<QuantumStateList*>(target)) {
        return static_cast<int>(qStateList->getStates().size());
    }
    
    return -1; // Unknown size
}

// Helper function to get qubit requirement for a gate kind
int SemanticAnalyzer::getGateQubitRequirement(GateKind kind) {
    switch (kind) {
        // Single-qubit gates (Pauli gates)
        case GateKind::I:    // Identity
        case GateKind::X:    // Pauli-X (NOT)
        case GateKind::Y:    // Pauli-Y
        case GateKind::Z:    // Pauli-Z
        
        // Single-qubit gates (Hadamard, Phase)
        case GateKind::H:    // Hadamard
        case GateKind::S:    // Phase gate (sqrt(Z))
        case GateKind::T:    // T gate (sqrt(S))
        
        // Single-qubit rotation gates (parametric)
        case GateKind::RX:   // Rotation around X-axis
        case GateKind::RY:   // Rotation around Y-axis
        case GateKind::RZ:   // Rotation around Z-axis
            return 1;
        
        // Two-qubit gates
        case GateKind::CNOT:   // Controlled-NOT (CX)
        case GateKind::CZ:     // Controlled-Z
        case GateKind::SWAP:   // SWAP gate
        
        // Two-qubit controlled rotation gates (parametric)
        case GateKind::CRX:    // Controlled rotation around X
        case GateKind::CRY:    // Controlled rotation around Y
        case GateKind::CRZ:    // Controlled rotation around Z
            return 2;
        
        // Three-qubit gates
        case GateKind::CSWAP:  // Fredkin gate (Controlled-SWAP)
        case GateKind::CCNOT:  // Toffoli gate (Controlled-Controlled-NOT)
            return 3;
        
        // CTRL is special - it's a modifier, not a standalone gate
        case GateKind::CTRL:
            return -1;  // Needs context to determine
        
        case GateKind::UNKNOWN:
        default:
            return -1;  // Unknown requirement
    }
}

// Helper function to count gates in a gate node
int SemanticAnalyzer::countGatesInNode(GateNode* gate) {
    if (!gate) return 0;
    
    // Simple gate - operates on 1 qubit (except multi-qubit gates)
    if (auto* simpleGate = dynamic_cast<SimpleGateNode*>(gate)) {
        return getGateQubitRequirement(simpleGate->getGateKind());
    }
    
    // Parametric gate - same as simple gate
    if (auto* paramGate = dynamic_cast<ParametricGateNode*>(gate)) {
        return getGateQubitRequirement(paramGate->getGateKind());
    }
    
    // Tensored gates like [H, X, Y] - count the number of gates
    if (auto* tensoredGate = dynamic_cast<TensoredGateNode*>(gate)) {
        return static_cast<int>(tensoredGate->getGates().size());
    }
    
    // Gate composition like H @ X - this is sequential, not parallel
    // Both gates must operate on the SAME number of qubits
    if (auto* compGate = dynamic_cast<GateCompositionNode*>(gate)) {
        int leftReq = countGatesInNode(compGate->getLeft());
        int rightReq = countGatesInNode(compGate->getRight());;
        // Both sides must have the same qubit requirement
        if (leftReq > 0 && rightReq > 0 && leftReq != rightReq) {
            // This will be caught in checkGateApplication, just return -1 for now
            return -1;
        }
        
        // If both are same, return that; otherwise return the known one
        if (leftReq > 0 && rightReq > 0) {
            return leftReq; // They're equal
        }
        return (leftReq > 0) ? leftReq : rightReq;
    }
    
    return -1; // Unknown
}

// Helper to get gate name for error messages
std::string SemanticAnalyzer::getGateName(GateNode* gate) {
    if (auto* simpleGate = dynamic_cast<SimpleGateNode*>(gate)) {
        switch (simpleGate->getGateKind()) {
            case GateKind::H: return "H";
            case GateKind::X: return "X";
            case GateKind::Y: return "Y";
            case GateKind::Z: return "Z";
            case GateKind::S: return "S";
            case GateKind::T: return "T";
            case GateKind::I: return "I";
            case GateKind::RX: return "RX";
            case GateKind::RY: return "RY";
            case GateKind::RZ: return "RZ";
            case GateKind::CNOT: return "CNOT";
            case GateKind::CZ: return "CZ";
            case GateKind::SWAP: return "SWAP";
            case GateKind::CSWAP: return "CSWAP";
            case GateKind::CCNOT: return "CCNOT";
            case GateKind::CRX: return "CRX";
            case GateKind::CRY: return "CRY";
            case GateKind::CRZ: return "CRZ";
            case GateKind::CTRL: return "CTRL";
            default: return "Unknown Gate";
        }
    }
    
    if (auto* paramGate = dynamic_cast<ParametricGateNode*>(gate)) {
        return getGateName(new SimpleGateNode(paramGate->getGateKind(), paramGate->loc));
    }
    
    if (dynamic_cast<TensoredGateNode*>(gate)) {
        return "Tensored Gate";
    }
    
    if (dynamic_cast<GateCompositionNode*>(gate)) {
        return "Gate Composition";
    }
    
    return "Gate";
}

// Validate parametric gates have correct parameter types
void SemanticAnalyzer::validateGateParameters(GateNode* gate, Location loc) {
    if (auto* paramGate = dynamic_cast<ParametricGateNode*>(gate)) {
        const auto& params = paramGate->getParams();
        
        // Rotation gates need exactly one angle parameter
        GateKind kind = paramGate->getGateKind();
        bool isRotationGate = (kind == GateKind::RX || kind == GateKind::RY || 
                               kind == GateKind::RZ || kind == GateKind::CRX || 
                               kind == GateKind::CRY || kind == GateKind::CRZ);
        
        if (isRotationGate) {
            if (params.size() != 1) {
                reportError(loc, "Rotation gate '" + getGateName(gate) + 
                           "' requires exactly one angle parameter");
                return;
            }
            
            // Check parameter is numeric
            SemanticType* paramType = getExprType(params[0]);
            if (paramType && 
                paramType->getKind() != SemanticType::Kind::INT && 
                paramType->getKind() != SemanticType::Kind::FLOAT) {
                reportError(loc, "Gate parameter must be numeric (int or float)");
            }
        }
    }
}

// Main gate application checker
void SemanticAnalyzer::checkGateApplication(GateNode* gate, Expr* target, Location loc) {
    SemanticType* targetType = getExprType(target);

    if (!targetType) return;

    // Target must be qubit or qubit array
    if (!isQubitType(targetType)) {
        reportError(loc, "Gate can only be applied to qubits");
        return;
    }

    // Validate gate parameters (for parametric gates)
    validateGateParameters(gate, loc);

    // Count qubits required by the gate
    int gateQubitCount = countGatesInNode(gate);
    
    // Count qubits provided by the target
    int targetQubitCount = countQubitsInTarget(target);
    
    // If either count is unknown, we can't validate further
    if (gateQubitCount < 0 || targetQubitCount < 0) {
        reportError(loc,"Invalid gate composition");
        return;
    }
    
    // For tensored gates, the number of gates must match the number of qubits
    if (auto* tensoredGate = dynamic_cast<TensoredGateNode*>(gate)) {
        if (gateQubitCount != targetQubitCount) {
            reportError(loc, "Tensored gate requires exactly " + 
                       std::to_string(gateQubitCount) + " qubits, but " + 
                       std::to_string(targetQubitCount) + " were provided");
            return;
        }
    }
    
    // For gate composition, check that target has enough qubits
    if (auto* compGate = dynamic_cast<GateCompositionNode*>(gate)) {
        int leftReq = countGatesInNode(compGate->getLeft());
        int rightReq = countGatesInNode(compGate->getRight());
        
        // CRITICAL: Both gates in composition must operate on same number of qubits
        if (leftReq > 0 && rightReq > 0 && leftReq != rightReq) {
            reportError(loc, "Cannot compose gates with different qubit requirements: " +
                       getGateName(compGate->getLeft()) + " (requires " + 
                       std::to_string(leftReq) + " qubit(s)) with " +
                       getGateName(compGate->getRight()) + " (requires " + 
                       std::to_string(rightReq) + " qubit(s))");
            return;
        }
        
        if (targetQubitCount < gateQubitCount) {
            reportError(loc, "Gate composition requires at least " + 
                       std::to_string(gateQubitCount) + " qubits, but only " + 
                       std::to_string(targetQubitCount) + " were provided");
            return;
        }
        
        // Recursively validate both sides of the composition
        checkGateApplication(compGate->getLeft(), target, loc);
        checkGateApplication(compGate->getRight(), target, loc);
        return;
    }
    
    // For simple/parametric gates, check qubit requirements
    if (auto* simpleGate = dynamic_cast<SimpleGateNode*>(gate)) {
        if (targetQubitCount < gateQubitCount) {
            reportError(loc, "Gate '" + getGateName(gate) + "' requires " + 
                       std::to_string(gateQubitCount) + " qubit(s), but only " + 
                       std::to_string(targetQubitCount) + " were provided");
            return;
        }
        
        // For single-qubit gates applied to multiple qubits, ensure it's not composition
        if (gateQubitCount == 1 && targetQubitCount > 1) {
            // This is allowed - single gate applied to multiple qubits in parallel
            // e.g., H @ [q0, q1, q2] applies H to each qubit
        }
    }
    
    if (auto* paramGate = dynamic_cast<ParametricGateNode*>(gate)) {
        if (targetQubitCount < gateQubitCount) {
            reportError(loc, "Gate '" + getGateName(gate) + "' requires " + 
                       std::to_string(gateQubitCount) + " qubit(s), but only " + 
                       std::to_string(targetQubitCount) + " were provided");
            return;
        }
    }
    
    // Validate that multi-qubit gates receive proper tensored states
    if (gateQubitCount > 1) {
        // For multi-qubit gates, we should ideally have a QuantumStateList or array
        bool isProperMultiQubitTarget = 
            dynamic_cast<QuantumStateList*>(target) != nullptr ||
            (targetType->getKind() == SemanticType::Kind::ARRAY);
        
        if (!isProperMultiQubitTarget && targetQubitCount == 1) {
            reportError(loc, "Multi-qubit gate '" + getGateName(gate) + 
                       "' requires a tensored quantum state like [q0, q1]");
        }
    }
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

} // namespace Heisen
