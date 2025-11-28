#ifndef HEISEN_MACROS_HPP
#define HEISEN_MACROS_HPP

#include <algorithm>
#include <any>
#include <bit>
#include <bitset>
#include <cassert>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <exception>
#include <expected>
#include <format>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#define LOG(x)  /* std::cout << x << std::endl */
#define LOGs(x) /* std::cout << x << ' ' */

#define DBG(...) dbg (__FILE__, __LINE__, __VA_ARGS__)
#define ERROR(...) dbg (__FILE__, __LINE__, __VA_ARGS__, 1)

namespace Heisen {

static const std::string GREEN       = "\033[32m";
static const std::string RED         = "\033[31m";
static const std::string CYAN        = "\033[36m";
static const std::string BOLD        = "\033[1m";
static const std::string RESET_COLOR = "\033[0m";


template <typename... Args>
void dbg (const char* file, int line, Args&&... args, int type = 0) {
    std::ostringstream oss;
    (oss << ... << args);

    if (type == 0) {
        std::cout << BOLD << GREEN << "DEBUG" << RESET_COLOR;
    } else if (type == 1) {
        std::cout << BOLD << RED << "ERROR" << RESET_COLOR;
    }

    std::cout << BOLD << " [" << file << ":" << line << "] " << RESET_COLOR
              << oss.str () << std::endl;
}

} // namespace Heisen

#endif // HEISEN_MACROS_HPP
