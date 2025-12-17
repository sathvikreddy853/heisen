#ifndef HEISEN_PARSER_HPP
#define HEISEN_PARSER_HPP

#include "Lex/Token.hpp"
#include "Macros.hpp"
#include "Parse/AST.hpp"

#include <memory>
#include <vector>

namespace heisen {

struct Parser {
    const std::vector<Token>& tokens;

    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    u32 curr = 0;

    bool parse();
    bool parse_function_definition();
    bool parse_statement();

    bool parse_type();
    bool parse_expression();

    Token peek();
    Token consume();
    bool match(TokenType);
};

} // namespace heisen

#endif // HEISEN_PARSER_HPP
