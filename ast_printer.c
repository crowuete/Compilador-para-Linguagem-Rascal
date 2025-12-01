#include "ast.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ======================================================================
// AUXILIAR
// ======================================================================
static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) printf("  ");
}

char* tipo_semantico_to_string(TipoSemantico tipo) {
    switch (tipo) {
        case T_INT: return "integer";
        case T_BOOL: return "boolean";
        case T_VOID: return "void/n.a.";
        default: return "UNKNOWN_TYPE";
    }
}

char* token_to_string(int token_op) {
    switch (token_op) {
        case '+': return "+";
        case '-': return "-";
        case '*': return "*";
        case DIV: return "div";
        case OR:  return "or";
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

// Apenas imprime lista sem recursões duplicadas
static void print_idlist(IdList *l) {
    while (l) {
        printf("%s", l->nome);
        l = l->prox;
        if (l) printf(", ");
    }
}


// ======================================================================
// EXPRESSÕES
// ======================================================================
void ast_print_expr(Expr* e, int indent) {
    if (!e) {
        print_indent(indent);
        printf("EXPR_NULL\n");
        return;
    }

    print_indent(indent);
    printf("EXPR [Tipo: %s] - ", tipo_semantico_to_string(e->tipo_semantico));

    switch (e->tipo) {

        case EXPR_NUM:
            printf("NUM %d\n", e->u.ival);
            break;

        case EXPR_BOOL:
            printf("BOOL %s\n", e->u.ival ? "true" : "false");
            break;

        case EXPR_VAR:
            printf("VAR %s\n", e->u.id);
            break;

        case EXPR_BIN:
            printf("BINOP %s\n", token_to_string(e->u.bin.op));

            print_indent(indent + 1);
            printf("Esq:\n");
            ast_print_expr(e->u.bin.esq, indent + 2);

            print_indent(indent + 1);
            printf("Dir:\n");
            ast_print_expr(e->u.bin.dir, indent + 2);
            break;

        case EXPR_UN:
            printf("UNOP %s\n", token_to_string(e->u.un.op));
            print_indent(indent + 1);
            printf("Arg:\n");
            ast_print_expr(e->u.un.arg, indent + 2);
            break;

        case EXPR_CALL_FUNC: {
            printf("CALL %s\n", e->u.func.nome);

            print_indent(indent + 1);
            printf("Args:\n");

            Expr *arg = e->u.func.args_lista;
            if (!arg) {
                print_indent(indent + 2);
                printf("(vazio)\n");
            }

            while (arg) {
                ast_print_expr(arg, indent + 2);
                arg = arg->prox;
            }
        } break;
    }
}


// ======================================================================
// COMANDOS
// ======================================================================
void ast_print_idlist(IdList* ids, int indent) {
    while (ids) {
        print_indent(indent);
        printf("ID: %s\n", ids->nome);
        ids = ids->prox;
    }
}

void ast_print_cmds(Comando* c, int indent) {
    while (c) {
        print_indent(indent);
        printf("COMMAND\n");

        switch (c->tipo) {

            case CMD_ATRIB:
                print_indent(indent + 1);
                printf("ATRIB %s :=\n", c->u.atrib.nome_var);
                ast_print_expr(c->u.atrib.expr, indent + 2);
                break;

            case CMD_IF:
                print_indent(indent + 1);
                printf("IF Cond:\n");
                ast_print_expr(c->u.cond.cond, indent + 2);

                print_indent(indent + 1);
                printf("Then:\n");
                ast_print_cmds(c->u.cond.then_cmd, indent + 2);

                print_indent(indent + 1);
                printf("Else:\n");
                if (c->u.cond.else_cmd)
                    ast_print_cmds(c->u.cond.else_cmd, indent + 2);
                else {
                    print_indent(indent + 2);
                    printf("(vazio)\n");
                }
                break;

            case CMD_WHILE:
                print_indent(indent + 1);
                printf("WHILE Cond:\n");
                ast_print_expr(c->u.loop.cond, indent + 2);

                print_indent(indent + 1);
                printf("Corpo:\n");
                ast_print_cmds(c->u.loop.body, indent + 2);
                break;

            case CMD_READ:
                print_indent(indent + 1);
                printf("READ IDs:\n");
                ast_print_idlist(c->u.leitura.lista_id, indent + 2);
                break;

            case CMD_WRITE: {
                print_indent(indent + 1);
                printf("WRITE:\n");
                Expr *e = c->u.escrita.lista_exp;
                while (e) {
                    ast_print_expr(e, indent + 2);
                    e = e->prox;
                }
            } break;

            case CMD_CALL_PROC: {
                print_indent(indent + 1);
                printf("CALL %s\n", c->u.proc_call.nome);

                print_indent(indent + 1);
                printf("Args:\n");

                Expr *a = c->u.proc_call.args_lista;
                if (!a) {
                    print_indent(indent + 2);
                    printf("(vazio)\n");
                }
                while (a) {
                    ast_print_expr(a, indent + 2);
                    a = a->prox;
                }
            } break;

            case CMD_COMPOSTO:
                print_indent(indent + 1);
                printf("BEGIN/END:\n");
                ast_print_bloco(c->u.composto, indent + 2);
                break;
        }

        c = c->prox;
    }
}


// ======================================================================
// DECLARAÇÕES
// ======================================================================
void ast_print_param_decl(ParamDecl* p, int indent) {
    while (p) {
        print_indent(indent);
        printf("PARAM (Tipo: %s): ", tipo_semantico_to_string(p->tipo_param));
        print_idlist(p->ids);
        printf("\n");
        p = p->prox;
    }
}

void ast_print_decls(Decl* d, int indent) {
    while (d) {
        switch (d->tipo) {

            case DECL_VAR:
                print_indent(indent);
                printf("DECL_VAR (Tipo: %s) IDs: ",
                       tipo_semantico_to_string(d->u.var.tipo_var));
                print_idlist(d->u.var.ids);
                printf("\n");
                break;

            case DECL_PROCEDURE:
                print_indent(indent);
                printf("PROCEDURE %s\n", d->u.subrot.nome);

                print_indent(indent + 1);
                printf("Params:\n");
                if (d->u.subrot.params)
                    ast_print_param_decl(d->u.subrot.params, indent + 2);
                else {
                    print_indent(indent + 2);
                    printf("(nenhum)\n");
                }

                print_indent(indent + 1);
                printf("Bloco:\n");
                ast_print_bloco(d->u.subrot.bloco, indent + 2);
                break;

            case DECL_FUNCTION:
                print_indent(indent);
                printf("FUNCTION %s Ret %s\n",
                       d->u.subrot.nome,
                       tipo_semantico_to_string(d->u.subrot.tipo_retorno));

                print_indent(indent + 1);
                printf("Params:\n");
                if (d->u.subrot.params)
                    ast_print_param_decl(d->u.subrot.params, indent + 2);
                else {
                    print_indent(indent + 2);
                    printf("(nenhum)\n");
                }

                print_indent(indent + 1);
                printf("Bloco:\n");
                ast_print_bloco(d->u.subrot.bloco, indent + 2);
                break;
        }

        d = d->prox;
    }
}


// ======================================================================
// BLOCO
// ======================================================================
void ast_print_bloco(Bloco* b, int indent) {
    if (!b) {
        print_indent(indent);
        printf("BLOCO_NULL\n");
        return;
    }

    print_indent(indent);
    printf("BLOCO\n");

    print_indent(indent + 1);
    printf("Decls VAR:\n");
    if (b->decls_var) ast_print_decls(b->decls_var, indent + 2);
    else {
        print_indent(indent + 2);
        printf("(nenhum)\n");
    }

    print_indent(indent + 1);
    printf("Decls SUBROT:\n");
    if (b->decls_subrotinas) ast_print_decls(b->decls_subrotinas, indent + 2);
    else {
        print_indent(indent + 2);
        printf("(nenhuma)\n");
    }

    print_indent(indent + 1);
    printf("Comandos:\n");
    if (b->comandos) ast_print_cmds(b->comandos, indent + 2);
    else {
        print_indent(indent + 2);
        printf("(nenhum)\n");
    }
}


// ======================================================================
// PROGRAMA
// ======================================================================
void ast_print_program(Programa* p) {
    printf("==========================================\n");
    printf("     ÁRVORE SINTÁTICA ABSTRATA (AST)      \n");
    printf("==========================================\n");

    printf("PROGRAMA %s\n", p->nome);
    printf("Bloco Principal:\n");
    ast_print_bloco(p->bloco_principal, 1);

    printf("==========================================\n");
}