#ifndef HEISEN_MACROS_HPP
#define HEISEN_MACROS_HPP

#include <filesystem>
#include <print>
#include <source_location>

namespace heisen {

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

using byte  = std::uint8_t;
using usize = std::size_t;
using isize = std::ptrdiff_t;

const std::string RESET  = "\033[0m";
const std::string BOLD   = "\033[1m";
const std::string DIM    = "\033[2m";
const std::string ITALIC = "\033[3m";

const std::string BLACK       = "\033[30m";
const std::string RED         = "\033[31m";
const std::string GREEN       = "\033[32m";
const std::string YELLOW      = "\033[33m";
const std::string BLUE        = "\033[34m";
const std::string PURPLE      = "\033[35m";
const std::string CYAN        = "\033[36m";
const std::string LIGHT_GRAY  = "\033[38;2;212;212;212m";
const std::string DIM_GRAY    = "\033[38;2;128;128;128m";
const std::string DARK_GRAY   = "\033[38;2;90;90;90m";
const std::string LIGHT_BLUE  = "\033[38;2;156;220;254m";
const std::string TEAL        = "\033[38;2;78;201;176m";
const std::string DIM_PURPLE  = "\033[38;2;197;134;192m";
const std::string LIGHT_GREEN = "\033[38;2;181;206;168m";
const std::string ORANGE      = "\033[38;2;206;145;120m";
const std::string DIM_BLUE    = "\033[38;2;86;156;214m";
const std::string DIM_YELLOW  = "\033[38;2;255;202;40m";

// Diagnostics

inline void LOG(auto msg, std::source_location loc = std::source_location::current()) {
    std::println("[{}:{}] {}", std::filesystem::path(loc.file_name()).filename().string(),
    loc.line(), msg);
}

inline void panic(std::string_view msg, std::source_location loc = std::source_location::current()) {
    std::println("{}Panic at {}:{} → {}{}", RED, loc.file_name(), loc.line(), msg, RESET);
    std::abort();
}

} // namespace heisen

#endif // HEISEN_MACROS_HPP
