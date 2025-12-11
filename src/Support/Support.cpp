#include "Support/Support.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace heisen {

void emit_error (const std::string& msg) {
    using namespace heisen;
    std::cerr << BOLD << RED << "error: " << RESET << BOLD << msg << RESET
              << std::endl;
}

std::string read_input_file (const std::string& filename) {
    std::ifstream input (filename);
    if (!input.is_open ()) {
        emit_error ("could not open input file");
    }

    std::stringstream buffer;
    buffer << input.rdbuf ();
    std::string source = buffer.str ();

    return source;
}

} // namespace heisen
