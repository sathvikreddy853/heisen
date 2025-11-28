#include "AST/AST.hpp"
#include "AST/ASTTraversal.hpp"
#include "Macros.hpp"
#include "Sema/Sema.hpp"
#include "Utils/CompilerOptions.hpp"
#include "Utils/FormattingFunctions.hpp"
#ifdef ENABLE_CODEGEN
#include <QIRCodeGen.hpp>
#endif

extern int yyparse ();
extern std::vector<ASTNode*> translationUnit;
extern FILE* yyin;

int main (int argc, char** argv) {
    Heisen::CompilerOptions opts = Heisen::parseArgs (argc, argv);

    if (opts.inputFile.empty ()) {
        Heisen::printError ("No input file specified");
        std::cerr << "Usage: " << argv[0] << " [options] <input-file>" << std::endl;
        std::cerr << "Use -h or --help for more information" << std::endl;
        return 1;
    }

    FILE* inputFile = fopen (opts.inputFile.c_str (), "r");
    if (!inputFile) {
        Heisen::printError ("Failed to open input file: " + opts.inputFile);
        return 1;
    }
    yyin = inputFile;

    if (yyparse () != 0) {
        Heisen::printError ("Parsing failed");
        fclose (inputFile);
        return 1;
    } else {
        Heisen::printSuccess ("Parsing Successful");
    }

    if (opts.dumpAST) {
        ASTPrinter printer (true);
        for (auto* node : translationUnit) { printer.print (node); }
    }

    Heisen::SemanticAnalyzer analyzer;
    analyzer.setSourceFilename (opts.inputFile);

    if (!analyzer.analyze (translationUnit)) {
        Heisen::printError ("Semantic analysis Failed");
        analyzer.printErrors ();
        return 1;
    } else {
        Heisen::printSuccess ("Semantic Analysis Passed");
    }

    if (opts.dumpSymbolTable) {
        // TODO: Implement symbol table dumping
        std::cout << "Symbol table dumping not yet implemented" << std::endl;
    }

#ifdef ENABLE_CODEGEN
    try {
        QIRCodeGen codeGen ("heisen_module");
        codeGen.generateCode (translationUnit);

        std::string errorMsg;
        if (!codeGen.verify (errorMsg)) {
            Heisen::printError ("QIR Verification Failed:");
            std::cerr << errorMsg << std::endl;
            return 1;
        }

        std::string outName = opts.outputFile.empty () ? "output.ll" : opts.outputFile;
        codeGen.writeToFile (outName);

        Heisen::printSuccess ("Successfully generated QIR: " + outName);

    } catch (const std::exception& e) {
        Heisen::printError ("CodeGen Error: " + std::string (e.what ()));
        return 1;
    }
#endif

    for (auto* node : translationUnit) delete node;
    fclose (inputFile);
    return 0;
}