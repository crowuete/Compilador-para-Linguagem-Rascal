%{
#include <stdio.h>
#include <string.h>
#include <ast.h>

int yylex(void);
void yyerror(const char *);

/* Tipos semânticos */
typedef enum { T_INT, T_BOOL, T_VOID } Tipo;

/* Função auxiliar para converter token para TipoSemantico */
Tipo token_to_tipo(int token) {
    switch (token) {
        case INTEGER: return T_INT;
        case BOOLEAN: return T_BOOL;
        default: return T_VOID; // Tipo inválido
    }
}
%}

%define parse.error verbose

/* Definição de Precedência e Associatividade (para a parte de expressões) */
%left OR
%left AND
%left IGUAL DIF MENOR MENOR_IGUAL MAIOR MAIOR_IGUAL // Operadores de relação
%left '+' '-'
%left '*' '/' DIV
%right NOT

%nonassoc THEN
%nonassoc ELSE

%union{
    int ival;
    char* sval;
    AST* no;
    int tipo_token;
    Tipo tipo_semantico;
    Expr* expr_no;
    Comando* cmd_no;
    Decl* decl_no;
    IdList* id_lista;
    ParamDecl* param_decl_no;
    Bloco* bloco_no;
    Programa* prog_no;
}

/* Tokens terminais (sem valor na união) */
%token PROCEDURE FUNCTION KW_BEGIN END
%token VAR IF WHILE DO READ WRITE 
%token INTEGER BOOLEAN TRUE FALSE ATRIB TK_PROGRAM

/* Tokens terminais (com valor na união) */
%token <sval> ID
%token <ival> NUM

/* Tokens não-terminais (com valor na união) */
%type <prog_no> programa 
%type <bloco_no> bloco bloco_subrot
%type <decl_no> secao_declaracao_var_opcional secao_declaracao_var declaracao_var secao_declaracao_subrotinas_opcional secao_declaracao_subrotinas declaracao_subrotina declaracao_procedimento declaracao_funcao
%type <id_lista> lista_id
%type <tipo_semantico> tipo_var
%type <param_decl_no> parametros_formais_opcional parametros_formais declaracao_parametros declaracao_parametros_lista
%type <cmd_no> comando_composto comando_lista comando atribuicao condicional repeticao leitura escrita chamada_procedimento
%type <expr_no> expressao lista_exp expressao_simples termo fator id_ou_chamada_funcao
%type <tipo_token> relacao // Usado para reter o token (IGUAL, DIF, etc)

%%

/* ---------------------------------------------- */
/* PROGRAMA E BLOCO */
/* ---------------------------------------------- */

programa      
    : TK_PROGRAM ID ';' bloco '.' { $$ = programa($2, $4); }
    ;

bloco         
    : secao_declaracao_var_opcional
      secao_declaracao_subrotinas_opcional
      comando_composto { $$ = criar_bloco($1, $2, $3); }
    ;

/* ---------------------------------------------- */
/* SEÇÃO DE VARIÁVEIS */
/* ---------------------------------------------- */

secao_declaracao_var_opcional         
    : secao_declaracao_var { $$ = $1; }
    | /* vazio */ { $$ = NULL; }
    ;

secao_declaracao_var   
    : VAR declaracao_var ';' { $$ = $2; }
    | secao_declaracao_var declaracao_var ';' { $$ = adiciona_decl($1, $2); }
    ;

declaracao_var         
    : lista_id ':' tipo_var { $$ = decl_var($1, $3); }
    ;

lista_id
    : ID { $$ = adiciona_id(NULL, $1); }
    | lista_id ',' ID { $$ = adiciona_id($1, $3); }
    ;

tipo_var         
    : BOOLEAN { $$ = T_BOOL; }
    | INTEGER { $$ = T_INT; }
    ;

/* ---------------------------------------------- */
/* SEÇÃO DE SUBROTINAS */
/* ---------------------------------------------- */

secao_declaracao_subrotinas_opcional   
    : secao_declaracao_subrotinas { $$ = $1; }
    | /* vazio */ { $$ = NULL; }
    ;

secao_declaracao_subrotinas 
    : declaracao_subrotina ';' { $$ = $1; }
    | secao_declaracao_subrotinas declaracao_subrotina ';' { $$ = adiciona_decl($1, $2); }
    ;

declaracao_subrotina
    : declaracao_procedimento { $$ = $1; }
    | declaracao_funcao { $$ = $1; }
    ;

declaracao_procedimento
    : PROCEDURE ID parametros_formais_opcional ';' bloco_subrot { $$ = decl_procedure($2, $3, $5); }
    ;

