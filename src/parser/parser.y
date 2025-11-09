%define parse.error verbose
%debug

%{
    #include <parser.tab.hpp>
    #include <macros.hpp>
    #include <token.hpp>
    #include <ast.hpp>

    int yylex();
    void yyerror(const std::string &);
    
    // Root of the AST
    std::vector<ASTNode*> translationUnit;
%}

%union {
    Heisen::Token* token;

    ASTNode* node;
    Expr* exp;
    LiteralExpr* literalExp;
    Stmt* stmt;
    Decl* decl;
    Type* type;
    
    GateDecl* gateDecl;
    GateNode* gateNode;
    QuantumStmt* qstmt;

    SimpleGateNode* simpgateNode;
    
    CompoundStmt* compoundStmt;
    DeclarationStmt* declStmt;
    VariableDecl* varDecl;
    ParameterDecl* paramDecl;
    FunctionDecl* funcDecl;
    MatchCase* matchCase;
    ApplyGateStmt* applyStmt;
    MeasureStmt* measureStmt;
    ResetStmt* resetStmt;
    
    std::vector<Stmt*>* stmtList;
    std::vector<Expr*>* expList;
    std::vector<GateNode*>* gList;
    std::vector<VariableDecl*>* varDeclList;
    std::vector<ParameterDecl*>* paramDeclList;
    std::vector<MatchCase*>* matchCaseList;
    std::vector<Type*>* typeList;
}

%code requires {
    #include <macros.hpp>
    #include <token.hpp>
    #include <ast.hpp>
}

%token<token> IDENTIFIER 
%token<token> FLOAT_LITERAL 
%token<token> INT_LITERAL 
%token<token> STRING_LITERAL

%token<token> QUBIT BIT INT FLOAT STRING BOOL STRUCT
%token<token> LET CONST APPLY
%token<token> FUNC GATE 

%token<token> AND OR NOT
%token<token> TRUE FALSE

%token<token> FOR WHILE DO BREAK CONTINUE MATCH
%token<token> IF ELIF ELSE 

%token<token> MEASURE_OP RESET_OP
%token<token> RETURN 
%token<token> PRINT PRINTLN SCAN CAST

%token<token> GATE_H GATE_S GATE_T GATE_CTRL
%token<token> GATE_I GATE_X GATE_Y GATE_Z GATE_RX GATE_RY GATE_RZ
%token<token> GATE_CNOT GATE_CZ GATE_SWAP GATE_CSWAP GATE_CCNOT 
%token<token> GATE_CRX GATE_CRY GATE_CRZ

%token<token> EXP DOUBLE_ARROW 

%token<token> EQUAL_TO
%token<token> ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN EXP_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN
%token<token> RIGHT_SHIFT LEFT_SHIFT RIGHT_SHIFT_ASSIGN LEFT_SHIFT_ASSIGN
%token<token> EQ_OP NE_OP GE_OP LE_OP 

%token<token> IMPORT SINGLE_ARROW GETLINE
%token<token> TRY CATCH THROW CLASS CIRCUIT 

%type<decl>declaration
%type<token> assignment_operator print_string 
%type<literalExp> boolean_literal
%type<gateNode>  quantum_gate gate_composition 
%type<simpgateNode> simple_gate
%type<stmt> jump_statement print_statement selection_statement iteration_statement assignment_statement statement
%type<compoundStmt> compound_statement
%type<declStmt> declaration_statement
%type<varDecl> variable_declaration
%type<varDeclList> variable_declaration_list
%type<paramDecl> parameter
%type<paramDeclList> parameter_list
%type<funcDecl> function_declaration function_header
%type<gateDecl> gate_declaration
%type<type> type type_name return_type function_object
%type<typeList> type_list 
%type<expList>array_list
%type<matchCase> match_statement
%type<matchCaseList> match_list
%type<stmtList> statement_list elif_clauses
%type<qstmt> quantum_statement
%type<applyStmt> apply_gate_statement
%type<measureStmt> measure_statement
%type<resetStmt> reset_statement
%type<exp> expression postfix_expression primary_expression condition optional_condition
%type<exp> expression_statement index_expression optional_expression
%type<exp> quantum_state tensored_state braced_init_list lambda_expression
%type<expList> expression_list postfix_expression_list optional_assignment_statement
%type<gList> quantum_gate_list

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
%right '@'
%right UNARY
%right EXP
%left '.'

%start translation_unit

%%

translation_unit
    : translation_unit external_declaration
    | external_declaration
    ;

