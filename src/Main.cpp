#include "Macros.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void report_error (const std::string& msg) {
    using namespace Heisen;
    std::cerr << BOLD << RED << "error:" << RESET << BOLD << msg << std::endl;
}

std::vector<std::string> read_input_file (const std::string& filename) {
    std::ifstream input (filename);
    if (!input.is_open ()) {
        report_error ("could not open input file");
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
        report_error ("no input file specified");
        std::cout << "usage: ./heisen <source-file>" << std::endl;
    }

    auto source = read_input_file (argv[1]);

    for (auto& line : source)
        std::cout << line << std::endl;

    return 0;
}