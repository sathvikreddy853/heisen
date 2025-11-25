#ifndef HEISEN_SYMBOL_TABLE_HPP
#define HEISEN_SYMBOL_TABLE_HPP

#include <Scope.hpp>
#include <Symbol.hpp>

namespace Heisen {

class SymbolTable {
    Scope* currentScope;
    std::vector<Scope*> allScopes;

    public:
    SymbolTable ();
    ~SymbolTable ();

    void enterScope ();
    void exitScope ();
    bool declare (const std::string& name, Symbol* symbol);
    Symbol* lookup (const std::string& name);
    Symbol* lookupLocal (const std::string& name);
    Scope* getCurrentScope () const;
};

} // namespace Heisen

#endif // HEISEN_SYMBOL_TABLE_HPP
