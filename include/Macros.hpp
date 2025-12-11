#ifndef HEISEN_MACROS_HPP
#define HEISEN_MACROS_HPP

#include <filesystem>
#include <print>
#include <source_location>

namespace heisen {

void LOG (auto msg, std::source_location loc = std::source_location::current ()) {
    std::println ("[{}:{}] - {}",
    std::filesystem::path (loc.file_name ()).filename ().string (),
    loc.line (), msg);
}

const std::string RESET = "\033[0m";
const std::string BOLD  = "\033[1m";

const std::string RED   = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string BLUE  = "\033[34m";
const std::string CYAN  = "\033[36m";

} // namespace heisen

#endif // HEISEN_MACROS_HPP