external_declaration
    : statement
        { translationUnit.push_back($1); }
    | declaration
        { translationUnit.push_back($1); }
    ;

declaration
    : function_declaration
        { $$ = $1; }
    | gate_declaration
        { $$ = $1; }
    ;

gate_declaration
    : GATE '{' APPLY ':' gate_composition '}'
        { $$ = new GateDecl($5, $1->loc); }
    ;

function_declaration
    : FUNC function_header compound_statement 
        { 
            $2->setBody($3);
            $$ = $2;
        }
    | FUNC function_header return_type compound_statement
        { 
            $2->setReturnType($3);
            $2->setBody($4);
            $$ = $2;
        }
    ;

function_header
    : IDENTIFIER '(' parameter_list ')'
        {
            auto* name = new IdentifierExpr(std::get<std::string>($1->value), $1->loc);
            $$ = new FunctionDecl(name, *$3, nullptr, nullptr, $1->loc);
            delete $3;
        }
    | IDENTIFIER '(' ')'
        {
            auto* name = new IdentifierExpr(std::get<std::string>($1->value), $1->loc);
            std::vector<ParameterDecl*> empty;
            $$ = new FunctionDecl(name, empty, nullptr, nullptr, $1->loc);
        }
    ;

return_type
    : ':' type
        { $$ = $2; }
    ;

parameter_list
    : parameter 
        { 
            $$ = new std::vector<ParameterDecl*>();
            $$->push_back($1);
        }
    | parameter_list ',' parameter
        { 
            $1->push_back($3);
            $$ = $1;
        }
    ;

parameter
    : IDENTIFIER ':' type
        {
            auto* name = new IdentifierExpr(std::get<std::string>($1->value), $1->loc);
            $$ = new ParameterDecl(name, $3, $1->loc);
        }
    ;

statement_list 
    : statement_list statement
        { 
            $1->push_back($2);
            $$ = $1;
        }
    | statement
        { 
            $$ = new std::vector<Stmt*>();
            $$->push_back($1);
        }
    ;

statement
    : declaration_statement ';'
        { $$ = $1; }
    | expression_statement ';'
        { $$ = new ExpressionStmt($1); }
    | assignment_statement ';'
        { $$ = $1; }
    | quantum_statement ';'
        { $$ = $1; }
    | jump_statement ';'
        { $$ = $1; }
    | print_statement ';'
        { $$ = $1; }
    | compound_statement
        { $$ = $1; }
    | selection_statement
        { $$ = $1; }
    | iteration_statement
        { $$ = $1; }
    ;

selection_statement
    : IF '(' condition ')' compound_statement
        { $$ = new IfStmt($3, $5, nullptr, $1->loc); }
    | IF '(' condition ')' compound_statement ELSE compound_statement
        { $$ = new IfStmt($3, $5, $7, $1->loc); }
    | IF '(' condition ')' compound_statement elif_clauses
        { 
            // Convert elif chain to nested if-else
            Stmt* elseBlock = (*$6)[0];
            for (size_t i = 1; i < $6->size(); i++) {
                elseBlock = new IfStmt(nullptr, elseBlock, (*$6)[i]);
            }
            $$ = new IfStmt($3, $5, elseBlock, $1->loc);
            delete $6;
        }
    | MATCH '(' expression ')' '{' match_list '}'
        { $$ = new MatchStmt($3, *$6, $1->loc); delete $6; }
    ;

elif_clauses
    : ELIF '(' condition ')' compound_statement
        { 
            $$ = new std::vector<Stmt*>();
            $$->push_back(new IfStmt($3, $5, nullptr, $1->loc));
        }
    | ELIF '(' condition ')' compound_statement ELSE compound_statement
        { 
            $$ = new std::vector<Stmt*>();
            $$->push_back(new IfStmt($3, $5, $7, $1->loc));
        }
    | ELIF '(' condition ')' compound_statement elif_clauses
        { 
            $6->insert($6->begin(), new IfStmt($3, $5, nullptr, $1->loc));
            $$ = $6;
        }
    ;

compound_statement
    : '{' statement_list '}'
        { $$ = new CompoundStmt(*$2); delete $2; }
    ;

expression_statement    
    : expression
        { $$ = $1; }
    ;

