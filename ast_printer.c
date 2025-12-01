#include "ast.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função auxiliar para imprimir a indentação
static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  "); // 2 espaços por nível
    }
}

// ======================================================================
// 1. Funções de Suporte (Strings)
// ======================================================================

// Converte TipoSemantico para string
char* tipo_semantico_to_string(TipoSemantico tipo) {
    switch (tipo) {
        case T_INT: return "integer";
        case T_BOOL: return "boolean";
        case T_VOID: return "void/n.a.";
        default: return "UNKNOWN_TYPE";
    }
}

// Converte token de operador para string
char* token_to_string(int token_op) {
    switch (token_op) {
        case '+': return "+";
        case '-': return "-";
        case '*': return "*";
        case DIV: return "div";
        case OR: return "or";
        case AND: return "and";
        case NOT: return "not";
        case IGUAL: return "=";
        case DIF: return "<>";
        case MENOR: return "<";
        case MENOR_IGUAL: return "<=";
        case MAIOR: return ">";
        case MAIOR_IGUAL: return ">=";
        default: return "UNKNOWN_OP";
    }
}

// Função auxiliar para imprimir uma lista de IDs
static void print_idlist(IdList* lista) {
    while (lista != NULL) {
        printf("%s", lista->nome);
        lista = lista->prox;
        if (lista != NULL) {
            printf(", ");
        }
    }
}


// ======================================================================
// 2. Impressão de Expressões
// ======================================================================

void ast_print_expr(Expr* e, int indent) {
    if (e == NULL) {
        print_indent(indent);
        printf("EXPR_NULL\n");
        return;
    }
    
    // Imprime o nó atual e seu tipo semântico (para uso futuro)
    print_indent(indent);
    printf("├── EXPR [Tipo Semântico: %s]\n", tipo_semantico_to_string(e->tipo_semantico));
    print_indent(indent + 1);
    
    switch (e->tipo) {
        case EXPR_NUM:
            printf("EXPR_NUM (Valor: %d)\n", e->u.ival);
            break;
        case EXPR_VAR:
            printf("EXPR_VAR (ID: %s)\n", e->u.id);
            break;
        case EXPR_BOOL:
            printf("EXPR_BOOL (Valor: %s)\n", e->u.ival ? "true" : "false");
            break;
        case EXPR_BIN:
            printf("EXPR_BIN (Op: %s)\n", token_to_string(e->u.bin.op));
            print_indent(indent + 2);
            printf("├── Esq:\n");
            ast_print_expr(e->u.bin.esq, indent + 3);
            print_indent(indent + 2);
            printf("└── Dir:\n");
            ast_print_expr(e->u.bin.dir, indent + 3);
            break;
        case EXPR_UN:
            printf("EXPR_UN (Op: %s)\n", token_to_string(e->u.un.op));
            print_indent(indent + 2);
            printf("└── Arg:\n");
            ast_print_expr(e->u.un.arg, indent + 3);
            break;
        case EXPR_CALL_FUNC:
            printf("EXPR_CALL_FUNC (Nome: %s)\n", e->u.func.nome);
            print_indent(indent + 2);
            printf("└── Args Lista:\n");
            // Itera e imprime a lista de argumentos
            Expr* arg = e->u.func.args_lista;
            while (arg != NULL) {
                ast_print_expr(arg, indent + 3);
                arg = arg->prox;
            }
            if (e->u.func.args_lista == NULL) {
                print_indent(indent + 3);
                printf("(vazia)\n");
            }
            break;
    }
}


// ======================================================================
// 3. Impressão de Comandos
// ======================================================================

void ast_print_cmds(Comando* c, int indent) {
    Comando* current = c;
    while (current != NULL) {
        print_indent(indent);
        printf("COMMAND\n");
        print_indent(indent + 1);
        
        switch (current->tipo) {
            case CMD_ATRIB:
                printf("├── CMD_ATRIB (Var: %s)\n", current->u.atrib.nome_var);
                print_indent(indent + 2);
                printf("└── Expr:\n");
                ast_print_expr(current->u.atrib.expr, indent + 3);
                break;
            case CMD_IF:
                printf("├── CMD_IF\n");
                print_indent(indent + 2);
                printf("├── Condição:\n");
                ast_print_expr(current->u.cond.cond, indent + 3);
                print_indent(indent + 2);
                printf("├── Then:\n");
                ast_print_cmds(current->u.cond.then_cmd, indent + 3);
                if (current->u.cond.else_cmd != NULL) {
                    print_indent(indent + 2);
                    printf("└── Else:\n");
                    ast_print_cmds(current->u.cond.else_cmd, indent + 3);
                } else {
                    print_indent(indent + 2);
                    printf("└── Else: (NULL)\n");
                }
                break;
            case CMD_WHILE:
                printf("├── CMD_WHILE\n");
                print_indent(indent + 2);
                printf("├── Condição:\n");
                ast_print_expr(current->u.loop.cond, indent + 3);
                print_indent(indent + 2);
                printf("└── Corpo:\n");
                ast_print_cmds(current->u.loop.body, indent + 3);
                break;
            case CMD_READ:
                printf("├── CMD_READ\n");
                print_indent(indent + 2);
                printf("└── Lista IDs (EXPR_VAR):\n");
                IdList* id_read = current->u.leitura.lista_id;
                 while (id_read != NULL) {
                    ast_print_expr(id_read, indent + 3);
                    id_read = id_read->prox;
                }
                break;
            case CMD_WRITE:
                printf("├── CMD_WRITE\n");
                print_indent(indent + 2);
                printf("└── Lista Expressões:\n");
                Expr* exp_write = current->u.escrita.lista_exp;
                while (exp_write != NULL) {
                    ast_print_expr(exp_write, indent + 3);
                    exp_write = exp_write->prox;
                }
                break;
            case CMD_CALL_PROC:
                printf("├── CMD_CALL_PROC (Nome: %s)\n", current->u.proc_call.nome);
                print_indent(indent + 2);
                printf("└── Args Lista:\n");
                Expr* arg_proc = current->u.proc_call.args_lista;
                while (arg_proc != NULL) {
                    ast_print_expr(arg_proc, indent + 3);
                    arg_proc = arg_proc->prox;
                }
                if (current->u.proc_call.args_lista == NULL) {
                    print_indent(indent + 3);
                    printf("(vazia)\n");
                }
                break;
            case CMD_COMPOSTO:
                printf("└── CMD_COMPOSTO (Bloco begin...end):\n");
                ast_print_bloco(current->u.composto, indent + 2);
                break;
        }
        
        current = current->prox;
        if (current != NULL) {
            print_indent(indent);
            printf("|\n"); // Separador entre comandos sequenciais
        }
    }
}

