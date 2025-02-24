%{
    // Include necessary headers
    #include <stdio.h>      // For standard input/output
    #include <stdlib.h>     // For memory management
    #include <stdbool.h>    // For boolean values
    #include "node.h"       // For defining AST node structures

    // External declarations
    extern int yylex();       // Lexer function
    extern int yyparse();     // Parser function
    extern FILE* yyin;        // Input file for lexer/parser

    extern int yylineno;      // Tracks the current line number
    extern char* yytext;      // Last matched token text

    void yyerror(const char* s); // Error handling function
    Node *root;              // Root node of the AST
%}

// Define the union for YYSTYPE to specify the possible value types used in the parser
%union {
    int num;          // Integer values
    char* str;        // String values
    int* array;       // Array of integers
    Node* node;       // Abstract Syntax Tree (AST) nodes
}

// Token definitions with data types for values
%token <str> INTEGER      // Numeric constants
%token <str> IDENTIFIER   // Variable or function names
%token <str> STRING       // String literals

// Define non-terminal types and their associated values
%type <node> program statement_list statement expr term factor var_declaration var_declaration_expressions
%type <node> integer_expression string_expression boolean_expression array_expression assigned_array_digits
%type <node> var_assignment boolean_condition_body expression_value condition print input if else
%type <node> for_loop while_loop
%type <str> var_declaration_types boolean

// End of line token for handling line endings in the source code
%token END_OF_LINE

// Keywords and operators in the language
%token PROGRAM_BEGIN PROGRAM_END INTEGER_TYPE STRING_TYPE INT_ARRAY_TYPE BOOL_TYPE TRUE FALSE ACCESS_ARRAY
%token VARIABLE_DECLARATION ARRAY_DECLARATION_DIVIDER INTEGER_ADDITION INTEGER_SUBTRACTION INTEGER_MULTIPLICATION
%token INTEGER_DIVISION INTEGER_MODULUS GREATER_THAN LESS_THAN GREATER_THAN_EQUAL LESS_THAN_EQUAL EQUAL
%token NOT_EQUAL NEGATION AND OR IF_START IF_END ELSE CONDITION_BEGIN CONDITION_END FOR_BEGIN FOR_END
%token FOR_CONDITION_SEPARATOR WHILE_BEGIN WHILE_END PRINT_STATEMENT INPUT_STATEMENT ASSIGN PARENTHESIS_OPEN PARENTHESIS_CLOSE

// Define operator precedence and associativity
%left OR
%left AND
%left EQUAL NOT_EQUAL
%left GREATER_THAN LESS_THAN GREATER_THAN_EQUAL LESS_THAN_EQUAL
%left INTEGER_ADDITION INTEGER_SUBTRACTION
%left INTEGER_MULTIPLICATION INTEGER_DIVISION INTEGER_MODULUS
%right NEGATION

// Grammar rules for the language
%%
program:
    // Program structure starts and ends with specific keywords
    PROGRAM_BEGIN e_o_l statement_list optional_end_of_lines PROGRAM_END { root = $3; }
;

optional_end_of_lines:
    {/* Allow multiple empty lines between statements */}
    | END_OF_LINE optional_end_of_lines 
;

statement_list:
    // A list of statements, linked together
    statement_list statement { $$ = $1; Node *last = $1; while (last->next) last = last->next; last->next = $2; }
    | statement { $$ = $1; }
;

// Skip extra END_OF_LINE tokens
e_o_l:
    END_OF_LINE optional_end_of_lines
;

// Handle different types of statements
statement:
    var_declaration e_o_l { $$ = $1; }
    | var_assignment e_o_l { $$ = $1; }
    | print e_o_l { $$ = $1; }
    | input e_o_l { $$ = $1; }
    | if e_o_l { $$ = $1; }
    | for_loop e_o_l { $$ = $1; }
    | while_loop e_o_l { $$ = $1; }
;

// Variable assignment rules
var_assignment:
    IDENTIFIER ASSIGN var_declaration_expressions {
        // Assign a value to a variable
        $$ = createNode(
            "ASSIGN", 
            createNode($1, $3, NULL), 
            NULL);
    }
    | IDENTIFIER ACCESS_ARRAY integer_expression ASSIGN var_declaration_expressions {
        // Assign a value to an array element
        $$ = createNode(
            "ASSIGN_ARRAY", 
            createNode($1, $3, $5), 
            NULL);
    }
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

