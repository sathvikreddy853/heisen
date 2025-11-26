// heisen_ast_printer.hpp
#ifndef HEISEN_AST_PRINTER_HPP
#define HEISEN_AST_PRINTER_HPP

#include <AST.hpp>
#include <Macros.hpp>

namespace Heisen {

class ASTPrinter {
    std::ostream& out;
    int indentSize;

    void indent(int level) {
        for (int i = 0; i < level * indentSize; ++i) out.put(' ');
    }
    
public:
    ASTPrinter(std::ostream& o = std::cout, int indent = 2) : out(o), indentSize(indent) {}

    void print(ASTNode* node, int level = 0) {
        if (!node) {
            indent(level); out << "<null ASTNode>\n"; return;
        }

        // Try to dispatch by safe dynamic_cast down major hierarchies
        if (auto e = dynamic_cast<Expr*>(node)) { printExpr(e, level); return; }
        if (auto s = dynamic_cast<Stmt*>(node)) { printStmt(s, level); return; }
        if (auto d = dynamic_cast<Decl*>(node)) { printDecl(d, level); return; }
        if (auto t = dynamic_cast<Type*>(node)) { printType(t, level); return; }
        if (auto q = dynamic_cast<Quantum*>(node)) { printQuantum(q, level); return; }

        // fallback generic
        indent(level);
        out << "ASTNode [" << node->loc << "]\n";
    }

private:
    // ============================
    // Expressions
    // ============================
    void printExpr(Expr* e, int level) {
        if (!e) { indent(level); out << "<null Expr>\n"; return; }

        if (auto lit = dynamic_cast<LiteralExpr*>(e)) {
            printLiteral(lit, level);
            return;
        }
        if (auto id = dynamic_cast<IdentifierExpr*>(e)) {
            indent(level); out << "IdentifierExpr: \"" << id->getValue() << "\" " << id->loc << "\n";
            return;
        }
        if (auto b = dynamic_cast<BinaryOpExpr*>(e)) {
            indent(level); out << "BinaryOpExpr: op='" << b->getOperator() << "' " << b->loc << "\n";
            printExpr(b->getLeft(), level+1);
            printExpr(b->getRight(), level+1);
            return;
        }
        if (auto u = dynamic_cast<UnaryOpExpr*>(e)) {
            indent(level); out << "UnaryOpExpr: op='" << u->getOperator() << "' " << u->loc << "\n";
            printExpr(u->getOperand(), level+1);
            return;
        }
        if (auto fc = dynamic_cast<FunctionCallExpr*>(e)) {
            indent(level); out << "FunctionCallExpr " << fc->loc << "\n";
            indent(level+1); out << "Callee:\n";
            printExpr(fc->getCallee(), level+2);
            const auto& args = fc->getArgs();
            if (!args.empty()) {
                indent(level+1); out << "Args:\n";
                for (auto* a : args) printExpr(a, level+2);
            }
            return;
        }
        if (auto ia = dynamic_cast<IndexAccessExpr*>(e)) {
            indent(level); out << "IndexAccessExpr " << ia->loc << "\n";
            indent(level+1); out << "Target:\n"; printExpr(ia->getTarget(), level+2);
            indent(level+1); out << "Index:\n"; printExpr(ia->getIndex(), level+2);
            return;
        }
        if (auto s = dynamic_cast<SliceExpr*>(e)) {
            indent(level); out << "SliceExpr " << s->loc << "\n";
            indent(level+1); out << "Start:\n"; printExpr(s->getStart(), level+2);
            indent(level+1); out << "Stop:\n"; printExpr(s->getStop(), level+2);
            indent(level+1); out << "Step:\n"; printExpr(s->getStep(), level+2);
            return;
        }
        if (auto ma = dynamic_cast<MemberAccessExpr*>(e)) {
            indent(level); out << "MemberAccessExpr " << ma->loc << "\n";
            indent(level+1); out << "Target:\n"; printExpr(ma->getTarget(), level+2);
            indent(level+1); out << "MemberName:\n"; printExpr(ma->getMemberName(), level+2);
            return;
        }
        if (auto c = dynamic_cast<CastExpr*>(e)) {
            indent(level); out << "CastExpr " << c->loc << "\n";
            indent(level+1); out << "TargetType:\n"; printType(c->getTargetType(), level+2);
            indent(level+1); out << "Expression:\n"; printExpr(c->getExpression(), level+2);
            return;
        }
        if (auto lam = dynamic_cast<LambdaExpr*>(e)) {
            indent(level); out << "LambdaExpr " << lam->loc << "\n";
            indent(level+1); out << "Parameters:\n";
            for (auto* p : lam->getParams()) printDecl(p, level+2);
            indent(level+1); out << "ReturnType:\n"; printType(lam->getReturnType(), level+2);
            indent(level+1); out << "Body:\n"; printStmt(lam->getBody(), level+2);
            return;
        }
        if (auto br = dynamic_cast<BracedInitList*>(e)) {
            indent(level); out << "BracedInitList " << br->loc << "\n";
            for (auto* el : br->getElements()) printExpr(el, level+1);
            return;
        }

        // Quantum state expressions are also Exprs
        if (auto qs = dynamic_cast<QuantumStateExpr*>(e)) {
            printQuantumState(qs, level);
            return;
        }

        // fallback
        indent(level); out << "Unknown Expr (type=" << typeid(*e).name() << ") " << e->loc << "\n";
    }

