#include <CompilerOptions.hpp>

namespace Heisen {

void CompilerOptions::printHelp (const char* programName) {
    std::cout << "Usage: " << programName << " [options] <input-file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --dump-ast           Dump the AST after parsing\n";
    std::cout << "  --dump-symtab        Dump the symbol table after "
                    "semantic analysis\n";
    std::cout << "  -s, --stats          Print AST statistics\n";
    std::cout << "  -o, --output <file>  Specify output file\n";
    std::cout << "  -h, --help           Show this help message\n";
    std::cout << std::endl;
}

CompilerOptions parseArgs (int argc, char** argv) {
    CompilerOptions opts;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--dump-ast") {
            opts.dumpAST = true;
        } else if (arg == "--dump-symtab") {
            opts.dumpSymbolTable = true;
        } else if (arg == "-s" || arg == "--stats") {
            opts.printStats = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                opts.outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o requires an argument\n";
                exit (1);
            }
        } else if (arg == "-h" || arg == "--help") {
            opts.printHelp (argv[0]);
            exit (0);
        } else if (arg[0] != '-') {
            opts.inputFile = arg;
        } else {
            std::cerr << "Error: Unknown option '" << arg << "'\n";
            std::cerr << "Use -h or --help for usage information\n";
            exit (1);
        }
    }

    return opts;
}

} // namespace Heisen