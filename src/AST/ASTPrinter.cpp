#include "AST/AST.hpp"
#include "AST/ASTTraversal.hpp"

void ASTPrinter::indent () {
    if (stateStack.empty ()) return;

    for (size_t i = 0; i < stateStack.size () - 1; ++i) {
        if (stateStack[i]) {
            std::cout << "  ";
        } else {
            std::cout << "│ ";
        }
    }

    if (stateStack.back ()) {
        std::cout << "└─";
    } else {
        std::cout << "├─";
    }
}

void ASTPrinter::printNode (const std::string& nodeType, const std::string& details) {
    indent ();
    if (useColor) {
        std::cout << "\033[1;34m" << nodeType << "\033[0m";
    } else {
        std::cout << nodeType;
    }
    if (!details.empty ()) { std::cout << " " << details; }
    std::cout << std::endl;
}

void ASTPrinter::printNode (const std::string& nodeType,
const Heisen::Location& loc,
const std::string& details) {
    indent ();

    // Print node type
    if (useColor) {
        std::cout << "\033[1;34m" << nodeType << "\033[0m";
    } else {
        std::cout << nodeType;
    }

    // Print location in simplified format: <line:col>
    if (loc.line != 0) {
        std::cout << " <";
        if (!loc.filename.empty ()) { std::cout << loc.filename << ":"; }
        std::cout << loc.line << ":" << loc.column;
        std::cout << ">";
    }

    // Print details
    if (!details.empty ()) { std::cout << " " << details; }

    std::cout << std::endl;
}

void ASTPrinter::visit (IntLiteralExpr* node) {
    printNode ("IntLiteral", node->loc, std::to_string (node->getValue ()));
}

void ASTPrinter::visit (FloatLiteralExpr* node) {
    printNode ("FloatLiteral", node->loc, std::to_string (node->getValue ()));
}

void ASTPrinter::visit (BoolLiteralExpr* node) {
    printNode ("BoolLiteral", node->loc, node->getValue () ? "true" : "false");
}

void ASTPrinter::visit (StringLiteralExpr* node) {
    printNode ("StringLiteral", node->loc, "\"" + node->getValue () + "\"");
}

void ASTPrinter::visit (IdentifierExpr* node) {
    printNode ("Identifier", node->loc, "'" + node->getValue () + "'");
}

void ASTPrinter::visit (BinaryOpExpr* node) {
    printNode ("BinaryOperator", node->loc, "'" + node->getOperator () + "'");
    stateStack.push_back (false);
    visitExpr (node->getLeft ());
    stateStack.back () = true;
    visitExpr (node->getRight ());
    stateStack.pop_back ();
}

void ASTPrinter::visit (UnaryOpExpr* node) {
    printNode ("UnaryOperator", node->loc, "'" + node->getOperator () + "'");
    stateStack.push_back (true);
    visitExpr (node->getOperand ());
    stateStack.pop_back ();
}

void ASTPrinter::visit (FunctionCallExpr* node) {
    printNode ("CallExpr", node->loc);
    if (node->getArgs ().empty ()) {
        stateStack.push_back (true);
        visitExpr (node->getCallee ());
        stateStack.pop_back ();
    } else {
        stateStack.push_back (false);
        visitExpr (node->getCallee ());
        stateStack.pop_back ();

        for (size_t i = 0; i < node->getArgs ().size (); ++i) {
            stateStack.push_back (i == node->getArgs ().size () - 1);
            visitExpr (node->getArgs ()[i]);
            stateStack.pop_back ();
        }
    }
}

void ASTPrinter::visit (VariableDecl* node) {
    std::string name = node->getVariableName ()->getValue ();
    printNode ("VarDecl", node->loc, name);
    if (node->getInitializer ()) {
        stateStack.push_back (true);
        visitExpr (node->getInitializer ());
        stateStack.pop_back ();
    }
}

