#include "Lex/Lexer.hpp"

#include <iostream>

namespace heisen {

void Lexer::tokenize_impl() {
    while (curr < length) {
        skip_whitespace();
        if (not is_valid()) return;
        char c = source[curr];
        if (std::isalnum(c)) {
            tokenize_identifier();
        } else if (c == '+') {
            advance();
            if (curr < length and source[curr] == '=') {
                tokens.push_back("+=");
                advance();
            } else {
                tokens.push_back("+");
            }
        } else {
            std::string token;
            token.push_back(c);
            tokens.push_back(token);
            advance();
        }
    }
}

void Lexer::tokenize_identifier() {
    std::string token;
    char c = source[curr];
    do {
        token.push_back(c);
        advance();
        c = source[curr];
    } while (std::isalnum(c) or c == '_');
    tokens.push_back(token);
}

std::vector<std::string> Lexer::tokenize() {
    this->tokenize_impl();
    LOG(tokens.size());
    return tokens;
}

void Lexer::advance() {
    curr += 1;
    if (curr > length) {
        throw std::out_of_range("invalid increment");
    }
}

bool Lexer::is_valid() const {
    return (curr < length);
}

char Lexer::peek() const {
    if (not is_valid()) return '\0';
    return source[curr];
}

void Lexer::skip_whitespace() {
    while (is_valid() and std::isspace(peek())) {
        advance();
    }
}

} // namespace heisen
