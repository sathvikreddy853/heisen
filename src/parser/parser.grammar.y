%define parse.error verbose
%debug

%{
    #include <parser.tab.hpp>
    #include <macros.hpp>
    #include <token.hpp>

    int yylex();
    void yyerror (const std::string &);
%}

%union {
    Heisen::Token* token;
    long long ival;
    double fval;
    char *sval;
}

%code requires {
    #include <macros.hpp>
    #include <token.hpp>
}

%token<sval> IDENTIFIER 
%token<fval> FLOAT_LITERAL 
%token<ival> INT_LITERAL 
%token<sval> STRING_LITERAL

%token<token> QUBIT BIT INT FLOAT STRING BOOL STRUCT
%token<token> LET CONST APPLY
%token<token> FUNC GATE 

%token<token> AND OR NOT
%token<token> TRUE FALSE

%token<token> FOR WHILE DO BREAK CONTINUE MATCH
%token<token> IF ELIF ELSE 
%nonassoc LOWER_THAN_ELSE 

%token<token> MEASURE_OP RESET_OP
%token<token> RETURN 
%token<token> PRINT PRINTLN SCAN  CAST

%token<token> GATE_H GATE_S GATE_T GATE_CTRL
%token<token> GATE_I GATE_X GATE_Y GATE_Z GATE_RX GATE_RY GATE_RZ
%token<token> GATE_CNOT GATE_CZ GATE_SWAP GATE_CSWAP GATE_CCNOT 
%token<token> GATE_CRX GATE_CRY GATE_CRZ

%token<token> EXP  DOUBLE_ARROW 

%token<token> ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN EXP_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN
%token<token> RIGHT_SHIFT LEFT_SHIFT RIGHT_SHIFT_ASSIGN LEFT_SHIFT_ASSIGN
%token<token> EQ_OP NE_OP GE_OP LE_OP 

/* Unused Tokens */
%token<token> IMPORT SINGLE_ARROW GETLINE
%token<token> TRY CATCH THROW CLASS CIRCUIT 

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
%right UNARY
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
    :   statement
    |   declaration
    ;

declaration
    :   function_declaration
    |   gate_declaration   
    ;

gate_declaration
    :   GATE '{' APPLY ':' gate_composition '}'
    ;

function_declaration
    :   FUNC function_header compound_statement
    |   FUNC function_header return_type compound_statement
    ;

function_header
    :   IDENTIFIER '(' parameter_list ')'
    |   IDENTIFIER '(' ')'
    ;

return_type
    :   ':' type
    ;

parameter_list
    :   parameter 
    |   parameter_list ',' parameter
    ;

parameter
    :   IDENTIFIER ':' type
    ;

statement_list 
    :   statement_list statement
    |   statement
    ;

statement
    :   declaration_statement ';'
    |   expression_statement ';'
    |   assignment_statement ';'
    |   quantum_statement ';'
    |   jump_statement ';'
    |   print_statement ';'
    |   compound_statement
    |   selection_statement
    |   iteration_statement
    ;

selection_statement
    :   IF '(' condition ')' compound_statement                        
    |   IF '(' condition ')' compound_statement ELSE compound_statement
    |   IF '(' condition ')' compound_statement elif_clauses           
    |   MATCH '(' expression ')' '{' match_list '}'
    ;

elif_clauses
    :   ELIF '(' condition ')' compound_statement
    |   ELIF '(' condition ')' compound_statement ELSE compound_statement
    |   ELIF '(' condition ')' compound_statement elif_clauses
    ;


compound_statement
    :   '{' statement_list '}'
    ;

expression_statement    
    :   expression
    ;

condition   
    :   expression '>' expression
    |   expression '<' expression
    |   expression GE_OP expression
    |   expression LE_OP expression
    |   expression EQ_OP expression
    |   expression NE_OP expression
    |   condition AND condition
    |   condition OR condition
    |   NOT condition %prec UNARY
    |   '(' condition ')'
    |   expression
    ;

match_list
    :   %empty
    |   match_list match_statement
    |   match_statement
    ;

match_statement
    :   expression DOUBLE_ARROW compound_statement
    ;

optional_condition
    :   %empty
    |   condition
    ;

