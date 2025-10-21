%{
    #include <macros.hpp>

    int yylex ();
    void yyerror (const std::string &);
%}

%code requires {
    #include <macros.hpp>
}

%union {
    long long ival;
    double fval;
    char *sval;

}

%token EOL 
%token<sval> IDENTIFIER 
%token<fval> FLOAT_CONSTANT 
%token<ival> INT_CONSTANT 
%token<sval> STRING_CONSTANT

%token QUBIT BIT INT FLOAT STRING BOOL
%token FUNC LET GATE APPLY

%token AND OR NOT
%token TRUE FALSE

%token FOR WHILE DO BREAK CONTINUE
%token IF ELIF ELSE MATCH

%token MEASURE RESET
%token IMPORT RETURN 
%token PRINT PRINTLN SCAN GETLINE

%token GATE_H GATE_S GATE_T GATE_CTRL
%token GATE_I GATE_X GATE_Y GATE_Z GATE_RX GATE_RY GATE_RZ
%token GATE_CNOT GATE_CZ GATE_SWAP GATE_CSWAP GATE_CCNOT 
%token GATE_CRX GATE_CRY GATE_CRZ

%token EXP RETURN_ARROW MEASURE_ARROW 

%token ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN EXP_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN
%token RIGHT_SHIFT LEFT_SHIFT RIGHT_SHIFT_ASSIGN LEFT_SHIFT_ASSIGN
%token EQ_OP NE_OP GE_OP LE_OP 
%token SCOPE

%token CONST CLASS CIRCUIT

%left '+' '-'
%left '*' '/' '%'
%right EXP

%start translation_unit

%%
translation_unit
    :   statement_list
    |   func_definition
    |   class_definition
    |   %empty
    ;
class_definition
    :   CLASS IDENTIFIER '{' statement_list'}'
    ; 
compound_statement
    : '{' statement_list '}'

statement_list
    :   %empty
    |   statement_list EOL statement
    |   statement_list EOL 
    |   statement
    ;

statement
    :   declaration_statement
    |   assignment_statement   
    |   print_statement
    |   quantum_statement
    |   compound_statement
    |   condtional_statement
    |   Iteration_statement
    |   labeled_statement
    |   jump_statement
    |   expression_statement
    ;

condtional_statement
    :   IF '(' expression ')' statement
    |   IF '(' expression ')' statement ELSE statement
    |   IF '(' expression ')' statement  ELIF statement
    |   MATCH '(' expression ')' statement
    ;

expression_statement
    :   ';'
    |   expression
    ;

Iteration_statement
    :   WHILE '(' expression ')' statement
    |   DO statement WHILE '(' expression ')' ';'
    |   FOR '(' expression_statement expression_statement ')' statement
    |   FOR '(' expression_statement expression_statement expression ')' statement
    ;

jump_statement
    :   CONTINUE ';'
    |   BREAK    ';'
    |   RETURN   ';'
    ;

labeled_statement 
    :   INT_CONSTANT ':' statement
    ;

declaration_statement
    :   LET identifier_list ':' type 
    |   LET identifier_list ':' type '=' expression
    |   LET identifier_list '=' expression 
    ;

identifier_list
    :   identifier_list ',' IDENTIFIER
    |   IDENTIFIER
    ;

type
    :   type_name array_list
    |   type_name
    ;

array_list
    :   array_list '[' expression ']'
    |   '[' expression ']'
    ;

type_name
    :   QUBIT 
    |   BIT
    |   BOOL
    |   INT 
    |   FLOAT
    |   STRING
    ;

assignment_statement
    :   IDENTIFIER assignment_operator expression
    ;

assignment_operator
    :   ADD_ASSIGN 
    |   SUB_ASSIGN
    |   MUL_ASSIGN 
    |   DIV_ASSIGN
    |   MOD_ASSIGN
    |   EXP_ASSIGN  
    |   AND_ASSIGN 
    |   OR_ASSIGN 
    |   XOR_ASSIGN
    |   RIGHT_SHIFT_ASSIGN
    |   LEFT_SHIFT_ASSIGN
    |   '='
    ;

expression
    :   expression bin_op expression
    |   IDENTIFIER
    |   constant
    ;

constant
    :   INT_CONSTANT
    |   FLOAT_CONSTANT
    ;

bin_op
    :   '+'
    |   '-'
    |   '*'
    |   '/'
    |   '%'
    |   EXP
    ;

print_statement
    :   PRINT '('  ')'
    |   PRINTLN '(' ')'
    |   SCAN '(' ')'
    ;

func_declaration
    :   FUNC IDENTIFIER '(' parameter_list ')'  optional_return_type
    |   FUNC IDENTIFIER '(' ')'  optional_return_type
    |   FUNC IDENTIFIER SCOPE '(' parameter_list ')' optional_return_type
    |   FUNC IDENTIFIER SCOPE '(' ')' optional_return_type
    ;

optional_return_type
    :   RETURN_ARROW type
    |
    ;
func_definition
    :   func_declaration '{' compound_statement '}'
    ;

parameter_list
    :   parameter 
    |   parameter_list ',' parameter
    ;

parameter
    : IDENTIFIER ':' type
    ;

quantum_statement
    :   apply_gate_statement
    |   measure_statement
    |   reset_statement
    ;

measure_statement
    :   MEASURE state MEASURE_ARROW state
    ;

reset_statement
    :   RESET state
    ;   

apply_gate_statement
    :   gate_composition '@' state
    ;

gate_composition
    :   gate_composition '@' quantum_gate
    |   quantum_gate
    ;

state
    :   '[' state_list ']'
    |   IDENTIFIER '[' expression ']'
    |   IDENTIFIER
    ;

state_list
    :   state_list ','  state
    |   state
    ;

quantum_gate
    :   '[' quantum_gate_list ']'
    |   simple_gate '(' expression ')'
    |   simple_gate
    ;

quantum_gate_list
    :   quantum_gate_list ','  quantum_gate
    |   quantum_gate
    ;

simple_gate
    :   GATE_H 
    |   GATE_S
    |   GATE_T 
    |   GATE_CTRL 
    |   GATE_I 
    |   GATE_X
    |   GATE_Y
    |   GATE_Z 
    |   GATE_RX 
    |   GATE_RY 
    |   GATE_RZ
    |   GATE_CNOT
    |   GATE_CZ 
    |   GATE_SWAP 
    |   GATE_CSWAP
    |   GATE_CCNOT 
    |   GATE_CRX 
    |   GATE_CRY 
    |   GATE_CRZ
    ;
%%

void yyerror (const std::string & e) {
    std::cout << "PARSE ERROR: " << e << std::endl;
}

int main () {
    yyparse ();
    return 0;
}
