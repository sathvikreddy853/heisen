#ifndef HEISEN_TOKEN_HPP
#define HEISEN_TOKEN_HPP

#include "Macros.hpp"

using TokValue = std::variant<double, long long, std::string, std::monostate>;

namespace Heisen {

struct Location {
    int line;
    int column;
    std::string filename;

    Location () : line (-1), column (-1), filename ("") {}
    Location (int l, int c, const std::string& f = "") : line (l), column (c), filename (f) {}

    Location (const Location& other)            = default;
    Location& operator= (const Location& other) = default;
    friend std::ostream& operator<< (std::ostream&, const Location&);
};

inline std::ostream& operator<< (std::ostream& os, const Location& loc) {
    if (loc.line <= 0 || loc.column <= 0) {
        os << "loc: <unknown>";
    } else if (not loc.filename.empty ()) {
        os << "loc: (" << loc.filename << ':' << loc.line << ", " << loc.column << ')';
    } else {
        os << "loc: (" << loc.line << ", " << loc.column << ')';
    }
    
    return os;
}


struct Token {
    Location loc;
    TokValue value;
    Token (Location l, TokValue v) : loc (l), value (std::move (v)) {}
};

} // namespace Heisen

#endif // HEISEN_TOKEN_HPP
