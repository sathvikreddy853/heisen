
#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Sema_Call, WrongArgCount) {
    const std::string src = R"(
        func add(a: int, b: int): int {
            return a + b;
        }
        func main() {
            let v: int = add(1); // missing one arg
        }
    )";
    EXPECT_TRUE(sema_fail(src));
    EXPECT_TRUE(sema_fail_contains(src, "argument") || sema_fail_contains(src, "parameters"));
}
