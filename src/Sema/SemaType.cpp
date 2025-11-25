#include <SemaType.hpp>

namespace Heisen {

/* SemanticType Method Definitions */
bool SemanticType::isQuantum () const {
    return false;
}

bool SemanticType::isCopyable () const {
    return true;
}

/* PrimitiveType Method Definitions */
bool PrimitiveType::equals (const SemanticType* other) const {
    return other && other->getKind () == kind;
}

bool PrimitiveType::isQuantum () const {
    return kind == Kind::QUBIT || kind == Kind::BIT;
}

bool PrimitiveType::isCopyable () const {
    return kind != Kind::QUBIT;
}

/* ArrayType Method Definitions */
SemanticType* ArrayType::getElementType () const {
    return elementType;
}

int ArrayType::getDimension () const {
    return dimension;
}

std::string ArrayType::toString () const {
    std::string dimStr =
    (dimension >= 0) ? "[" + std::to_string (dimension) + "]" : "[]";
    return elementType->toString () + dimStr;
}

bool ArrayType::equals (const SemanticType* other) const {
    if (!other || other->getKind () != Kind::ARRAY) return false;
    auto* otherArr = static_cast<const ArrayType*> (other);
    return elementType->equals (otherArr->elementType);
}

bool ArrayType::isQuantum () const {
    return elementType->isQuantum ();
}
bool ArrayType::isCopyable () const {
    return elementType->isCopyable ();
}

/* FunctionType Method Definitions */
const std::vector<SemanticType*>& FunctionType::getParamTypes () const {
    return paramTypes;
}

SemanticType* FunctionType::getReturnType () const {
    return returnType;
}

std::string FunctionType::toString () const {
    std::string result = "(";
    for (size_t i = 0; i < paramTypes.size (); ++i) {
        if (i > 0) result += ", ";
        result += paramTypes[i]->toString ();
    }
    result += ") => ";
    result += returnType ? returnType->toString () : "()";
    return result;
}

bool FunctionType::equals (const SemanticType* other) const {
    if (!other || other->getKind () != Kind::FUNCTION) return false;
    auto* otherFunc = static_cast<const FunctionType*> (other);

    if (paramTypes.size () != otherFunc->paramTypes.size ())
        return false;

    for (size_t i = 0; i < paramTypes.size (); ++i) {
        if (!paramTypes[i]->equals (otherFunc->paramTypes[i]))
            return false;
    }

    if (returnType && otherFunc->returnType) {
        return returnType->equals (otherFunc->returnType);
    }
    return (!returnType && !otherFunc->returnType);
}

} // namespace Heisen