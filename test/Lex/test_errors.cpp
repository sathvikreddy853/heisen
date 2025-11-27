#include <gtest/gtest.h>
#include <Test.hpp>

TEST(ErrorTest, UnknownCharacter) {
    int t = run_lexer("@");
    EXPECT_NE(t, IDENTIFIER);
}

TEST(ErrorTest, UnterminatedString) {
    int t = run_lexer("\"abc");
    EXPECT_NE(t, STRING_LITERAL);
}

TEST(ErrorTest, InvalidFloat) {
    int t = run_lexer(".");
    EXPECT_NE(t, FLOAT_LITERAL);
}
