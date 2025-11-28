#include "Sema/Sema.hpp"

namespace Heisen {

SemanticType* SemanticAnalyzer::visitBinaryOpExpr (BinaryOpExpr* expr) {
    SemanticType* leftType  = getExprType (expr->getLeft ());
    SemanticType* rightType = getExprType (expr->getRight ());

    if (!leftType || !rightType) {
        return new PrimitiveType (SemanticType::Kind::ERROR);
    }

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
        "Function expects " + std::to_string (paramTypes.size ()) +
        " arguments, got " + std::to_string (args.size ()));
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
            if (!dynamic_cast<IdentifierExpr*> (args[i]) &&
            !dynamic_cast<IndexAccessExpr*> (args[i])) {
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

    if (!targetType || !indexType) {
        return new PrimitiveType (SemanticType::Kind::ERROR);
    }

    // Check if target is indexable
    if (targetType->getKind () != SemanticType::Kind::ARRAY) {
        reportError (expr->loc,
        "Cannot index non-array type '" + targetType->toString () + "'");
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

    reportError (expr->loc,
    "Type '" + targetType->toString () + "' has no member '" + memberName + "'");
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
    SemanticType* retType = expr->getReturnType () ?
    resolveType (expr->getReturnType ()) :
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
            "Inconsistent types in initializer list: expected '" +
            elemType->toString () + "', got '" + currType->toString () + "'");
        }
    }

    return new ArrayType (cloneType (elemType), static_cast<int> (elements.size ()));
}

SemanticType* SemanticAnalyzer::visitQuantumStateList (QuantumStateList* expr) {
    const auto& states = expr->getStates ();

    if (states.empty ()) {
        return new PrimitiveType (SemanticType::Kind::ERROR);
    }

    // All states must be qubits
    for (auto* state : states) {
        SemanticType* stateType = getExprType (state);

        if (!stateType || !isQubitType (stateType)) {
            reportError (expr->loc, "Quantum state list can only contain qubits");
            return new PrimitiveType (SemanticType::Kind::ERROR);
        }
    }

    return new ArrayType (new PrimitiveType (SemanticType::Kind::QUBIT),
    static_cast<int> (states.size ()));
}

SemanticType* SemanticAnalyzer::inferBinaryOpType (const std::string& op,
SemanticType* left,
SemanticType* right,
Location loc) {
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
        bool leftNumeric =
        (leftKind == SemanticType::Kind::INT || leftKind == SemanticType::Kind::FLOAT);
        bool rightNumeric = (rightKind == SemanticType::Kind::INT ||
        rightKind == SemanticType::Kind::FLOAT);

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
            "Cannot compare incompatible types '" + left->toString () +
            "' and '" + right->toString () + "'");
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

SemanticType* SemanticAnalyzer::inferUnaryOpType (const std::string& op,
SemanticType* operand,
Location loc) {
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
        if (!dynamic_cast<IdentifierExpr*> (expr) &&
        !dynamic_cast<IndexAccessExpr*> (expr)) {
            reportError (loc, "Invalid qubit usage - must be accessed by reference");
        }
    }
}

} // namespace Heisen