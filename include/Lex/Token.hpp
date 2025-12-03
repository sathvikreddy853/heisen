#include "Macros.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace Heisen {

enum class TokenType {
    // Keywords
    Func,
    Let,
    Return,
    If,
    Else,
    For,
    While,
    True,
    False,

    // Identifiers and Literals
    Identifier,
    Int,
    Float,
    String,

    // Operators
    Plus,         // +
    Minus,        // -
    Star,         // *
    Slash,        // /
    Assign,       // =
    Equal,        // ==
    NotEqual,     // !=
    Less,         // <
    LessEqual,    // <=
    Greater,      // >
    GreaterEqual, // >=
    PlusAssign,    // +=
    MinusAssign,   // -=
    StarAssign,    // *=
    SlashAssign,   // /=

    // Punctuation
    LParen,    // (
    RParen,    // )
    LBrace,    // {
    RBrace,    // }
    LBracket,  // [
    RBracket,  // ]
    Comma,     // ,
    Colon,     // :
    Semicolon, // ;
    Dot,       // .

    // Special
    EndOfFile,
    Unknown,
};

struct Token {
    TokenType type;
    std::optional<std::string> lexeme;
    int line;
    int column;

    Token (TokenType type, std::string_view lexeme, int line, int column)
    : type (type), lexeme (lexeme), line (line), column (column) {
    }
};


inline std::string tokenTypeToString (TokenType type) {
    switch (type) {
    case TokenType::Func: return "Func";
    case TokenType::Let: return "Let";
    case TokenType::Return: return "Return";
    case TokenType::If: return "If";
    case TokenType::Else: return "Else";
    case TokenType::For: return "For";
    case TokenType::While: return "While";
    case TokenType::True: return "True";
    case TokenType::False: return "False";
    case TokenType::Identifier: return "Identifier";
    case TokenType::Int: return "Int";
    case TokenType::Float: return "Float";
    case TokenType::String: return "String";
    case TokenType::Plus: return "Plus";
    case TokenType::Minus: return "Minus";
    case TokenType::Star: return "Star";
    case TokenType::Slash: return "Slash";
    case TokenType::Assign: return "Equal";
    case TokenType::Equal: return "Equal";
    case TokenType::NotEqual: return "NotEqual";
    case TokenType::Less: return "Less";
    case TokenType::LessEqual: return "LessEqual";
    case TokenType::Greater: return "Greater";
    case TokenType::GreaterEqual: return "GreaterEqual";
    case TokenType::PlusAssign: return "PlusAssign";
    case TokenType::MinusAssign: return "MinusAssign";
    case TokenType::StarAssign: return "StarAssign";
    case TokenType::SlashAssign: return "SlashAssign";
    case TokenType::LParen: return "LParen";
    case TokenType::RParen: return "RParen";
    case TokenType::LBrace: return "LBrace";
    case TokenType::RBrace: return "RBrace";
    case TokenType::LBracket: return "LBracket";
    case TokenType::RBracket: return "RBracket";
    case TokenType::Comma: return "Comma";
    case TokenType::Colon: return "Colon";
    case TokenType::Semicolon: return "Semicolon";
    case TokenType::Dot: return "Dot";
    case TokenType::EndOfFile: return "EOF";
    case TokenType::Unknown: return "Unknown";
    default: return "Invalid";
    }
}

} // namespace Heisen