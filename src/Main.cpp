#include "Lex/Lexer.hpp"
#include "Support/Support.hpp"

using namespace heisen;

bool isprint (const std::string &str) {
    if (str.empty())
        return false;
    
    for (auto c: str) {
        if (not std::isprint(c)) {
            return false;
        }
    }

    return true;
}

int main (int argc, const char* argv[]) {
    if (argc != 2) {
        emit_error ("no input file specified");
        std::cout << "usage: ./heisen [options] <source-file>" << std::endl;
        std::exit (EXIT_FAILURE);
    }

    auto source = read_input_file (argv[1]);
    std::cout << source << std::endl;

    vars.source = source;
    vars.length = source.length();
    tokenize ();
    std::cout << tokens.size () << std::endl;

    for (auto& token : tokens) {
        std::cout << token << std::endl;
    }

    return 0;
}
