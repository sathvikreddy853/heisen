#include "Sema/SymbolTable.hpp"

namespace Heisen {

SymbolTable::SymbolTable () {
    currentScope = new Scope ();
    allScopes.push_back (currentScope);
}

SymbolTable::~SymbolTable () {
    for (auto* scope : allScopes) { delete scope; }
}

void SymbolTable::enterScope () {
    Scope* newScope = new Scope (currentScope);
    allScopes.push_back (newScope);
    currentScope = newScope;
}

void SymbolTable::exitScope () {
    if (currentScope->getParent ()) {
        currentScope = currentScope->getParent ();
    }
}

bool SymbolTable::declare (const std::string& name, Symbol* symbol) {
    return currentScope->declare (name, symbol);
}

Symbol* SymbolTable::lookup (const std::string& name) {
    return currentScope->lookup (name);
}

Symbol* SymbolTable::lookupLocal (const std::string& name) {
    return currentScope->lookupLocal (name);
}

Scope* SymbolTable::getCurrentScope () const {
    return currentScope;
}

}; // namespace Heisen