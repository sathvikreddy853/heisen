#include <gtest/gtest.h>
#include "Test.hpp"

TEST(ParserErrors, InvalidSyntax) {
    parseSource("let x: ;"); // Missing type

    // translationUnit should be empty or contain an error node depending on your parser design.
    EXPECT_TRUE(translationUnit.empty());
}
