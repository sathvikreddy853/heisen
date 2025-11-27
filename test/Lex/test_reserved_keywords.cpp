#include <gtest/gtest.h>
#include <Test.hpp>

TEST(ReservedKeywordTest, Circuit) { EXPECT_EQ(run_lexer("circuit"), CIRCUIT); }
TEST(ReservedKeywordTest, Try)     { EXPECT_EQ(run_lexer("try"), TRY); }
TEST(ReservedKeywordTest, Catch)   { EXPECT_EQ(run_lexer("catch"), CATCH); }
TEST(ReservedKeywordTest, Throw)   { EXPECT_EQ(run_lexer("throw"), THROW); }