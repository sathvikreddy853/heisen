#include "Sema/Sema.hpp"

namespace Heisen {

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
    symbolTable.declare (funcName,
    new Symbol (funcName, funcType, Symbol::Kind::FUNCTION, decl->loc));
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
            "Type mismatch: cannot initialize '" + declaredType->toString () +
            "' with '" + initType->toString () + "'");
            delete declaredType;
            return;
        }

        // Special handling for qubit assignments (aliasing)
        if (isQubitType (declaredType)) {
            // Check if initializer is a simple identifier (reference)
            if (auto* idExpr = dynamic_cast<IdentifierExpr*> (decl->getInitializer ())) {
                auto* sym =
                new Symbol (varName, declaredType, Symbol::Kind::VARIABLE, decl->loc);
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
    auto* sym = new Symbol (paramName, paramType, Symbol::Kind::PARAMETER, decl->loc);
    sym->isInitialized = true; // Parameters are always initialized

    // Parameters of qubit type are always references
    if (isQubitType (paramType)) { sym->isQubitAlias = true; }

    symbolTable.declare (paramName, sym);
}

} // namespace Heisen
