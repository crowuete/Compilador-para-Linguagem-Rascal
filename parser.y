%{
#include <stdio.h>
#include <string.h>

int yylex(void);
void yyerror(const char *);

/* Tipos semânticos */
typedef enum { T_INT, T_BOOL } Tipo;

/* Auxiliar para retornar o nome do tipo como string */
static const char* nomeTipo (Tipo t) {
   switch(t){
      case T_INT:  return "int";
      case T_BOOL: return "bool";
      default:     return "erro";
   }
}
%}

%define parse.error verbose

%union{
   int ival;
   char* sval;
   int tipo;
}

%token <sval> ID
%token <ival> NUM
%token PROCEDURE FUNCTION KW_BEGIN END
%token VAR IF THEN ELSE WHILE DO READ WRITE 
%token INTEGER BOOLEAN TRUE FALSE NOT OR 
%token AND DIV ATRIB IGUAL DIF MENOR MAIOR
%token MENOR_IGUAL MAIOR_IGUAL TK_PROGRAM

%type <tipo> programa bloco tipo_var expressao expressao_simples termo fator

%%

/* ---------------------------------------------- */
/* PROGRAMA E BLOCO */
/* ---------------------------------------------- */

programa      
    : TK_PROGRAM ID ';' bloco '.'
    ;

bloco         
    : secao_declaracao_var_opcional
      secao_declaracao_subrotinas_opcional
      comando_composto
    ;

/* ---------------------------------------------- */
/* SEÇÃO DE VARIÁVEIS */
/* ---------------------------------------------- */

secao_declaracao_var_opcional         
    : secao_declaracao_var
    | /* vazio */
    ;

secao_declaracao_var          
    : VAR declaracao_var ';'  
    | secao_declaracao_var declaracao_var ';'
    ;

declaracao_var         
    : lista_id ':' tipo_var
    ;

lista_id         
    : ID
    | lista_id ',' ID
    ;

tipo_var         
    : BOOLEAN
    | INTEGER
    ;

/* ---------------------------------------------- */
/* SEÇÃO DE SUBROTINAS */
/* ---------------------------------------------- */

secao_declaracao_subrotinas_opcional   
    : secao_declaracao_subrotinas
    | /* vazio */
    ;

secao_declaracao_subrotinas 
    : declaracao_subrotina ';'
    | secao_declaracao_subrotinas declaracao_subrotina ';'
    ;

declaracao_subrotina
    : declaracao_procedimento
    | declaracao_funcao
    ;

declaracao_procedimento
    : PROCEDURE ID parametros_formais_opcional ';' bloco_subrot
    ;

declaracao_funcao
    : FUNCTION ID parametros_formais_opcional ':' tipo_var ';' bloco_subrot
    ;

parametros_formais_opcional
    : parametros_formais
    | /* vazio */
    ;

parametros_formais
    : '(' declaracao_parametros ')'
    | '(' declaracao_parametros_lista ')'
    ;

declaracao_parametros_lista
    : declaracao_parametros ';' declaracao_parametros
    | declaracao_parametros_lista ';' declaracao_parametros
    ;

declaracao_parametros
    : lista_id ':' tipo_var
    ;

bloco_subrot
    : secao_declaracao_var_opcional comando_composto
    ;

/* ---------------------------------------------- */
/* COMANDOS */
/* ---------------------------------------------- */

comando_composto
    : KW_BEGIN comando_lista END
    ;

comando_lista
    : comando
    | comando_lista ';' comando
    ;

comando
    : atribuicao
    | chamada_procedimento
    | condicional
    | repeticao
    | leitura
    | escrita
    | comando_composto
    ;

atribuicao
    : ID ATRIB expressao
    ;

chamada_procedimento
    : ID
    | ID '(' lista_exp ')'
    ;

condicional
    : IF expressao THEN comando
    | IF expressao THEN comando ELSE comando
    ;

repeticao
    : WHILE expressao DO comando
    ;

leitura
    : READ '(' lista_id ')'
    ;

escrita
    : WRITE '(' lista_exp ')'
    ;

/* ---------------------------------------------- */
/* EXPRESSÕES */
/* ---------------------------------------------- */

lista_exp
    : expressao
    | lista_exp ',' expressao
    ;

expressao
    : expressao_simples
    | expressao_simples relacao expressao_simples
    ;

relacao
    : IGUAL
    | DIF
    | MENOR
    | MENOR_IGUAL
    | MAIOR
    | MAIOR_IGUAL
    ;

expressao_simples
    : termo
    | expressao_simples '+' termo
    | expressao_simples '-' termo
    | expressao_simples OR termo
    ;

termo
    : fator
    | termo '*' fator
    | termo DIV fator
    | termo AND fator
    ;

fator
    : ID
    | NUM
    | TRUE
    | FALSE
    | chamada_funcao
    | '(' expressao ')'
    | NOT fator
    | '-' fator
    ;

chamada_funcao
    : ID
    | ID '(' lista_exp ')'
    ;

%%

void yyerror(const char * msg){
   fprintf(stderr, "ERRO: %s\n", msg);
}

int main (int argc, char** argv) {
   yyparse();
   printf("\n");
   return 0;
}