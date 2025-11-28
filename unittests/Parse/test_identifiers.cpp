#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Identifiers, BasicIdentifier) {
    auto* id = dynamic_cast<IdentifierExpr*>(parseExpr("foo;"));
    ASSERT_NE(id, nullptr);
    EXPECT_EQ(id->getValue(), "foo");
}
