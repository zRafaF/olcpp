%{
    // Definitions

%}

%union {
    int num;
    char *str;
}


%token END_OF_LINE
%token <num> INTEGER
%token <str> IDENTIFIER STRING

%token PROGRAM_BEGIN PROGRAM_END INTEGER_TYPE STRING_TYPE VARIABLE_DECLARATION ARRAY_DECLARATION ARRAY_DECLARATION_DIVIDER INTEGER_ADDITION INTEGER_SUBTRACTION INTEGER_MULTIPLICATION INTEGER_DIVISION
%token INTEGER_MODULUS GREATER_THAN LESS_THAN GREATER_THAN_EQUAL LESS_THAN_EQUAL EQUAL NOT_EQUAL NEGATION AND OR IF_START IF_END ELSE CONDITION_BEGIN CONDITION_END FOR_BEGIN FOR_END
%token FOR_CONDITION_SEPARATOR WHILE_BEGIN WHILE_END PRINT INPUT SINGLE_LINE_COMMENT

// Rules
%%

program:
    PROGRAM_BEGIN END_OF_LINE program_body PROGRAM_END
    ;

program_body:
    statement
    | program_body statement
    ;

statement:
    var_declaration
    | array_declaration
    | assignment
    | condition
    | for_loop
    | while_loop
    | print
    | input
    | comment
    ;
var_declaration:
    VARIABLE_DECLARATION IDENTIFIER INTEGER_TYPE INTEGER END_OF_LINE
    | VARIABLE_DECLARATION IDENTIFIER STRING_TYPE STRING END_OF_LINE

%%

int main() {
    yyparse();
    return 0;
}