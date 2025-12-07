#include "Lex/Lexer.hpp"
#include "Support/Support.hpp"

using namespace Heisen;

int main (int argc, const char* argv[]) {
    if (argc != 2) {
        emit_error ("no input file specified");
        std::cout << "usage: ./heisen [options] <source-file>" << std::endl;
        std::exit (EXIT_FAILURE);
    }

    auto source = read_input_file (argv[1]);
    std::cout << source << std::endl;

    vars.source = source;
    tokenize ();
    std::cout << tokens.size() << std::endl;

    for (auto& token : tokens) {
        std::cout << token << std::endl;
    }

    return 0;
}
