#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Statements, IfStmt) {
    auto* stmt = dynamic_cast<IfStmt*>(parseStmt("if (1 < 2) { }"));
    ASSERT_NE(stmt, nullptr);

    auto* cond = dynamic_cast<BinaryOpExpr*>(stmt->getCondition());
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->getOperator(), "<");
}
