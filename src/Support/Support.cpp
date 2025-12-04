#include "Support/Support.hpp"

#include <fstream>

namespace Heisen {

void emit_error (const std::string& msg) {
    using namespace Heisen;
    std::cerr << BOLD << RED << "error: " << RESET << BOLD << msg << RESET << std::endl;
}

std::vector<std::string> read_input_file (const std::string& filename) {
    std::ifstream input (filename);
    if (!input.is_open ()) {
        emit_error ("could not open input file");
    }

    std::vector<std::string> source;
    std::string line;
    while (std::getline (input, line)) {
        source.push_back (line);
    }

    return source;
}

} // namespace Heisen