    void printLiteral(LiteralExpr* lit, int level) {
        if (!lit) { indent(level); out << "<null Literal>\n"; return; }
        if (auto i = dynamic_cast<IntLiteralExpr*>(lit)) {
            indent(level); out << "IntLiteral: " << i->getValue() << " " << i->loc << "\n"; return;
        }
        if (auto f = dynamic_cast<FloatLiteralExpr*>(lit)) {
            indent(level); out << "FloatLiteral: " << f->getValue() << " " << f->loc << "\n"; return;
        }
        if (auto b = dynamic_cast<BoolLiteralExpr*>(lit)) {
            indent(level); out << "BoolLiteral: " << (b->getValue() ? "true" : "false") << " " << b->loc << "\n"; return;
        }
        if (auto s = dynamic_cast<StringLiteralExpr*>(lit)) {
            indent(level); out << "StringLiteral: \"" << s->getValue() << "\" " << s->loc << "\n"; return;
        }
        indent(level); out << "LiteralExpr (unknown subclass) " << lit->loc << "\n";
    }

    // ============================
    // Quantum state expressions
    // ============================
    void printQuantumState(QuantumStateExpr* q, int level) {
        if (!q) { indent(level); out << "<null QuantumStateExpr>\n"; return; }
        if (auto id = dynamic_cast<QuantumStateIdentifier*>(q)) {
            indent(level); out << "QuantumStateIdentifier " << id->loc << "\n";
            printExpr(id->getIdentifier(), level+1);
            return;
        }
        if (auto ia = dynamic_cast<QuantumStateIndexAccess*>(q)) {
            indent(level); out << "QuantumStateIndexAccess " << ia->loc << "\n";
            printExpr(ia->getIdentifier(), level+1);
            printExpr(ia->getIndex(), level+1);
            return;
        }
        if (auto l = dynamic_cast<QuantumStateList*>(q)) {
            indent(level); out << "QuantumStateList " << l->loc << "\n";
            for (auto* s : l->getStates()) printExpr(s, level+1);
            return;
        }
        indent(level); out << "Unknown QuantumStateExpr (type=" << typeid(*q).name() << ")\n";
    }