// ======================================================================
// 4. Impressão de Declarações e Parâmetros
// ======================================================================

void ast_print_param_decl(ParamDecl* p, int indent) {
    ParamDecl* current = p;
    while (current != NULL) {
        print_indent(indent);
        printf("├── PARAM_DECL (Tipo: %s)\n", tipo_semantico_to_string(current->tipo_param));
        print_indent(indent + 1);
        printf("└── IDs: ");
        print_idlist(current->ids);
        printf("\n");
        current = current->prox;
    }
}

void ast_print_decls(Decl* d, int indent) {
    Decl* current = d;
    while (current != NULL) {
        print_indent(indent);
        
        switch (current->tipo) {
            case DECL_VAR:
                printf("├── DECL_VAR (Tipo: %s)\n", tipo_semantico_to_string(current->u.var.tipo_var));
                print_indent(indent + 1);
                printf("└── IDs: ");
                print_idlist(current->u.var.ids);
                printf("\n");
                break;
            case DECL_PROCEDURE:
                printf("├── DECL_PROCEDURE (Nome: %s)\n", current->u.subrot.nome);
                print_indent(indent + 1);
                printf("├── Parâmetros:\n");
                ast_print_param_decl(current->u.subrot.params, indent + 2);
                if (current->u.subrot.params == NULL) {
                     print_indent(indent + 2);
                     printf("(nenhum)\n");
                }
                print_indent(indent + 1);
                printf("└── Bloco:\n");
                ast_print_bloco(current->u.subrot.bloco, indent + 2);
                break;
            case DECL_FUNCTION:
                printf("├── DECL_FUNCTION (Nome: %s, Retorno: %s)\n", 
                       current->u.subrot.nome, tipo_semantico_to_string(current->u.subrot.tipo_retorno));
                print_indent(indent + 1);
                printf("├── Parâmetros:\n");
                ast_print_param_decl(current->u.subrot.params, indent + 2);
                if (current->u.subrot.params == NULL) {
                     print_indent(indent + 2);
                     printf("(nenhum)\n");
                }
                print_indent(indent + 1);
                printf("└── Bloco:\n");
                ast_print_bloco(current->u.subrot.bloco, indent + 2);
                break;
        }
        
        current = current->prox;
    }
}

// ======================================================================
// 5. Impressão de Bloco
// ======================================================================

void ast_print_bloco(Bloco* b, int indent) {
    if (b == NULL) {
        print_indent(indent);
        printf("BLOCO_NULL\n");
        return;
    }
    
    print_indent(indent);
    printf("BLOCO\n");
    
    print_indent(indent + 1);
    printf("├── Declarações de Variáveis:\n");
    if (b->decls_var != NULL) {
        ast_print_decls(b->decls_var, indent + 2);
    } else {
        print_indent(indent + 2);
        printf("(nenhuma)\n");
    }
    
    print_indent(indent + 1);
    printf("├── Declarações de Sub-rotinas:\n");
    if (b->decls_subrotinas != NULL) {
        ast_print_decls(b->decls_subrotinas, indent + 2);
    } else {
        print_indent(indent + 2);
        printf("(nenhuma)\n");
    }
    
    print_indent(indent + 1);
    printf("└── Comandos:\n");
    if (b->comandos != NULL) {
        ast_print_cmds(b->comandos, indent + 2);
    } else {
        print_indent(indent + 2);
        printf("(nenhum)\n");
    }
}

// ======================================================================
// 6. Impressão do Programa (Raiz)
// ======================================================================

void ast_print_program(Programa* p) {
    if (p == NULL) {
        printf("AST Raiz é NULL\n");
        return;
    }
    
    printf("==========================================\n");
    printf("         ÁRVORE SINTÁTICA ABSTRATA        \n");
    printf("==========================================\n");
    
    printf("PROGRAMA (Nome: %s)\n", p->nome);
    printf("└── Bloco Principal:\n");
    ast_print_bloco(p->bloco_principal, 1);
    
    printf("==========================================\n");
}