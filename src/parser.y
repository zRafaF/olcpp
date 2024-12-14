%{
    // Definitions and includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include "node.h"

    extern int yylex();
    extern int yyparse();
    extern FILE* yyin;

    extern int yylineno; // Tracks the current line number
    extern char* yytext; // Last matched token text

    void yyerror(const char* s);
    Node *root;
%}

// Define the union for the parser
// It means the possible types that can be stored in the yylval variable (the return using $$)
%union {
    int num;          // For integer values
    char* str;        // For string values
    int* array;       // For array of integers
    Node* node;       // For AST nodes
}

// Token definitions
/* %token <num> INTEGER  // Integer token with a value */
%token <str> INTEGER  // Integer token with a value as string due to IR being string-based
%token <str> IDENTIFIER  // Identifier and string tokens
%token <str> STRING  // Identifier and string tokens

// Non terminal definitions
%type <node> program statement_list statement expr term factor var_declaration var_declaration_expressions 
%type <node> integer_expression string_expression boolean_expression array_expression assigned_array_digits
%type <str> var_declaration_types boolean 

/* %token END_OF_LINE    // Marks the end of a line */

// Keywords and operators used in the language
%token PROGRAM_BEGIN PROGRAM_END INTEGER_TYPE STRING_TYPE INT_ARRAY_TYPE BOOL_TYPE BOOL_TRUE BOOL_FALSE
%token VARIABLE_DECLARATION ARRAY_DECLARATION_DIVIDER INTEGER_ADDITION INTEGER_SUBTRACTION INTEGER_MULTIPLICATION
%token INTEGER_DIVISION INTEGER_MODULUS GREATER_THAN LESS_THAN GREATER_THAN_EQUAL LESS_THAN_EQUAL EQUAL
%token NOT_EQUAL NEGATION AND OR IF_START IF_END ELSE CONDITION_BEGIN CONDITION_END FOR_BEGIN FOR_END
%token FOR_CONDITION_SEPARATOR WHILE_BEGIN WHILE_END PRINT INPUT ASSIGN PARENTHESIS_OPEN PARENTHESIS_CLOSE


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
    PROGRAM_BEGIN statement_list PROGRAM_END { root = $2; }
;

statement_list:
    statement_list statement { $$ = $1; Node *last = $1; while (last->next) last = last->next; last->next = $2; }
    | statement { $$ = $1; }
;

statement:
    var_declaration { $$ = $1; }
    | IDENTIFIER ASSIGN expr {
        $$ = createNode("ASSIGN", createNode($1, $3, NULL), NULL);
    }
    | expr { $$ = $1; }
;

// Variable declaration with initialization
var_declaration:
    VARIABLE_DECLARATION IDENTIFIER var_declaration_types ASSIGN var_declaration_expressions {
        $$ = createNode(
            "VARIABLE_DECLARATION", 
            createNode(
                $2, 
                createNode($3, $5, NULL), 
                NULL), 
            NULL);
    }
    ;

var_declaration_types: // Variable types literals
    INTEGER_TYPE { $$ = "INTEGER_TYPE"; }
    | STRING_TYPE { $$ = "STRING_TYPE"; }
    | INT_ARRAY_TYPE { $$ = "INT_ARRAY_TYPE"; }
    | BOOL_TYPE { $$ = "BOOL_TYPE"; }
    ;

boolean: // Boolean literals
    BOOL_TRUE { $$ = "BOOL_TRUE"; }
    | BOOL_FALSE { $$ = "BOOL_FALSE"; }
    ; 

var_declaration_expressions:
    integer_expression { $$ = $1; }
    | string_expression { $$ = $1; }
    | array_expression { $$ = $1; }
    | boolean_expression { $$ = $1; }
    | IDENTIFIER { 
        $$ = createNode(
            "VARIABLE", 
            createNode($1, NULL, NULL), 
            NULL); 
    }
    ;

integer_expression:
    INTEGER { 
        $$ = createNode(
            "CONSTANT", 
            createNode(
                $1,
                NULL, 
                NULL), 
            NULL); 
    }
    | expr { $$ = $1; }
    ; // Handles integers and identifiers in expressions

string_expression:
    STRING { 
        $$ = createNode(
            "CONSTANT", 
            createNode($1, NULL, NULL), 
            NULL); 
    }
    ; // Handles strings and identifiers in expressions

array_expression:
    CONDITION_BEGIN assigned_array_digits CONDITION_END { 
        $$ = createNode(
            "ARRAY", 
            $2, 
            NULL);
    }
    ;

assigned_array_digits:
    INTEGER { 
        $$ = createNode(
            "CONSTANT", 
            createNode($1, NULL, NULL), 
            NULL);
    }
    | INTEGER ARRAY_DECLARATION_DIVIDER assigned_array_digits {
        $$ = createNode(
            "CONSTANT", 
            createNode($1, NULL, NULL), 
            $3);
    }
    ;

boolean_expression:
    boolean { 
        $$ = createNode(
            "CONSTANT", 
            createNode($1, NULL, NULL), 
            NULL); 
    }
    /* | condition */
    ;


expr:
    expr INTEGER_ADDITION term { $$ = createNode("INTEGER_ADDITION", $1, $3); }
    | expr INTEGER_SUBTRACTION term { $$ = createNode("INTEGER_SUBTRACTION", $1, $3); }
    | term { $$ = $1; } // Non terminal nodes should be returned as is
;

term:
    term INTEGER_MULTIPLICATION factor { $$ = createNode("INTEGER_MULTIPLICATION", $1, $3); }
    | term INTEGER_DIVISION factor { $$ = createNode("INTEGER_DIVISION", $1, $3); }
    | factor { $$ = $1; }  // Non terminal nodes should be returned as is
;

factor:
    PARENTHESIS_OPEN expr PARENTHESIS_CLOSE { $$ = $2; }
    | INTEGER {
        $$ = createNode("CONSTANT", createNode($1, NULL, NULL), NULL);
    }
    | IDENTIFIER { $$ = createNode("VARIABLE", createNode($1, NULL, NULL), NULL); }
;


/* statement:
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
    ; // Input statement */

%%

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input-file> <output-file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }
    yyin = input;

    FILE *output = fopen(argv[2], "w");
    if (!output) {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    if (yyparse() == 0 && root) { // Parse the input and build the AST
        generateIR(root, output); // Generate intermediate representation and write to output
        freeNode(root); // Free the AST
    } else {
        fprintf(stderr, "Parsing failed. See errors above.\n");
    }

    fclose(input);
    fclose(output);
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s near '%s'\n", yylineno, s, yytext);
}
