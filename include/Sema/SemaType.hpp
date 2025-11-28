#ifndef HEISEN_SEMATYPE_HPP
#define HEISEN_SEMATYPE_HPP

#include <Macros.hpp>

namespace Heisen {

class SemanticType {
    public:
    enum class Kind {
        INT,
        FLOAT,
        BOOL,
        STRING,
        QUBIT,
        BIT,
        ARRAY,
        FUNCTION,
        VOID,
        ERROR
    };

    protected:
    Kind kind;

    public:
    SemanticType (Kind k) : kind (k) {
    }
    virtual ~SemanticType () = default;
    Kind getKind () const {
        return kind;
    }
    virtual std::string toString () const                 = 0;
    virtual bool equals (const SemanticType* other) const = 0;
    virtual bool isQuantum () const;
    virtual bool isCopyable () const;
};

class PrimitiveType : public SemanticType {
    public:
    PrimitiveType (Kind k) : SemanticType (k) {
    }

    std::string toString () const override {
        switch (kind) {
        case Kind::INT: return "int";
        case Kind::FLOAT: return "float";
        case Kind::BOOL: return "bool";
        case Kind::STRING: return "string";
        case Kind::QUBIT: return "qubit";
        case Kind::BIT: return "bit";
        case Kind::VOID: return "void";
        case Kind::ERROR: return "ERROR";
        default: return "UNKNOWN";
        }
    }

    bool equals (const SemanticType* other) const override;
    bool isQuantum () const override;
    bool isCopyable () const override;
};

class ArrayType : public SemanticType {
    SemanticType* elementType;
    int dimension;

    public:
    ArrayType (SemanticType* elem, int dim = -1)
    : SemanticType (Kind::ARRAY), elementType (elem), dimension (dim) {
    }

    ~ArrayType () {
        delete elementType;
    }

    SemanticType* getElementType () const;
    int getDimension () const;
    std::string toString () const override;
    bool equals (const SemanticType* other) const override;
    bool isQuantum () const override;
    bool isCopyable () const override;
};

class FunctionType : public SemanticType {
    std::vector<SemanticType*> paramTypes;
    SemanticType* returnType;

    public:
    FunctionType (std::vector<SemanticType*> params, SemanticType* ret)
    : SemanticType (Kind::FUNCTION), paramTypes (std::move (params)), returnType (ret) {
    }

    ~FunctionType () {
        for (auto* p : paramTypes) delete p;
        delete returnType;
    }

    const std::vector<SemanticType*>& getParamTypes () const;
    SemanticType* getReturnType () const;
    std::string toString () const override;
    bool equals (const SemanticType* other) const override;
};

} // namespace Heisen

#endif // HEISEN_SEMATYPE_HPP
