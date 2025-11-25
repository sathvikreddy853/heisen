#ifndef HEISEN_SEMATYPE_HPP
#define HEISEN_SEMATYPE_HPP

#include <Macros.hpp>

namespace Heisen {

class SemanticType {
    public:
    enum class Kind { INT, FLOAT, BOOL, STRING, QUBIT, BIT, ARRAY, FUNCTION, VOID, ERROR };

    protected:
    Kind kind;

    public:
    SemanticType (Kind k) : kind (k) {}
    virtual ~SemanticType () = default;
    Kind getKind () const { return kind; }
    virtual std::string toString () const                 = 0;
    virtual bool equals (const SemanticType* other) const = 0;
    virtual bool isQuantum () const { return false; }
    virtual bool isCopyable () const { return true; }
};

class PrimitiveType : public SemanticType {
    public:
    PrimitiveType (Kind k) : SemanticType (k) {}

    std::string toString () const override {
        switch (kind) {
        case Kind::INT: return "int";
        case Kind::FLOAT: return "float";
        case Kind::BOOL: return "bool";
        case Kind::STRING: return "string";
        case Kind::QUBIT: return "qubit";
        case Kind::BIT: return "bit";
        case Kind::VOID: return "void";
        case Kind::ERROR: return "<error>";
        default: return "<unknown>";
        }
    }

    bool equals (const SemanticType* other) const override {
        return other && other->getKind () == kind;
    }

    bool isQuantum () const override { return kind == Kind::QUBIT || kind == Kind::BIT; }

    bool isCopyable () const override {
        // Qubits cannot be copied (no-cloning theorem)
        return kind != Kind::QUBIT;
    }
};

class ArrayType : public SemanticType {
    SemanticType* elementType;
    int dimension; // -1 for unknown size

    public:
    ArrayType (SemanticType* elem, int dim = -1)
    : SemanticType (Kind::ARRAY), elementType (elem), dimension (dim) {}

    ~ArrayType () { delete elementType; }

    SemanticType* getElementType () const { return elementType; }
    int getDimension () const { return dimension; }

    std::string toString () const override {
        std::string dimStr = (dimension >= 0) ? "[" + std::to_string (dimension) + "]" : "[]";
        return elementType->toString () + dimStr;
    }

    bool equals (const SemanticType* other) const override {
        if (!other || other->getKind () != Kind::ARRAY) return false;
        auto* otherArr = static_cast<const ArrayType*> (other);
        return elementType->equals (otherArr->elementType);
    }

    bool isQuantum () const override { return elementType->isQuantum (); }

    bool isCopyable () const override { return elementType->isCopyable (); }
};

class FunctionType : public SemanticType {
    std::vector<SemanticType*> paramTypes;
    SemanticType* returnType;

    public:
    FunctionType (std::vector<SemanticType*> params, SemanticType* ret)
    : SemanticType (Kind::FUNCTION), paramTypes (std::move (params)), returnType (ret) {}

    ~FunctionType () {
        for (auto* p : paramTypes) delete p;
        delete returnType;
    }

    const std::vector<SemanticType*>& getParamTypes () const { return paramTypes; }
    SemanticType* getReturnType () const { return returnType; }

    std::string toString () const override {
        std::string result = "(";
        for (size_t i = 0; i < paramTypes.size (); ++i) {
            if (i > 0) result += ", ";
            result += paramTypes[i]->toString ();
        }
        result += ") => ";
        result += returnType ? returnType->toString () : "()";
        return result;
    }

    bool equals (const SemanticType* other) const override {
        if (!other || other->getKind () != Kind::FUNCTION) return false;
        auto* otherFunc = static_cast<const FunctionType*> (other);

        if (paramTypes.size () != otherFunc->paramTypes.size ()) return false;

        for (size_t i = 0; i < paramTypes.size (); ++i) {
            if (!paramTypes[i]->equals (otherFunc->paramTypes[i])) return false;
        }

        if (returnType && otherFunc->returnType) {
            return returnType->equals (otherFunc->returnType);
        }
        return (!returnType && !otherFunc->returnType);
    }
};
  
} // namespace Heisen

#endif // HEISEN_SEMATYPE_HPP
