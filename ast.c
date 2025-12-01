#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variável externa (declarada no ast.h)
Programa* raiz_ast = NULL;

// Macro auxiliar para alocação de memória e tratamento de erro
#define ALLOC(type) (type*)safe_malloc(sizeof(type))

static void* safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        perror("Erro ao alocar memória para nó da AST");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// ======================================================================
// FUNÇÕES DE MANIPULAÇÃO DE LISTAS (Auxiliares)
// ======================================================================

// --------------------- IdList ---------------------

IdList* adiciona_id(IdList* lista, char* nome) {
    IdList *novo = ALLOC(IdList);
    novo->nome = strdup(nome);
    novo->prox = NULL;
    
    if (lista == NULL) {
        return novo;
    }
    
    IdList *temp = lista;
    while (temp->prox != NULL) {
        temp = temp->prox;
    }
    temp->prox = novo;
    return lista;
}

static void idlist_free(IdList* lista) {
    IdList *temp;
    while (lista != NULL) {
        temp = lista;
        lista = lista->prox;
        free(temp->nome);
        free(temp);
    }
}

// --------------------- Expressões (Expr) ---------------------

Expr* adiciona_exp(Expr* lista, Expr* novo) {
    if (lista == NULL) {
        return novo;
    }
    
    Expr *temp = lista;
    while (temp->prox != NULL) {
        temp = temp->prox;
    }
    temp->prox = novo;
    return lista;
}

// --------------------- Comandos (Comando) ---------------------

Comando* adiciona_cmd(Comando* lista, Comando* novo) {
    if (lista == NULL) {
        return novo;
    }
    
    Comando *temp = lista;
    while (temp->prox != NULL) {
        temp = temp->prox;
    }
    temp->prox = novo;
    return lista;
}

// --------------------- ParamDecl ---------------------

ParamDecl* adiciona_param_decl(ParamDecl* lista, ParamDecl* novo) {
    if (lista == NULL) {
        return novo;
    }
    
    ParamDecl *temp = lista;
    while (temp->prox != NULL) {
        temp = temp->prox;
    }
    temp->prox = novo;
    return lista;
}

static void paramdecl_free(ParamDecl* lista) {
    ParamDecl *temp;
    while (lista != NULL) {
        temp = lista;
        lista = lista->prox;
        idlist_free(temp->ids);
        free(temp);
    }
}


// --------------------- Declarações (Decl) ---------------------

Decl* adiciona_decl(Decl* lista, Decl* novo) {
    if (lista == NULL) {
        return novo;
    }
    
    Decl *temp = lista;
    while (temp->prox != NULL) {
        temp = temp->prox;
    }
    temp->prox = novo;
    return lista;
}

// ======================================================================
// 1. CONSTRUTORES DE NÓS
// ======================================================================

// --------------------- EXPRESSÕES (Expr) ---------------------

Expr* expr_num(int valor) {
    Expr *e = ALLOC(Expr);
    e->tipo = EXPR_NUM;
    e->tipo_semantico = T_INT;
    e->u.ival = valor;
    e->prox = NULL;
    return e;
}

Expr* expr_bool(int valor) {
    Expr *e = ALLOC(Expr);
    e->tipo = EXPR_BOOL;
    e->tipo_semantico = T_BOOL;
    e->u.ival = (valor != 0); // Armazena 1 para TRUE, 0 para FALSE
    e->prox = NULL;
    return e;
}

Expr* expr_id(char* nome) {
    Expr *e = ALLOC(Expr);
    e->tipo = EXPR_VAR;
    e->tipo_semantico = T_VOID; // Tipo inferido na análise semântica
    e->u.id = strdup(nome);
    e->prox = NULL;
    return e;
}

Expr* expr_bin(int op, Expr* esq, Expr* dir) {
    Expr *e = ALLOC(Expr);
    e->tipo = EXPR_BIN;
    e->tipo_semantico = T_VOID; // Tipo inferido na análise semântica
    e->u.bin.op = op;
    e->u.bin.esq = esq;
    e->u.bin.dir = dir;
    e->prox = NULL;
    return e;
}

