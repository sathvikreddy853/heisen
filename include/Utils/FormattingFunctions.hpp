#include <Macros.hpp>

void printHeader (const std::string& title, bool useColor = true) {
    std::string separator (60, '=');

    if (useColor) {
        std::cout << "\033[1;36m" << separator << "\033[0m" << std::endl;
        std::cout << "\033[1;36m  " << title << "\033[0m" << std::endl;
        std::cout << "\033[1;36m" << separator << "\033[0m" << std::endl;
    } else {
        std::cout << separator << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << separator << std::endl;
    }
}

void printSuccess (const std::string& message, bool useColor = true) {
    if (useColor) {
        std::cout << "\033[1;32m✓ " << message << "\033[0m" << std::endl;
    } else {
        std::cout << "✓ " << message << std::endl;
    }
}

void printError (const std::string& message, bool useColor = true) {
    if (useColor) {
        std::cerr << "\033[1;31m✗ " << message << "\033[0m" << std::endl;
    } else {
        std::cerr << "✗ " << message << std::endl;
    }
}
