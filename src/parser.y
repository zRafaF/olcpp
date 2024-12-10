%{
    // Definitions and includes
    #include <stdio.h>    // For standard input/output operations
    #include <stdlib.h>   // For memory allocation and general utilities
    #include <stdbool.h>  // For boolean type and constants


    extern int yylex();
    extern int yyparse();
    extern FILE* yyin;
    void yyerror(const char* s);

%}

%union {
    int num;          // For integer values
    char *str;        // For string values
    int *array;       // For array of integers
}

// Token definitions
%token END_OF_LINE    // Marks the end of a line
%token <num> INTEGER  // Integer token with a value
%token <str> IDENTIFIER  // Identifier and string tokens
%token <str> STRING  // Identifier and string tokens

// Keywords and operators used in the language
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

// Grammar rules
%%
program:
    PROGRAM_BEGIN END_OF_LINE program_body PROGRAM_END
    ; // Represents a complete program structure

program_body:
    statement
    | program_body statement
    | program_body error END_OF_LINE { yyerror("Syntax error in program body"); }
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
    ; // Each statement is one of several valid constructs

assigned_array_digits:
    INTEGER
    | INTEGER ARRAY_DECLARATION_DIVIDER assigned_array_digits
    ; // Handles assignment of multiple integers to an array

// Variable declaration with initialization
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
    ; // Boolean literals

var_declaration_expressions:
    integer_expression
    | string_expression
    | array_expression
    | boolean_expression
    | IDENTIFIER
    ;

integer_expression:
    INTEGER
    | term
    ; // Handles integers and identifiers in expressions

string_expression:
    STRING
    ; // Handles strings and identifiers in expressions

array_expression:
    CONDITION_BEGIN assigned_array_digits CONDITION_END
    ; // Arrays can be assigned directly or using condition delimiters

boolean_expression:
    boolean
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
    ; // Arithmetic operations

assignment:
    IDENTIFIER ASSIGN integer_expression
    | IDENTIFIER ASSIGN string_expression
    | IDENTIFIER ASSIGN CONDITION_BEGIN assigned_array_digits CONDITION_END
    ; // Assignment syntax for different types

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
    | term GREATER_THAN term
    | term LESS_THAN term
    | term GREATER_THAN_EQUAL term
    | term LESS_THAN_EQUAL term
    | expression_value EQUAL expression_value
    | expression_value NOT_EQUAL expression_value
    | NEGATION expression
    ; // Logical and comparison expressions

expression_value:
    STRING
    | term
    | boolean
    ;

if:
    IF_START condition END_OF_LINE program_body IF_END
    ; // If statement with a condition and body

else:
    if ELSE if
    | if ELSE if else
    | if ELSE IF_START program_body IF_END
    ;

for_loop:
    FOR_BEGIN CONDITION_BEGIN expression FOR_CONDITION_SEPARATOR assignment CONDITION_END END_OF_LINE program_body FOR_END
    ; // For loop syntax

while_loop:
    WHILE_BEGIN condition END_OF_LINE program_body WHILE_END
    ; // While loop syntax

print:
    PRINT expression_value
    ; // Print statement

input:
    INPUT IDENTIFIER
    ; // Input statement

%%

int main() {
    yyparse(); // Starts parsing
    return 0;
}

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s at line %d, token: %s\n", s);
    exit(1);
}

