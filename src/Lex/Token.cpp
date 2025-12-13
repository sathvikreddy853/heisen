#include "Lex/Token.hpp"

#include <sstream>

namespace heisen {

std::string Token::to_str() const {
    std::stringstream ss;
    std::stringstream type_ss;

    switch (type.value) {
    case TokenType::Identifier:
        type_ss << GREEN << BOLD << type << RESET;
        type_ss << "(" << GREEN << str << RESET << ")";
        break;
    case TokenType::IntLiteral:
    case TokenType::FloatLiteral:
    case TokenType::StringLiteral:
        type_ss << YELLOW << BOLD << type << RESET;
        type_ss << "(" << YELLOW << str << RESET << ")";
        break;

    default: type_ss << type; break;
    }

    ss << "Token[ " << type_ss.str() << " Loc(" << loc.row << ":" << loc.col << ") ]";

    std::string str = ss.str();
    return str;
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
    out << token.to_str();
    return out;
}

} // namespace heisen
