#ifndef HEISEN_SYMBOL_TABLE_HPP
#define HEISEN_SYMBOL_TABLE_HPP

#include <Symbol.hpp>
#include <Scope.hpp>

namespace Heisen {

class SymbolTable {
    Scope* currentScope;
    std::vector<Scope*> allScopes;

    public:
    SymbolTable () {
        currentScope = new Scope ();
        allScopes.push_back (currentScope);
    }

    ~SymbolTable () {
        for (auto* scope : allScopes) { delete scope; }
    }

    void enterScope () {
        Scope* newScope = new Scope (currentScope);
        allScopes.push_back (newScope);
        currentScope = newScope;
    }

    void exitScope () {
        if (currentScope->getParent ()) { currentScope = currentScope->getParent (); }
    }

    bool declare (const std::string& name, Symbol* symbol) {
        return currentScope->declare (name, symbol);
    }

    Symbol* lookup (const std::string& name) { return currentScope->lookup (name); }

    Symbol* lookupLocal (const std::string& name) { return currentScope->lookupLocal (name); }

    Scope* getCurrentScope () const { return currentScope; }
};

} // namespace Heisen

#endif // HEISEN_SYMBOL_TABLE_HPP
