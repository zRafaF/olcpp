%{
    #include "node.h" // Include the shared header for Node structure and functions
    #include "parser.tab.h"
    #include <stdlib.h>
    #include <string.h>

    extern char *yytext; // Declare yytext
    extern void yyerror(const char* s);  // Declare yyerror

%}

%%

[0-9]+ {
    // yylval.num = atoi(yytext);
    yylval.str = strdup(yytext); // return as string due to IR being string-based
    return INTEGER;
}

\n+ { /* Ignore whitespace */ }

[ \t\n]+ { /* Ignore whitespace */ }

[ \r\n]+ { /* Ignore whitespace */ }


    
"👅".* { /* Ignore comments*/ }

"(" { return PARENTHESIS_OPEN; }   // Return '(' token
")" { return PARENTHESIS_CLOSE; }   // Return ')' token


👍️ { return PROGRAM_BEGIN; }
👎️ { return PROGRAM_END; }
💯 { return INTEGER_TYPE; }
🔤 { return STRING_TYPE; }
🔢 { return INT_ARRAY_TYPE; }
⁉️ { return BOOL_TYPE; }
✅ { return BOOL_TRUE; }
❌ { return BOOL_FALSE; }

👉([^👈]+)👈 {
    char *str = strdup(yytext);

    // Remove the first and last character
    str+=4;
    str[strlen(str) - 4] = '\0';

    // Emojis are UTF-8 encoded, so we need to remove the extra bytes

    yylval.str = str;
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

[a-zA-Z_][a-zA-Z0-9_]* { // Needs to be declared last
    yylval.str = strdup(yytext); // Copy the identifier name
    return IDENTIFIER;          // Return the IDENTIFIER token
}

[a-zA-Z_] { // Needs to be declared last
    yylval.str = strdup(yytext); // Copy the identifier name
    return IDENTIFIER;          // Return the IDENTIFIER token
}



. {
    yyerror("Invalid character encountered");
    return 0; // This returns control back to Bison
}

%%

int yywrap() {
    return 1;
}
