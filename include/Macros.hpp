#ifndef HEISEN_MACROS_HPP
#define HEISEN_MACROS_HPP

#include <iostream>
#include <string>

namespace Heisen {

#define LOG(x) std::cout << BOLD << "LOG" << RESET << "(" << __FILE__ << ")(" << __LINE__ << "):" << x << std::endl;

const std::string RESET = "\033[0m";
const std::string BOLD  = "\033[1m";

const std::string RED   = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string BLUE  = "\033[34m";
const std::string CYAN  = "\033[36m";


} // namespace Heisen

#endif // HEISEN_MACROS_HPP 
