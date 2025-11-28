#include "AST/AST.hpp"
#include "AST/ASTTraversal.hpp"
#include "Macros.hpp"
#include "Sema/Sema.hpp"
#include "Utils/CompilerOptions.hpp"
#include "Utils/FormattingFunctions.hpp"
#ifdef ENABLE_CODEGEN
#include <QIRCodeGen.hpp>
#endif

extern int yydebug;
extern int yyparse ();
extern std::vector<ASTNode*> translationUnit;
extern FILE* yyin;

int main (int argc, char** argv) {
    Heisen::CompilerOptions opts = Heisen::parseArgs (argc, argv);

    FILE* inputFile = nullptr;
    if (!opts.inputFile.empty ()) {
        inputFile = fopen (opts.inputFile.c_str (), "r");
        if (!inputFile) {
            Heisen::printError (
            "Failed to open input file: " + opts.inputFile, opts.enableColor);
            return 1;
        }
        yyin = inputFile;
    }

    // Parsing
    if (opts.verbose)
        Heisen::printHeader ("Phase 1: Parsing", opts.enableColor);
    if (yyparse () != 0) {
        Heisen::printError ("Parsing failed", opts.enableColor);
        if (inputFile) fclose (inputFile);
        return 1;
    } else {
        Heisen::printSuccess ("Parsing Successful", opts.enableColor);
    }

    // Semantic Analysis
    if (opts.verbose)
        Heisen::printHeader ("Phase 2: Analysis", opts.enableColor);
    Heisen::SemanticAnalyzer analyzer;

    // Set source filename for better error reporting
    if (!opts.inputFile.empty ()) {
        analyzer.setSourceFilename (opts.inputFile);
    }

    if (!analyzer.analyze (translationUnit)) {
        Heisen::printError ("Semantic analysis failed.", opts.enableColor);
        analyzer.printErrors ();
        return 1;
    } else {
        Heisen::printSuccess ("Semantic Analysis Passed", opts.enableColor);
    }

    // Code Generation
#ifdef ENABLE_CODEGEN
    if (!opts.semanticOnly) {
        if (opts.verbose)
            Heisen::printHeader ("Phase 3: QIR Generation", opts.enableColor);

        try {
            QIRCodeGen codeGen ("heisen_module");
            codeGen.generateCode (translationUnit);

            std::string errorMsg;
            if (!codeGen.verify (errorMsg)) {
                Heisen::printError ("QIR Verification Failed:", opts.enableColor);
                std::cerr << errorMsg << std::endl;
                return 1;
            }

            std::string outName = opts.outputFile.empty () ? "output.ll" : opts.outputFile;
            codeGen.writeToFile (outName);

            Heisen::printSuccess ("Successfully generated QIR: " + outName, opts.enableColor);
            if (opts.verbose) {
                std::cout << "You can run this with: qir-runner -f " << outName
                          << std::endl;
            }

        } catch (const std::exception& e) {
            Heisen::printError (
            "CodeGen Error: " + std::string (e.what ()), opts.enableColor);
            return 1;
        }
    }
#endif

    // Cleanup
    for (auto* node : translationUnit) delete node;
    if (inputFile) fclose (inputFile);
    return 0;
}