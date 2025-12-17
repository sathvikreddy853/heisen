#include "Parse/Parser.hpp"

namespace heisen {

bool Parser::parse() {
    return false;
};

bool Parser::parse_function_definition() {
    return false;
}

bool Parser::parse_statement() {
    Token tok = consume();
    switch (tok.type.value) {
    case TokenType::Let:
    case TokenType::Const:
        if (not match(TokenType::Identifier)) throw std::runtime_error("not an id");
        if (not match(TokenType::Colon)) throw std::runtime_error("not colon");
        if (not parse_type()) throw std::runtime_error("not a type");
        if (match(TokenType::Semicolon)) return true;
        if (not match(TokenType::Assign)) throw std::runtime_error("not an assignemnt statement");
        if (not parse_expression()) throw std::runtime_error("not a expression");
        break;
    default: throw std::runtime_error("invalid parse");
    }
    return true;
}

Token Parser::peek() {
    return tokens[curr];
}

Token Parser::consume() {
    return tokens[curr++];
}

bool Parser::match(TokenType expected) {
    if (peek().type != expected) {
        return false;
    }

    consume();
    return true;
}

bool Parser::parse_type() {
    consume();
    return true;
}

bool Parser::parse_expression() {
    consume();
    return true;
}

} // namespace heisen
