#ifndef HEISEN_LEXER_HPP
#define HEISEN_LEXER_HPP

#include "Lex/Token.hpp"
#include "Macros.hpp"

#include <vector>

namespace Heisen {

class Lexer {
    private:
    std::vector<std::string> source;
    int current = 0;
    int start   = 0;
    Lexer (const std::vector<std::string>& source) : source (source) {
    }

    public:
    std::vector<Token> tokenize ();
    void skipWhitespace ();
};

} // namespace Heisen

#endif // HEISEN_LEXER_HPP
