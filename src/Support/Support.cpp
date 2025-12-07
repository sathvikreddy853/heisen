#include "Support/Support.hpp"

#include <fstream>
#include <sstream>

namespace Heisen {

void emit_error (const std::string& msg) {
    using namespace Heisen;
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

} // namespace Heisen
