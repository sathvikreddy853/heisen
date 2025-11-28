#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Sema_Undeclared, UsesUnknownVar) {
    const std::string src = R"(
        func main() {
            let x: int = y; // y is undeclared
        }
    )";
    EXPECT_TRUE(sema_fail(src));
    EXPECT_TRUE(sema_fail_contains(src, "undeclared") || sema_fail_contains(src, "not declared"));
}
