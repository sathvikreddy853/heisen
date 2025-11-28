#include <AST.hpp>
#include <ASTTraversal.hpp>
#include <CompilerOptions.hpp>
#include <FormattingFunctions.hpp>
#include <Macros.hpp>
#include <SemanticAnalyzer.hpp>
#ifdef ENABLE_CODEGEN
#include <QIRCodeGen.hpp>
#endif

extern int yydebug;
extern int yyparse ();
extern std::vector<ASTNode*> translationUnit;
extern FILE* yyin;

int main (int argc, char** argv) {
    Heisen::CompilerOptions opts = Heisen::parseArgs (argc, argv);

    // Open input file
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
    if (!analyzer.analyze (translationUnit)) {
        Heisen::printError ("Semantic analysis failed.", opts.enableColor);
        // (Error printing logic omitted for brevity)
        return 1;
    } else {
        Heisen::printSuccess ("Semantic Analysis Passed", opts.enableColor);
    }

    // Code Generation
#ifdef ENABLE_CODEGEN
    if (!opts.semanticOnly) {
        if (opts.verbose)
            Heisen::printHeader ("Phase 3: QIR Generation", opts.enableColor);

        // Generate QIR code
        try {
            QIRCodeGen codeGen ("heisen_module");
            // Pass the entire translation unit (statements + decls)
            codeGen.generateCode (translationUnit);

            // Verify
            std::string errorMsg;
            if (!codeGen.verify (errorMsg)) {
                Heisen::printError ("QIR Verification Failed:", opts.enableColor);
                std::cerr << errorMsg << std::endl;
                return 1;
            }

            // Write Output
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