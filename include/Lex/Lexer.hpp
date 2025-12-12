#ifndef HEISEN_LEXER_HPP
#define HEISEN_LEXER_HPP

#include "Lex/Token.hpp"
#include "Macros.hpp"

#include <vector>

namespace heisen {

struct Lexer {
    std::string source;
    std::vector<std::string> tokens;

    u32 length;
    u32 curr = 0;

    explicit Lexer(const std::string& source)
    : source(source), length(source.length()) {}

    std::vector<std::string> tokenize();
    void tokenize_impl();
    void tokenize_identifier();
    void advance();
    bool is_valid();
    char peek();
    void skip_whitespace();
};

} // namespace heisen

#endif // HEISEN_LEXER_HPP
