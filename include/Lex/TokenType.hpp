#ifndef HEISEN_TOKENTYPE_HPP
#define HEISEN_TOKENTYPE_HPP

#include "Macros.hpp"

#include <format>
#include <iostream>

namespace heisen {

struct TokenType {
    enum Value : i32 {

#define TOK(X) X,
#include "Lex/TokenType.def"
#undef TOK

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