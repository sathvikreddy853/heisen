#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Sema_Return, MissingReturn) {
    const std::string src = R"(
        func f(): int {
            let x: int = 1;
            // missing return
        }
    )";
    EXPECT_TRUE(sema_fail(src));
    EXPECT_TRUE(sema_fail_contains(src, "return") || sema_fail_contains(src, "missing"));
}
