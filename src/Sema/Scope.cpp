#include "Sema/Scope.hpp"
#include "Macros.hpp"

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

void Scope::print (int indent) const {
    std::string prefix (indent * 2, ' ');

    for (const auto& pair : symbols) {
        const Symbol* sym = pair.second;
        std::cout << prefix << "  " << CYAN << sym->name << RESET_COLOR << ": ";

        // Print type
        if (sym->type) {
            std::cout << sym->type->toString ();
        } else {
            std::cout << "???";
        }

        // Print kind
        std::cout << " (" << GREEN;
        switch (sym->kind) {
        case Symbol::Kind::VARIABLE: std::cout << "variable"; break;
        case Symbol::Kind::FUNCTION: std::cout << "function"; break;
        case Symbol::Kind::PARAMETER: std::cout << "parameter"; break;
        }
        std::cout << RESET_COLOR << ")";

        std::cout << std::endl;
    }
}

} // namespace Heisen
