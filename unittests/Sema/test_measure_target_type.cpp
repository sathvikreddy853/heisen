#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Sema_Measure, WrongTarget) {
    const std::string src = R"(
        func main() {
            let x: int;
            measure x >> y; // measure expects quantum states
        }
    )";
    EXPECT_TRUE(sema_fail(src));
    EXPECT_TRUE(sema_fail_contains(src, "measure") || sema_fail_contains(src, "quantum"));
}
