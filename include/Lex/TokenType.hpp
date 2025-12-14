#ifndef HEISEN_TOKENTYPE_HPP
#define HEISEN_TOKENTYPE_HPP

#include "Macros.hpp"

#include <format>
#include <iostream>

namespace heisen {

struct TokenType {
    enum Value : i32 {
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

        SingleArrow,   // ->
        DoubleArrow,   // =?

        // Identifiers and Literals
        Identifier,
        IntLiteral,
        FloatLiteral,
        StringLiteral,

        // Punctuation
        LParen,    // (
        RParen,    // )
        LCurly,    // {
        RCurly,    // }
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

    bool operator==(TokenType other) const;
    bool operator!=(TokenType other) const;
    bool operator==(Value other) const;
    bool operator!=(Value other) const;

    std::string to_str() const;
    friend std::ostream& operator<<(std::ostream& os, TokenType type);
};

} // namespace heisen

template <> struct std::formatter<heisen::TokenType> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const heisen::TokenType& type, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", type.to_str());
    }
};

#endif // HEISEN_TOKENTYPE_HPP