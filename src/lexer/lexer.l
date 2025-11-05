%option noyywrap

%{
    #include <parser.tab.hpp> 
    #include <macros.hpp>
    #include<token.hpp>
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
"#"[^\n]*                                       { /* ignore single line comments */ }
"'''"([^']|('[^']|''[^']))*"'''"                { /* ignore multi line comments */ }
"\"\"\""([^\"]|(\"[^\"]|\"\"[^\"]))*"\"\"\""    { /* ignore multi line comments */ }
    /* Types */
"qubit"             { LOGs ("QUBIT"); return QUBIT; }
"bit"               { LOGs ("BIT"); return BIT; }
"int"               { LOGs ("INT"); return INT; }
"float"             { LOGs ("FLOAT"); return FLOAT; }
"string"            { LOGs ("STRING"); return STRING; }
"bool"              { LOGs ("BOOL"); return BOOL; }
    /* Keywords */
"func"              { LOGs ("FUNC"); return FUNC; }
"let"               { LOGs ("LET"); return LET; }
"and"               { LOGs ("AND"); return AND; }
"or"                { LOGs ("OR"); return OR; }
"not"               { LOGs ("NOT"); return NOT; }
"true"              { LOGs ("TRUE"); return TRUE; }
"false"             { LOGs ("FALSE"); return FALSE; }
"for"               { LOGs ("FOR"); return FOR; }
"while"             { LOGs ("WHILE"); return WHILE; }
"do"                { LOGs ("DO");  return DO; }
"break"             { LOGs ("BREAK"); return BREAK; }
"continue"          { LOGs ("CONTINUE"); return CONTINUE; }
"if"                { LOGs ("IF"); return IF; }
"elif"              { LOGs ("ELIF"); return ELIF; }
"else"              { LOGs ("ELSE"); return ELSE; }
"match"             { LOGs ("MATCH"); return MATCH; }
"import"            { LOGs ("IMPORT"); return IMPORT; }
"return"            { LOGs ("RETURN"); return RETURN; }
"const"             { LOGs ("CONST"); return CONST; }
"class"             { LOGs ("CLASS"); return CLASS; }
"measure"           { LOGs ("MEASURE"); return MEASURE_OP; }
"reset"             { LOGs ("RESET"); return RESET_OP; }
"gate"              { LOGs ("GATE"); return GATE; }
"apply"             { LOGs ("APPLY"); return APPLY; }
    /* Reserved Keywords */
"circuit"           { LOGs ("CIRCUIT"); return CIRCUIT; }
"try"               { LOGs ("TRY"); return TRY; }
"catch"             { LOGs ("CATCH"); return CATCH; }
"throw"             { LOGs ("THROW"); return THROW; }
    /* Reserved Function Names */
"print"             { LOGs ("PRINT"); return PRINT; }
"println"           { LOGs ("PRINTLN"); return PRINTLN; }
"scan"              { LOGs ("SCAN"); return SCAN; }
"cast"              { LOGs ("CAST"); return CAST; }
"getline"           { LOGs ("GETLINE"); return GETLINE; }
    /* "main"              { LOG ("MAIN"); return MAIN; } */
    /* Quantum Gates */
"H"                 { LOGs ("H");  return GATE_H;  }
"S"                 { LOGs ("S");  return GATE_S;  }
"T"                 { LOGs ("T");  return GATE_T;  }
"CTRL"              { LOGs ("CTRL");  return GATE_CTRL;  }
    /* Pauli Gates */
"I"                 { LOGs ("I"); return GATE_I; }
"X"                 { LOGs ("X"); return GATE_X; }
"Y"                 { LOGs ("Y"); return GATE_Y; }
"Z"                 { LOGs ("Z"); return GATE_Z; }
    /* Rotation Gates */
"RX"                { LOGs ("RX"); return GATE_RX; }
"RY"                { LOGs ("RY"); return GATE_RY; }
"RZ"                { LOGs ("RZ"); return GATE_RZ; }
    /* Controlled Gates - Swap, Toffoli, CNOT */
"CNOT"              { LOGs ("CNOT"); return GATE_CNOT; }
"CZ"                { LOGs ("CZ"); return GATE_CZ; }
"SWAP"              { LOGs ("SWAP"); return GATE_SWAP; }
"CSWAP"             { LOGs ("CSWAP"); return GATE_CSWAP; }
"CCNOT"             { LOGs ("CCNOT"); return GATE_CCNOT; }
    /* Controlled Rotation Gates */
"CRX"               { LOGs ("CRX"); return GATE_CRX; }
"CRY"               { LOGs ("CRY"); return GATE_CRY; }
"CRZ"               { LOGs ("CRZ"); return GATE_CRZ; }
    /* Constants And Identifiers */
{FLOAT}             { LOGs ("FLOAT(" << yytext << ")"); return FLOAT_LITERAL; }
{INTEGER}           { LOGs ("INT(" << yytext << ")"); return INT_LITERAL; }
{STRING}            {   yylval.sval = yytext;  LOGs ("STRING(" << yytext << ")"); return STRING_LITERAL; }
{IDENTIFIER}        {   yylval.sval = yytext; LOGs ("IDENTIFIER(" << yytext << ")"); return IDENTIFIER; 
                    }
    /* Operators */
    /* Binary Operators */
"+"                 { LOGs ("+"); return '+'; }         
"-"                 { LOGs ("-"); return '-'; }         
"*"                 { LOGs ("*"); return '*'; }         
"/"                 { LOGs ("/"); return '/'; }         
"%"                 { LOGs ("%"); return '%'; }         
"**"                { LOGs ("**"); return EXP; }
"@"                 { LOGs ("@"); return '@'; }
"->"                { LOGs ("->"); return SINGLE_ARROW; }
"=>"                { LOGs ("=>"); return DOUBLE_ARROW; }
"="                 { LOGs ("="); return '='; }
"+="                { LOGs ("+="); return ADD_ASSIGN; }
"-="                { LOGs ("-="); return SUB_ASSIGN; }
"*="                { LOGs ("*="); return MUL_ASSIGN; }
"/="                { LOGs ("/="); return DIV_ASSIGN; }
"%="                { LOGs ("%="); return MOD_ASSIGN; }
"**="               { LOGs ("**="); return EXP_ASSIGN; }
    /* Bitwise Operators */
"&"                 { LOGs ("&"); return '&'; }
"|"                 { LOGs ("|"); return '|'; }
"^"                 { LOGs ("^"); return '^'; }
"!"                 { LOGs ("!"); return '!'; }
"&="                { LOGs ("&="); return AND_ASSIGN;  }
"|="                { LOGs ("|="); return OR_ASSIGN;  }
"^="                { LOGs ("^="); return XOR_ASSIGN;  }
">>"                { LOGs (">>"); return RIGHT_SHIFT;  }
"<<"                { LOGs ("<<"); return LEFT_SHIFT;  }
">>="               { LOGs (">>="); return RIGHT_SHIFT_ASSIGN;  }
"<<="               { LOGs ("<<="); return LEFT_SHIFT_ASSIGN;  }
    /* Comparison Operators */
"=="                { LOGs ("=="); return EQ_OP; }
"!="                { LOGs ("!="); return NE_OP; }
">="                { LOGs (">="); return GE_OP; }
"<="                { LOGs ("<="); return LE_OP; }
">"                 { LOGs (">"); return '>'; }
"<"                 { LOGs ("<"); return '<'; }
    /* Punctuations */
"["                 { LOGs ("["); return '['; }
"]"                 { LOGs ("]"); return ']'; }
"{"                 { LOGs ("{"); return '{'; }
"}"                 { LOGs ("}"); return '}'; }
"("                 { LOGs ("("); return '('; }
")"                 { LOGs (")"); return ')'; }
";"                 { LOGs (";"); return ';'; }
":"                 { LOGs (":"); return ':'; }
","                 { LOGs (","); return ','; }
"."                 { LOGs ("."); return '.'; }
    /* NEWLINE and WHITESPACE */
{NEWLINE}           { LOG (""); }
[ \t\v\f\r]         { /* do nothing */ }
.                   { LOGs ("Error here. Please check."); }
%%
