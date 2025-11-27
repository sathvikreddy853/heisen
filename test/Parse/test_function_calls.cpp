#include <gtest/gtest.h>
#include "Test.hpp"

TEST(FunctionCalls, OneArgument) {
    auto* call = dynamic_cast<FunctionCallExpr*>(parseExpr("f(10);"));
    ASSERT_NE(call, nullptr);

    auto* arg = dynamic_cast<IntLiteralExpr*>(argOf(call, 0));
    ASSERT_NE(arg, nullptr);
    EXPECT_EQ(arg->getValue(), 10);
}
