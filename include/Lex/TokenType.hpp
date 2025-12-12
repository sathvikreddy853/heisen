#ifndef HEISEN_TOKENTYPE_HPP
#define HEISEN_TOKENTYPE_HPP

#include "Macros.hpp"

#include <iostream>

namespace heisen {

struct TokenType {
    enum Value {
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
        Plus,    // +
        Minus,   // -
        Star,    // *
        Slash,   // /
        Percent, // %
        Exp,     // **
        Assign,  // =

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
        PercentAssign, // %=
        ExpAssign,     // **=

        // Identifiers and Literals
        Identifier,
        IntegerLiteral,
        FloatLiteral,
        StringLiteral,

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
        Unknown
    };

    Value value;

    TokenType(Value v) : value(v) {}

    const char* to_str();
    friend std::ostream& operator<<(std::ostream& os, TokenType type);
};

} // namespace heisen

#endif // HEISEN_TOKENTYPE_HPP