#ifndef HEISEN_LEXER_HPP
#define HEISEN_LEXER_HPP

#include "Lex/Token.hpp"
#include "Macros.hpp"

#include <vector>

namespace heisen {

struct Lexer {
    const std::string_view source;
    const u32 length;

    std::vector<std::string> tokens;
    u32 curr = 0;

    explicit Lexer(const std::string_view& source)
    : source(source), length(source.length()) {}

    explicit Lexer(const std::string& source)
    : source(source), length(source.length()) {}

    std::vector<std::string> tokenize();
    void tokenize_identifier();
    void scan_token();

    void skip_whitespace();
    void advance();

    bool is_valid() const;
    char peek() const;
};

} // namespace heisen

#endif // HEISEN_LEXER_HPP
