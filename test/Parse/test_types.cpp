#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Types, ArrayType) {
    auto* decl = dynamic_cast<DeclarationStmt*>(parseStmt("let x: int[5];"));
    ASSERT_NE(decl, nullptr);

    auto* var = decl->getDeclarations()[0];
    auto* arr = dynamic_cast<ArrayTypeNode*>(var->getVariableType());
    ASSERT_NE(arr, nullptr);
}
