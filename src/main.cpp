#include <macros.hpp>
#include <ast.hpp>
#include <ast_traversal.hpp>
#include <semantic_analyzer.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>

extern int yydebug;
extern int yyparse();
extern std::vector<ASTNode*> translationUnit;

// Command-line options
struct CompilerOptions {
    bool verbose = false;
    bool printAST = false;
    bool printStats = false;
    bool enableColor = true;
    bool semanticOnly = false;
    std::string outputFile;
    
    void printHelp(const char* programName) {
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

CompilerOptions parseArgs(int argc, char** argv) {
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
                exit(1);
            }
        } else if (arg == "--no-color") {
            opts.enableColor = false;
        } else if (arg == "-d" || arg == "--debug") {
            yydebug = 1;
        } else if (arg == "-h" || arg == "--help") {
            opts.printHelp(argv[0]);
            exit(0);
        }
    }
    
    return opts;
}

void printHeader(const std::string& title, bool useColor = true) {
    std::string separator(60, '=');
    
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

void printSuccess(const std::string& message, bool useColor = true) {
    if (useColor) {
        std::cout << "\033[1;32m✓ " << message << "\033[0m" << std::endl;
    } else {
        std::cout << "✓ " << message << std::endl;
    }
}

void printError(const std::string& message, bool useColor = true) {
    if (useColor) {
        std::cerr << "\033[1;31m✗ " << message << "\033[0m" << std::endl;
    } else {
        std::cerr << "✗ " << message << std::endl;
    }
}

int main(int argc, char** argv) {
    CompilerOptions opts = parseArgs(argc, argv);
    
    // Banner
    if (opts.verbose) {
        printHeader("Heisen Quantum Programming Language Compiler", opts.enableColor);
        std::cout << std::endl;
    }
    
    // ===================================================================
    // Phase 1: Parsing
    // ===================================================================
    if (opts.verbose) {
        printHeader("Phase 1: Parsing", opts.enableColor);
    }
    
    yydebug = 0;
    int parseResult = yyparse();
    
    if (parseResult != 0) {
        printError("Parsing failed", opts.enableColor);
        return 1;
    }
    
    if (translationUnit.empty()) {
        printError("No translation unit generated", opts.enableColor);
        return 1;
    }
    
    printSuccess("Parsing completed successfully", opts.enableColor);
    if (opts.verbose) {
        std::cout << "  Generated " << translationUnit.size() 
                  << " top-level declarations" << std::endl;
    }
    std::cout << std::endl;
    
    // ===================================================================
    // Optional: Print AST
    // ===================================================================
    if (opts.printAST) {
        printHeader("Abstract Syntax Tree", opts.enableColor);
        ASTPrinter printer(opts.enableColor);
        
        for (auto* node : translationUnit) {
            printer.print(node);
        }
        std::cout << std::endl;
    }
    
    // ===================================================================
    // Optional: Print Statistics
    // ===================================================================
    if (opts.printStats) {
        printHeader("AST Statistics", opts.enableColor);
        ASTStatistics stats;
        
        for (auto* node : translationUnit) {
            stats.analyzeNode(node);
        }
        
        stats.printStats();
        std::cout << std::endl;
    }
    
    // ===================================================================
    // Phase 2: Semantic Analysis
    // ===================================================================
    if (opts.verbose) {
        printHeader("Phase 2: Semantic Analysis", opts.enableColor);
    }
    
    Heisen::SemanticAnalyzer analyzer;
    bool semanticSuccess = analyzer.analyze(translationUnit);
    
    if (!semanticSuccess) {
        printError("Semantic analysis failed with errors:", opts.enableColor);
        std::cout << std::endl;
        
        // Print all errors
        const auto& errors = analyzer.getErrors();
        for (size_t i = 0; i < errors.size(); ++i) {
            const auto& error = errors[i];
            
            if (opts.enableColor) {
                std::cerr << "\033[1;31m[Error " << (i + 1) << "]\033[0m ";
                std::cerr << "\033[1mLine " << error.loc.line 
                         << ", Column " << error.loc.column << ":\033[0m" << std::endl;
                std::cerr << "  " << error.message << std::endl;
            } else {
                std::cerr << "[Error " << (i + 1) << "] ";
                std::cerr << "Line " << error.loc.line 
                         << ", Column " << error.loc.column << ":" << std::endl;
                std::cerr << "  " << error.message << std::endl;
            }
            std::cerr << std::endl;
        }
        
        // Summary
        if (opts.enableColor) {
            std::cerr << "\033[1;31m" << errors.size() 
                     << " error(s) found\033[0m" << std::endl;
        } else {
            std::cerr << errors.size() << " error(s) found" << std::endl;
        }
        
        // Clean up
        for (auto* node : translationUnit) {
            delete node;
        }
        
        return 1;
    }
    
    printSuccess("Semantic analysis completed successfully", opts.enableColor);
    if (opts.verbose) {
        std::cout << "  All type checks passed" << std::endl;
        std::cout << "  Quantum semantics validated" << std::endl;
        std::cout << "  Symbol table constructed" << std::endl;
    }
    std::cout << std::endl;
    
    // ===================================================================
    // Phase 3: Code Generation (TODO)
    // ===================================================================
    if (!opts.semanticOnly) {
        if (opts.verbose) {
            printHeader("Phase 3: Code Generation", opts.enableColor);
        }
        
        // TODO: Implement code generation
        std::cout << "Code generation not yet implemented" << std::endl;
        std::cout << "Use --semantic-only flag to stop after semantic analysis" << std::endl;
        std::cout << std::endl;
        
        /*
        // Future implementation:
        
        QASMGenerator generator;
        std::string qasmCode = generator.generate(translationUnit);
        
        if (!opts.outputFile.empty()) {
            std::ofstream outFile(opts.outputFile);
            if (!outFile) {
                printError("Failed to open output file: " + opts.outputFile, opts.enableColor);
                return 1;
            }
            outFile << qasmCode;
            outFile.close();
            printSuccess("Generated QASM code written to: " + opts.outputFile, opts.enableColor);
        } else {
            std::cout << qasmCode << std::endl;
        }
        */
    }
    
    // ===================================================================
    // Success Summary
    // ===================================================================
    if (opts.verbose) {
        printHeader("Compilation Summary", opts.enableColor);
        printSuccess("Compilation successful!", opts.enableColor);
        std::cout << std::endl;
    }
    
    // Clean up
    for (auto* node : translationUnit) {
        delete node;
    }
    
    return 0;
}