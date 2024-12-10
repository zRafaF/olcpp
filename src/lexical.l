%{
    #include "parser.tab.h"
    #include <stdlib.h>
    #include <string.h>

    extern char *yytext; // Declare yytext
    extern void yyerror(const char* s);  // Declare yyerror

%}

%%

[0-9]+ {
    yylval.num = atoi(yytext);
    return INTEGER;
}

\n { return END_OF_LINE; }

[ \t]+ { /* Ignore whitespace */ }

. {
    yyerror("Invalid character encountered");
    return 0;
}

👍️ { return PROGRAM_BEGIN; }
👎️ { return PROGRAM_END; }
💯 { return INTEGER_TYPE; }
🔤 { return STRING_TYPE; }
🔢 { return INT_ARRAY_TYPE; }
⁉️ { return BOOL_TYPE; }
✅ { return BOOL_TRUE; }
❌ { return BOOL_FALSE; }

👉[^"\n]*👈 {
    char *temp = strdup(yytext + 2);  // Remove "👉"
    temp[strlen(temp) - 2] = '\0';    // Remove "👈"
    yylval.str = temp;
    return STRING;
}

🪣 { return VARIABLE_DECLARATION; }
🚬 { return ARRAY_DECLARATION_DIVIDER; }
➕ { return INTEGER_ADDITION; }
➖ { return INTEGER_SUBTRACTION; }
✖ { return INTEGER_MULTIPLICATION; }
➗ { return INTEGER_DIVISION; }
♻ { return INTEGER_MODULUS; }
🟰 { return ASSIGN; }
🔼 { return GREATER_THAN; }
🔽 { return LESS_THAN; }
⏫ { return GREATER_THAN_EQUAL; }
⏬ { return LESS_THAN_EQUAL; }
🔁 { return EQUAL; }
🔀 { return NOT_EQUAL; }
❗ { return NEGATION; }
🅰 { return AND; }
🅾 { return OR; }
🤔 { return IF_START; }
😬 { return IF_END; }
😒 { return ELSE; }
🫸 { return CONDITION_BEGIN; }
🫷 { return CONDITION_END; }
🐣 { return FOR_BEGIN; }
🐥 { return FOR_END; }
🚭 { return FOR_CONDITION_SEPARATOR; }
🐳 { return WHILE_BEGIN; }
🐋 { return WHILE_END; }
💅 { return PRINT; }
✍ { return INPUT; }

    /* Ignore comments*/
👅.* { /* Do nothing */ }

%%

int yywrap() {
    return 1;
}