    // ============================
    // Statements
    // ============================
    void printStmt(Stmt* s, int level) {
        if (!s) { indent(level); out << "<null Stmt>\n"; return; }

        if (auto c = dynamic_cast<CompoundStmt*>(s)) {
            indent(level); out << "CompoundStmt " << c->loc << "\n";
            for (auto* st : c->getStatements()) printStmt(st, level+1);
            return;
        }
        if (auto ds = dynamic_cast<DeclarationStmt*>(s)) {
            indent(level); out << "DeclarationStmt " << ds->loc << "\n";
            for (auto* vd : ds->getDeclarations()) printDecl(vd, level+1);
            return;
        }
        if (auto as = dynamic_cast<AssignmentStmt*>(s)) {
            indent(level); out << "AssignmentStmt op='" << as->getOperator() << "' " << as->loc << "\n";
            indent(level+1); out << "Left:\n"; printExpr(as->getLeft(), level+2);
            indent(level+1); out << "Right:\n"; printExpr(as->getRight(), level+2);
            return;
        }
        if (auto es = dynamic_cast<ExpressionStmt*>(s)) {
            indent(level); out << "ExpressionStmt " << es->loc << "\n";
            printExpr(es->getExpression(), level+1);
            return;
        }
        if (auto is = dynamic_cast<IfStmt*>(s)) {
            indent(level); out << "IfStmt " << is->loc << "\n";
            indent(level+1); out << "Condition:\n"; printExpr(is->getCondition(), level+2);
            indent(level+1); out << "Then:\n"; printStmt(is->getThenBlock(), level+2);
            if (is->getElseBlock()) { indent(level+1); out << "Else:\n"; printStmt(is->getElseBlock(), level+2); }
            return;
        }
        if (auto ms = dynamic_cast<MatchStmt*>(s)) {
            indent(level); out << "MatchStmt " << ms->loc << "\n";
            indent(level+1); out << "MatchExpr:\n"; printExpr(ms->getMatchExpr(), level+2);
            indent(level+1); out << "Cases:\n";
            for (auto* ccase : ms->getCases()) printMatchCase(ccase, level+2);
            return;
        }
        if (auto w = dynamic_cast<WhileStmt*>(s)) {
            indent(level); out << "WhileStmt " << w->loc << "\n";
            indent(level+1); out << "Condition:\n"; printExpr(w->getCondition(), level+2);
            indent(level+1); out << "Body:\n"; printStmt(w->getBody(), level+2);
            return;
        }
        if (auto dw = dynamic_cast<DoWhileStmt*>(s)) {
            indent(level); out << "DoWhileStmt " << dw->loc << "\n";
            indent(level+1); out << "Body:\n"; printStmt(dw->getBody(), level+2);
            indent(level+1); out << "Condition:\n"; printExpr(dw->getCondition(), level+2);
            return;
        }
        if (auto f = dynamic_cast<ForStmt*>(s)) {
            indent(level); out << "ForStmt " << f->loc << "\n";
            indent(level+1); out << "Init:\n"; printStmt(f->getInit(), level+2);
            indent(level+1); out << "Condition:\n"; printExpr(f->getCondition(), level+2);
            indent(level+1); out << "Update:\n"; printExpr(f->getUpdate(), level+2);
            indent(level+1); out << "Body:\n"; printStmt(f->getBody(), level+2);
            return;
        }
        if (auto cont = dynamic_cast<ContinueStmt*>(s)) { indent(level); out << "ContinueStmt " << cont->loc << "\n"; return; }
        if (auto br = dynamic_cast<BreakStmt*>(s)) { indent(level); out << "BreakStmt " << br->loc << "\n"; return; }
        if (auto ret = dynamic_cast<ReturnStmt*>(s)) {
            indent(level); out << "ReturnStmt " << ret->loc << "\n";
            if (ret->hasReturnValue()) { indent(level+1); out << "Value:\n"; printExpr(ret->getReturnValue(), level+2); }
            return;
        }
        if (auto ps = dynamic_cast<PrintStmt*>(s)) {
            indent(level); out << "PrintStmt kind=" << static_cast<int>(ps->getKind()) << " " << ps->loc << "\n";
            if (ps->getArgument()) printExpr(ps->getArgument(), level+1);
            return;
        }
        if (auto qs = dynamic_cast<QuantumStmt*>(s)) {
            // quantum statements: try Apply/Measure/Reset
            if (auto ag = dynamic_cast<ApplyGateStmt*>(s)) {
                indent(level); out << "ApplyGateStmt " << ag->loc << "\n";
                indent(level+1); out << "Gate:\n"; printQuantum(dynamic_cast<GateNode*>(ag->getGate()), level+2);
                indent(level+1); out << "Target:\n"; printExpr(ag->getTarget(), level+2);
                return;
            }
            if (auto msr = dynamic_cast<MeasureStmt*>(s)) {
                indent(level); out << "MeasureStmt " << msr->loc << "\n";
                indent(level+1); out << "Source:\n"; printExpr(msr->getSource(), level+2);
                indent(level+1); out << "Target:\n"; printExpr(msr->getTarget(), level+2);
                return;
            }
            if (auto rz = dynamic_cast<ResetStmt*>(s)) {
                indent(level); out << "ResetStmt " << rz->loc << "\n";
                indent(level+1); out << "Target:\n"; printExpr(rz->getTarget(), level+2);
                return;
            }
            // fallback
            indent(level); out << "QuantumStmt (unknown subclass) " << s->loc << "\n";
            return;
        }

        indent(level); out << "Unknown Stmt (type=" << typeid(*s).name() << ") " << s->loc << "\n";
    }

