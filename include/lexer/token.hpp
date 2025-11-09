#ifndef HEISEN_TOKEN_HPP
#define HEISEN_TOKEN_HPP

#include <macros.hpp>
#include<variant>

using TokValue = std::variant<double, long long, std::string>;

namespace Heisen {

enum class Type {
    IDENTIFIER,
    FLOAT_LITERAL,
    INT_LITERAL,
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

struct Location {
    int line;
    int column;
    std::string filename;
    
    Location() : line(1), column(1), filename("") {}
    Location(int l, int c, const std::string& f = "") 
        : line(l), column(c), filename(f) {}
    
    Location(const Location& other) = default;
    Location& operator=(const Location& other) = default;
};



struct Token {
    Location loc;
    TokValue value;
      Token( Location l, TokValue v)
        :  loc(l), value(std::move(v)) {}
};

} // namespace Heisen

#endif // HEISEN_TOKEN_HPP
