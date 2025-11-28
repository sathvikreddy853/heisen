
#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Sema_Qubit, CopyNotAllowed) {
    const std::string src = R"(
        func main() {
            let q1: qubit;
            let q2: qubit = q1; // copying qubit should be forbidden
        }
    )";
    EXPECT_TRUE(sema_fail(src));
    EXPECT_TRUE(sema_fail_contains(src, "qubit") || sema_fail_contains(src, "copy"));
}
