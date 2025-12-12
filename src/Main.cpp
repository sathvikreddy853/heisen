#include "Lex/Lexer.hpp"
#include "Support/Support.hpp"

using namespace heisen;

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        emit_error("no input file specified");
        std::cout << "usage: ./heisen [options] <source-file>" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto source = read_input_file(argv[1]);
    std::cout << source << std::endl;

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    for (auto& token : tokens) {
        LOG(token);
        std::cout << token << std::endl;
    }

    return 0;
}
