/* calc.y com AST */
%{
#include <stdio.h>
#include <stdlib.h>

/* Tipos de nós da AST */
typedef enum {
  NO_NUM, NO_NUM_NEG,
  NO_SOMA, NO_SUB, NO_MUL, NO_DIV
} TipoNo;

/* Nó da AST */
typedef struct No {
  TipoNo tipo;
  struct No *esq, *dir;     /* operadores binários usam 'esq' e 'dir'; NO_NUM_NEG usa só 'esq' */
  double valor;             /* 'valor' é usado só por NO_NUM */
} No;

/* Construtoras dos nós da AST */
static No* num(double v) {
  No* n = (No*)malloc(sizeof(No));
  n->tipo = NO_NUM; n->esq = n->dir = NULL; n->valor = v;
  return n;
}
static No* op_bin(TipoNo k, No* l, No* r) {
  No* n = (No*)malloc(sizeof(No));
  n->tipo = k; n->esq = l; n->dir = r; n->valor = 0.0;
  return n;
}
static No* num_neg(TipoNo k, No* c) {
  No* n = (No*)malloc(sizeof(No));
  n->tipo = k; n->esq = c; n->dir = NULL; n->valor = 0.0;
  return n;
}

/* Função para imprimir a AST */
static void imprime_ast(const No* n) {
  if (!n) { printf("()"); return; }
  switch (n->tipo) {
    case NO_NUM: printf("%g", n->valor); break;
    case NO_SOMA: printf("(+ ");  imprime_ast(n->esq); printf(" "); imprime_ast(n->dir); printf(")"); break;
    case NO_SUB: printf("(- ");  imprime_ast(n->esq); printf(" "); imprime_ast(n->dir); printf(")"); break;
    case NO_MUL: printf("(* ");  imprime_ast(n->esq); printf(" "); imprime_ast(n->dir); printf(")"); break;
    case NO_DIV: printf("(/ ");  imprime_ast(n->esq); printf(" "); imprime_ast(n->dir); printf(")"); break;
    case NO_NUM_NEG: printf("(- ");  imprime_ast(n->esq); printf(")"); break;
  }
}

/* Função para liberar a memória ocupada pela AST */
static void libera_ast(No* n) {
  if (!n) return;
  libera_ast(n->esq);
  libera_ast(n->dir);
  free(n);
}

int yylex(void);
int yyerror(const char*);

%}

%code requires {
  typedef struct No No;
}


%union {
  double val;
  No*  no;
}

%token <val> NUM
%type  <no> expr

%left '+' '-'
%left '*' '/'
%right NEG

%%

linhas
    : linhas expr '\n'       { imprime_ast($2); printf("\n"); libera_ast($2); }
    | linhas '\n'
    | /* vazio */
    ;

expr
    : expr '+' expr         { $$ = op_bin(NO_SOMA, $1, $3); }
    | expr '-' expr         { $$ = op_bin(NO_SUB, $1, $3); }
    | expr '*' expr         { $$ = op_bin(NO_MUL, $1, $3); }
    | expr '/' expr         { $$ = op_bin(NO_DIV, $1, $3); }
    | '(' expr ')'          { $$ = $2; }
    | '-' expr %prec NEG    { $$ = num_neg(NO_NUM_NEG, $2); }
    | NUM                   { $$ = num($1); }
    ;

%%

/* Mensagens de erro sintático do Bison */
int yyerror(const char* s) {
  printf("%s", s);
  return 0;
}

/* main: aceita múltiplas linhas */
int main(void) {
  return yyparse();
}