condition   
    : expression '>' expression
        { $$ = new BinaryOpExpr($1, ">", $3); }
    | expression '<' expression
        { $$ = new BinaryOpExpr($1, "<", $3); }
    | expression GE_OP expression
        { $$ = new BinaryOpExpr($1, ">=", $3); }
    | expression LE_OP expression
        { $$ = new BinaryOpExpr($1, "<=", $3); }
    | expression EQ_OP expression
        { $$ = new BinaryOpExpr($1, "==", $3); }
    | expression NE_OP expression
        { $$ = new BinaryOpExpr($1, "!=", $3); }
    | condition AND condition
        { $$ = new BinaryOpExpr($1, "and", $3); }
    | condition OR condition
        { $$ = new BinaryOpExpr($1, "or", $3); }
    | NOT condition %prec UNARY
        { $$ = new UnaryOpExpr("not", $2); }
    | '(' condition ')'
        { $$ = $2; }
    | expression
        { $$ = $1; }
    ;

match_list
    : %empty
        { $$ = new std::vector<MatchCase*>(); }
    | match_list match_statement
        { 
            $1->push_back($2);
            $$ = $1;
        }
    ;

match_statement
    : expression DOUBLE_ARROW compound_statement
        { $$ = new MatchCase($1, $3); }
    ;

optional_condition
    : %empty
        { $$ = nullptr; }
    | condition
        { $$ = $1; }
    ;

iteration_statement
    : WHILE '(' condition ')' compound_statement
        { $$ = new WhileStmt($3, $5, $1->loc); }
    | DO compound_statement WHILE '(' condition ')'
        { $$ = new DoWhileStmt($2, $5, $1->loc); }
    | FOR '(' variable_declaration_list ';' optional_condition ';' optional_assignment_statement ')' compound_statement
        { 
            auto* init = new DeclarationStmt(*$3);
            delete $3;
            Expr* update = nullptr;
            if ($7 && !$7->empty()) {
                update = (*$7)[0];
                delete $7;
            }
            $$ = new ForStmt(init, $5, update, $9, $1->loc);
        }
    ;

optional_assignment_statement
    : %empty
        { $$ = new std::vector<Expr*>(); }
    | assignment_statement
        { 
            $$ = new std::vector<Expr*>();
            // Extract the assignment as an expression (simplified)
            auto* assign = dynamic_cast<AssignmentStmt*>($1);
            if (assign) {
                $$->push_back(new BinaryOpExpr(assign->getLeft(), assign->getOperator(), assign->getRight()));
            }
        }
    ;

jump_statement
    : CONTINUE
        { $$ = new ContinueStmt($1->loc); }
    | BREAK
        { $$ = new BreakStmt($1->loc); }
    | RETURN
        { $$ = new ReturnStmt($1->loc); }
    | RETURN expression
        { $$ = new ReturnStmt($2, $1->loc); }
    ;

declaration_statement
    : LET variable_declaration_list
        { $$ = new DeclarationStmt(*$2); delete $2; }
    ;

variable_declaration_list
    : variable_declaration_list ',' variable_declaration
        { 
            $1->push_back($3);
            $$ = $1;
        }
    | variable_declaration
        { 
            $$ = new std::vector<VariableDecl*>();
            $$->push_back($1);
        }
    ;

variable_declaration
    : IDENTIFIER ':' type 
        {
            auto* name = new IdentifierExpr(std::get<std::string>($1->value), $1->loc);
            $$ = new VariableDecl(name, $3, nullptr, $1->loc);
        }
    | IDENTIFIER ':' type EQUAL_TO expression
        {
            auto* name = new IdentifierExpr(std::get<std::string>($1->value), $1->loc);
            $$ = new VariableDecl(name, $3, $5, $1->loc);
        }
    | IDENTIFIER ':' type EQUAL_TO braced_init_list
        {
            auto* name = new IdentifierExpr(std::get<std::string>($1->value), $1->loc);
            $$ = new VariableDecl(name, $3, $5, $1->loc);
        }
    | IDENTIFIER ':' type EQUAL_TO tensored_state
        {
            auto* name = new IdentifierExpr(std::get<std::string>($1->value), $1->loc);
            $$ = new VariableDecl(name, $3, $5, $1->loc);
        }
    ;

type
    :   type_name array_list
        {
            Type* baseType = $1;
            // Build from innermost to outermost
            for (auto* dimExpr : *$2) {
                baseType = new ArrayTypeNode(baseType, dimExpr);
            }
            $$ = baseType;
            delete $2;  // Clean up the vector
        }
    | type_name
        { $$ = $1; }
    | '(' function_object ')'
        { $$ = $2; }
    ;

type_list
    : type 
        { 
            $$ = new std::vector<Type*>();
            $$->push_back($1);
        }
    | type_list ',' type
        { 
            $1->push_back($3);
            $$ = $1;
        }
    ;