iteration_statement
    :   WHILE '(' condition ')' compound_statement
    |   DO compound_statement WHILE '(' condition ')'
    |   FOR '(' variable_declaration_list ';' optional_condition ';' optional_assignment_statement ')' compound_statement
    ;

optional_assignment_statement
    :   %empty
    |   assignment_statement
    ;

jump_statement
    :   CONTINUE
    |   BREAK
    |   RETURN
    |   RETURN expression  
    ;

declaration_statement
    :   LET variable_declaration_list     
    ;

variable_declaration_list
    :   variable_declaration_list ',' variable_declaration
    |   variable_declaration
    ;

variable_declaration
    :   IDENTIFIER ':' type 
    |   IDENTIFIER ':' type '=' expression
    |   IDENTIFIER ':' type '=' braced_init_list
    |   IDENTIFIER ':' type '=' quantum_state
    ;

type
    :   type_name array_list
    |   type_name
    |   '(' function_object ')'
    ;

type_list
    :   type 
    |   type_list ',' type
    ;

function_object
    :   '(' type_list ')' DOUBLE_ARROW '(' ')'    
    |   '(' type_list ')' DOUBLE_ARROW type   
    |   type DOUBLE_ARROW '(' ')'
    |   type DOUBLE_ARROW type
    ;

array_list
    :   array_list '[' index_expression ']'
    |   '[' index_expression ']'
    |   '[' ']'   
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
    :   expression assignment_operator expression
    |   expression assignment_operator braced_init_list
    |   expression assignment_operator quantum_state
    ;

braced_init_list
    : '[' expression_list ']'
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
    |   expression RIGHT_SHIFT expression 
    |   expression LEFT_SHIFT expression
    |   expression EXP expression
    |   expression '^' expression 
    |   '+' expression %prec UNARY
    |   '-' expression %prec UNARY
    |   '!' expression %prec UNARY
    |   postfix_expression
    ;
    

postfix_expression
    :   primary_expression
    |   postfix_expression '[' index_expression ']'
    |   postfix_expression '(' expression_list ')'
    |   postfix_expression '(' ')'
    |   postfix_expression '.' IDENTIFIER
    |   CAST '<' type_name '>' '(' expression ')'
    ;

 expression_list
    :   expression_list ',' expression
    |   expression
    ;

primary_expression
    :   lambda_expression
    |   INT_LITERAL
    |   FLOAT_LITERAL
    |   STRING_LITERAL
    |   IDENTIFIER 
    |   boolean_literal 
    ;

lambda_expression
    :   '(' parameter_list ')' compound_statement
    |   '(' parameter_list ')' ':' type compound_statement
    ;

boolean_literal
    :   TRUE    
    |   FALSE 
    ;

print_statement
    :   PRINT '(' print_string ')'
    |   PRINTLN '(' print_string ')'
    |   SCAN '(' print_string ')'
    ;

print_string
    :   %empty
    |   STRING_LITERAL
    ;

quantum_statement
    :   apply_gate_statement
    |   measure_statement
    |   reset_statement
    ;

measure_statement
    :   MEASURE_OP quantum_state DOUBLE_ARROW quantum_state
    ;

reset_statement
    :   RESET_OP quantum_state
    ;   

apply_gate_statement
    :   gate_composition '@' expression
    ;   

gate_composition
    :   gate_composition '@' quantum_gate
    |   quantum_gate 
    ;

quantum_state
    :   '[' quantum_state_list ']'
    |   postfix_expression
    ;

quantum_state_list
    :   quantum_state_list ',' quantum_state
    |   quantum_state
    ;

    /* |   IDENTIFIER '[' index_expression ']'     
    |   IDENTIFIER */

quantum_gate
    :   '[' quantum_gate_list ']' 
    |   simple_gate '(' expression ')' 
    |   simple_gate  
    ;

quantum_gate_list
    :   quantum_gate_list ',' quantum_gate
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

optional_expression
    :   %empty
    |   expression
    ;

index_expression
    :   expression
    |   optional_expression ':' optional_expression
    |   optional_expression ':' optional_expression ':' optional_expression
    ;
%%

void yyerror(const std::string &msg) {
    std::cerr << "Parse Error: " << msg << std::endl;
}

int main() {
    yydebug = 0;
    yyparse();
    return 0;
}