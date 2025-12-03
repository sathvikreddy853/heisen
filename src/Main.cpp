#include "Macros.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void emitError (const std::string& msg) {
    using namespace Heisen;
    std::cerr << BOLD << RED << "error:" << RESET << BOLD << msg << std::endl;
}

std::vector<std::string> readInputFile (const std::string& filename) {
    std::ifstream input (filename);
    if (!input.is_open ()) {
        emitError ("could not open input file");
    }

    std::vector<std::string> source;
    std::string line;
    while (std::getline (input, line)) {
        source.push_back (line);
    }

    return source;
}

int main (int argc, const char* argv[]) {
    using namespace Heisen;

    if (argc != 2) {
        emitError ("no input file specified");
        std::cout << "usage: ./heisen <source-file>" << std::endl;
    }

    auto source = readInputFile (argv[1]);
    
    for (auto& line : source)
        std::cout << line << std::endl;

    return 0;
}