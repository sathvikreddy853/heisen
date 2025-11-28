#ifndef HEISEN_FORMATTING_FUNCTIONS_HPP
#define HEISEN_FORMATTING_FUNCTIONS_HPP

#include "Macros.hpp"

namespace Heisen {

void printHeader (const std::string& title) {
    std::string separator (60, '=');
    std::cout << BOLD << CYAN << separator << RESET_COLOR << std::endl;
    std::cout << BOLD << CYAN << "  " << title << RESET_COLOR << std::endl;
    std::cout << BOLD << CYAN << separator << RESET_COLOR << std::endl;
}

void printSuccess (const std::string& message) {
    std::cout << BOLD << GREEN << message << RESET_COLOR << std::endl;
}

void printError (const std::string& message) {
    std::cerr << BOLD << RED << message << RESET_COLOR << std::endl;
}

} // namespace Heisen

#endif // HEISEN_FORMATTING_FUNCTIONS_HPP
