#ifndef HEISEN_SCOPE_HPP
#define HEISEN_SCOPE_HPP

#include "Macros.hpp"
#include "Symbol.hpp"

namespace Heisen {

class Scope {
    std::map<std::string, Symbol*> symbols;
    Scope* parent;

    public:
    Scope (Scope* p = nullptr) : parent (p) {
    }

    ~Scope () {
        for (auto& pair : symbols) {
            delete pair.second->type;
            delete pair.second;
        }
    }

    bool declare (const std::string& name, Symbol* symbol);
    Symbol* lookup (const std::string& name);
    Symbol* lookupLocal (const std::string& name);
    Scope* getParent () const;

    void print (int indent = 0) const;
    const std::map<std::string, Symbol*>& getSymbols () const {
        return symbols;
    }
};

} // namespace Heisen

#endif
