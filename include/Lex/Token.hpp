#ifndef HEISEN_TOKEN_HPP
#define HEISEN_TOKEN_HPP

#include "Macros.hpp"
#include <iostream>

namespace heisen {

struct Loc {
    public:
    u32 row, col;
    explicit Loc () : row (-1), col (-1) {}
    explicit Loc (u32 row, u32 col) : row (row), col (col) {}
};

struct Token {
    std::string str;
    Loc loc;

    Token (std::string str, u32 row, u32 col)
    : str (str), loc (row, col) {}

    friend std::ostream& operator<< (std::ostream& out, const Token& token) {
        out << "\"" << token.str << "\"\t\t(" << token.loc.row << ", "
            << token.loc.col << ")";
        return out;
    }
};

} // namespace heisen

#endif // HEISEN_TOKEN_HPP
