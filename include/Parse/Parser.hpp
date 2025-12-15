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

    bool parse();
    bool parse_function_definition();
    bool parse_expression();
    bool parse_statement();
};

} // namespace heisen

#endif // HEISEN_PARSER_HPP