function_object
    : '(' type_list ')' DOUBLE_ARROW '(' ')'
        { $$ = new FunctionTypeNode(*$2, nullptr); delete $2; }
    | '(' type_list ')' DOUBLE_ARROW type
        { $$ = new FunctionTypeNode(*$2, $5); delete $2; }
    | type DOUBLE_ARROW '(' ')'
        { 
            std::vector<Type*> params;
            params.push_back($1);
            $$ = new FunctionTypeNode(params, nullptr);
        }
    | type DOUBLE_ARROW type
        { 
            std::vector<Type*> params;
            params.push_back($1);
            $$ = new FunctionTypeNode(params, $3);
        }
    ;

array_list
    : array_list '[' index_expression ']'
        { 
            $1->push_back($3);  // Collect dimension expression
            $$ = $1;
        }
    | '[' index_expression ']'
        { 
            $$ = new std::vector<Expr*>();
            $$->push_back($2);  // First dimension
        }
    | '[' ']'
        { 
            $$ = new std::vector<Expr*>();
            $$->push_back(nullptr);  // nullptr = unspecified size
        }
    ;

type_name
    : QUBIT 
        { $$ = new BaseTypeNode(BaseTypeKind::TYPE_QUBIT, $1->loc); }
    | BIT
        { $$ = new BaseTypeNode(BaseTypeKind::TYPE_BIT, $1->loc); }
    | BOOL
        { $$ = new BaseTypeNode(BaseTypeKind::TYPE_BOOL, $1->loc); }
    | INT 
        { $$ = new BaseTypeNode(BaseTypeKind::TYPE_INT, $1->loc); }
    | FLOAT
        { $$ = new BaseTypeNode(BaseTypeKind::TYPE_FLOAT, $1->loc); }
    | STRING
        { $$ = new BaseTypeNode(BaseTypeKind::TYPE_STRING, $1->loc); }
    ;

assignment_statement
    : postfix_expression assignment_operator expression
        { $$ = new AssignmentStmt($1, std::get<std::string>($2->value), $3, $2->loc); }
    | postfix_expression assignment_operator braced_init_list
        { $$ = new AssignmentStmt($1, std::get<std::string>($2->value), $3, $2->loc); }
    | postfix_expression assignment_operator tensored_state
        { $$ = new AssignmentStmt($1, std::get<std::string>($2->value), $3, $2->loc); }
    ;

braced_init_list
    : '{' expression_list '}'
        { $$ = new BracedInitList(*$2); delete $2; }
    ; 

assignment_operator
    : EQUAL_TO
        { $$ = $1; }
    | ADD_ASSIGN 
        { $$ = $1; }
    | SUB_ASSIGN
        { $$ = $1; }
    | MUL_ASSIGN 
        { $$ = $1; }
    | DIV_ASSIGN
        { $$ = $1; }
    | MOD_ASSIGN
        { $$ = $1; }
    | EXP_ASSIGN  
        { $$ = $1; }
    | AND_ASSIGN 
        { $$ = $1; }
    | OR_ASSIGN 
        { $$ = $1; }
    | XOR_ASSIGN
        { $$ = $1; }
    | RIGHT_SHIFT_ASSIGN
        { $$ = $1; }
    | LEFT_SHIFT_ASSIGN
        { $$ = $1; }
    ;

expression
    : expression '+' expression   
        { $$ = new BinaryOpExpr($1, "+", $3); }  
    | expression '|' expression
        { $$ = new BinaryOpExpr($1, "|", $3); }  
    | expression '-' expression
        { $$ = new BinaryOpExpr($1, "-", $3); } 
    | expression '*' expression
        { $$ = new BinaryOpExpr($1, "*", $3); }  
    | expression '&' expression
        { $$ = new BinaryOpExpr($1, "&", $3); }  
    | expression '/' expression
        { $$ = new BinaryOpExpr($1, "/", $3); }  
    | expression '%' expression
        { $$ = new BinaryOpExpr($1, "%", $3); }  
    | expression RIGHT_SHIFT expression 
        { $$ = new BinaryOpExpr($1, ">>", $3); } 
    | expression LEFT_SHIFT expression
        { $$ = new BinaryOpExpr($1, "<<", $3); } 
    | expression EXP expression
        { $$ = new BinaryOpExpr($1, "**", $3); } 
    | expression '^' expression 
        { $$ = new BinaryOpExpr($1, "^", $3); }  
    | '+' expression %prec UNARY
        { $$ = new UnaryOpExpr("+", $2); }
    | '-' expression %prec UNARY
        { $$ = new UnaryOpExpr("-", $2); }
    | '!' expression %prec UNARY
        { $$ = new UnaryOpExpr("!", $2); }
    | postfix_expression 
        { $$ = $1; }
    ;

