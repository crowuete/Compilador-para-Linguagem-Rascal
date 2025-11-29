#include "ast_calculadin.h"
#include <stdlib.h>
#include <string.h>

// Implementação das construtoras dos tipos Expr
Expr* expr_num(float valor) {
    Expr* e = (Expr*)malloc(sizeof(Expr));
    e->tipo = EXPR_NUM;
    e->u.num = valor;
    return e;
}

Expr* expr_id(char* nome) {
    Expr* e = (Expr*)malloc(sizeof(Expr));
    e->tipo = EXPR_ID;
    e->u.id = nome;
    return e;
}

Expr* expr_bin(int op, Expr* esq, Expr* dir) {
    Expr* e = (Expr*)malloc(sizeof(Expr));
    e->tipo = EXPR_BIN;
    e->u.bin.op  = op;
    e->u.bin.esq = esq;
    e->u.bin.dir = dir;
    return e;
}

// Implementação das construtoras dos tipos Cmd
Comando* cmd_atrib(char* nome, Expr* expr) {
    Comando* c = (Comando*)malloc(sizeof(Comando));
    c->tipo = CMD_ATRIB;
    c->u.atrib.nome = nome;
    c->u.atrib.expr = expr;
    c->prox = NULL;
    return c;
}

Comando* cmd_input(char* nome) {
    Comando* c = (Comando*)malloc(sizeof(Comando));
    c->tipo = CMD_INPUT;
    c->u.io.nome = nome;
    c->prox = NULL;
    return c;
}

Comando* cmd_output(char* nome) {
    Comando* c = (Comando*)malloc(sizeof(Comando));
    c->tipo = CMD_OUTPUT;
    c->u.io.nome = nome;
    c->prox = NULL;
    return c;
}

Comando* adiciona_cmd(Comando* lista, Comando* novo) {
    if (!lista) return novo;
    Comando* it = lista;
    while (it->prox) it = it->prox;
    it->prox = novo;
    return lista;
}

// Implementação da construtora do tipo Programa
Programa* programa(Comando* lista) {
    Programa* p = (Programa*)malloc(sizeof(Programa));
    p->comandos = lista;
    return p;
}

// Implementação das funções de impressão
void expr_print(const Expr* e, FILE* out) {
    if (!e) { fprintf(out, "<null>"); return; }
    switch (e->tipo) {
        case EXPR_NUM:
            fprintf(out, "%.6g", e->u.num);
            break;
        case EXPR_ID:
            fprintf(out, "%s", e->u.id);
            break;
        case EXPR_BIN:
            fprintf(out, "(");
            fprintf(out, "%c ", e->u.bin.op);
            expr_print(e->u.bin.esq, out);
            fprintf(out, " ");
            expr_print(e->u.bin.dir, out);
            fprintf(out, ")");
            break;
    }
}

void cmd_print(const Comando* c, FILE* out) {
    for (; c; c = c->prox) {
        switch (c->tipo) {
            case CMD_ATRIB:
                fprintf(out, "(atr %s ", c->u.atrib.nome);
                expr_print(c->u.atrib.expr, out);
                fprintf(out, ")");
                break;
            case CMD_INPUT:
                fprintf(out, "(input %s)", c->u.io.nome);
                break;
            case CMD_OUTPUT:
                fprintf(out, "(output %s)", c->u.io.nome);
                break;
        }
    }
}

void prog_print(const Programa* p, FILE* out) {
    fprintf(out, "(programa ");
    cmd_print(p->comandos, out);
    fprintf(out, ")\n");
}

void ast_print(const Programa* raiz_ast, FILE* out) {
    prog_print(raiz_ast, out);
}

// Implementação das funções de liberação de memória
void expr_free(Expr* e) {
    if (!e) return;
    switch (e->tipo) {
        case EXPR_NUM: break;
        case EXPR_ID:  free(e->u.id); break;
        case EXPR_BIN:
            expr_free(e->u.bin.esq);
            expr_free(e->u.bin.dir);
            break;
    }
    free(e);
}

void cmd_free(Comando* c) {
    while (c) {
        Comando* nxt = c->prox;
        switch (c->tipo) {
            case CMD_ATRIB:
                free(c->u.atrib.nome);
                expr_free(c->u.atrib.expr);
                break;
            case CMD_INPUT:
            case CMD_OUTPUT:
                free(c->u.io.nome);
                break;
        }
        free(c);
        c = nxt;
    }
}

void prog_free(Programa* p) {
    if (!p) return;
    cmd_free(p->comandos);
    free(p);
}

void ast_free(Programa* raiz_ast) {
    prog_free(raiz_ast);
}
