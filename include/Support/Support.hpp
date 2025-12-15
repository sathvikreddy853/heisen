#include "Macros.hpp"

namespace heisen {

void emit_error(const std::string& msg);
std::string read_input_file(const std::string& filename);

inline bool isprint(const std::string& str) {
    if (str.empty()) return false;

    for (auto c : str) {
        if (not std::isprint(c)) {
            return false;
        }
    }

    return true;
}

} // namespace heisen