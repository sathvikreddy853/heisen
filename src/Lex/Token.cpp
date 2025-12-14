#include "Lex/Token.hpp"

#include <sstream>

namespace heisen {

std::string Token::to_str() const {
    std::stringstream token_stream;
    std::stringstream token_type_stream;

    switch (type.value) {
    case TokenType::Identifier:
        token_type_stream << LIGHT_BLUE << type << RESET;
        token_type_stream << "(" << LIGHT_BLUE << str.value() << RESET << ")";
        break;
    case TokenType::IntLiteral:
    case TokenType::FloatLiteral:
    case TokenType::StringLiteral:
        token_type_stream << YELLOW << BOLD << type << RESET;
        token_type_stream << "(" << YELLOW << str.value() << RESET << ")";
        break;

    case TokenType::Int:
    case TokenType::Float:
    case TokenType::Bool:
    case TokenType::String:
    case TokenType::Bit:
    case TokenType::Qubit:
        token_type_stream << RED << BOLD << type << RESET;
        break;

    case TokenType::Unknown:
        token_type_stream << RED << BOLD << type << RESET;
        break;

    default: token_type_stream << type; break;
    }

    token_stream << "Token[ " << token_type_stream.str() << " Loc(" << loc.row
                 << ":" << loc.col << ") ]";

    std::string str = token_stream.str();
    return str;
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
    out << token.to_str();
    return out;
}

} // namespace heisen
