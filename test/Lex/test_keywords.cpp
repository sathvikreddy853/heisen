#include <gtest/gtest.h>
#include <Test.hpp>

TEST(TypeTest, Qubit)  { EXPECT_EQ(run_lexer("qubit"), QUBIT); }
TEST(TypeTest, Bit)    { EXPECT_EQ(run_lexer("bit"), BIT); }
TEST(TypeTest, Int)    { EXPECT_EQ(run_lexer("int"), INT); }
TEST(TypeTest, Float)  { EXPECT_EQ(run_lexer("float"), FLOAT); }
TEST(TypeTest, String) { EXPECT_EQ(run_lexer("string"), STRING); }
TEST(TypeTest, Bool)   { EXPECT_EQ(run_lexer("bool"), BOOL); }
TEST(KeywordTest, Func)     { EXPECT_EQ(run_lexer("func"), FUNC); }
TEST(KeywordTest, Let)      { EXPECT_EQ(run_lexer("let"), LET); }
TEST(KeywordTest, And)      { EXPECT_EQ(run_lexer("and"), AND); }
TEST(KeywordTest, Or)       { EXPECT_EQ(run_lexer("or"), OR); }
TEST(KeywordTest, Not)      { EXPECT_EQ(run_lexer("not"), NOT); }
TEST(KeywordTest, True)   { EXPECT_EQ(run_lexer("true"), TRUE); }
TEST(KeywordTest, False)  { EXPECT_EQ(run_lexer("false"), FALSE); }
TEST(KeywordTest, For)      { EXPECT_EQ(run_lexer("for"), FOR); }
TEST(KeywordTest, While)    { EXPECT_EQ(run_lexer("while"), WHILE); }
TEST(KeywordTest, Do)       { EXPECT_EQ(run_lexer("do"), DO); }
TEST(KeywordTest, Break)    { EXPECT_EQ(run_lexer("break"), BREAK); }
TEST(KeywordTest, Continue) { EXPECT_EQ(run_lexer("continue"), CONTINUE); }
TEST(KeywordTest, If)       { EXPECT_EQ(run_lexer("if"), IF); }
TEST(KeywordTest, Elif)     { EXPECT_EQ(run_lexer("elif"), ELIF); }
TEST(KeywordTest, Else)     { EXPECT_EQ(run_lexer("else"), ELSE); }
TEST(KeywordTest, Match)    { EXPECT_EQ(run_lexer("match"), MATCH); }
TEST(KeywordTest, Import)   { EXPECT_EQ(run_lexer("import"), IMPORT); }
TEST(KeywordTest, Return)   { EXPECT_EQ(run_lexer("return"), RETURN); }
TEST(KeywordTest, Const)    { EXPECT_EQ(run_lexer("const"), CONST); }
TEST(KeywordTest, Class)    { EXPECT_EQ(run_lexer("class"), CLASS); }
TEST(KeywordTest, Measure)  { EXPECT_EQ(run_lexer("measure"), MEASURE_OP); }
TEST(KeywordTest, Reset)    { EXPECT_EQ(run_lexer("reset"), RESET_OP); }
TEST(KeywordTest, Gate)     { EXPECT_EQ(run_lexer("gate"), GATE); }
TEST(KeywordTest, Apply)    { EXPECT_EQ(run_lexer("apply"), APPLY); }


TEST(ReservedKeywordTest, Circuit) { EXPECT_EQ(run_lexer("circuit"), CIRCUIT); }
TEST(ReservedKeywordTest, Try)     { EXPECT_EQ(run_lexer("try"), TRY); }
TEST(ReservedKeywordTest, Catch)   { EXPECT_EQ(run_lexer("catch"), CATCH); }
TEST(ReservedKeywordTest, Throw)   { EXPECT_EQ(run_lexer("throw"), THROW); }
