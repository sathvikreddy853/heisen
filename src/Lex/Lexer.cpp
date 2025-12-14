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

    start  = curr;
    char c = advance();

    if (std::isalnum(c)) {
        tokenize_identifier();
        return;
    }

    switch (c) {
    case '+':
        tokens.emplace_back(
        match('=') ? TokenType::PlusAssign : TokenType::Plus, row, col - 1);
        break;
    case '-':
        tokens.emplace_back(
        match('=') ? TokenType::MinusAssign : TokenType::Minus, row, col - 1);
        break;
    case '*':
        if (match('*')) {
            tokens.emplace_back(
            match('=') ? TokenType::ExpAssign : TokenType::Exp, row, col - 1);
        } else {
            tokens.emplace_back(
            match('=') ? TokenType::StarAssign : TokenType::Star, row, col - 1);
        }
        break;
    case '/':
        tokens.emplace_back(
        match('=') ? TokenType::SlashAssign : TokenType::Slash, row, col - 1);
        break;
    case '%':
        tokens.emplace_back(
        match('=') ? TokenType::PercentAssign : TokenType::Percent, row, col - 1);
        break;
    case '=':
        if (match('=')) {
            tokens.emplace_back(TokenType::Equal, row, col - 1);
        } else if (match('>')) {
            tokens.emplace_back(TokenType::DoubleArrow, row, col - 1);
        } else {
            tokens.emplace_back(TokenType::Assign, row, col - 1);
        }
        break;
    case '!':
        tokens.emplace_back(
        match('=') ? TokenType::NotEqual : TokenType::Unknown, row, col - 1);
        break;
    case '<':
        tokens.emplace_back(
        match('=') ? TokenType::LessEqual : TokenType::Less, row, col - 1);
        break;
    case '>':
        tokens.emplace_back(
        match('=') ? TokenType::GreaterEqual : TokenType::Greater, row, col - 1);
        break;
    case '(': tokens.emplace_back(TokenType::LParen, row, col - 1); break;
    case ')': tokens.emplace_back(TokenType::RParen, row, col - 1); break;
    case '{': tokens.emplace_back(TokenType::LCurly, row, col - 1); break;
    case '}': tokens.emplace_back(TokenType::LCurly, row, col - 1); break;
    case '[': tokens.emplace_back(TokenType::LBracket, row, col - 1); break;
    case ']': tokens.emplace_back(TokenType::RBracket, row, col - 1); break;
    case ',': tokens.emplace_back(TokenType::Comma, row, col - 1); break;
    case ':': tokens.emplace_back(TokenType::Colon, row, col - 1); break;
    case ';': tokens.emplace_back(TokenType::Semicolon, row, col - 1); break;
    case '.': tokens.emplace_back(TokenType::Dot, row, col - 1); break;

    default: tokens.emplace_back(TokenType::Unknown, row, col - 1); break;
    }
}

bool Lexer::match(char expected) {
    if (not is_valid()) return false;
    if (source[curr] != expected) return false;
    advance();
    return true;
}

void Lexer::tokenize_identifier() {
    while (std::isalnum(peek()) or peek() == '_') {
        (void)advance();
    }

    std::string_view token = source.substr(start, curr - start);
    tokens.emplace_back(token, TokenType::Identifier, row, col - (curr - start));
}

char Lexer::advance() {
    if (not is_valid()) {
        throw std::runtime_error("invalid increment");
    }

    col++;
    return source[curr++];
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
            (void)advance();
        } else {
            break;
        }
    }
}

} // namespace heisen
