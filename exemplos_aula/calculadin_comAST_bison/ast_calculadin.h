#ifndef AST_CALCULADIN_H
#define AST_CALCULADIN_H

#include <stdio.h>

// Nó expressão
typedef enum { EXPR_NUM, EXPR_ID, EXPR_BIN } TipoExpr;

typedef struct Expr {
    TipoExpr tipo;
    union {
        float num;          // EXPR_NUM
        char* id;            // EXPR_ID 
        struct {             // EXPR_BIN 
            int op;          // '+', '-', '*', '/' 
            struct Expr* esq;
            struct Expr* dir;
        } bin;
    } u;
} Expr;

// Nó Comando 
typedef enum { CMD_ATRIB, CMD_INPUT, CMD_OUTPUT } TipoCmd;

typedef struct Comando {
    TipoCmd tipo;
    union {
        struct { char* nome; Expr* expr; } atrib;     // id = expr
        struct { char* nome; } io;                    // input/output
    } u;
    struct Comando* prox; /* para encadear na lista */
} Comando;

// Nó Programa 
typedef struct Programa {
    Comando* comandos;  /* lista encadeada */
} Programa;

// Funções construtoras dos nós
Expr* expr_num(float valor);
Expr* expr_id(char* nome);
Expr* expr_bin(int op, Expr* esq, Expr* dir);

Comando* cmd_atrib(char* nome, Expr* expr);
Comando* cmd_input(char* nome);
Comando* cmd_output(char* nome);

// Função para adicionar comando no final da lista de comandos
Comando* adiciona_cmd(Comando* lista, Comando* novo);

Programa* programa(Comando* lista);

// Funções de impressão 
void expr_print(const Expr* e, FILE* out);
void cmd_print(const Comando* c, FILE* out);
void prog_print(const Programa* p, FILE* out);
void ast_print(const Programa* raiz_ast, FILE* out);

// Funções de liberação de memória
void expr_free(Expr* e);
void cmd_free(Comando* c);
void prog_free(Programa* p);
void ast_free(Programa* raiz_ast);

extern Programa* raiz_ast;

#endif /* AST_H */
