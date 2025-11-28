%option noyywrap

%{
    #include "Macros.hpp"
    #include "Token.hpp"
    #include "Parser.tab.hpp"
    int yycolumn = 1;

    #define YY_USER_ACTION  yycolumn += yyleng;
    using namespace Heisen;

%}
%option yylineno

L   [a-zA-Z_]
D   [0-9]

IDENTIFIER  {L}({L}|{D})*
INTEGER     {D}+
FLOAT       ({D}+\.{D}*|\.{D}+)([eE][+-]?{D}+)?
STRING      (\"([^\"\\]|\\.)*\"|'([^'\\]|\\.)*')
NEWLINE     "\n"

%%
    /* Comments */
"#"[^\n]*                                       { /* ignore single line comments */ }
"'''"([^']|('[^']|''[^']))*"'''"                { /* ignore multi line comments */ }
"\"\"\""([^\"]|(\"[^\"]|\"\"[^\"]))*"\"\"\""    { /* ignore multi line comments */ }

    /* Types */
"qubit"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("qubit"));
                        LOGs("QUBIT"); return QUBIT; 
                    }
"bit"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("bit"));
                        LOGs("BIT"); return BIT; 
                    }
"int"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("int"));
                        LOGs("INT"); return INT; 
                    }
"float"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("float"));
                        LOGs("FLOAT"); return FLOAT; 
                    }
"string"            { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("string"));
                        LOGs("STRING"); return STRING; 
                    }
"bool"              { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("bool"));
                        LOGs("BOOL"); return BOOL; 
                    }

    /* Keywords */
"func"              { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("func"));
                        LOGs("FUNC"); return FUNC; 
                    }
"let"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("let"));
                        LOGs("LET"); return LET; 
                    }
"and"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("and"));
                        LOGs("AND"); return AND; 
                    }
"or"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("or"));
                        LOGs("OR"); return OR; 
                    }
"not"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("not"));
                        LOGs("NOT"); return NOT; 
                    }
"true"              {   
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("true"));
                        LOGs("TRUE"); return TRUE; 
                    }
"false"             {   
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("false"));
                        LOGs("FALSE"); return FALSE; 
                    }
"for"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("for"));
                        LOGs("FOR"); return FOR; 
                    }
"while"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("while"));
                        LOGs("WHILE"); return WHILE; 
                    }
"do"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("do"));
                        LOGs("DO"); return DO; 
                    }
"break"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("break"));
                        LOGs("BREAK"); return BREAK; 
                    }
"continue"          { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("continue"));
                        LOGs("CONTINUE"); return CONTINUE; 
                    }
"if"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("if"));
                        LOGs("IF"); return IF; 
                    }
"elif"              { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("elif"));
                        LOGs("ELIF"); return ELIF; 
                    }
"else"              { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("else"));
                        LOGs("ELSE"); return ELSE; 
                    }
"match"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("match"));
                        LOGs("MATCH"); return MATCH; 
                    }
"import"            { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("import"));
                        LOGs("IMPORT"); return IMPORT; 
                    }
"return"            { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("return"));
                        LOGs("RETURN"); return RETURN; 
                    }
"const"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("const"));
                        LOGs("CONST"); return CONST; 
                    }
"class"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("class"));
                        LOGs("CLASS"); return CLASS; 
                    }
"measure"           { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("measure"));
                        LOGs("MEASURE"); return MEASURE_OP; 
                    }
"reset"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("reset"));
                        LOGs("RESET"); return RESET_OP; 
                    }
"gate"              { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("gate"));
                        LOGs("GATE"); return GATE; 
                    }
"apply"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("apply"));
                        LOGs("APPLY"); return APPLY; 
                    }

    /* Reserved Keywords */
"circuit"           { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("circuit"));
                        LOGs("CIRCUIT"); return CIRCUIT; 
                    }
"try"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("try"));
                        LOGs("TRY"); return TRY; 
                    }
"catch"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("catch"));
                        LOGs("CATCH"); return CATCH; 
                    }
"throw"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("throw"));
                        LOGs("THROW"); return THROW; 
                    }

    /* Reserved Function Names */
"print"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("print"));
                        LOGs("PRINT"); return PRINT; 
                    }
"println"           { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("println"));
                        LOGs("PRINTLN"); return PRINTLN; 
                    }
"scan"              { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("scan"));
                        LOGs("SCAN"); return SCAN; 
                    }
"cast"              { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("cast"));
                        LOGs("CAST"); return CAST; 
                    }
"getline"           { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("getline"));
                        LOGs("GETLINE"); return GETLINE; 
                    }

    /* Quantum Gates */
"H"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("H"));
                        LOGs("H"); return GATE_H; 
                    }
"S"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("S"));
                        LOGs("S"); return GATE_S; 
                    }
"T"                 {
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("T"));
                        LOGs("T"); return GATE_T; 
                    }
"CTRL"              {    
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("CTRL"));   
                        LOGs("CTRL"); return GATE_CTRL; 
                    }

    /* Pauli Gates */
"I"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("I"));
                        LOGs("I"); return GATE_I; 
                    }
"X"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("X"));
                        LOGs("X"); return GATE_X; 
                    }
"Y"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("Y"));
                        LOGs("Y"); return GATE_Y; 
                    }
"Z"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("Z"));
                        LOGs("Z"); return GATE_Z; 
                    }

    /* Rotation Gates */
"RX"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("RX"));
                        LOGs("RX"); return GATE_RX; 
                    }
"RY"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("RY"));
                        LOGs("RY"); return GATE_RY; 
                    }
"RZ"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("RZ"));
                        LOGs("RZ"); return GATE_RZ; 
                    }

    /* Controlled Gates */
