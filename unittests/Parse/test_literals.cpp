#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Literals, IntegerLiteral) {
    auto* lit = dynamic_cast<IntLiteralExpr*>(parseExpr("42;"));
    ASSERT_NE(lit, nullptr);
    EXPECT_EQ(lit->getValue(), 42);
}

TEST(Literals, FloatLiteral) {
    auto* lit = dynamic_cast<FloatLiteralExpr*>(parseExpr("3.14;"));
    ASSERT_NE(lit, nullptr);
    EXPECT_DOUBLE_EQ(lit->getValue(), 3.14);
}

TEST(Literals, BoolLiteralTrue) {
    auto* lit = dynamic_cast<BoolLiteralExpr*>(parseExpr("true;"));
    ASSERT_NE(lit, nullptr);
    EXPECT_TRUE(lit->getValue());
}

TEST(Literals, BoolLiteralFalse) {
    auto* lit = dynamic_cast<BoolLiteralExpr*>(parseExpr("false;"));
    ASSERT_NE(lit, nullptr);
    EXPECT_FALSE(lit->getValue());
}

TEST(Literals, StringLiteral) {
    auto* lit = dynamic_cast<StringLiteralExpr*>(parseExpr("\"hello\";"));
    ASSERT_NE(lit, nullptr);
    EXPECT_EQ(lit->getValue(), "hello");
}
