    /* Comments should be indented like this */

%{
    /* Declarations */

%}

    /* Rules */

%%

    /* Returns the token NUMBER */
[0-9]+ {
    return NUMBER;
}

    /* Returns the token NUMBER */
\n {
    return END_OF_LINE;
}

. {}

%%



yywrap() {}