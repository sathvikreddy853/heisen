#ifndef HEISEN_LEXER_HPP
#define HEISEN_LEXER_HPP

#include "Lex/Token.hpp"
#include "Macros.hpp"

#include <vector>

namespace Heisen {

struct Vars {
    std::string source;
    int curr = 0;
    void advance () {
        curr += 1;
        if (curr > source.length ()) {
            throw std::out_of_range("invalid increment");
        }
    }
};

inline std::vector<std::string> tokens;
inline Vars vars;
void tokenize ();

class Lexer {
    private:
    std::vector<std::string> source;
    int current = 0;
    int start   = 0;
    Lexer (const std::vector<std::string>& source) : source (source) {
    }

    public:
    std::vector<Token> tokenize ();
    void skip_whitespace ();
};

} // namespace Heisen

#endif // HEISEN_LEXER_HPP
