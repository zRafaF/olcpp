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
\n  {
    return END_OF_LINE;
}

[ \t\n]+        /* ignore whitespace */;

"👍️" {
    return PROGRAM_BEGIN;
}

"👎️" {
    return PROGRAM_END;
}


"💯" {
    return INTEGER_TYPE;
}

"🔤" {
    return STRING_TYPE;
}

"🔢" {
    return INT_ARRAY_TYPE;
}

"⁉️" {
    return BOOL_TYPE;
}

"✅" {
    return BOOL_TRUE;
}

"❌" {
    return BOOL_FALSE;
}

"👉"[^"\n]*"👈" { 
    yylval.str = strdup(yytext); 
    return STRING; 
}

"🪣" {
    return VARIABLE_DECLARATION;
}

"🚬" {
    return ARRAY_DECLARATION_DIVIDER;
}

"➕" {
    return INTEGER_ADDITION;
}

"➖" {
    return INTEGER_SUBTRACTION;
}

"✖" {
    return INTEGER_MULTIPLICATION;
}

"➗" {
    return INTEGER_DIVISION;
}

"♻" {
    return INTEGER_MODULUS;
}

"🟰" {
    return ASSIGN;
}


"🔼" {
    return GREATER_THAN;
}

"🔽" {
    return LESS_THAN;
}

"⏫" {
    return GREATER_THAN_EQUAL;
}

"⏬" {
    return LESS_THAN_EQUAL;
}

"🔁" {
    return EQUAL;
}

"🔀" {
    return NOT_EQUAL;
}

"❗" {
    return NEGATION;
}

"🅰" {
    return AND;
}

"🅾" {
    return OR;
}

"🤔" {
    return IF_START;
}

"😬" {
    return IF_END;
}

"😬" {
    return ELSE;
}

"🫸" {
    return CONDITION_BEGIN;
}

"🫷" {
    return CONDITION_END;
}

"🐣" {
    return FOR_BEGIN;
}

"🐥" {
    return FOR_END;
}

"🚭" {
    return FOR_CONDITION_SEPARATOR;
}

"🐳" {
    return WHILE_BEGIN;
}

"🐋" {
    return WHILE_END;
}

    /* Prints the following variable */
"💅" {
    return PRINT;
}

    /* stores the user input on the following variable */
"✍" {
    return INPUT;
}

"👅".* /* Ignore line comments */;


%%



yywrap() {}