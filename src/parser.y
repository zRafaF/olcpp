%{
    // Definitions and includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>

    // Error function
    void yyerror(const char *s);
    int yylex();
%}

%union {
    int num;
    char *str;
    int *array;
}

// Token definitions
%token END_OF_LINE
%token <num> INTEGER
%token <str> IDENTIFIER STRING

%token PROGRAM_BEGIN PROGRAM_END INTEGER_TYPE STRING_TYPE INT_ARRAY_TYPE BOOL_TYPE BOOL_TRUE BOOL_FALSE
%token VARIABLE_DECLARATION ARRAY_DECLARATION_DIVIDER INTEGER_ADDITION INTEGER_SUBTRACTION INTEGER_MULTIPLICATION
%token INTEGER_DIVISION INTEGER_MODULUS GREATER_THAN LESS_THAN GREATER_THAN_EQUAL LESS_THAN_EQUAL EQUAL
%token NOT_EQUAL NEGATION AND OR IF_START IF_END ELSE CONDITION_BEGIN CONDITION_END FOR_BEGIN FOR_END
%token FOR_CONDITION_SEPARATOR WHILE_BEGIN WHILE_END PRINT INPUT ASSIGN

// Operator precedence
%left OR
%left AND
%left EQUAL NOT_EQUAL
%left GREATER_THAN LESS_THAN GREATER_THAN_EQUAL LESS_THAN_EQUAL
%left INTEGER_ADDITION INTEGER_SUBTRACTION
%left INTEGER_MULTIPLICATION INTEGER_DIVISION INTEGER_MODULUS
%right NEGATION

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
    var_declaration END_OF_LINE
    | assignment END_OF_LINE
    | condition END_OF_LINE
    | if END_OF_LINE
    | else END_OF_LINE
    | print END_OF_LINE
    | input END_OF_LINE
    | for_loop END_OF_LINE
    | while_loop END_OF_LINE
    ;

assigned_array_digits:
    INTEGER
    | INTEGER ARRAY_DECLARATION_DIVIDER assigned_array_digits
    ;

// Variables MUST be assigned on declaration
var_declaration:
    VARIABLE_DECLARATION IDENTIFIER var_declaration_types ASSIGN var_declaration_expressions
    ;

var_declaration_types:
    INTEGER_TYPE
    | STRING_TYPE
    | INT_ARRAY_TYPE
    | BOOL_TYPE
    ;

boolean:
    BOOL_TRUE
    | BOOL_FALSE
    ;

var_declaration_expressions:
    integer_expression
    | string_expression
    | array_expression
    | boolean_expression
    ;

integer_expression:
    INTEGER
    | IDENTIFIER
    | term
    ;

string_expression:
    STRING
    | IDENTIFIER
    ;

array_expression:
    IDENTIFIER
    | CONDITION_BEGIN assigned_array_digits CONDITION_END
    ;

boolean_expression:
    boolean
    | IDENTIFIER
    | condition
    ;

term:
    INTEGER
    | IDENTIFIER
    | term INTEGER_ADDITION term
    | term INTEGER_SUBTRACTION term
    | term INTEGER_MULTIPLICATION term
    | term INTEGER_DIVISION term
    | term INTEGER_MODULUS term
    ;

assignment:
    IDENTIFIER ASSIGN integer_expression
    | IDENTIFIER ASSIGN string_expression
    // Arrays can only be assigned with arrays, no single value manipulation
    | IDENTIFIER ASSIGN CONDITION_BEGIN assigned_array_digits CONDITION_END
    ;

condition:
    CONDITION_BEGIN condition_body CONDITION_END
    ;

condition_body:
    expression
    | condition_body AND expression
    | condition_body OR expression
    ;

expression:
    boolean
    | numeric_expression GREATER_THAN numeric_expression
    | numeric_expression LESS_THAN numeric_expression
    | numeric_expression GREATER_THAN_EQUAL numeric_expression
    | numeric_expression LESS_THAN_EQUAL numeric_expression
    | expression_value EQUAL expression_value
    | expression_value NOT_EQUAL expression_value
    | NEGATION expression
    ;

numeric_expression:
    INTEGER
    | IDENTIFIER
    | term
    ;

expression_value:
    IDENTIFIER
    | INTEGER
    | STRING
    | term
    | boolean
    ;

if:
    IF_START condition END_OF_LINE program_body IF_END
    ;

else:
    if ELSE if
    | if ELSE if else
    | if ELSE IF_START program_body IF_END
    ;


for_loop:
    FOR_BEGIN CONDITION_BEGIN expression FOR_CONDITION_SEPARATOR assignment CONDITION_END END_OF_LINE program_body FOR_END
    ;

while_loop:
    WHILE_BEGIN condition END_OF_LINE program_body WHILE_END
    ;


print:
    PRINT expression_value
    ;

input:
    INPUT IDENTIFIER
    ;

%%

int main() {
    yyparse();
    return 0;
}