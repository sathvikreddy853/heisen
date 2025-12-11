#include "Lex/Lexer.hpp"

#include <iostream>

namespace heisen {

void tokenize_identifier () {
    std::string token;
    char c = vars.source[vars.curr];
    do {
        token.push_back (c);
        vars.advance ();
        c = vars.source[vars.curr];
    } while (std::isalnum (c) or c == '_');
    tokens.push_back (token);
}

void tokenize () {
    while (vars.curr < vars.length) {
        char c = vars.source[vars.curr];
        if (std::isspace (c)) {
            vars.advance ();
        } else if (std::isalnum (c)) {
            tokenize_identifier ();
        } else if (c == '+') {
            vars.advance();
            if (vars.curr < vars.length and vars.source[vars.curr] == '=') {
                tokens.push_back("+=");
                vars.advance ();
            } else {
                tokens.push_back("+");
            }
        } else {
            std::string token;
            token.push_back (c);
            tokens.push_back (token);
            vars.advance ();
        }
    }
}

} // namespace heisen
