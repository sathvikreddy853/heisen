#ifndef HEISEN_TOKEN_HPP
#define HEISEN_TOKEN_HPP

#include "Lex/TokenType.hpp"
#include "Macros.hpp"

#include <iostream>
#include <optional>

namespace heisen {

struct Loc {
    public:
    u32 row, col;
    explicit Loc() : row(-1), col(-1) {}
    explicit Loc(u32 row, u32 col) : row(row), col(col) {}
};

struct Token {
    std::optional<std::string_view> str;
    TokenType type;
    Loc loc;

    explicit Token(TokenType type, u32 row, u32 col)
    : str(std::nullopt), type(type), loc(row, col) {
        switch (type.value) {
        case TokenType::Identifier:
        case TokenType::IntLiteral:
        case TokenType::StringLiteral:
        case TokenType::FloatLiteral:
            throw std::runtime_error(
            std::format("invalid constructor called for token of type {}", type));
            break;

        default: break;
        }
    }

    explicit Token(std::string_view str, TokenType type, u32 row, u32 col)
    : str(str), type(type), loc(row, col) {
        switch (type.value) {
        case TokenType::Identifier:
        case TokenType::IntLiteral:
        case TokenType::StringLiteral:
        case TokenType::FloatLiteral: break;

        default:
            throw std::runtime_error(
            std::format("invalid constructor called for token of type {}", type));
            break;
        }
    }

    std::string to_str() const;
    friend std::ostream& operator<<(std::ostream& out, const Token& token);
};

} // namespace heisen

template <> struct std::formatter<heisen::Token> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const heisen::Token& token, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", token.to_str());
    }
};

#endif // HEISEN_TOKEN_HPP