void ASTPrinter::visit (FunctionDecl* node) {
    std::string name = node->getFunctionName ()->getValue ();
    printNode ("FunctionDecl", node->loc, name);

    size_t paramCount = node->getParameters ().size ();
    bool hasBody      = node->getBody () != nullptr;

    for (size_t i = 0; i < paramCount; ++i) {
        stateStack.push_back (i == paramCount - 1 && !hasBody);
        RecursiveASTVisitor::visit (node->getParameters ()[i]);
        stateStack.pop_back ();
    }
    if (hasBody) {
        stateStack.push_back (true);
        visit (node->getBody ());
        stateStack.pop_back ();
    }
}

void ASTPrinter::visit (CompoundStmt* node) {
    printNode ("CompoundStmt");
    const auto& statements = node->getStatements ();
    for (size_t i = 0; i < statements.size (); ++i) {
        stateStack.push_back (i == statements.size () - 1);
        visitStmt (statements[i]);
        stateStack.pop_back ();
    }
}

void ASTPrinter::visit (IfStmt* node) {
    printNode ("IfStmt");

    // Condition
    stateStack.push_back (false);
    printNode ("Condition:");
    stateStack.push_back (true);
    visitExpr (node->getCondition ());
    stateStack.pop_back ();
    stateStack.pop_back ();

    // Then
    bool hasElse = node->getElseBlock () != nullptr;
    stateStack.push_back (!hasElse);
    printNode ("Then:");
    stateStack.push_back (true);
    visitStmt (node->getThenBlock ());
    stateStack.pop_back ();
    stateStack.pop_back ();

    // Else
    if (hasElse) {
        stateStack.push_back (true);
        printNode ("Else:");
        stateStack.push_back (true);
        visitStmt (node->getElseBlock ());
        stateStack.pop_back ();
        stateStack.pop_back ();
    }
}

void ASTPrinter::visit (WhileStmt* node) {
    printNode ("WhileStmt");

    // Condition
    stateStack.push_back (false);
    printNode ("Condition:");
    stateStack.push_back (true);
    visitExpr (node->getCondition ());
    stateStack.pop_back ();
    stateStack.pop_back ();

    // Body
    stateStack.push_back (true);
    printNode ("Body:");
    stateStack.push_back (true);
    visitStmt (node->getBody ());
    stateStack.pop_back ();
    stateStack.pop_back ();
}

void ASTPrinter::visit (ForStmt* node) {
    printNode ("ForStmt");

    // Init
    bool hasCondition = node->getCondition () != nullptr;
    bool hasUpdate    = node->getUpdate () != nullptr;
    bool hasBody      = node->getBody () != nullptr;

    if (node->getInit ()) {
        stateStack.push_back (!hasCondition && !hasUpdate && !hasBody);
        printNode ("Init:");
        stateStack.push_back (true);
        RecursiveASTVisitor::visit (node->getInit ());
        stateStack.pop_back ();
        stateStack.pop_back ();
    }

    // Condition
    if (node->getCondition ()) {
        stateStack.push_back (!hasUpdate && !hasBody);
        printNode ("Condition:");
        stateStack.push_back (true);
        visitExpr (node->getCondition ());
        stateStack.pop_back ();
        stateStack.pop_back ();
    }

    // Update
    if (node->getUpdate ()) {
        stateStack.push_back (!hasBody);
        printNode ("Update:");
        stateStack.push_back (true);
        visitStmt (node->getUpdate ());
        stateStack.pop_back ();
        stateStack.pop_back ();
    }

    // Body
    stateStack.push_back (true);
    printNode ("Body:");
    stateStack.push_back (true);
    visitStmt (node->getBody ());
    stateStack.pop_back ();
    stateStack.pop_back ();
}

void ASTPrinter::visit (ReturnStmt* node) {
    printNode ("ReturnStmt", node->loc);
    if (node->getReturnValue ()) {
        stateStack.push_back (true);
        visitExpr (node->getReturnValue ());
        stateStack.pop_back ();
    }
}

void ASTPrinter::visit (ApplyGateStmt* node) {
    printNode ("ApplyGateStmt", node->loc);
    stateStack.push_back (false);
    printNode ("Gate:");
    stateStack.push_back (true);
    visitGate (node->getGate ());
    stateStack.pop_back ();
    stateStack.pop_back ();

    stateStack.push_back (true);
    printNode ("Target:");
    stateStack.push_back (true);
    visitExpr (node->getTarget ());

    stateStack.pop_back ();
    stateStack.pop_back ();
}

