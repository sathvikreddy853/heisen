#include "Lex/TokenType.hpp"

namespace heisen {

std::string TokenType::to_str() const {
    switch (value) {
    case Int: return "Int"; break;
    case Float: return "Float"; break;
    case Bool: return "Bool"; break;
    case String: return "String"; break;
    case Bit: return "Bit"; break;
    case Qubit: return "Qubit"; break;
    case Cast: return "Cast"; break;
    case Generic: return "Generic"; break;
    case And: return "And"; break;
    case Or: return "Or"; break;
    case Not: return "Not"; break;
    case Try: return "Try"; break;
    case Throw: return "Throw"; break;
    case Catch: return "Catch"; break;
    case Let: return "Let"; break;
    case Const: return "Const"; break;
    case Break: return "Break"; break;
    case Continue: return "Continue"; break;
    case Return: return "Return"; break;
    case Func: return "Func"; break;
    case Struct: return "Struct"; break;
    case Scope: return "Scope"; break;
    case If: return "If"; break;
    case Elif: return "Elif"; break;
    case Else: return "Else"; break;
    case Match: return "Match"; break;
    case With: return "With"; break;
    case True: return "True"; break;
    case False: return "False"; break;
    case For: return "For"; break;
    case While: return "While"; break;
    case Gate: return "Gate"; break;
    case Circuit: return "Circuit"; break;
    case Measure: return "Measure"; break;
    case Reset: return "Reset"; break;
    case Plus: return "Plus"; break;
    case Minus: return "Minus"; break;
    case Star: return "Star"; break;
    case Slash: return "Slash"; break;
    case Percent: return "Percent"; break;
    case Exp: return "Exp"; break;
    case Assign: return "Assign"; break;
    case Equal: return "Equal"; break;
    case NotEqual: return "NotEqual"; break;
    case Less: return "Less"; break;
    case LessEqual: return "LessEqual"; break;
    case Greater: return "Greater"; break;
    case GreaterEqual: return "GreaterEqual"; break;
    case PlusAssign: return "PlusAssign"; break;
    case MinusAssign: return "MinusAssign"; break;
    case StarAssign: return "StarAssign"; break;
    case SlashAssign: return "SlashAssign"; break;
    case PercentAssign: return "PercentAssign"; break;
    case ExpAssign: return "ExpAssign"; break;
    case Identifier: return "Identifier"; break;
    case IntLiteral: return YELLOW + "IntLiteral" + RESET; break;
    case FloatLiteral: return YELLOW + "FloatLiteral" + RESET; break;
    case StringLiteral: return YELLOW +"StringLiteral" + RESET; break;
    case LParen: return "LParen"; break;
    case RParen: return "RParen"; break;
    case LBrace: return "LBrace"; break;
    case RBrace: return "RBrace"; break;
    case LBracket: return "LBracket"; break;
    case RBracket: return "RBracket"; break;
    case Comma: return "Comma"; break;
    case Colon: return "Colon"; break;
    case Semicolon: return "Semicolon"; break;
    case Dot: return "Dot"; break;
    case EndOfFile: return "EndOfFile"; break;
    case Unknown: return "Unknown"; break;
    default: return "InvalidToken"; break;
    }
}

std::ostream& operator<<(std::ostream& os, TokenType type) {
    os << type.to_str();
    return os;
}

bool TokenType::operator==(TokenType other) const {
    return value == other.value;
}

bool TokenType::operator!=(TokenType other) const {
    return value != other.value;
}

bool TokenType::operator==(Value other) const {
    return value == other;
}

bool TokenType::operator!=(Value other) const {
    return value != other;
}

} // namespace heisen
