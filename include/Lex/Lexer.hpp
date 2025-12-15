#ifndef HEISEN_LEXER_HPP
#define HEISEN_LEXER_HPP

#include "Lex/Token.hpp"
#include "Macros.hpp"

#include <functional>
#include <vector>

namespace heisen {

struct Lexer {
    const std::string_view source;
    const u32 length;

    std::vector<Token> tokens;
    u32 curr  = 0;
    u32 col   = 1;
    u32 row   = 1;
    u32 start = 0;

    explicit Lexer(const std::string_view& source)
    : source(source), length(source.length()) {}

    explicit Lexer(const std::string& source) : source(source), length(source.length()) {}

    std::vector<Token> tokenize();

    void tokenize_identifier();
    void tokenize_numeric(bool is_float = false);
    void tokenize_str_literal();

    void scan_token();

    void skip_whitespace();

    char advance();
    bool match(char);
    bool match(std::function<bool(char)>);
    bool is_valid() const;
    char peek() const;
};

} // namespace heisen

#endif // HEISEN_LEXER_HPP