expression_list
    : expression_list ',' expression
        { 
            $1->push_back($3);
            $$ = $1;
        }
    | expression
        { 
            $$ = new std::vector<Expr*>();
            $$->push_back($1);
        }
    ;
    
postfix_expression
    : primary_expression
        { $$ = $1; }
    | postfix_expression '[' index_expression ']'
        { $$ = new IndexAccessExpr($1, $3); }
    | postfix_expression '(' expression_list ')'
        { $$ = new FunctionCallExpr($1, *$3); delete $3; }
    | postfix_expression '(' ')'
        { 
            std::vector<Expr*> empty;
            $$ = new FunctionCallExpr($1, empty); 
        }
    | postfix_expression '.' IDENTIFIER
        { 
            auto* member = new IdentifierExpr(std::get<std::string>($3->value), $3->loc);
            $$ = new MemberAccessExpr($1, member); 
        }
    | CAST '<' type_name '>' '(' expression ')'
        { $$ = new CastExpr($3, $6, $1->loc); }
    ;

primary_expression
    : INT_LITERAL
        { $$ = new IntLiteralExpr(std::get<long long>($1->value), $1->loc); }
    | FLOAT_LITERAL
        { $$ = new FloatLiteralExpr(std::get<double>($1->value), $1->loc); }
    | STRING_LITERAL
        { $$ = new StringLiteralExpr(std::get<std::string>($1->value), $1->loc); }
    | IDENTIFIER
        { $$ = new IdentifierExpr(std::get<std::string>($1->value), $1->loc); }
    | lambda_expression
        { $$ = $1; }
    | boolean_literal 
        { $$ = $1; }
    | '(' expression ')'
        { $$ = $2; }
    ;

lambda_expression
    : '(' parameter_list ')' compound_statement
        { $$ = new LambdaExpr(*$2, nullptr, $4); delete $2; }
    | '(' parameter_list ')' ':' type compound_statement
        { $$ = new LambdaExpr(*$2, $5, $6); delete $2; }
    ;

boolean_literal
    : TRUE    
        { $$ = new BoolLiteralExpr(true, $1->loc); }
    | FALSE 
        { $$ = new BoolLiteralExpr(false, $1->loc); }
    ;

print_statement
    : PRINT '(' print_string ')'
        { 
            Expr* arg = $3 ? new StringLiteralExpr(std::get<std::string>($3->value), $3->loc) : nullptr;
            $$ = new PrintStmt(PrintOperationKind::PRINT, arg, $1->loc); 
        }
    | PRINTLN '(' print_string ')'
        { 
            Expr* arg = $3 ? new StringLiteralExpr(std::get<std::string>($3->value), $3->loc) : nullptr;
            $$ = new PrintStmt(PrintOperationKind::PRINTLN, arg, $1->loc); 
        }
    | SCAN '(' print_string ')'
        { 
            Expr* arg = $3 ? new StringLiteralExpr(std::get<std::string>($3->value), $3->loc) : nullptr;
            $$ = new PrintStmt(PrintOperationKind::SCAN, arg, $1->loc); 
        }
    ;

print_string
    : %empty
        { $$ = nullptr; }
    | STRING_LITERAL
        { $$ = $1; }
    ;

quantum_statement
    : apply_gate_statement
        { $$ = $1; }
    | measure_statement
        { $$ = $1; }
    | reset_statement
        { $$ = $1; }
    ;

measure_statement
    : MEASURE_OP quantum_state DOUBLE_ARROW quantum_state
        { $$ = new MeasureStmt($2, $4, $1->loc); }
    ;

reset_statement
    : RESET_OP quantum_state
        { $$ = new ResetStmt($2, $1->loc); }
    ;   

apply_gate_statement
    : gate_composition '@' quantum_state
        { $$ = new ApplyGateStmt($1, $3); }
    ;   

gate_composition
    : gate_composition '@' quantum_gate
        { $$ = new GateCompositionNode($1, $3); }
    | quantum_gate 
        { $$ = $1; }
    ;

quantum_state
    : postfix_expression
        { $$ = $1; }
    | tensored_state
        { $$ = $1; }
    ;