    void printMatchCase(MatchCase* c, int level) {
        if (!c) { indent(level); out << "<null MatchCase>\n"; return; }
        indent(level); out << "MatchCase " << c->loc << "\n";
        indent(level+1); out << "Pattern:\n"; printExpr(c->getMatchExpr(), level+2);
        indent(level+1); out << "Body:\n"; printStmt(c->getBody(), level+2);
    }

    // ============================
    // Declarations
    // ============================
    void printDecl(Decl* d, int level) {
        if (!d) { indent(level); out << "<null Decl>\n"; return; }

        if (auto vd = dynamic_cast<VariableDecl*>(d)) {
            indent(level); out << "VariableDecl " << vd->loc << "\n";
            indent(level+1); out << "Name:\n"; printExpr(vd->getVariableName(), level+2);
            indent(level+1); out << "Type:\n"; printType(vd->getVariableType(), level+2);
            if (vd->getInitializer()) { indent(level+1); out << "Initializer:\n"; printExpr(vd->getInitializer(), level+2); }
            return;
        }
        if (auto pd = dynamic_cast<ParameterDecl*>(d)) {
            indent(level); out << "ParameterDecl " << pd->loc << "\n";
            indent(level+1); out << "Name:\n"; printExpr(pd->getName(), level+2);
            indent(level+1); out << "Type:\n"; printType(pd->getType(), level+2);
            return;
        }
        if (auto fd = dynamic_cast<FunctionDecl*>(d)) {
            indent(level); out << "FunctionDecl " << fd->loc << "\n";
            indent(level+1); out << "Name:\n"; printExpr(fd->getFunctionName(), level+2);
            indent(level+1); out << "Parameters:\n";
            for (auto* p : fd->getParameters()) printDecl(p, level+2);
            indent(level+1); out << "ReturnType:\n"; printType(fd->getReturnType(), level+2);
            indent(level+1); out << "Body:\n"; printStmt(fd->getBody(), level+2);
            return;
        }
        if (auto gd = dynamic_cast<GateDecl*>(d)) {
            indent(level); out << "GateDecl " << gd->loc << "\n";
            indent(level+1); out << "Gate:\n"; printQuantum(dynamic_cast<GateNode*>(gd->getGate()), level+2);
            return;
        }

        indent(level); out << "Unknown Decl (type=" << typeid(*d).name() << ") " << d->loc << "\n";
    }

    // ============================
    // Types
    // ============================
    void printType(Type* t, int level) {
        if (!t) { indent(level); out << "<null Type>\n"; return; }

        if (auto bt = dynamic_cast<BaseTypeNode*>(t)) {
            indent(level); out << "BaseTypeNode: ";
            switch (bt->getBaseType()) {
                case BaseTypeKind::INT: out << "INT"; break;
                case BaseTypeKind::FLOAT: out << "FLOAT"; break;
                case BaseTypeKind::QUBIT: out << "QUBIT"; break;
                case BaseTypeKind::BIT: out << "BIT"; break;
                case BaseTypeKind::BOOL: out << "BOOL"; break;
                case BaseTypeKind::STRING: out << "STRING"; break;
                default: out << "UNKNOWN";
            }
            out << " " << bt->loc << "\n";
            return;
        }
        if (auto at = dynamic_cast<ArrayTypeNode*>(t)) {
            indent(level); out << "ArrayTypeNode " << at->loc << "\n";
            indent(level+1); out << "ElementType:\n"; printType(at->getElementType(), level+2);
            if (at->getDimension()) { indent(level+1); out << "Dimension:\n"; printExpr(at->getDimension(), level+2); }
            return;
        }
        if (auto ft = dynamic_cast<FunctionTypeNode*>(t)) {
            indent(level); out << "FunctionTypeNode " << ft->loc << "\n";
            indent(level+1); out << "ParamTypes:\n";
            for (auto* pt : ft->getParamTypes()) printType(pt, level+2);
            indent(level+1); out << "ReturnType:\n"; printType(ft->getReturnType(), level+2);
            return;
        }

        indent(level); out << "Unknown Type (type=" << typeid(*t).name() << ")\n";
    }

