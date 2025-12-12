#include "Lex/TokenType.hpp"

namespace heisen {

const char* TokenType::to_str() const {
    // FIX-REQUIRED
    return "FIX-REQUIRED";
}

std::ostream& operator<<(std::ostream& os, TokenType type) {
    os << type.to_str();
    return os;
}

} // namespace heisen
