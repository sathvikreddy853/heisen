#include "Lex/Lexer.hpp"

namespace Heisen {

int current = 0;

void tokenize (const std::string& source) {
    while (current < source.length ()) {
        char c = source[current];
        if (std::isspace (c)) {
            current += 1;
        } else if (std::isalnum (c)) {
            std::string token;
            while (std::isalnum(c)) {
                // std::cout << c << '-';
                token.push_back(c);
                c = source[++current];
            }
            // std::cout << std::endl;
            tokens.push_back(token);
        } else if (c == '+') {
            std::string token;
            // std::cout << c << '-' << std::endl;
            token.push_back(c);
            tokens.push_back(token);
            current += 1;
        } else {
            std::string token;
            // std::cout << c << '-' << std::endl;
            token.push_back(c);
            tokens.push_back(token);
            current += 1;
        }
    }
}

} // namespace Heisen
