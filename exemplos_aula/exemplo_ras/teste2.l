%{
enum {TOK_PROGRAM=256, TOK_NUMERO, TOK_ID, TOK_END, TOK_ERRO};
char lexema[100];
int linha = 1;
%}

%option noyywrap

quebra      [\n]
id          [a-zA-Z][a-zA-Z0-9_]*
digito      [0-9]
espaco      [ \t]

%%

{quebra}    { linha++; }
"+"         { strcpy(lexema, yytext); return '+'; }
"-"         { strcpy(lexema, yytext); return '-'; }
"*"         { strcpy(lexema, yytext); return '*'; }
"/"         { strcpy(lexema, yytext); return '/'; }
"="         { strcpy(lexema, yytext); return '='; }
"program"   { strcpy(lexema, yytext); return TOK_PROGRAM; }
"end"       { strcpy(lexema, yytext); return TOK_END; }
{digito}+   { strcpy(lexema, yytext); return TOK_NUMERO; }
{id}        { strcpy(lexema, yytext); return TOK_ID; }
{espaco}+   /* ignora espaços, \t, \n e \r */ 
.           { strcpy(lexema, yytext); return TOK_ERRO; }

%%

int main (int argc, char *argv[]) {
    int tok;
    yyin = fopen(argv[1], "r");
    while (tok=yylex()) {
        printf("<%s, %d>\t(linha: %d)\n", lexema, tok, linha);
    }
    fclose(yyin);
}