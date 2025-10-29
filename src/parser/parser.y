%define parse.lac full
%define parse.error verbose

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
%token LET CONST APPLY
%token FUNC CLASS CIRCUIT GATE

%token AND OR NOT
%token TRUE FALSE

%token FOR WHILE DO BREAK CONTINUE
%token IF ELIF ELSE MATCH

%token TRY CATCH THROW

%token MEASURE_OP RESET_OP
%token IMPORT RETURN 
%token PRINT PRINTLN SCAN GETLINE CAST

%token GATE_H GATE_S GATE_T GATE_CTRL
%token GATE_I GATE_X GATE_Y GATE_Z GATE_RX GATE_RY GATE_RZ
%token GATE_CNOT GATE_CZ GATE_SWAP GATE_CSWAP GATE_CCNOT 
%token GATE_CRX GATE_CRY GATE_CRZ

%token EXP RETURN_ARROW MEASURE_ARROW 

%token ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN EXP_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN
%token RIGHT_SHIFT LEFT_SHIFT RIGHT_SHIFT_ASSIGN LEFT_SHIFT_ASSIGN
%token EQ_OP NE_OP GE_OP LE_OP 
%token SCOPE


/* operators in reverse precedence order */
%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left EQ_OP NE_OP
%left '<' LE_OP '>' GE_OP
%left LEFT_SHIFT RIGHT_SHIFT
%left '+' '-'
%left '*' '/' '%'
%right UMINUS
%right EXP
%left '.'
%left SCOPE

%start translation_unit

%%
translation_unit
    :   translation_unit external_declaration
    |   external_declaration
    ;

external_declaration
    :   statement_line
    |   statement_line statement
    |   declaration
    ;

declaration
    :   import_declaration
    |   function_declaration
    |   gate_declaration
    ;

gate_declaration
    :   GATE '{' APPLY ':' gate_composition '}'
    ;

gate_definition
    :   GATE
    ;

import_declaration
    :   IMPORT scoped_identifier
    ;

scoped_identifier
    :   scoped_identifier SCOPE IDENTIFIER
    |   IDENTIFIER
    ;

function_declaration
    :   function_header compound_statement
    |   function_header return_type compound_statement
    ;

function_header
    :   FUNC IDENTIFIER '(' parameter_list ')'
    |   FUNC IDENTIFIER '(' ')'
    ;

return_type
    :   RETURN_ARROW type
    ;

parameter_list
    :   parameter 
    |   parameter_list ',' parameter
    ;

parameter
    : IDENTIFIER ':' type
    ;

statement_list 
    :   statement_list statement_line
    |   statement_list statement_line statement
    |   statement_line
    ;

statement_line
    : statement EOL
    | EOL 
    ;

statement
    :   declaration_statement
    |   expression_statement
    |   assignment_statement   
    |   quantum_statement
    |   compound_statement
    |   labeled_statement
    |   iteration_statement
    |   jump_statement
    |   print_statement
    ;

expression_statement    
    :   expression
    ;

iteration_statement
    :   
    ;

compound_statement
    : '{' statement_list '}'
    ;

jump_statement
    :   CONTINUE ';'
    |   BREAK ';'
    |   RETURN ';'
    |   RETURN expression ';'
    ;

labeled_statement 
    :   IDENTIFIER ':' statement
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

type_qualifier
    :   CONST
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
    :   '='
    |   ADD_ASSIGN 
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
    ;

expression
    :   expression '+' expression
    |   expression '|' expression
    |   expression '-' expression
    |   expression '*' expression
    |   expression '&' expression
    |   expression '/' expression
    |   expression '%' expression
    |   expression EXP expression
    |   '+' expression %prec UMINUS
    |   '-' expression %prec UMINUS
    |   '!' expression %prec UMINUS
    |   expression '^' expression 
    |   '(' expression ')'
    |   postfix_expression 
    ;

postfix_expression
    :   primary_expression
    |   postfix_expression '[' expression ']'
    |   postfix_expression '(' expression ')'
    |   postfix_expression '(' ')'
    |   postfix_expression '.' IDENTIFIER
    |   CAST '<' type_name '>' '(' expression ')'
    ;

primary_expression
    :   INT_CONSTANT
    |   FLOAT_CONSTANT
    |   IDENTIFIER
    ;

print_statement
    :   PRINT '('  ')'
    |   PRINTLN '(' ')'
    |   SCAN '(' ')'
    ;

quantum_statement
    :   apply_gate_statement
    |   measure_statement
    |   reset_statement
    ;

measure_statement
    :   MEASURE_OP state MEASURE_ARROW state
    ;

reset_statement
    :   RESET_OP state
    ;   

apply_gate_statement
    :   gate_composition '@' state
    ;

gate_composition
    :   gate_composition '@' quantum_gate
    |   quantum_gate
    ;

boolean_literal
    :   TRUE
    |   FALSE
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