Expr* expr_un(int op, Expr* arg) {
    Expr *e = ALLOC(Expr);
    e->tipo = EXPR_UN;
    e->tipo_semantico = T_VOID; // Tipo inferido na análise semântica
    e->u.un.op = op;
    e->u.un.arg = arg;
    e->prox = NULL;
    return e;
}

Expr* expr_call_func(char* nome, Expr* args_lista) {
    Expr *e = ALLOC(Expr);
    e->tipo = EXPR_CALL_FUNC;
    e->tipo_semantico = T_VOID; // Tipo inferido na análise semântica
    e->u.func.nome = strdup(nome);
    e->u.func.args_lista = args_lista;
    e->prox = NULL;
    return e;
}

// --------------------- COMANDOS (Comando) ---------------------

Comando* cmd_atrib(char* nome_var, Expr* expr) {
    Comando *c = ALLOC(Comando);
    c->tipo = CMD_ATRIB;
    c->u.atrib.nome_var = strdup(nome_var);
    c->u.atrib.expr = expr;
    c->prox = NULL;
    return c;
}

Comando* cmd_if(Expr* cond, Comando* then_cmd, Comando* else_cmd) {
    Comando *c = ALLOC(Comando);
    c->tipo = CMD_IF;
    c->u.cond.cond = cond;
    c->u.cond.then_cmd = then_cmd;
    c->u.cond.else_cmd = else_cmd; // Pode ser NULL
    c->prox = NULL;
    return c;
}

Comando* cmd_while(Expr* cond, Comando* body) {
    Comando *c = ALLOC(Comando);
    c->tipo = CMD_WHILE;
    c->u.loop.cond = cond;
    c->u.loop.body = body;
    c->prox = NULL;
    return c;
}

Comando* cmd_read(IdList* lista_id) {
    Comando *c = ALLOC(Comando);
    c->tipo = CMD_READ;

    c->u.leitura.lista_id = lista_id;
    c->prox = NULL;
    return c;
}

Comando* cmd_write(Expr* lista_exp) {
    Comando *c = ALLOC(Comando);
    c->tipo = CMD_WRITE;
    c->u.escrita.lista_exp = lista_exp;
    c->prox = NULL;
    return c;
}

Comando* cmd_call_proc(char* nome, Expr* args_lista) {
    Comando *c = ALLOC(Comando);
    c->tipo = CMD_CALL_PROC;
    c->u.proc_call.nome = strdup(nome);
    c->u.proc_call.args_lista = args_lista;
    c->prox = NULL;
    return c;
}

Comando* cmd_composto(Bloco* bloco) {
    Comando *c = ALLOC(Comando);
    c->tipo = CMD_COMPOSTO;
    c->u.composto = bloco;
    c->prox = NULL;
    return c;
}

// --------------------- DECLARAÇÕES (Decl) ---------------------

Decl* decl_var(IdList* lista_id, TipoSemantico tipo) {
    Decl *d = ALLOC(Decl);
    d->tipo = DECL_VAR;
    d->u.var.ids = lista_id;
    d->u.var.tipo_var = tipo;
    d->prox = NULL;
    return d;
}

Decl* decl_procedure(char* nome, ParamDecl* params, Bloco* bloco) {
    Decl *d = ALLOC(Decl);
    d->tipo = DECL_PROCEDURE;
    d->u.subrot.nome = strdup(nome);
    d->u.subrot.params = params;
    d->u.subrot.bloco = bloco;
    d->u.subrot.tipo_retorno = T_VOID; // Procedimentos são sempre VOID
    d->prox = NULL;
    return d;
}

Decl* decl_function(char* nome, ParamDecl* params, TipoSemantico tipo_retorno, Bloco* bloco) {
    Decl *d = ALLOC(Decl);
    d->tipo = DECL_FUNCTION;
    d->u.subrot.nome = strdup(nome);
    d->u.subrot.params = params;
    d->u.subrot.bloco = bloco;
    d->u.subrot.tipo_retorno = tipo_retorno;
    d->prox = NULL;
    return d;
}

// --------------------- SUB-ESTRUTURAS (ParamDecl e Bloco) ---------------------

ParamDecl* param_decl(IdList* ids, TipoSemantico tipo) {
    ParamDecl *p = ALLOC(ParamDecl);
    p->ids = ids;
    p->tipo_param = tipo;
    p->prox = NULL;
    return p;
}

