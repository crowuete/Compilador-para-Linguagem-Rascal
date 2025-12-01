#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

// ----------------------------------------------------------------------
// 1. Tipos de Informação de Suporte (Semântica)
// ----------------------------------------------------------------------

// O tipo de dado da linguagem Rascal
typedef enum { T_INT, T_BOOL, T_VOID } TipoSemantico;

// Forward Declarations
typedef struct Expr Expr;
typedef struct Comando Comando;
typedef struct Decl Decl;
typedef struct Bloco Bloco;
typedef struct Programa Programa;
typedef struct ParamDecl ParamDecl;


// ----------------------------------------------------------------------
// 2. EXPRESSÕES (Expr)
// ----------------------------------------------------------------------

typedef enum { 
    EXPR_NUM, 
    EXPR_VAR,      // Uso de ID (variável)
    EXPR_BOOL,     // TRUE/FALSE
    EXPR_BIN,      // Operação binária (+, -, OR, IGUAL, MENOR, etc.)
    EXPR_UN,       // Operação unária (NOT, -)
    EXPR_CALL_FUNC // Chamada de função
} TipoExpr;

struct Expr {
    TipoExpr tipo;
    TipoSemantico tipo_semantico; // Para uso na Análise Semântica
    union {
        int ival;                 // EXPR_NUM (inteiro), EXPR_BOOL (1/0)
        char* id;                 // EXPR_VAR, EXPR_CALL_FUNC (nome)
        
        struct {                  // EXPR_BIN
            int op;               // Token do operador
            Expr* esq;
            Expr* dir;
        } bin;
        
        struct {                  // EXPR_UN
            int op;               // Token do operador (NOT ou -)
            Expr* arg;
        } un;
        
        struct {                  // EXPR_CALL_FUNC
            char* nome;
            Expr* args_lista;     // Lista encadeada de expressões (argumentos)
        } func;
        
    } u;
    Expr* prox; // Usado para encadear listas de argumentos (lista_exp)
};


// ----------------------------------------------------------------------
// 5. PARÂMETROS (ParamDecl)
// ----------------------------------------------------------------------

// Usado para lista_id e para a lista de IDs dentro de ParamDecl
typedef struct IdList {
    char* nome;
    struct IdList* prox;
} IdList;

struct ParamDecl {
    IdList* ids;
    TipoSemantico tipo_param;
    struct ParamDecl* prox; // Lista de declarações de parâmetros (para múltiplos tipos)
};

// ----------------------------------------------------------------------
// 3. COMANDOS (Comando)
// ----------------------------------------------------------------------

typedef enum { 
    CMD_ATRIB, 
    CMD_IF, 
    CMD_WHILE,
    CMD_READ, 
    CMD_WRITE,
    CMD_CALL_PROC,
    CMD_COMPOSTO    // begin...end (um Bloco de comandos)
} TipoCmd;

struct Comando {
    TipoCmd tipo;
    union {
        struct { char* nome_var; Expr* expr; } atrib; 
        
        struct { Expr* cond; Comando* then_cmd; Comando* else_cmd; } cond; // IF/IF_ELSE
        
        struct { Expr* cond; Comando* body; } loop; // WHILE
        
        struct { Expr* lista_exp; } escrita; // WRITE (lista de expressões)
        
        struct { IdList* lista_id; } leitura; // READ (lista de IDs, representadas como EXPR_VAR)
        
        struct { char* nome; Expr* args_lista; } proc_call; // Chamada de procedimento
        
        Bloco* composto; // CMD_COMPOSTO aponta para a estrutura Bloco
        
    } u;
    struct Comando* prox; // Para encadear na lista de comandos (comando_lista)
};


// ----------------------------------------------------------------------
// 4. DECLARAÇÕES (Decl)
// ----------------------------------------------------------------------

typedef enum { 
    DECL_VAR, 
    DECL_PROCEDURE, 
    DECL_FUNCTION 
} TipoDecl;

struct Decl {
    TipoDecl tipo;
    Decl* prox; // Lista de declarações (var ou sub-rotinas)
    
    union {
        struct { // DECL_VAR
            IdList* ids;
            TipoSemantico tipo_var;
        } var;
        
        struct { // DECL_PROCEDURE / DECL_FUNCTION
            char* nome;
            ParamDecl* params; // Lista de parâmetros
            Bloco* bloco;
            TipoSemantico tipo_retorno; // Apenas para FUNCTION
        } subrot;
    } u;
};


// ----------------------------------------------------------------------
// 6. BLOCO (Bloco)
// ----------------------------------------------------------------------

struct Bloco {
    Decl* decls_var;
    Decl* decls_subrotinas;
    Comando* comandos; // Lista encadeada de comandos
};


// ----------------------------------------------------------------------
// 7. PROGRAMA (Programa)
// ----------------------------------------------------------------------

struct Programa {
    char* nome;
    Bloco* bloco_principal;
};

// ----------------------------------------------------------------------
// 8. Funções Construtoras (Serão implementadas no ast.c)
// ----------------------------------------------------------------------

// Expressões
Expr* expr_num(int valor);
Expr* expr_bool(int valor);
Expr* expr_id(char* nome); // Usado para variáveis e lista de IDs em READ/WRITE
Expr* expr_bin(int op, Expr* esq, Expr* dir);
Expr* expr_un(int op, Expr* arg);
Expr* expr_call_func(char* nome, Expr* args_lista);

// Listas (Expressões e Comandos)
Expr* adiciona_exp(Expr* lista, Expr* novo);
Comando* adiciona_cmd(Comando* lista, Comando* novo);
IdList* adiciona_id(IdList* lista, char* nome);

// Comandos
Comando* cmd_atrib(char* nome_var, Expr* expr);
Comando* cmd_if(Expr* cond, Comando* then_cmd, Comando* else_cmd);
Comando* cmd_while(Expr* cond, Comando* body);
Comando* cmd_read(IdList* lista_id);
Comando* cmd_write(Expr* lista_exp);
Comando* cmd_call_proc(char* nome, Expr* args_lista);
Comando* cmd_composto(Bloco* bloco);

// Declarações
Decl* decl_var(IdList* lista_id, TipoSemantico tipo);
Decl* decl_procedure(char* nome, ParamDecl* params, Bloco* bloco);
Decl* decl_function(char* nome, ParamDecl* params, TipoSemantico tipo_retorno, Bloco* bloco);
Decl* adiciona_decl(Decl* lista, Decl* novo);

// Sub-estruturas
ParamDecl* param_decl(IdList* ids, TipoSemantico tipo);
ParamDecl* adiciona_param_decl(ParamDecl* lista, ParamDecl* novo);
Bloco* criar_bloco(Decl* decls_var, Decl* decls_subrotinas, Comando* comandos);

// Raiz
Programa* criar_programa(char* nome, Bloco* bloco_principal);

// Funções de Liberação de Memória
void expr_free(Expr* e);
void cmd_free(Comando* c);
void prog_free(Programa* p);
void ast_free(Programa* raiz_ast);

extern Programa* raiz_ast;

// ----------------------------------------------------------------------
// 9. Funções de Impressão da AST (Debug)
// ----------------------------------------------------------------------

char* tipo_semantico_to_string(TipoSemantico tipo);
char* token_to_string(int token_op);

void ast_print_program(Programa* p);
void ast_print_bloco(Bloco* b, int indent);
void ast_print_decls(Decl* d, int indent);
void ast_print_cmds(Comando* c, int indent);
void ast_print_expr(Expr* e, int indent);
void ast_print_param_decl(ParamDecl* p, int indent);

#endif