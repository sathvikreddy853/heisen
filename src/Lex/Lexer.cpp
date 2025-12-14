#include "Lex/Lexer.hpp"

#include <iostream>
#include <map>

namespace heisen {

const std::map<std::string_view, TokenType> keywords = {
    #define KEYWORD(X, Y) {Y, TokenType::X},
    #include "Lex/TokenType.def"
    #undef KEYWORD
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

    if (std::isalpha(c) or c == '_') {
        tokenize_identifier();
        return;
    } else if (std::isdigit(c)) {
        tokenize_numeric();
        return;
    } else if (c == '.' && match([](char c) {
                   return std::isdigit(c);
               })) {
        tokenize_numeric(true);
        return;
    }

    switch (c) {
    case '+':
        tokens.emplace_back(match('=') ? TokenType::PlusAssign : TokenType::Plus, row, col - 1);
        break;
    case '-':
        tokens.emplace_back(match('=') ? TokenType::MinusAssign : TokenType::Minus, row, col - 1);
        break;
    case '*':
        if (match('*')) {
            tokens.emplace_back(match('=') ? TokenType::ExpAssign : TokenType::Exp, row, col - 1);
        } else {
            tokens.emplace_back(match('=') ? TokenType::StarAssign : TokenType::Star, row, col - 1);
        }
        break;
    case '/':
        tokens.emplace_back(match('=') ? TokenType::SlashAssign : TokenType::Slash, row, col - 1);
        break;
    case '%':
        tokens.emplace_back(match('=') ? TokenType::PercentAssign : TokenType::Percent, row, col - 1);
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
        tokens.emplace_back(match('=') ? TokenType::NotEqual : TokenType::Unknown, row, col - 1);
        break;
    case '<':
        tokens.emplace_back(match('=') ? TokenType::LessEqual : TokenType::Less, row, col - 1);
        break;
    case '>':
        tokens.emplace_back(match('=') ? TokenType::GreaterEqual : TokenType::Greater, row, col - 1);
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
    case '\"': tokenize_str_literal(); break;

    default: tokens.emplace_back(TokenType::Unknown, row, col - 1); break;
    }
}

void Lexer::tokenize_identifier() {
    while (std::isalnum(peek()) or peek() == '_') {
        (void)advance();
    }

    std::string_view token = source.substr(start, curr - start);

    if (keywords.find(token) != keywords.end()) {
        tokens.emplace_back(keywords.at(token), row, col - (curr - start));
        return;
    }

    tokens.emplace_back(token, TokenType::Identifier, row, col - (curr - start));
}

void Lexer::tokenize_numeric(bool is_float) {
    while (std::isdigit(peek()) or peek() == '.') {
        if (is_float and peek() == '.') {
            throw std::runtime_error("second floating point symbol in numeric");
        } else if (not is_float and peek() == '.') {
            is_float = true;
        }

        (void)advance();
    }

    std::string_view token = source.substr(start, curr - start);
    if (not is_float) {
        tokens.emplace_back(token, TokenType::IntLiteral, row, col - (curr - start));
    } else {
        tokens.emplace_back(token, TokenType::FloatLiteral, row, col - (curr - start));
    }
}

void Lexer::tokenize_str_literal() {
    while (peek() != '\"') {
        std::cout << peek() ;
        (void)advance();
    }
    (void)advance();

    std::string_view token = source.substr(start + 1, curr - start - 2);
    tokens.emplace_back(token, TokenType::StringLiteral, row, col - (curr - start));
}

bool Lexer::match(char expected) {
    if (not is_valid()) return false;
    if (source[curr] != expected) return false;
    advance();
    return true;
}

bool Lexer::match(std::function<bool(char)> matcher) {
    if (not is_valid()) return false;
    if (not matcher(source[curr])) return false;
    advance();
    return true;
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
