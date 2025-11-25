#ifndef HEISEN_SCOPE_HPP
#define HEISEN_SCOPE_HPP

#include <Macros.hpp>
#include <Symbol.hpp>

namespace Heisen {

class Scope {
    std::map<std::string, Symbol*> symbols;
    Scope* parent;

    public:
    Scope (Scope* p = nullptr) : parent (p) {}

    ~Scope () {
        for (auto& pair : symbols) {
            delete pair.second->type;
            delete pair.second;
        }
    }

    bool declare (const std::string& name, Symbol* symbol) {
        if (symbols.find (name) != symbols.end ()) {
            return false; // Already declared in this scope
        }
        symbols[name] = symbol;
        return true;
    }

    Symbol* lookup (const std::string& name) {
        auto it = symbols.find (name);
        if (it != symbols.end ()) { return it->second; }
        if (parent) { return parent->lookup (name); }
        return nullptr;
    }

    Symbol* lookupLocal (const std::string& name) {
        auto it = symbols.find (name);
        return (it != symbols.end ()) ? it->second : nullptr;
    }

    Scope* getParent () const { return parent; }
};

} // namespace Heisen

#endif
