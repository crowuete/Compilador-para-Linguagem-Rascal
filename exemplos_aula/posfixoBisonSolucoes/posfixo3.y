/* posfixo3.y */
%{
#include <stdio.h>
#include <string.h>

int yylex(void);
void yyerror(const char *);

/* Tipos semânticos */
typedef enum { T_INT, T_BOOL} Tipo;

/* Auxiliar para retornar o nome do tipo como string */
static const char* nomeTipo (Tipo t) {
   switch(t){
      case T_INT:  return "int";
      case T_BOOL: return "bool";
      default:     return "erro";
   }
}

/* Auxiliar para imprimir erro semântico */
static void erro_semantico (const char* op, Tipo le, Tipo ld, char * esperado) {
   fprintf(stderr,
           "ERRO SEMÂNTICO: operador '%s' incompatível com operandos (%s, %s). Esperado: %s\n",
           op, nomeTipo(le), nomeTipo(ld), esperado);
}
%}

%define parse.error verbose

%union{
   char* sval;
   int   tipo;
}

%token <sval> IDENT
%token MAIS MENOS ASTERISCO DIV AND OR

%type <tipo> exprl expr termo fator

%%

exprl      : exprl AND expr   {
                                 if ($1 != T_BOOL || $3 != T_BOOL)
                                    erro_semantico("and", (Tipo)$1, (Tipo)$3, "(bool, bool)");
                                 $$ = T_BOOL;
                                 printf(" and "); 
                              }
           | exprl OR expr    {
                                 if ($1 != T_BOOL || $3 != T_BOOL)
                                    erro_semantico("and", (Tipo)$1, (Tipo)$3, "(bool, bool)");
                                 $$ = T_BOOL;
                                 printf(" or "); 
                              }
           | expr
           ;

expr       : expr MAIS termo  {
                                 if ($1 != T_INT || $3 != T_INT)
                                    erro_semantico("+", (Tipo)$1, (Tipo)$3, "(int, int)");
                                 $$ = T_INT;
                                 printf ("+"); 
                              }
           | expr MENOS termo {
                                 if ($1 != T_INT || $3 != T_INT)
                                    erro_semantico("+", (Tipo)$1, (Tipo)$3, "(int, int)");
                                 $$ = T_INT;
                                 printf ("-"); 
                              } 
           | termo
           ;

termo      : termo ASTERISCO fator  {
                                       if ($1 != T_INT || $3 != T_INT)
                                          erro_semantico("*", (Tipo)$1, (Tipo)$3, "(int, int)");
                                       $$ = T_INT;
                                       printf ("*"); 
                                    } 
           | termo DIV fator        {
                                       if ($1 != T_INT || $3 != T_INT)
                                          erro_semantico("/", (Tipo)$1, (Tipo)$3, "(int, int)");
                                       $$ = T_INT;
                                       printf ("/"); 
                                    }
           | fator
           ;

fator      : IDENT {
                     if (strcmp($1, "a") == 0)  { $$ = T_INT;  printf("A"); }
                     else                       { $$ = T_BOOL; printf("B"); }
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
