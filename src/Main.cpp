#include "Lex/Lexer.hpp"
#include "Macros.hpp"
#include "Support/Support.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Loc {
    public:
    int row, col;
    explicit Loc () : row (-1), col (-1) {
    }
    explicit Loc (int row, int col) : row (row), col (col) {
    }
};

struct Token {
    std::string str;
    Loc loc;

    Token (std::string str, int row, int col)
    : str (str), loc (Loc (row, col)) {
    }

    friend std::ostream& operator<< (std::ostream& out, const Token& token) {
        out << "\"" << token.str << "\"\t\t(" << token.loc.row << ", " <<  token.loc.col << ")";
        return out;
    }
};

std::size_t current = 0;

std::string tokenize_identifier (const std::string& line) {
    std::string token;
    while (std::isalnum (line[current])) {
        token.push_back (line[current]);
        current++;
    }

    return token;
}

std::vector<Token> tokenize_line (const std::string& line, int row) {
    std::vector<Token> line_tokens;
    current = 0;
    for (; current < line.size ();) {
        if (std::isblank (line[current])) {
            current++;
        } else if (std::isalpha (line[current]) or line[current] == '_') {
            int start = current;
            line_tokens.emplace_back (tokenize_identifier (line), row, start + 1);
        } else if (std::isdigit (line[current])) {
            int start = current;
            line_tokens.emplace_back (tokenize_identifier (line), row, start + 1);
        } else {
            std::string token;
            token.push_back (line[current++]);
            line_tokens.emplace_back (token, row, current);
        }
    }

    return line_tokens;
}

std::vector<Token> tokenize (std::vector<std::string> lines) {
    std::vector<Token> tokens;

    for (std::size_t i = 0; i < lines.size (); i++) {
        auto line_tokens = tokenize_line (lines[i], i + 1);

        tokens.insert (tokens.end (), line_tokens.begin (), line_tokens.end ());
    }

    return tokens;
}

int main (int argc, const char* argv[]) {
    using namespace Heisen;

    if (argc != 2) {
        emit_error ("no input file specified");
        std::cout << "usage: ./heisen [options] <source-file>" << std::endl;
        std::exit (EXIT_FAILURE);
    }

    auto source = read_input_file (argv[1]);

    for (auto& line : source)
        std::cout << line << std::endl;

    auto tokens = tokenize (source);

    for (auto& token: tokens) {
        std::cout << token << std::endl;
    }

    return 0;
}
