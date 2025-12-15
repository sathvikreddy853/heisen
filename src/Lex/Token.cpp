#include "Lex/Token.hpp"

#include <sstream>

namespace heisen {

std::string Token::to_str() const {
    std::stringstream token_stream;
    std::stringstream token_type_stream;

    switch (type.value) {
    case TokenType::Identifier:
        token_type_stream << DIM_PURPLE << BOLD << type << RESET;
        token_type_stream << ", " << DIM_PURPLE << str.value() << RESET;
        break;

#define LITERAL(X, Y) case TokenType::X:
#include "Lex/TokenType.def"
#undef LITERAL
        token_type_stream << DIM_YELLOW << BOLD << type << RESET;
        token_type_stream << ", " << DIM_YELLOW << str.value() << RESET;
        break;

#define TYPENAME(X, Y) case TokenType::X:
#include "Lex/TokenType.def"
#undef TYPENAME
        token_type_stream << DIM_PURPLE << BOLD << type << RESET;
        break;

#define KEYWORD(X, Y) case TokenType::X:
#include "Lex/TokenType.def"
#undef KEYWORD
        token_type_stream << DIM_BLUE << BOLD << type << RESET;
        break;

#define OPERATOR(X, Y) case TokenType::X:
#include "Lex/TokenType.def"
#undef OPERATOR
        token_type_stream << ORANGE << BOLD << type << RESET;
        break;

#define PUNCTUATOR(X, Y) case TokenType::X:
#include "Lex/TokenType.def"
#undef PUNCTUATOR
        token_type_stream << TEAL << BOLD << type << RESET;
        break;

    case TokenType::EndOfFile:
        token_type_stream << DARK_GRAY << BOLD << type << RESET;
        break;

    case TokenType::Unknown: token_type_stream << RED << BOLD << type << RESET; break;

    default: token_type_stream << type; break;
    }

    token_stream << "Token(" << token_type_stream.str() << ", " << loc.row << ":" << loc.col << ")";

    std::string str = token_stream.str();
    return str;
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
    out << token.to_str();
    return out;
}

} // namespace heisen