Bloco* criar_bloco(Decl* decls_var, Decl* decls_subrotinas, Comando* comandos) {
    Bloco *b = ALLOC(Bloco);
    b->decls_var = decls_var;
    b->decls_subrotinas = decls_subrotinas;
    b->comandos = comandos;
    return b;
}

// --------------------- RAIZ (Programa) ---------------------

Programa* criar_programa(char* nome, Bloco* bloco_principal) {
    Programa *p = ALLOC(Programa);
    p->nome = strdup(nome);
    p->bloco_principal = bloco_principal;
    // Define a raiz global para a semântica/liberação
    raiz_ast = p;
    return p;
}


// ======================================================================
// 2. FUNÇÕES DE LIBERAÇÃO DE MEMÓRIA
// ======================================================================

// Funções internas de liberação
static void decl_free_internal(Decl* d);
static void bloco_free(Bloco* b);


void expr_free(Expr* e) {
    if (e == NULL) return;
    
    // Libera a lista encadeada (argumentos ou lista_exp)
    expr_free(e->prox);
    
    switch (e->tipo) {
        case EXPR_VAR:
            free(e->u.id);
            break;
        case EXPR_CALL_FUNC:
            free(e->u.func.nome);
            expr_free(e->u.func.args_lista);
        break;
        case EXPR_BIN:
            // Libera as sub-expressões
            expr_free(e->u.bin.esq);
            expr_free(e->u.bin.dir);
            break;
        case EXPR_UN:
            // Libera o argumento
            expr_free(e->u.un.arg);
            break;
        case EXPR_NUM:
        case EXPR_BOOL:
            // Nada para liberar dentro da união
            break;
        default:
            break;
    }
    
    free(e);
}

void cmd_free(Comando* c) {
    if (c == NULL) return;
    
    // Libera o próximo comando na lista
    cmd_free(c->prox);
    
    switch (c->tipo) {
        case CMD_ATRIB:
            free(c->u.atrib.nome_var);
            expr_free(c->u.atrib.expr);
            break;
        case CMD_IF:
            expr_free(c->u.cond.cond);
            cmd_free(c->u.cond.then_cmd);
            cmd_free(c->u.cond.else_cmd); // Pode ser NULL
            break;
        case CMD_WHILE:
            expr_free(c->u.loop.cond);
            cmd_free(c->u.loop.body);
            break;
        case CMD_READ:
            idlist_free(c->u.leitura.lista_id); // Lista de EXPR_VAR
            break;
        case CMD_WRITE:
            expr_free(c->u.escrita.lista_exp);
            break;
        case CMD_CALL_PROC:
            free(c->u.proc_call.nome);
            expr_free(c->u.proc_call.args_lista);
            break;
        case CMD_COMPOSTO:
            bloco_free(c->u.composto);
            break;
        default:
            break;
    }
    
    free(c);
}

static void decl_free_internal(Decl* d) {
    if (d == NULL) return;
    
    // Libera a próxima declaração na lista
    decl_free_internal(d->prox);
    
    switch (d->tipo) {
        case DECL_VAR:
            idlist_free(d->u.var.ids);
            break;
        case DECL_PROCEDURE:
        case DECL_FUNCTION:
            free(d->u.subrot.nome);
            paramdecl_free(d->u.subrot.params);
            bloco_free(d->u.subrot.bloco);
            break;
        default:
            break;
    }
    
    free(d);
}

static void bloco_free(Bloco* b) {
    if (b == NULL) return;
    
    decl_free_internal(b->decls_var);
    decl_free_internal(b->decls_subrotinas);
    cmd_free(b->comandos);
    
    free(b);
}

void prog_free(Programa* p) {
    // prog_free é o mesmo que ast_free, mas recebe Programa*
    ast_free(p);
}

void ast_free(Programa* raiz_ast) {
    if (raiz_ast == NULL) return;
    
    free(raiz_ast->nome);
    bloco_free(raiz_ast->bloco_principal);
    
    free(raiz_ast);
    // Limpa a variável global após a liberação
    raiz_ast = NULL; 
}