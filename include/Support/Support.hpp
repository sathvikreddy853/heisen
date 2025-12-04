#include "Macros.hpp"

#include <string>
#include <vector>

namespace Heisen {

void emit_error (const std::string& msg);
std::vector<std::string> read_input_file (const std::string& filename);

} // namespace Heisen