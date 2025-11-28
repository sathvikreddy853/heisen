#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Declarations, VariableDeclaration) {
    auto* decl = dynamic_cast<DeclarationStmt*>(parseStmt("let x: int;"));
    ASSERT_NE(decl, nullptr);

    auto* var = decl->getDeclarations()[0];
    auto* name = var->getVariableName();
    EXPECT_EQ(name->getValue(), "x");
}