void ASTPrinter::visit (MeasureStmt* node) {
    printNode ("MeasureStmt", node->loc);
    stateStack.push_back (false);
    printNode ("Source:");
    stateStack.push_back (true); // Assuming source is always the first child
    visitExpr (node->getSource ());
    stateStack.pop_back ();
    stateStack.pop_back ();

    stateStack.push_back (true);
    printNode ("Target:");
    stateStack.push_back (true); // Assuming target is always the last child
    visitExpr (node->getTarget ());
    stateStack.pop_back ();
    stateStack.pop_back ();
}

void ASTPrinter::visit (SimpleGateNode* node) {
    std::string gateName;
    switch (node->getGateKind ()) {
    case GateKind::H: gateName = "H"; break;
    case GateKind::X: gateName = "X"; break;
    case GateKind::Y: gateName = "Y"; break;
    case GateKind::Z: gateName = "Z"; break;
    case GateKind::S: gateName = "S"; break;
    case GateKind::T: gateName = "T"; break;
    case GateKind::CNOT: gateName = "CNOT"; break;
    case GateKind::CZ: gateName = "CZ"; break;
    case GateKind::SWAP: gateName = "SWAP"; break;
    case GateKind::CSWAP: gateName = "CSWAP"; break;
    case GateKind::CCNOT: gateName = "CCNOT"; break;
    default: gateName = "UNKNOWN"; break;
    }
    printNode ("SimpleGate", node->loc, gateName);
}

void ASTPrinter::visit (ParametricGateNode* node) {
    std::string gateName;
    switch (node->getGateKind ()) { // Changed from getKind() to getGateKind() to match original
    case GateKind::RX: gateName = "RX"; break;
    case GateKind::RY: gateName = "RY"; break;
    case GateKind::RZ: gateName = "RZ"; break;
    case GateKind::CRX: gateName = "CRX"; break;
    case GateKind::CRY: gateName = "CRY"; break;
    case GateKind::CRZ: gateName = "CRZ"; break;
    default: gateName = "UNKNOWN"; break;
    }
    printNode ("ParametricGate", node->loc, gateName);
    for (size_t i = 0; i < node->getParams ().size (); ++i) {
        stateStack.push_back (i == node->getParams ().size () - 1);
        visitExpr (node->getParams ()[i]);
        stateStack.pop_back ();
    }
}

void ASTPrinter::visit (TensoredGateNode* node) {
    printNode ("TensoredGate", node->loc);
    const auto& gates = node->getGates ();
    for (size_t i = 0; i < gates.size (); ++i) {
        stateStack.push_back (i == gates.size () - 1);
        visitGate (gates[i]);
        stateStack.pop_back ();
    }
}

void ASTPrinter::visit (GateCompositionNode* node) {
    printNode ("GateComposition", node->loc, "'@'");
    stateStack.push_back (false);
    visitGate (node->getLeft ());
    stateStack.back () = true;
    visitGate (node->getRight ());
    stateStack.pop_back ();
}

void ASTPrinter::printTranslationUnit (const std::vector<ASTNode*>& nodes) {
    printNode ("TranslationUnitDecl");

    for (size_t i = 0; i < nodes.size (); ++i) {
        stateStack.push_back (i == nodes.size () - 1);
        if (auto* expr = dynamic_cast<Expr*> (nodes[i])) {
            visitExpr (expr);
        } else if (auto* stmt = dynamic_cast<Stmt*> (nodes[i])) {
            visitStmt (stmt);
        } else if (auto* decl = dynamic_cast<Decl*> (nodes[i])) {
            visitDecl (decl);
        }
        stateStack.pop_back ();
    }
}

void ASTPrinter::print (ASTNode* root) {
    stateStack.clear ();

    if (auto* expr = dynamic_cast<Expr*> (root)) {
        visitExpr (expr);
    } else if (auto* stmt = dynamic_cast<Stmt*> (root)) {
        visitStmt (stmt);
    } else if (auto* decl = dynamic_cast<Decl*> (root)) {
        visitDecl (decl);
    }
}