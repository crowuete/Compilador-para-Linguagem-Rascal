%{
enum {TOK_PROGRAM=256, TOK_NUMERO, TOK_ID, TOK_END, TOK_ENDERECO, TOK_PONTO, TOK_ERRO, TOK_NUM_MALFORMADO};
char lexema[100];
%}

%option yylineno

id         [a-zA-Z][a-zA-Z0-9_]*
digito     [0-9]
espaco     [ \t\n]

%%

"+"                 { strcpy(lexema, yytext); return '+'; }
"-"                 { strcpy(lexema, yytext); return '-'; }
"*"                 { strcpy(lexema, yytext); return '*'; }
"/"                 { strcpy(lexema, yytext); return '/'; }
"="                 { strcpy(lexema, yytext); return '='; }
"program"           { strcpy(lexema, yytext); return TOK_PROGRAM; }
"end"               { strcpy(lexema, yytext); return TOK_END; }
"endereco"          { strcpy(lexema, yytext); return TOK_ENDERECO; }
"."                 { strcpy(lexema, yytext); return TOK_PONTO; }
{digito}+{id}+      { strcpy(lexema, yytext); return TOK_NUM_MALFORMADO; }
{digito}+           { strcpy(lexema, yytext); return TOK_NUMERO; }
{id}                { strcpy(lexema, yytext); return TOK_ID; }
{espaco}+           /* ignora espaços, tabs e quebras de linha */ 
.                   { strcpy(lexema, yytext); return TOK_ERRO; }

%%

// Implementacao obrigatoria, pq eu tirei o %option noyywrap
int yywrap(void) {
    /* Retorna 1 para indicar "fim da entrada" */
    return 1;
}

// Implementacao do main para que o analisador funcione sozinho
// Quando o analisador está integrado com o parser, nao tem main()
int main (int argc, char *argv[]) {
    int tok;
    yyin = fopen(argv[1], "r");
    while (tok=yylex()) {
        printf("<%s, %d>\t(linha: %d)\n", lexema, tok, yylineno);
    }
    fclose(yyin);
}