#include "Lex/TokenType.hpp"

namespace heisen {

std::string TokenType::to_str() const {
    switch (value) {
#define TOK(X) \
    case X: return #X; break;
#include "Lex/TokenType.def"
#undef TOK
    default: return "InvalidToken"; break;
    }
}

std::ostream& operator<<(std::ostream& os, TokenType type) {
    os << type.to_str();
    return os;
}

bool TokenType::operator==(TokenType other) const {
    return value == other.value;
}

bool TokenType::operator!=(TokenType other) const {
    return value != other.value;
}

bool TokenType::operator==(Value other) const {
    return value == other;
}

bool TokenType::operator!=(Value other) const {
    return value != other;
}

} // namespace heisen
