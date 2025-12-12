#ifndef HEISEN_TOKENTYPE_HPP
#define HEISEN_TOKENTYPE_HPP

#include "Macros.hpp"

#include <iostream>
#include <format>

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

    const char* to_str() const;
    friend std::ostream& operator<<(std::ostream& os, TokenType type);
};

} // namespace heisen

template <>
struct std::formatter<heisen::TokenType> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const heisen::TokenType& type, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", type.to_str());
    }
};

#endif // HEISEN_TOKENTYPE_HPP