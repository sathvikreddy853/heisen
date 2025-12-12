#ifndef HEISEN_TOKEN_HPP
#define HEISEN_TOKEN_HPP

#include "Lex/TokenType.hpp"
#include "Macros.hpp"

#include <iostream>

namespace heisen {

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

    const char* to_str() const {
        // FIX-REQUIRED
        return "FIX-REQUIRED";
    }

    friend std::ostream& operator<<(std::ostream& out, const Token& token) {
        out << "Token(" << token.type << ", " << token.str << ", " << token.loc.row << ", "
            << token.loc.col << ")";
        return out;
    }
};

} // namespace heisen

template <>
struct std::formatter<heisen::Token> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const heisen::Token& token, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", token.to_str());
    }
};

#endif // HEISEN_TOKEN_HPP
