#include "ASTTraversal.hpp"
#include "AST.hpp"

void RecursiveASTVisitor::visitExpr (Expr* expr) {
    if (!expr) return;

    if (auto* binOp = dynamic_cast<BinaryOpExpr*> (expr)) {
        visit (binOp);
    } else if (auto* unOp = dynamic_cast<UnaryOpExpr*> (expr)) {
        visit (unOp);
    } else if (auto* call = dynamic_cast<FunctionCallExpr*> (expr)) {
        visit (call);
    } else if (auto* idx = dynamic_cast<IndexAccessExpr*> (expr)) {
        visit (idx);
    } else if (auto* slice = dynamic_cast<SliceExpr*> (expr)) {
        visit (slice);
    } else if (auto* member = dynamic_cast<MemberAccessExpr*> (expr)) {
        visit (member);
    } else if (auto* cast = dynamic_cast<CastExpr*> (expr)) {
        visit (cast);
    } else if (auto* lambda = dynamic_cast<LambdaExpr*> (expr)) {
        visit (lambda);
    } else if (auto* braced = dynamic_cast<BracedInitList*> (expr)) {
        visit (braced);
    } else if (auto* qstate = dynamic_cast<QuantumStateList*> (expr)) {
        visit (qstate);
    }
    // Leaf nodes (literals, identifiers) don't need visiting
    // as they have no children
}

void RecursiveASTVisitor::visitStmt (Stmt* stmt) {
    if (!stmt) return;

    if (auto* compound = dynamic_cast<CompoundStmt*> (stmt)) {
        visit (compound);
    } else if (auto* declStmt = dynamic_cast<DeclarationStmt*> (stmt)) {
        visit (declStmt);
    } else if (auto* assignStmt = dynamic_cast<AssignmentStmt*> (stmt)) {
        visit (assignStmt);
    } else if (auto* exprStmt = dynamic_cast<ExpressionStmt*> (stmt)) {
        visit (exprStmt);
    } else if (auto* ifStmt = dynamic_cast<IfStmt*> (stmt)) {
        visit (ifStmt);
    } else if (auto* matchStmt = dynamic_cast<MatchStmt*> (stmt)) {
        visit (matchStmt);
    } else if (auto* whileStmt = dynamic_cast<WhileStmt*> (stmt)) {
        visit (whileStmt);
    } else if (auto* doWhileStmt = dynamic_cast<DoWhileStmt*> (stmt)) {
        visit (doWhileStmt);
    } else if (auto* forStmt = dynamic_cast<ForStmt*> (stmt)) {
        visit (forStmt);
    } else if (auto* retStmt = dynamic_cast<ReturnStmt*> (stmt)) {
        visit (retStmt);
    } else if (auto* breakStmt = dynamic_cast<BreakStmt*> (stmt)) {
        // BreakStmt is a leaf node, no children to visit
    } else if (auto* contStmt = dynamic_cast<ContinueStmt*> (stmt)) {
        // ContinueStmt is a leaf node, no children to visit
    } else if (auto* applyStmt = dynamic_cast<ApplyGateStmt*> (stmt)) {
        visit (applyStmt);
    } else if (auto* measureStmt = dynamic_cast<MeasureStmt*> (stmt)) {
        visit (measureStmt);
    } else if (auto* resetStmt = dynamic_cast<ResetStmt*> (stmt)) {
        visit (resetStmt);
    } else if (auto* printStmt = dynamic_cast<PrintStmt*> (stmt)) {
        // PrintStmt has a child expression, visit it
        if (printStmt->getArgument ()) {
            visitExpr (printStmt->getArgument ());
        }
    }
}

void RecursiveASTVisitor::visitDecl (Decl* decl) {
    if (!decl) return;

    if (auto* varDecl = dynamic_cast<VariableDecl*> (decl)) {
        visit (varDecl);
    } else if (auto* paramDecl = dynamic_cast<ParameterDecl*> (decl)) {
        visit (paramDecl);
    } else if (auto* funcDecl = dynamic_cast<FunctionDecl*> (decl)) {
        visit (funcDecl);
    } else if (auto* gateDecl = dynamic_cast<GateDecl*> (decl)) {
        visit (gateDecl);
    }
}

