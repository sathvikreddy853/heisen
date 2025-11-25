#include <Scope.hpp>

namespace Heisen {

bool Scope::declare (const std::string& name, Symbol* symbol) {
    if (symbols.find (name) != symbols.end ()) {
        return false; // Already declared in this scope
    }
    symbols[name] = symbol;
    return true;
}

Symbol* Scope::lookup (const std::string& name) {
    auto it = symbols.find (name);
    if (it != symbols.end ()) { return it->second; }
    if (parent) { return parent->lookup (name); }
    return nullptr;
}

Symbol* Scope::lookupLocal (const std::string& name) {
    auto it = symbols.find (name);
    return (it != symbols.end ()) ? it->second : nullptr;
}

Scope* Scope::getParent () const {
    return parent;
}

} // namespace Heisen