tensored_state
    : '[' postfix_expression_list ']'
        { $$ = new QuantumStateList(*$2); delete $2; }
    ;

postfix_expression_list
    : postfix_expression
        { 
            $$ = new std::vector<Expr*>();
            $$->push_back($1);
        }
    | postfix_expression_list ',' postfix_expression
        { 
            $1->push_back($3);
            $$ = $1;
        }
    ;

quantum_gate
    : '[' quantum_gate_list ']'
        { $$ = new CompositeGateNode(*$2); delete $2; }
    | simple_gate '(' expression ')'
        { 
            std::vector<Expr*> params;
            params.push_back($3);
            $$ = new ParametricGateNode($1->getGateKind(), params, $1->loc);
            delete $1;
        }
    | simple_gate
        { $$ = $1; }
    ;

quantum_gate_list
    : quantum_gate_list ',' simple_gate '(' expression ')' 
        { 
            std::vector<Expr*> params;
            params.push_back($5);
            $1->push_back(new ParametricGateNode($3->getGateKind(), params, $3->loc));
            delete $3;
            $$ = $1;
        }
    | quantum_gate_list ',' simple_gate 
        { 
            $1->push_back($3);
            $$ = $1;
        }
    | simple_gate '(' expression ')'
        { 
            $$ = new std::vector<GateNode*>();
            std::vector<Expr*> params;
            params.push_back($3);
            $$->push_back(new ParametricGateNode($1->getGateKind(), params, $1->loc));
            delete $1;
        }
    | simple_gate
        { 
            $$ = new std::vector<GateNode*>();
            $$->push_back($1);
        }
    ;

simple_gate
    : GATE_H 
        { $$ = new SimpleGateNode(GateKind::H, $1->loc); }
    | GATE_S
        { $$ = new SimpleGateNode(GateKind::S, $1->loc); }
    | GATE_T 
        { $$ = new SimpleGateNode(GateKind::T, $1->loc); }
    | GATE_CTRL
        { $$ = new SimpleGateNode(GateKind::CTRL, $1->loc); }
    | GATE_I 
        { $$ = new SimpleGateNode(GateKind::I, $1->loc); }
    | GATE_X
        { $$ = new SimpleGateNode(GateKind::X, $1->loc); }
    | GATE_Y
        { $$ = new SimpleGateNode(GateKind::Y, $1->loc); }
    | GATE_Z 
        { $$ = new SimpleGateNode(GateKind::Z, $1->loc); }
    | GATE_RX 
        { $$ = new SimpleGateNode(GateKind::RX, $1->loc); }
    | GATE_RY 
        { $$ = new SimpleGateNode(GateKind::RY, $1->loc); }
    | GATE_RZ
        { $$ = new SimpleGateNode(GateKind::RZ, $1->loc); }
    | GATE_CNOT
        { $$ = new SimpleGateNode(GateKind::CNOT, $1->loc); }
    | GATE_CZ 
        { $$ = new SimpleGateNode(GateKind::CZ, $1->loc); }
    | GATE_SWAP 
        { $$ = new SimpleGateNode(GateKind::SWAP, $1->loc); }
    | GATE_CSWAP
        { $$ = new SimpleGateNode(GateKind::CSWAP, $1->loc); }
    | GATE_CCNOT 
        { $$ = new SimpleGateNode(GateKind::CCNOT, $1->loc); }
    | GATE_CRX 
        { $$ = new SimpleGateNode(GateKind::CRX, $1->loc); }
    | GATE_CRY
        { $$ = new SimpleGateNode(GateKind::CRY, $1->loc); }
    | GATE_CRZ
        { $$ = new SimpleGateNode(GateKind::CRZ, $1->loc); }
    ;

optional_expression
    : %empty
        { $$ = nullptr; }
    | expression
        { $$ = $1; }
    ;

index_expression
    : expression
        { $$ = $1; }
    | optional_expression ':' optional_expression
        { $$ = new SliceExpr($1, $3, nullptr); }
    | optional_expression ':' optional_expression ':' optional_expression
        { $$ = new SliceExpr($1, $3, $5); }
    ;

%%

void yyerror(const std::string &msg) {
    std::cerr << "Parse Error: " << msg << std::endl;
}

int main() {
    yydebug = 0;
    yyparse();
    
    // Print AST or process it here
    std::cout << "Translation unit contains " << translationUnit.size() << " top-level nodes." << std::endl;
    
    // Clean up
    for (auto* node : translationUnit) {
        delete node;
    }
    
    return 0;
}