void RecursiveASTVisitor::visitType (Type* type) {
    if (!type) return;

    if (auto* baseType = dynamic_cast<BaseTypeNode*> (type)) {
        // BaseTypeNode is a leaf node, no children to visit
        return;
    } else if (auto* arrayType = dynamic_cast<ArrayTypeNode*> (type)) {
        visit (arrayType);
    } else if (auto* funcType = dynamic_cast<FunctionTypeNode*> (type)) {
        visit (funcType);
    }
}

void RecursiveASTVisitor::visitGate (GateNode* gate) {
    if (!gate) return;

    if (auto* simpleGate = dynamic_cast<SimpleGateNode*> (gate)) {
        visit (simpleGate);
    } else if (auto* paramGate = dynamic_cast<ParametricGateNode*> (gate)) {
        visit (paramGate);
    } else if (auto* compGate = dynamic_cast<TensoredGateNode*> (gate)) {
        visit (compGate);
    } else if (auto* gateComp = dynamic_cast<GateCompositionNode*> (gate)) {
        visit (gateComp);
    }
}

// Expression visitors
void RecursiveASTVisitor::visit (BinaryOpExpr* node) {
    visitExpr (node->getLeft ());
    visitExpr (node->getRight ());
}

void RecursiveASTVisitor::visit (UnaryOpExpr* node) {
    visitExpr (node->getOperand ());
}

void RecursiveASTVisitor::visit (FunctionCallExpr* node) {
    visitExpr (node->getCallee ());
    for (auto* arg : node->getArgs ()) { visitExpr (arg); }
}

void RecursiveASTVisitor::visit (IndexAccessExpr* node) {
    visitExpr (node->getTarget ());
    visitExpr (node->getIndex ());
}

void RecursiveASTVisitor::visit (SliceExpr* node) {
    visitExpr (node->getStart ());
    visitExpr (node->getStop ());
    visitExpr (node->getStep ());
}

void RecursiveASTVisitor::visit (MemberAccessExpr* node) {
    visitExpr (node->getTarget ());
    visitExpr (node->getMemberName ());
}

void RecursiveASTVisitor::visit (CastExpr* node) {
    visitType (node->getTargetType ());
    visitExpr (node->getExpression ());
}

void RecursiveASTVisitor::visit (LambdaExpr* node) {
    for (auto* param : node->getParams ()) { visit (param); }
    visitType (node->getReturnType ());
    visit (node->getBody ());
}

void RecursiveASTVisitor::visit (BracedInitList* node) {
    for (auto* elem : node->getElements ()) { visitExpr (elem); }
}

void RecursiveASTVisitor::visit (QuantumStateList* node) {
    for (auto* state : node->getStates ()) { visitExpr (state); }
}

// Declaration visitors
void RecursiveASTVisitor::visit (VariableDecl* node) {
    visitExpr (node->getVariableName ());
    visitType (node->getVariableType ());
    visitExpr (node->getInitializer ());
}

void RecursiveASTVisitor::visit (ParameterDecl* node) {
    visitExpr (node->getName ());
    visitType (node->getType ());
}

void RecursiveASTVisitor::visit (FunctionDecl* node) {
    visitExpr (node->getFunctionName ());
    for (auto* param : node->getParameters ()) { visit (param); }
    visitType (node->getReturnType ());
    visit (node->getBody ());
}

void RecursiveASTVisitor::visit (GateDecl* node) {
    visitGate (node->getGate ());
}

// Statement visitors
void RecursiveASTVisitor::visit (CompoundStmt* node) {
    for (auto* stmt : node->getStatements ()) { visitStmt (stmt); }
}

void RecursiveASTVisitor::visit (DeclarationStmt* node) {
    for (auto* decl : node->getDeclarations ()) { visit (decl); }
}

