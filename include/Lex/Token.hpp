#ifndef HEISEN_TOKEN_HPP
#define HEISEN_TOKEN_HPP

#include "Macros.hpp"
#include <iostream>

namespace heisen {

enum class TokenType {
    // Keywords
    Int,
    Float,
    Bool,
    String,
    Bit,
    Qubit,
    Cast,
    Generic,
    And,
    Or,
    Not,
    Try,
    Throw,
    Catch,
    Let,
    Const,
    Break,
    Continue,
    Return,
    Func,
    Struct,
    Scope,
    If,
    Elif,
    Else,
    Match,
    With,
    True,
    False,
    For,
    While,
    Gate,
    Circuit,
    Measure,
    Reset,

    // Operators
    Plus,           // +
    Minus,          // -
    Star,           // *
    Slash,          // /
    Percent,        // %
    Exp,            // **
    Assign,         // =

    Equal,          // ==
    NotEqual,       // !=
    Less,           // <
    LessEqual,      // <=
    Greater,        // >
    GreaterEqual,   // >=

    PlusAssign,     // +=
    MinusAssign,    // -=
    StarAssign,     // *=
    SlashAssign,    // /=
    PercentAssign,  // %=
    ExpAssign,      // **=

    // Punctuation
    LParen,         // (
    RParen,         // )
    LBrace,         // {
    RBrace,         // }
    LBracket,       // [
    RBracket,       // ]
    Comma,          // ,
    Colon,          // :
    Semicolon,      // ;
    Dot,            // .

    // Special
    EndOfFile,
};

struct Loc {
    public:
    u32 row, col;
    explicit Loc() : row(-1), col(-1) {}
    explicit Loc(u32 row, u32 col) : row(row), col(col) {}
};

struct Token {
    std::string str;
    TokenType type;
    Loc loc;

    Token(std::string str, TokenType type, u32 row, u32 col)
    : str(str), type(type), loc(row, col) {}

    friend std::ostream& operator<<(std::ostream& out, const Token& token) {
        out << "\"" << token.str << "\"\t\t(" << token.loc.row << ", "
            << token.loc.col << ")";
        return out;
    }
};

} // namespace heisen

#endif // HEISEN_TOKEN_HPP
