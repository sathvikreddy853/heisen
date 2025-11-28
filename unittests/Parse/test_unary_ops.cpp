#include <gtest/gtest.h>
#include "Test.hpp"

TEST(UnaryOps, Negate) {
    auto* e = dynamic_cast<UnaryOpExpr*>(parseExpr("-5;"));
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->getOperator(), "-");
}