// Supported variable types
var_declaration_types:
    INTEGER_TYPE { $$ = "INTEGER_TYPE"; }
    | STRING_TYPE { $$ = "STRING_TYPE"; }
    | INT_ARRAY_TYPE { $$ = "INT_ARRAY_TYPE"; }
    | BOOL_TYPE { $$ = "BOOL_TYPE"; }
    ;

// Boolean literals
boolean:
    TRUE { $$ = "TRUE"; }
    | FALSE { $$ = "FALSE"; }
    ; 

// Expressions for variable declarations
var_declaration_expressions:
    integer_expression { $$ = $1; }
    | string_expression { $$ = $1; }
    | array_expression { $$ = $1; }
    | boolean_condition_body { $$ = $1; }
    | IDENTIFIER { 
        $$ = createNode(
            "VARIABLE", 
            createNode($1, NULL, NULL), 
            NULL); 
    }
    ;

// Handles integer constants, expressions, and array access by index
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
    | expr { $$ = $1; } // Pass through an evaluated expression
    | IDENTIFIER ACCESS_ARRAY integer_expression { 
        $$ = createNode(
            "ACCESS_ARRAY", 
            createNode($1, $3, NULL), 
            NULL);
    }
    ;

// Handles string literals and input statements
string_expression:
    STRING { 
        $$ = createNode(
            "CONSTANT", 
            createNode($1, NULL, NULL), 
            NULL); 
    }
    | input { $$ = $1; } // Allows input statements to serve as string expressions
    ;

// Handles input operations
input:
    INPUT_STATEMENT { 
        $$ = createNode(
            "INPUT_STATEMENT", 
            NULL, 
            NULL);
    }
    ;

// Defines an array by wrapping digits in a condition delimiter
array_expression:
    CONDITION_BEGIN assigned_array_digits CONDITION_END { 
        $$ = createNode(
            "ARRAY", 
            $2, 
            NULL);
    }
    ; 

// Defines array initialization with constants or variables
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
    | IDENTIFIER { 
        $$ = createNode(
            "VARIABLE", 
            createNode($1, NULL, NULL), 
            NULL); 
    }
    | IDENTIFIER ARRAY_DECLARATION_DIVIDER assigned_array_digits {
        $$ = createNode(
            "VARIABLE", 
            createNode($1, NULL, NULL), 
            $3);
    }
    ;

// Logical operations including AND, OR, equality, and parentheses for grouping
boolean_condition_body:
    boolean_expression { $$ = $1; }
    | boolean_condition_body AND boolean_condition_body {
        $$ = createNode(
            "AND", 
            $1, 
            $3);
    }
    | boolean_condition_body OR boolean_condition_body {
        $$ = createNode(
            "OR", 
            $1, 
            $3);
    }
    | PARENTHESIS_OPEN boolean_condition_body PARENTHESIS_CLOSE { $$ = $2; }
    | boolean_condition_body EQUAL boolean_condition_body { $$ = createNode("EQUAL", $1, $3); }
    | boolean_condition_body NOT_EQUAL boolean_condition_body { $$ = createNode("NOT_EQUAL", $1, $3); }
    ;

// Logical comparisons for terms
boolean_expression:
    expression_value { $$ = $1; }
    | term GREATER_THAN term { $$ = createNode("GREATER_THAN", $1, $3); }
    | term LESS_THAN term { $$ = createNode("LESS_THAN", $1, $3); }
    | term GREATER_THAN_EQUAL term { $$ = createNode("GREATER_THAN_EQUAL", $1, $3); }
    | term LESS_THAN_EQUAL term { $$ = createNode("LESS_THAN_EQUAL", $1, $3); }
    ;

// Handles constants, terms, booleans, and negation
expression_value:
    STRING { 
        $$ = createNode(
            "CONSTANT", 
            createNode($1, NULL, NULL), 
            NULL); 
    }
    | term { $$ = $1; }
    | boolean { 
        $$ = createNode(
            "CONSTANT", 
            createNode($1, NULL, NULL), 
            NULL); 
    }
    | NEGATION expression_value { $$ = createNode("NEGATION", $2, NULL); }
    ;

