#ifndef HEISEN_SYMBOL_HPP
#define HEISEN_SYMBOL_HPP

#include <Macros.hpp>
#include <Token.hpp>
#include <SemaType.hpp>

namespace Heisen {

struct Symbol {
    enum class Kind { VARIABLE, FUNCTION, PARAMETER };

    std::string name;
    SemanticType* type;
    Kind kind;
    bool isInitialized;
    bool isQubitAlias; // For tracking qubit references
    Location loc;

    Symbol (std::string n, SemanticType* t, Kind k, Location l)
    : name (std::move (n)), type (t), kind (k), isInitialized (false), isQubitAlias (false), loc (l) {}
};

} // namespace Heisen

#endif // HEISEN_SYMBOL_HPP