    // ============================
    // Quantum / Gate Nodes
    // ============================
    void printQuantum(Quantum* q, int level) {
        if (!q) { indent(level); out << "<null Quantum>\n"; return; }

        if (auto gn = dynamic_cast<GateNode*>(q)) {
            // Gate kinds: Simple, Parametric, Composite, Composition
            if (auto sg = dynamic_cast<SimpleGateNode*>(gn)) {
                indent(level); out << "SimpleGateNode kind=" << gateKindToString(sg->getGateKind()) << " " << sg->loc << "\n";
                return;
            }
            if (auto pg = dynamic_cast<ParametricGateNode*>(gn)) {
                indent(level); out << "ParametricGateNode kind=" << gateKindToString(pg->getGateKind()) << " " << pg->loc << "\n";
                indent(level+1); out << "Params:\n";
                for (auto* p : pg->getParams()) printExpr(p, level+2);
                return;
            }
            if (auto cg = dynamic_cast<TensoredGateNode*>(gn)) {
                indent(level); out << "TensoredGateNode " << cg->loc << "\n";
                for (auto* g : cg->getGates()) printQuantum(g, level+1);
                return;
            }
            if (auto gc = dynamic_cast<GateCompositionNode*>(gn)) {
                indent(level); out << "GateCompositionNode " << gc->loc << "\n";
                indent(level+1); out << "Left:\n"; printQuantum(gc->getLeft(), level+2);
                indent(level+1); out << "Right:\n"; printQuantum(gc->getRight(), level+2);
                return;
            }

            indent(level); out << "GateNode (unknown subclass) " << gn->loc << "\n";
            return;
        }

        // fallback: other quantum nodes (if any)
        indent(level); out << "Quantum (unknown subclass type=" << typeid(*q).name() << ")\n";
    }

    const char* gateKindToString(GateKind k) {
        switch (k) {
            case GateKind::H: return "H";
            case GateKind::S: return "S";
            case GateKind::T: return "T";
            case GateKind::I: return "I";
            case GateKind::X: return "X";
            case GateKind::Y: return "Y";
            case GateKind::Z: return "Z";
            case GateKind::RX: return "RX";
            case GateKind::RY: return "RY";
            case GateKind::RZ: return "RZ";
            case GateKind::CNOT: return "CNOT";
            case GateKind::CZ: return "CZ";
            case GateKind::SWAP: return "SWAP";
            case GateKind::CSWAP: return "CSWAP";
            case GateKind::CCNOT: return "CCNOT";
            case GateKind::CRX: return "CRX";
            case GateKind::CRY: return "CRY";
            case GateKind::CRZ: return "CRZ";
            case GateKind::UNKNOWN: return "UNKNOWN";
            case GateKind::CTRL: return "CTRL";
            default: return "GATE-UNRECOGNIZED";
        }
    }
};

inline void printTranslationUnit(std::ostream& os, const std::vector<ASTNode*>& tu, int indent = 2) {
    ASTPrinter printer(os, indent);
    os << "TranslationUnit: (" << tu.size() << " top-level nodes)\n";
    for (size_t i = 0; i < tu.size(); ++i) {
        os << "  [" << i << "]:\n";
        printer.print(tu[i], 2); // start printing each node at indentation level 2
    }
}

} // namespace Heisen

#endif // HEISEN_AST_PRINTER_HPP