expr:
    expr INTEGER_ADDITION term { $$ = createNode("INTEGER_ADDITION", $1, $3); }
    | expr INTEGER_SUBTRACTION term { $$ = createNode("INTEGER_SUBTRACTION", $1, $3); }
    | term { $$ = $1; } // Allows expressions to resolve to a term
;

term:
    term INTEGER_MULTIPLICATION factor { $$ = createNode("INTEGER_MULTIPLICATION", $1, $3); }
    | term INTEGER_DIVISION factor { $$ = createNode("INTEGER_DIVISION", $1, $3); }
    | term INTEGER_MODULUS factor { $$ = createNode("INTEGER_MODULUS", $1, $3); }
    | factor { $$ = $1; }  // Resolves to a single factor
;

factor:
    PARENTHESIS_OPEN expr PARENTHESIS_CLOSE { $$ = $2; }
    | INTEGER {
        $$ = createNode("CONSTANT", createNode($1, NULL, NULL), NULL);
    }
    | IDENTIFIER { $$ = createNode("VARIABLE", createNode($1, NULL, NULL), NULL); }
;

// Encapsulates boolean conditions
condition:
    CONDITION_BEGIN boolean_condition_body CONDITION_END { $$ = $2; }
    ;

// Handles print statements
print:
    PRINT_STATEMENT condition {
        $$ = createNode("PRINT_STATEMENT", $2, NULL);
    }
    ; 


// If statement control flow, the if content is on the "value" parameter, if the execution flow do not enter it it goes right to "next"
if:
    IF_START condition e_o_l statement_list optional_end_of_lines IF_END{
        $$ = createNode("IF", createNode("CONDITION", $2, $4), NULL);
    }
    | IF_START condition e_o_l statement_list optional_end_of_lines IF_END else{
        $$ = createNode("IF_ELSE_CONDITION", createNode("IF", createNode("CONDITION", $2, $4), $7), NULL);
    }
    ;

// Else statement control flow, it can be an else or an else if
else:
    ELSE e_o_l statement_list optional_end_of_lines IF_END { $$ =  createNode("ELSE", $3, NULL); }
    | ELSE if { $$ = createNode("ELSE", $2, NULL); }
    ;

// For loop control flow, the for loop content is on the "value" parameter
for_loop:
    FOR_BEGIN CONDITION_BEGIN boolean_condition_body FOR_CONDITION_SEPARATOR var_assignment CONDITION_END e_o_l statement_list optional_end_of_lines FOR_END {
        $$ = createNode("FOR_LOOP", createNode("FOR", $3, createNode("FOR_ASSIGN", $5, $8)), NULL);
    }
    ;

// While loop control flow, the while loop content is on the "value" parameter
while_loop:
    WHILE_BEGIN condition e_o_l statement_list optional_end_of_lines WHILE_END{
        $$ = createNode("WHILE_LOOP", createNode("CONDITION", $2, $4), NULL);
    }
    ;

%%

int main(int argc, char **argv) {
    // Check if the correct number of arguments (input and output files) are provided
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input-file> <output-file>\n", argv[0]);
        return 1;  // Exit with an error code if the arguments are incorrect
    }

    // Open the input file in read mode
    FILE *input = fopen(argv[1], "r");
    if (!input) {  // Check if file opening fails
        perror("Error opening input file");  // Display the error if opening fails
        return 1;  // Exit with an error code
    }
    yyin = input;  // Assign the input file to the lexer input stream

    // Open the output file in write mode
    FILE *output = fopen(argv[2], "w");
    if (!output) {  // Check if file opening fails
        perror("Error opening output file");  // Display the error if opening fails
        fclose(input);  // Close the input file if output file opening fails
        return 1;  // Exit with an error code
    }

    // Parse the input and generate the AST if parsing succeeds
    if (yyparse() == 0 && root) {  // If parsing is successful and the AST is built
        generateIR(root, output);  // Generate intermediate representation and write it to output
        freeNode(root);            // Free the memory allocated for the AST
    } else {
        // If parsing fails, print an error message
        fprintf(stderr, "Parsing failed. See errors above.\n");
    }

    // Close the input and output files before exiting
    fclose(input);
    fclose(output);
    return 0;  // Successful execution
}

// Error handling function for the parser, called when an error is encountered
void yyerror(const char *s) {
    // Print the error message with the line number and the problematic token
    fprintf(stderr, "Error at line %d: %s near '%s'\n", yylineno, s, yytext);
}
