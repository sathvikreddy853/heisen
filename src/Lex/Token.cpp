#include "Lex/Token.hpp"

#include <sstream>

namespace heisen {

std::string Token::to_str() const {
    std::stringstream token_stream;
    std::stringstream token_type_stream;

    switch (type.value) {
    case TokenType::Identifier:
        token_type_stream << LIGHT_BLUE << BOLD << type << RESET;
        token_type_stream << "(" << LIGHT_BLUE << str.value() << RESET << ")";
        break;

    case TokenType::IntLiteral:
    case TokenType::FloatLiteral:
    case TokenType::StringLiteral:
        token_type_stream << DIM_YELLOW << BOLD << type << RESET;
        token_type_stream << "(" << DIM_YELLOW << str.value() << RESET << ")";
        break;

    case TokenType::Int:
    case TokenType::Float:
    case TokenType::Bool:
    case TokenType::String:
    case TokenType::Bit:
    case TokenType::Qubit:
        token_type_stream << DIM_PURPLE << BOLD << type << RESET;
        break;

    case TokenType::Cast:
    case TokenType::Generic:
    case TokenType::And:
    case TokenType::Or:
    case TokenType::Not:
    case TokenType::Try:
    case TokenType::Throw:
    case TokenType::Catch:
    case TokenType::Let:
    case TokenType::Const:
    case TokenType::Break:
    case TokenType::Continue:
    case TokenType::Return:
    case TokenType::Func:
    case TokenType::Struct:
    case TokenType::Scope:
    case TokenType::If:
    case TokenType::Elif:
    case TokenType::Else:
    case TokenType::Match:
    case TokenType::With:
    case TokenType::True:
    case TokenType::False:
    case TokenType::For:
    case TokenType::While:
    case TokenType::Gate:
    case TokenType::Circuit:
    case TokenType::Measure:
    case TokenType::Reset:
        token_type_stream << LIGHT_GREEN << BOLD << type << RESET;
        break;

    case TokenType::Plus:
    case TokenType::Minus:
    case TokenType::Star:
    case TokenType::Slash:
    case TokenType::Percent:
    case TokenType::Exp:
    case TokenType::Assign:
    case TokenType::Equal:
    case TokenType::NotEqual:
    case TokenType::Less:
    case TokenType::LessEqual:
    case TokenType::Greater:
    case TokenType::GreaterEqual:
    case TokenType::PlusAssign:
    case TokenType::MinusAssign:
    case TokenType::StarAssign:
    case TokenType::SlashAssign:
    case TokenType::PercentAssign:
    case TokenType::ExpAssign:
    case TokenType::SingleArrow:
    case TokenType::DoubleArrow:
        token_type_stream << ORANGE << BOLD << type << RESET;
        break;

    case TokenType::RParen:
    case TokenType::LParen:
    case TokenType::LCurly:
    case TokenType::RCurly:
    case TokenType::LBracket:
    case TokenType::RBracket:
        token_type_stream << DARK_GRAY << BOLD << type << RESET;
        break;

    case TokenType::Comma:
    case TokenType::Colon:
    case TokenType::Semicolon:
    case TokenType::Dot:
    case TokenType::EndOfFile:
        token_type_stream << TEAL << BOLD << type << RESET;
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
