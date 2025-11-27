#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Sema_TypeMismatch, AssignFloatToInt) {
    const std::string src = R"(
        func main() {
            let a: int = 3.14;
        }
    )";
    EXPECT_TRUE(sema_fail(src));
    EXPECT_TRUE(sema_fail_contains(src, "type") || sema_fail_contains(src, "incompatible"));
}