void RecursiveASTVisitor::visit (AssignmentStmt* node) {
    visitExpr (node->getLeft ());
    visitExpr (node->getRight ());
}

void RecursiveASTVisitor::visit (ExpressionStmt* node) {
    visitExpr (node->getExpression ());
}

void RecursiveASTVisitor::visit (IfStmt* node) {
    visitExpr (node->getCondition ());
    visitStmt (node->getThenBlock ());
    visitStmt (node->getElseBlock ());
}

void RecursiveASTVisitor::visit (MatchStmt* node) {
    visitExpr (node->getMatchExpr ());
    for (auto* matchCase : node->getCases ()) { visit (matchCase); }
}

void RecursiveASTVisitor::visit (MatchCase* node) {
    visitExpr (node->getMatchExpr ());
    visitStmt (node->getBody ());
}

void RecursiveASTVisitor::visit (WhileStmt* node) {
    visitExpr (node->getCondition ());
    visitStmt (node->getBody ());
}

void RecursiveASTVisitor::visit (DoWhileStmt* node) {
    visitStmt (node->getBody ());
    visitExpr (node->getCondition ());
}

void RecursiveASTVisitor::visit (ForStmt* node) {
    visit (node->getInit ());
    visitExpr (node->getCondition ());
    visitStmt (node->getUpdate ());
    visitStmt (node->getBody ());
}

void RecursiveASTVisitor::visit (ReturnStmt* node) {
    visitExpr (node->getReturnValue ());
}

// Quantum statement visitors
void RecursiveASTVisitor::visit (ApplyGateStmt* node) {
    visitGate (node->getGate ());
    visitExpr (node->getTarget ());
}

void RecursiveASTVisitor::visit (MeasureStmt* node) {
    visitExpr (node->getSource ());
    visitExpr (node->getTarget ());
}

void RecursiveASTVisitor::visit (ResetStmt* node) {
    visitExpr (node->getTarget ());
}

// Gate visitors
void RecursiveASTVisitor::visit (SimpleGateNode* node) {
    // Simple gates have no children to visit
}

void RecursiveASTVisitor::visit (ParametricGateNode* node) {
    for (auto* param : node->getParams ()) { visitExpr (param); }
}

void RecursiveASTVisitor::visit (TensoredGateNode* node) {
    for (auto* gate : node->getGates ()) { visitGate (gate); }
}

void RecursiveASTVisitor::visit (GateCompositionNode* node) {
    visitGate (node->getLeft ());
    visitGate (node->getRight ());
}

// Type visitors
void RecursiveASTVisitor::visit (ArrayTypeNode* node) {
    visitType (node->getElementType ());
    visitExpr (node->getDimension ());
}

void RecursiveASTVisitor::visit (FunctionTypeNode* node) {
    for (auto* paramType : node->getParamTypes ()) { visitType (paramType); }
    visitType (node->getReturnType ());
}

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

void ASTStatistics::analyzeNode (ASTNode* node) {
    if (!node) return;

    node->visit (this);
}

void ASTStatistics::printStats () const {
    std::cout << std::setw (25) << std::left
              << "Total Nodes:" << stats.totalNodes << std::endl;
    std::cout << std::setw (25) << std::left
              << "Expressions:" << stats.expressions << std::endl;
    std::cout << std::setw (25) << std::left
              << "Statements:" << stats.statements << std::endl;
    std::cout << std::setw (25) << std::left
              << "Declarations:" << stats.declarations << std::endl;
    std::cout << std::setw (25) << std::left << "Functions:" << stats.functions
              << std::endl;
    std::cout << std::setw (25) << std::left << "Loops:" << stats.loops << std::endl;
    std::cout << std::setw (25) << std::left
              << "Quantum Operations:" << stats.quantumOps << std::endl;
    std::cout << std::setw (25) << std::left
              << "  Gate Applications:" << stats.gateApplications << std::endl;
    std::cout << std::setw (25) << std::left
              << "  Measurements:" << stats.measurements << std::endl;
}