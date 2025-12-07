#include "Lex/Lexer.hpp"

namespace Heisen {

void tokenize_identifier () {
    std::string token;
    char c = vars.source[vars.curr];
    do {
        std::cout << c << '-';
        token.push_back (c);
        vars.advance ();
        c = vars.source[vars.curr];
    } while (std::isalnum (vars.source[vars.curr]) or vars.source[vars.curr] == '_');
    std::cout << std::endl;
    tokens.push_back(token);
}

void tokenize () {
    while (vars.curr < vars.source.length ()) {
        char c = vars.source[vars.curr];
        if (std::isspace (c)) {
            vars.advance ();
        } else if (std::isalnum (c)) {
            // std::string token;
            tokenize_identifier ();
            // while (std::isalnum (c)) {
            //     std::cout << c << '-';
            //     token.push_back (c);
            //     vars.advance ();
            //     c = vars.source[vars.curr];
            // }
            // std::cout << std::endl;
            // tokens.push_back (token);
        } else if (c == '+') {
            std::string token;
            std::cout << c << '-' << std::endl;
            token.push_back (c);
            tokens.push_back (token);
            vars.advance ();
        } else {
            std::string token;
            std::cout << c << '-' << std::endl;
            token.push_back (c);
            tokens.push_back (token);
            vars.advance ();
        }
    }
}

} // namespace Heisen
