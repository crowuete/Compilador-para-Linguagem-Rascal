%{
#include <stdio.h>
#include "ast_calculadin.h"     /* para incluir no calculadin.tab.c */

int yylex(void);
void yyerror(const char* msg);
extern int yylineno;

Programa* raiz_ast = NULL;
%}

%code requires {
  #include "ast_calculadin.h"    /* para incluir no calculadin.tab.h */
}

%define parse.error verbose

%union {
    float   dval;
    char*    sval;
    Expr*    expr;
    Comando* cmd;     /* um comando */
    Comando* clist;   /* lista de comandos */
    Programa* prog;
}

%token INICIAR FINALIZAR CALCULADIN VAR INPUT OUTPUT
%token <sval> ID
%token <dval> NUM

%type <expr> expr
%type <cmd>  comando
%type <clist> lista_comandos
%type <prog> programa

%left '+' '-'
%left '*' '/'

%%

programa
    : INICIAR CALCULADIN ':' lista_comandos FINALIZAR CALCULADIN '.'
      { $$ = programa($4); 
        raiz_ast = $$;
      }
    ;

lista_comandos
    : lista_comandos comando ';'   { $$ = adiciona_cmd($1, $2); }
    | /* vazio */                  { $$ = NULL; }
    ;

comando
    : ID '=' expr                  { $$ = cmd_atrib($1, $3); }
    | INPUT '(' ID ')'             { $$ = cmd_input($3); }
    | OUTPUT '(' ID ')'            { $$ = cmd_output($3); }
    ;

expr
    : expr '+' expr                { $$ = expr_bin('+', $1, $3); }
    | expr '-' expr                { $$ = expr_bin('-', $1, $3); }
    | expr '*' expr                { $$ = expr_bin('*', $1, $3); }
    | expr '/' expr                { $$ = expr_bin('/', $1, $3); }
    | '(' expr ')'                 { $$ = $2; }
    | NUM                          { $$ = expr_num($1); }
    | ID                           { $$ = expr_id($1); }
    ;

%%

void yyerror(const char* msg) {
    printf("ERRO SINTÁTICO na linha %d: %s\n", yylineno, msg);
}
