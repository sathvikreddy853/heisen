%option noyywrap

%{
    #include <iostream>
    #include <cstdlib>
    #include <cstring>
    #include <fstream>
    #include <parser.tab.hpp> 
    #define LOG(x)  std::cout << x << ' ';
    #define LOGN(x) std::cout << x << std::endl;
%}

L   [a-zA-Z_]
D   [0-9]

IDENTIFIER  {L}({L}|{D})*
INTEGER     {D}+
FLOAT       ({D}+\.{D}*|\.{D}+)([eE][+-]?{D}+)?
STRING      (\"([^\"\\]|\\.)*\"|'([^'\\]|\\.)*')
NEWLINE     "\n"

%%
    /* Comments */
"#"[^\n]*                           { /* ignore single line comments */ }
"'''"([^']|('[^']|''[^']))*"'''"    { /* ignore multi line comments*/ }
    /* Types */
"qubit"             { LOG ("QUBIT"); return QUBIT; }
"bit"               { LOG ("BIT"); return BIT; }
"int"               { LOG ("INT"); return INT; }
"float"             { LOG ("FLOAT"); return FLOAT; }
"string"            { LOG ("STRING"); return STRING; }
"bool"              { LOG ("BOOL"); return BOOL; }
    /* Keywords */
"func"              { LOG ("FUNC"); return FUNC; }
"let"               { LOG ("LET"); return LET; }
"and"               { LOG ("AND"); return AND; }
"or"                { LOG ("OR"); return OR; }
"not"               { LOG ("NOT"); return NOT; }
"true"              { LOG ("TRUE"); return TRUE; }
"false"             { LOG ("FALSE"); return FALSE; }
"for"               { LOG ("FOR"); return FOR; }
"while"             { LOG ("WHILE"); return WHILE; }
"do"                { LOG ("DO");  return DO; }
"break"             { LOG ("BREAK"); return BREAK; }
"continue"          { LOG ("CONTINUE"); return CONTINUE; }
"if"                { LOG ("IF"); return IF; }
"elif"              { LOG ("ELIF"); return ELIF; }
"else"              { LOG ("ELSE"); return ELSE; }
"match"             { LOG ("MATCH"); return MATCH; }
"import"            { LOG ("IMPORT"); return IMPORT; }
"return"            { LOG ("RETURN"); return RETURN; }
"const"             { LOG ("CONST"); return CONST; }
"class"             { LOG ("CLASS"); return CLASS; }
"measure"           { LOG ("MEASURE"); return MEASURE; }
"reset"             { LOG ("RESET"); return RESET; }
"gate"              { LOG ("GATE"); return GATE; }
"apply"             { LOG ("APPLY"); return APPLY; }
    /* Reserved Keywords */
"circuit"           { LOG ("CIRCUIT"); return CIRCUIT; }
    /* Reserved Function Names */
"print"             { LOG ("PRINT"); return PRINT; }
"println"           { LOG ("PRINTLN"); return PRINTLN; }
"scan"              { LOG ("SCAN"); return SCAN; }
"getline"           { LOG ("GETLINE"); return GETLINE; }
    /* "main"              { LOG ("MAIN"); return MAIN; } */
    /* Quantum Gates */
"H"                 { LOG ("H");  return GATE_H;  }
"S"                 { LOG ("S");  return GATE_S;  }
"T"                 { LOG ("T");  return GATE_T;  }
"CTRL"              { LOG ("CTRL");  return GATE_CTRL;  }
    /* Pauli Gates */
"I"                 { LOG ("I"); return GATE_I; }
"X"                 { LOG ("X"); return GATE_X; }
"Y"                 { LOG ("Y"); return GATE_Y; }
"Z"                 { LOG ("Z"); return GATE_Z; }
    /* Rotation Gates */
"RX"                { LOG ("RX"); return GATE_RX; }
"RY"                { LOG ("RY"); return GATE_RY; }
"RZ"                { LOG ("RZ"); return GATE_RZ; }
    /* Controlled Gates - Swap, Toffoli, CNOT */
"CNOT"              { LOG ("CNOT"); return GATE_CNOT; }
"CZ"                { LOG ("CZ"); return GATE_CZ; }
"SWAP"              { LOG ("SWAP"); return GATE_SWAP; }
"CSWAP"             { LOG ("CSWAP"); return GATE_CSWAP; }
"CCNOT"             { LOG ("CCNOT"); return GATE_CCNOT; }
    /* Controlled Rotation Gates */
"CRX"               { LOG ("CRX"); return GATE_CRX; }
"CRY"               { LOG ("CRY"); return GATE_CRY; }
"CRZ"               { LOG ("CRZ"); return GATE_CRZ; }
    /* Constants And Identifiers */
{FLOAT}             { LOG ("FLOAT(" << yytext << ")"); return FLOAT_CONSTANT; }
{INTEGER}           { LOG ("INT(" << yytext << ")"); return INT_CONSTANT; }
{STRING}            { LOG ("STRING(" << yytext << ")"); return STRING_CONSTANT; }
{IDENTIFIER}        { LOG ("IDENTIFIER(" << yytext << ")"); return IDENTIFIER; }
    /* Operators */
    /* Binary Operators */
"+"                 { LOG ("+"); return '+'; }         
"-"                 { LOG ("-"); return '-'; }         
"*"                 { LOG ("*"); return '*'; }         
"/"                 { LOG ("/"); return '/'; }         
"%"                 { LOG ("%"); return '%'; }         
"**"                { LOG ("**"); return EXP; }
"@"                 { LOG ("@"); return '@'; }
"->"                { LOG ("RETURN_ARROW"); return RETURN_ARROW; }
"=>"                { LOG ("MEASURE_ARROW"); return MEASURE_ARROW; }
"="                 { LOG ("="); return '='; }
"+="                { LOG ("ADD_ASSIGN"); return ADD_ASSIGN; }
"-="                { LOG ("SUB_ASSIGN"); return SUB_ASSIGN; }
"*="                { LOG ("MUL_ASSIGN"); return MUL_ASSIGN; }
"/="                { LOG ("DIV_ASSIGN"); return DIV_ASSIGN; }
"%="                { LOG ("MOD_ASSIGN"); return MOD_ASSIGN; }
"**="               { LOG ("EXP_ASSIGN"); return EXP_ASSIGN; }
    /* Bitwise Operators */
"&"                 { LOG ("&"); return '&'; }
"|"                 { LOG ("|"); return '|'; }
"^"                 { LOG ("^"); return '^'; }
"&="                { LOG ("&="); return AND_ASSIGN;  }
"|="                { LOG ("|="); return OR_ASSIGN;  }
"^="                { LOG ("^="); return XOR_ASSIGN;  }
">>"                { LOG ("RIGHT_SHIFT"); return RIGHT_SHIFT;  }
"<<"                { LOG ("LEFT_SHIFT"); return LEFT_SHIFT;  }
">>="               { LOG ("RIGHT_SHIFT_ASSIGN"); return RIGHT_SHIFT_ASSIGN;  }
"<<="               { LOG ("LEFT_SHIFT_ASSIGN"); return LEFT_SHIFT_ASSIGN;  }
    /* Comparison Operators */
"=="                { LOG ("=="); return EQ_OP; }
"!="                { LOG ("!="); return NE_OP; }
">="                { LOG (">="); return GE_OP; }
"<="                { LOG ("<="); return LE_OP; }
">"                 { LOG (">"); return '>'; }
"<"                 { LOG ("<"); return '<'; }
    /* Punctuations */
"::"                { LOG ("SCOPE"); return SCOPE; }
"["                 { LOG ("["); return '['; }
"]"                 { LOG ("]"); return ']'; }
"{"                 { LOG ("{"); return '{'; }
"}"                 { LOG ("}"); return '}'; }
"("                 { LOG ("("); return '('; }
")"                 { LOG (")"); return ')'; }
";"                 { LOG (";"); return ';'; }
":"                 { LOG (":"); return ':'; }
","                 { LOG (","); return ','; }
"."                 { LOG ("."); return '.'; }
    /* NEWLINE and WHITESPACE */
{NEWLINE}           { LOGN (""); return EOL; }
[ \t\v\f\r]         { /* do nothing */ }
.                   { LOG ("Error here. Please check."); }
%%
