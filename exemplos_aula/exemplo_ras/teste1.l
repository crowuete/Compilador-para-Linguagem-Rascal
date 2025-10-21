%{
enum {TOK_PROGRAM=256, TOK_NUMERO, TOK_ID, TOK_MAIS};
int linha = 1;
char lexema[100];
%}

%option noyywrap

quebra      [\n]
id          [a-zA-Z][a-zA-Z0-9]*
digito      [0-9]

%%

{quebra}        { linha++; }
"+"             { strcpy(lexema, yytext); return TOK_MAIS; }
{digito}+       { strcpy(lexema, yytext); return TOK_NUMERO; }
"program"       { strcpy(lexema, yytext); return TOK_PROGRAM; }
{id}            { strcpy(lexema, yytext); return TOK_ID; }

%%

int main (int argc, char *argv[]) {
    int tok;
    yyin = fopen(argv[1], "r");
    while (tok=yylex()) {
        printf("<%s, %d>\t(linha: %d)\n", lexema, tok, linha);
    }
    fclose(yyin);
}
