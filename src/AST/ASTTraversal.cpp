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
