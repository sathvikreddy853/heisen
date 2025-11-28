
#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Sema_Duplicate, RedeclareVariable) {
    const std::string src = R"(
        func main() {
            let x: int;
            let x: int;
        }
    )";
    EXPECT_TRUE(sema_fail(src));
    EXPECT_TRUE(sema_fail_contains(src, "already declared") || sema_fail_contains(src, "duplicate"));
}
