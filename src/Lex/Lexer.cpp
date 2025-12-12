#include "Lex/Lexer.hpp"

#include <iostream>
#include <map>

namespace heisen {

const std::map<std::string, TokenType> keywords = {
    { "int", TokenType::Int },
    { "float", TokenType::Float },
    { "bool", TokenType::Bool },
    { "string", TokenType::String },
    { "bit", TokenType::Bit },
    { "qubit", TokenType::Qubit },
    { "cast", TokenType::Cast },
    { "and", TokenType::And },
    { "or", TokenType::Or },
    { "not", TokenType::Not },
    { "try", TokenType::Try },
    { "throw", TokenType::Throw },
    { "catch", TokenType::Catch },
    { "let", TokenType::Let },
    { "const", TokenType::Const },
    { "break", TokenType::Break },
    { "continue", TokenType::Continue },
    { "generic", TokenType::Generic },
    { "struct", TokenType::Struct },
    { "scope", TokenType::Scope },
    { "if", TokenType::If },
    { "elif", TokenType::Elif },
    { "else", TokenType::Else },
    { "match", TokenType::Match },
    { "with", TokenType::With },
    { "true", TokenType::True },
    { "false", TokenType::False },
    { "for", TokenType::For },
    { "while", TokenType::While },
    { "func", TokenType::Func },
    { "return", TokenType::Return },
    { "gate", TokenType::Gate },
    { "circuit", TokenType::Circuit },
    { "measure", TokenType::Measure },
    { "reset", TokenType::Reset },
};

std::vector<Token> Lexer::tokenize() {
    while (is_valid()) scan_token();
    LOG(tokens.size());
    return tokens;
}

void Lexer::scan_token() {
    skip_whitespace();
    if (not is_valid()) return;

    char c = source[curr];
    if (std::isalnum(c)) {
        tokenize_identifier();
    } else if (c == '+') {
        advance();
        if (curr < length and source[curr] == '=') {
            tokens.emplace_back("+=", TokenType::PlusAssign, row, col - 1);
            advance();
        } else {
            tokens.emplace_back("+", TokenType::Plus, row, col - 1);
        }
    } else {
        std::string token;
        token.push_back(c);
        tokens.emplace_back(token, TokenType::Unknown, row, col);
        advance();
    }
}

void Lexer::tokenize_identifier() {
    std::string token;
    char c    = source[curr];
    int start = col;
    do {
        token.push_back(c);
        advance();
        c = source[curr];
    } while (std::isalnum(c) or c == '_');
    tokens.emplace_back(token, TokenType::Identifier, row, start);
}

void Lexer::advance() {
    curr += 1;
    col += 1;
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
    while (true) {
        char c = peek();
        if (is_valid() and c == '\n') {
            curr += 1;
            row += 1;
            col = 1;
        } else if (is_valid() and std::isspace(peek())) {
            advance();
        } else {
            break;
        }
    }
}

} // namespace heisen