declaracao_funcao
    : FUNCTION ID parametros_formais_opcional ':' tipo_var ';' bloco_subrot { $$ = decl_function($2, $3, $5, $7); }
    ;

parametros_formais_opcional
    : parametros_formais { $$ = $1; }
    | /* vazio */ { $$ = NULL; }
    ;

parametros_formais
     : '(' declaracao_parametros_lista ')' { $$ = $2; }
     ;

declaracao_parametros_lista
    : declaracao_parametros  { $$ = $1; }
    | declaracao_parametros_lista ';' declaracao_parametros { $$ = adiciona_param_decl($1, $3); }
    ;

declaracao_parametros
    : lista_id ':' tipo_var { $$ = param_decl($1, $3); }
    ;

bloco_subrot
    : secao_declaracao_var_opcional comando_composto { $$ = criar_bloco($1, NULL, $2); }
    ;

/* ---------------------------------------------- */
/* COMANDOS */
/* ---------------------------------------------- */

comando_composto
    : KW_BEGIN comando_lista END { $$ = cmd_composto(criar_bloco(NULL, NULL, $2)); }
    ;

comando_lista
    : comando { $$ = $1; }
    | comando_lista ';' comando { $$ = adiciona_cmd($1, $3); }
    ;

comando 
    : atribuicao { $$ = $1; }
    | chamada_procedimento { $$ = $1; }
    | condicional { $$ = $1; }
    | repeticao { $$ = $1; }
    | leitura { $$ = $1; }
    | escrita { $$ = $1; }
    | comando_composto { $$ = $1; }
    ;

atribuicao
    : ID ATRIB expressao { $$ = cmd_atrib($1, $3); }
    ;

chamada_procedimento
    : ID { $$ = cmd_call_proc($1, NULL); } // Procedimento sem argumentos
    | ID '(' lista_exp ')' { $$ = cmd_call_proc($1, $3); } // Procedimento com argumentos
    ;

condicional
    : IF expressao THEN comando { $$ = cmd_if($2, $4, NULL); }
    | IF expressao THEN comando ELSE comando { $$ = cmd_if($2, $4, $6); }
    ;

repeticao
    : WHILE expressao DO comando { $$ = cmd_while($2, $4); }
    ;

leitura
    : READ '(' lista_id ')' { $$ = cmd_read($3); }
    ;

escrita
    : WRITE '(' lista_exp ')' { $$ = cmd_write($3); }
    ;

/* ---------------------------------------------- */
/* EXPRESSÕES */
/* ---------------------------------------------- */

lista_exp
    : expressao { $$ = $1;}
    | lista_exp ',' expressao { $$ = adiciona_exp($1, $3); }
    ;

expressao
    : expressao_simples { $$ = $1; }
    | expressao_simples relacao expressao_simples { $$ = expr_bin($2, $1, $3); }
    ;

relacao
    : IGUAL { $$ = IGUAL; }
    | DIF { $$ = DIF; }
    | MENOR { $$ = MENOR; }
    | MENOR_IGUAL { $$ = MENOR_IGUAL; }
    | MAIOR { $$ = MAIOR; }
    | MAIOR_IGUAL { $$ = MAIOR_IGUAL; }
    ;

expressao_simples
    : termo { $$ = $1; }
    | expressao_simples '+' termo { $$ = expr_bin('+', $1, $3); }
    | expressao_simples '-' termo { $$ = expr_bin('-', $1, $3); }
    | expressao_simples OR termo { $$ = expr_bin(OR, $1, $3); }
    ;

termo
    : fator { $$ = $1; }
    | termo '*' fator { $$ = expr_bin('*', $1, $3); }
    | termo DIV fator  { $$ = expr_bin(DIV, $1, $3); }
    | termo AND fator { $$ = expr_bin(AND, $1, $3); }
    ;

fator
    : NUM { $$ = expr_num($1); }
    | TRUE { $$ = expr_bool(1); }
    | FALSE { $$ = expr_bool(0); }
    | id_ou_chamada_funcao { $$ = $1; }
    | '(' expressao ')' { $$ = $2; }
    | NOT fator { $$ = expr_un(NOT, $2); }
    | '-' fator { $$ = expr_un('-', $2); }
    ;

id_ou_chamada_funcao
    : ID { $$ = expr_id($1); }
    | ID '(' lista_exp ')' { $$ = expr_call_func($1, $3); }
    ;

%%

void yyerror(const char * msg){
   fprintf(stderr, "ERRO SINTÁTICO na linha %d: %s\n", yylineno, msg);
}

int main (int argc, char** argv) {
   yyparse();
   printf("\n");
   return 0;
}