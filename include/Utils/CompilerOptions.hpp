#ifndef HEISEN_COMPILER_OPTIONS_HPP
#define HEISEN_COMPILER_OPTIONS_HPP

#include <Macros.hpp>

extern int yydebug;

namespace Heisen {

struct CompilerOptions {
    bool verbose      = false;
    bool printAST     = false;
    bool printStats   = false;
    bool enableColor  = true;
    bool semanticOnly = false;
    std::string outputFile;

    void printHelp (const char* programName) {
        std::cout << "Usage: " << programName << " [options] <input-file>\n\n";
        std::cout << "Options:\n";
        std::cout << "  -v, --verbose        Enable verbose output\n";
        std::cout << "  -a, --print-ast      Print the AST after parsing\n";
        std::cout << "  -s, --stats          Print AST statistics\n";
        std::cout << "  -c, --semantic-only  Only perform semantic analysis (no codegen)\n";
        std::cout << "  -o, --output <file>  Specify output file\n";
        std::cout << "  --no-color           Disable colored output\n";
        std::cout << "  -d, --debug          Enable parser debug output\n";
        std::cout << "  -h, --help           Show this help message\n";
        std::cout << std::endl;
    }
};

CompilerOptions parseArgs (int argc, char** argv) {
    CompilerOptions opts;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-v" || arg == "--verbose") {
            opts.verbose = true;
        } else if (arg == "-a" || arg == "--print-ast") {
            opts.printAST = true;
        } else if (arg == "-s" || arg == "--stats") {
            opts.printStats = true;
        } else if (arg == "-c" || arg == "--semantic-only") {
            opts.semanticOnly = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                opts.outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o requires an argument\n";
                exit (1);
            }
        } else if (arg == "--no-color") {
            opts.enableColor = false;
        } else if (arg == "-d" || arg == "--debug") {
            yydebug = 1;
        } else if (arg == "-h" || arg == "--help") {
            opts.printHelp (argv[0]);
            exit (0);
        }
    }

    return opts;
}

} // namespace Heisen

#endif // HEISEN_COMPILER_OPTIONS_HPP