/* posfixo.y */
%{
#include <stdio.h>

int yylex(void);
void yyerror(const char *);
%}

%define parse.error verbose

%token IDENT MAIS MENOS ASTERISCO DIV

%%

expr       : expr MAIS termo        {printf ("+"); } 
           | expr MENOS termo       {printf ("-"); }
           | termo
           ;

termo      : termo ASTERISCO fator  {printf ("*"); } 
           | termo DIV fator        {printf ("/"); }
           | fator
           ;

fator      : IDENT                  {printf ("A"); }
           ;

%%

void yyerror(const char * msg){
   fprintf(stderr, "ERRO: %s ->", msg);
}

int main (int argc, char** argv) {
   yyparse();
   printf("\n");
}