"CNOT"              { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("CNOT"));
                        LOGs("CNOT"); return GATE_CNOT; 
                    }
"CZ"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("CZ"));
                        LOGs("CZ"); return GATE_CZ; 
                    }
"SWAP"              { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("SWAP"));
                        LOGs("SWAP"); return GATE_SWAP; 
                    }
"CSWAP"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("CSWAP"));
                        LOGs("CSWAP"); return GATE_CSWAP; 
                    }
"CCNOT"             { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("CCNOT"));
                        LOGs("CCNOT"); return GATE_CCNOT; 
                    }

    /* Controlled Rotation Gates */
"CRX"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("CRX"));
                        LOGs("CRX"); return GATE_CRX; 
                    }
"CRY"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("CRY"));
                        LOGs("CRY"); return GATE_CRY; 
                    }
"CRZ"               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("CRZ"));
                        LOGs("CRZ"); return GATE_CRZ; 
                    }

    /* Constants And Identifiers */
{FLOAT}             {   
                        yylval.token = new Token(Location(yylineno, yycolumn), std::stod(std::string(yytext)));
                        LOGs("FLOAT(" << yytext << ")"); return FLOAT_LITERAL; 
                    }
{INTEGER}           {   
                        yylval.token = new Token(Location(yylineno, yycolumn), std::stoll(std::string(yytext)));
                        LOGs("INT(" << yytext << ")"); return INT_LITERAL; 
                    }
{STRING}            {   
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(yytext));
                        LOGs("STRING(" << yytext << ")"); return STRING_LITERAL; 
                    }
{IDENTIFIER}        {   
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(yytext));
                        LOGs("IDENTIFIER(" << yytext << ")"); return IDENTIFIER; 
                    }

    /* Operators */
"+"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("+"));
                        LOGs("+"); return '+'; 
                    }         
"-"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("-"));
                        LOGs("-"); return '-'; 
                    }         
"*"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("*"));
                        LOGs("*"); return '*'; 
                    }         
"/"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("/"));
                        LOGs("/"); return '/'; 
                    }         
"%"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("%"));
                        LOGs("%"); return '%'; 
                    }         
"**"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("**"));
                        LOGs("**"); return EXP; 
                    }
"@"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("@"));
                        LOGs("@"); return '@'; 
                    }
"->"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("->"));
                        LOGs("->"); return SINGLE_ARROW; 
                    }
"=>"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("=>"));
                        LOGs("=>"); return DOUBLE_ARROW; 
                    }
"="                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("="));
                        LOGs("="); return ASSIGN; 
                    }
"+="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("+="));
                        LOGs("+="); return ADD_ASSIGN; 
                    }
"-="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("-="));
                        LOGs("-="); return SUB_ASSIGN; 
                    }
"*="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("*="));
                        LOGs("*="); return MUL_ASSIGN; 
                    }
"/="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("/="));
                        LOGs("/="); return DIV_ASSIGN; 
                    }
"%="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("%="));
                        LOGs("%="); return MOD_ASSIGN; 
                    }
"**="               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("**="));
                        LOGs("**="); return EXP_ASSIGN; 
                    }

    /* Bitwise Operators */
"&"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("&"));
                        LOGs("&"); return '&'; 
                    }
"|"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("|"));
                        LOGs("|"); return '|'; 
                    }
"^"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("^"));
                        LOGs("^"); return '^'; 
                    }
"!"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("!"));
                        LOGs("!"); return '!'; 
                    }
"&="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("&="));
                        LOGs("&="); return AND_ASSIGN; 
                    }
"|="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("|="));
                        LOGs("|="); return OR_ASSIGN; 
                    }
"^="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("^="));
                        LOGs("^="); return XOR_ASSIGN; 
                    }
">>"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(">>"));
                        LOGs(">>"); return RIGHT_SHIFT; 
                    }
"<<"                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("<<"));
                        LOGs("<<"); return LEFT_SHIFT; 
                    }
">>="               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(">>="));
                        LOGs(">>="); return RIGHT_SHIFT_ASSIGN; 
                    }
"<<="               { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("<<="));
                        LOGs("<<="); return LEFT_SHIFT_ASSIGN; 
                    }

    /* Comparison Operators */
"=="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("=="));
                        LOGs("=="); return EQ_OP; 
                    }
"!="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("!="));
                        LOGs("!="); return NE_OP; 
                    }
">="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(">="));
                        LOGs(">="); return GE_OP; 
                    }
"<="                { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("<="));
                        LOGs("<="); return LE_OP; 
                    }
">"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(">"));
                        LOGs(">"); return '>'; 
                    }
"<"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("<"));
                        LOGs("<"); return '<'; 
                    }

    /* Punctuations */
"["                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("["));
                        LOGs("["); return '['; 
                    }
"]"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("]"));
                        LOGs("]"); return ']'; 
                    }
"{"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("{"));
                        LOGs("{"); return '{'; 
                    }
"}"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("}"));
                        LOGs("}"); return '}'; 
                    }
"("                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("("));
                        LOGs("("); return '('; 
                    }
")"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(")"));
                        LOGs(")"); return ')'; 
                    }
";"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(";"));
                        LOGs(";"); return ';'; 
                    }
":"                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(":"));
                        LOGs(":"); return ':'; 
                    }
","                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string(","));
                        LOGs(","); return ','; 
                    }
"."                 { 
                        yylval.token = new Token(Location(yylineno, yycolumn), std::string("."));
                        LOGs("."); return '.'; 
                    }

    /* NEWLINE and WHITESPACE */
{NEWLINE}           { yycolumn = 1; LOG(""); }
[ \t\v\f\r]         { /* do nothing */ }
.                   { LOGs("Error here. Please check."); }
%%