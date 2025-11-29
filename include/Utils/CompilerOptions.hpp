#ifndef HEISEN_COMPILER_OPTIONS_HPP
#define HEISEN_COMPILER_OPTIONS_HPP

#include "Macros.hpp"

namespace Heisen {

struct CompilerOptions {
    bool dumpAST         = false;
    bool printStats      = false;
    bool dumpSymbolTable = false;
    std::string outputFile;
    std::string inputFile;

    void printHelp (const char* programName);
};

CompilerOptions parseArgs (int argc, char** argv); 

} // namespace Heisen

#endif // HEISEN_COMPILER_OPTIONS_HPP