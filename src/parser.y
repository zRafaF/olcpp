%{
    // Definitions

%}

%union {
    int num;
    char sys; // single character symbol
}


%token END_OF_LINE
%token<num> NUMBER
%type<num> EXPRESSION


// Rules
%%

// Can be either an expression followed by a new line or just a new line
input: EXPRESSION END_OF_LINE
    |  END_OF_LINE
    ;

EXPRESSION: NUMBER
    | EXPRESSION '+' EXPRESSION
    | EXPRESSION '-' EXPRESSION
    | EXPRESSION '*' EXPRESSION
    | EXPRESSION '/' EXPRESSION
    ;


%%

int main() {
    yyparse();
    return 0;
}