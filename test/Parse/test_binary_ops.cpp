#include <gtest/gtest.h>
#include "Test.hpp"

TEST(BinaryOps, Addition) {
    auto* e = dynamic_cast<BinaryOpExpr*>(parseExpr("1 + 2;"));
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->getOperator(), "+");
}

TEST(BinaryOps, Multiplication) {
    auto* e = dynamic_cast<BinaryOpExpr*>(parseExpr("3 * 4;"));
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->getOperator(), "*");
}
