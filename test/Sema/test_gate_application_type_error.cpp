
#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Sema_Gate, ApplicationTargetType) {
    const std::string src = R"(
        func main() {
            let a: int = 0;
            H @ a; // applying gate to non-qubit should error
        }
    )";
    EXPECT_TRUE(sema_fail(src));
    EXPECT_TRUE(sema_fail_contains(src, "gate") || sema_fail_contains(src, "qubit"));
}
