/* posfixo1.y */
%{
#include <stdio.h>
#include <string.h>

int yylex(void);
void yyerror(const char *);
%}

%define parse.error verbose

%union{
   char * sval;
}
%token <sval> IDENT 
%token MAIS MENOS ASTERISCO DIV

%%

expr       : expr MAIS termo  {printf ("+"); }
           | expr MENOS termo {printf ("-"); } 
           | termo
           ;

termo      : termo ASTERISCO fator  {printf ("*"); } 
           | termo DIV fator        {printf ("/"); }
           | fator
           ;

fator      : IDENT {
                     if (strcmp($1, "a") == 0) printf ("A");
                     else printf("B");
                     free($1);
                   }
           ;

%%

void yyerror(const char * msg){
   fprintf(stderr, "ERRO: %s ->", msg);
}

int main (int argc, char** argv) {
   yyparse();
   printf("\n");
}
