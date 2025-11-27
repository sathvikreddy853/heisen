#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Sema_FunctionReturn, WrongReturnType) {
    const std::string src = R"(
        func f(): int {
            return 3.14;
        }
    )";
    EXPECT_TRUE(sema_fail(src));
    EXPECT_TRUE(sema_fail_contains(src, "return") || sema_fail_contains(src, "type"));
}
