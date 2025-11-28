#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Whitespace, LeadingSpaces) {
    EXPECT_EQ(run_lexer("   let"), LET);
}

TEST(Whitespace, NewlineResetsColumn) {
    run_lexer("let\n");
    EXPECT_EQ(yycolumn, 1);
}

TEST(Whitespace, CommentIgnored) {
    EXPECT_EQ(run_lexer("# comment\nlet"), LET);
}
