#ifndef HEISEN_TOKEN_HPP
#define HEISEN_TOKEN_HPP

#include <macros.hpp>
#include<optional>
namespace Heisen {

enum class Type {
    IDENTIFIER,
    LITERAL,
    TYPE_NAME,
    KEYWORD,
    OPERATOR,
    GATE,
    BRACKET,
    COMMA,
    SEMICOLON,
    COLON,
    DOT,
};

struct Loc {
    int row, col;
    Loc (int row, int col) : row (row), col (col) {}
};

struct Token {
    Type type;
    Loc loc;
    std::optional<std::string> value;
};

} // namespace Heisen

#endif // HEISEN_TOKEN_HPP
