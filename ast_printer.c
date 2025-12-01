#include "ast.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ======================================================================
// AUXILIAR
// ======================================================================
// Print a repeated string chunk (used for prefix building)
static void print_chunk(const char *s, int n) {
    for (int i = 0; i < n; ++i) fputs(s, stdout);
}

// Old indentation helper kept for backward-compatibility in some calls
static void print_indent(int indent) {
    print_chunk("  ", indent);
}

// Helper: prints prefix + marker (├── or └──) depending on is_last
static void print_prefix_marker(const char* prefix, int is_last) {
    if (prefix && *prefix) fputs(prefix, stdout);
    printf("%s", is_last ? "└── " : "├── ");
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
// Public API kept: still accepts (Expr*, int) to avoid header changes.
// Internally we use prefixedhelpers for tree visualization.
void ast_print_expr(Expr* e, int indent) {
    if (!e) {
        print_indent(indent);
        printf("EXPR_NULL\n");
        return;
    }
    // For backward compatibility with the 'indent' parameter we just
    // use spaces and behave like previously when called from outside.
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


// --- New prefixed-version helpers for nicer tree layout. These functions
// convert recursive calls into calls that carry a `prefix` string which
// contains the accumulated vertical bars and spaces. `is_last` signals if
// the current node is the last sibling so the correct marker (└──/├──)
// is printed and which prefix to propagate to children.

static void ast_print_expr_pref(Expr* e, const char* prefix, int is_last);
static void ast_print_cmds_pref(Comando* c, const char* prefix, int is_last);
static void ast_print_decls_pref(Decl* d, const char* prefix, int is_last);
static void ast_print_bloco_pref(Bloco* b, const char* prefix, int is_last);
static void ast_print_param_decl_pref(ParamDecl* p, const char* prefix, int is_last);
static void ast_print_idlist_pref(IdList* ids, const char* prefix, int is_last);
static void build_child_prefix(const char* prefix, int is_last, char* out, size_t out_size);

static void ast_print_expr_pref(Expr* e, const char* prefix, int is_last) {
    if (!e) {
        print_prefix_marker(prefix, is_last);
        printf("EXPR_NULL\n");
        return;
    }

    print_prefix_marker(prefix, is_last);
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

        case EXPR_BIN: {
            printf("BINOP %s\n", token_to_string(e->u.bin.op));
            // prepare next prefix
            char next_prefix[512];
            build_child_prefix(prefix, is_last, next_prefix, sizeof(next_prefix));

            ast_print_expr_pref(e->u.bin.esq, next_prefix, 0);
            ast_print_expr_pref(e->u.bin.dir, next_prefix, 1);
        } break;

        case EXPR_UN:
        {
            printf("UNOP %s\n", token_to_string(e->u.un.op));
            char next_prefix[512];
            build_child_prefix(prefix, is_last, next_prefix, sizeof(next_prefix));

            ast_print_expr_pref(e->u.un.arg, next_prefix, 1);
        } break;

        case EXPR_CALL_FUNC: {
            printf("CALL %s\n", e->u.func.nome);
            char next_prefix[512];
            build_child_prefix(prefix, is_last, next_prefix, sizeof(next_prefix));

            Expr *arg = e->u.func.args_lista;
            if (!arg) {
                print_prefix_marker(next_prefix, 1);
                printf("(vazio)\n");
            } else {
                while (arg) {
                    int last = arg->prox == NULL;
                    ast_print_expr_pref(arg, next_prefix, last);
                    arg = arg->prox;
                }
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
    // Use pretty tree printer for the main block
    ast_print_bloco_pref(p->bloco_principal, "", 1);

    printf("==========================================\n");
}

static void ast_print_idlist_pref(IdList* ids, const char* prefix, int is_last) {
    if (!ids) {
        print_prefix_marker(prefix, is_last);
        printf("(vazio)\n");
        return;
    }

    // Print each id as sibling nodes
    while (ids) {
        int last = ids->prox == NULL;
        print_prefix_marker(prefix, last);
        printf("ID: %s\n", ids->nome);
        ids = ids->prox;
    }
}

static void ast_print_param_decl_pref(ParamDecl* p, const char* prefix, int is_last) {
    if (!p) return;

    while (p) {
        int last = p->prox == NULL;
        print_prefix_marker(prefix, last);
        printf("PARAM (Tipo: %s): ", tipo_semantico_to_string(p->tipo_param));
        // inline ID list after marker
        print_idlist(p->ids);
        printf("\n");
        p = p->prox;
    }
}

static void ast_print_cmds_pref(Comando* c, const char* prefix, int is_last) {
    if (!c) {
        print_prefix_marker(prefix, is_last);
        printf("(nenhum)\n");
        return;
    }

    while (c) {
        int last = c->prox == NULL;
        print_prefix_marker(prefix, last);
        printf("COMMAND\n");

        // build prefix for children
        char child_prefix[512];
        build_child_prefix(prefix, last, child_prefix, sizeof(child_prefix));

        switch (c->tipo) {
            case CMD_ATRIB: {
                print_prefix_marker(child_prefix, 1);
                printf("ATRIB %s :=\n", c->u.atrib.nome_var);

                char expr_prefix[512];
                build_child_prefix(child_prefix, 1, expr_prefix, sizeof(expr_prefix));
                ast_print_expr_pref(c->u.atrib.expr, expr_prefix, 1);
            } break;

            case CMD_IF: {
                // Cond
                print_prefix_marker(child_prefix, 0);
                printf("IF Cond:\n");

                char cond_pref[512];
                build_child_prefix(child_prefix, 0, cond_pref, sizeof(cond_pref));
                ast_print_expr_pref(c->u.cond.cond, cond_pref, 1);

                // Then
                print_prefix_marker(child_prefix, 0);
                printf("Then:\n");
                char then_pref[512];
                build_child_prefix(child_prefix, 0, then_pref, sizeof(then_pref));
                ast_print_cmds_pref(c->u.cond.then_cmd, then_pref, 1);

                // Else
                print_prefix_marker(child_prefix, 1);
                printf("Else:\n");
                char else_pref[512];
                build_child_prefix(child_prefix, 1, else_pref, sizeof(else_pref));
                if (c->u.cond.else_cmd)
                    ast_print_cmds_pref(c->u.cond.else_cmd, else_pref, 1);
                else {
                    print_prefix_marker(else_pref, 1);
                    printf("(vazio)\n");
                }
            } break;

            case CMD_WHILE: {
                print_prefix_marker(child_prefix, 0);
                printf("WHILE Cond:\n");

                char cond_pref[512];
                build_child_prefix(child_prefix, 0, cond_pref, sizeof(cond_pref));
                ast_print_expr_pref(c->u.loop.cond, cond_pref, 1);

                print_prefix_marker(child_prefix, 1);
                printf("Corpo:\n");
                char body_pref[512];
                build_child_prefix(child_prefix, 1, body_pref, sizeof(body_pref));
                ast_print_cmds_pref(c->u.loop.body, body_pref, 1);
            } break;

            case CMD_READ: {
                print_prefix_marker(child_prefix, 1);
                printf("READ IDs:\n");
                char ids_pref[512];
                build_child_prefix(child_prefix, 1, ids_pref, sizeof(ids_pref));
                ast_print_idlist_pref(c->u.leitura.lista_id, ids_pref, 1);
            } break;

            case CMD_WRITE: {
                print_prefix_marker(child_prefix, 1);
                printf("WRITE:\n");

                Expr* e = c->u.escrita.lista_exp;
                if (!e) {
                    char ep[512]; build_child_prefix(child_prefix, 1, ep, sizeof(ep));
                    print_prefix_marker(ep, 1);
                    printf("(vazio)\n");
                } else {
                    while (e) {
                        int last_e = e->prox == NULL;
                        char ep[512];
                        if (last_e) build_child_prefix(child_prefix, 1, ep, sizeof(ep));
                        else build_child_prefix(child_prefix, 0, ep, sizeof(ep));
                        ast_print_expr_pref(e, ep, last_e);
                        e = e->prox;
                    }
                }
            } break;

            case CMD_CALL_PROC: {
                print_prefix_marker(child_prefix, 1);
                printf("CALL %s\n", c->u.proc_call.nome);

                print_prefix_marker(child_prefix, 1);
                printf("Args:\n");

                Expr* a = c->u.proc_call.args_lista;
                if (!a) {
                    char ap[512]; build_child_prefix(child_prefix, 1, ap, sizeof(ap));
                    print_prefix_marker(ap, 1);
                    printf("(vazio)\n");
                } else {
                    while (a) {
                        int last_a = a->prox == NULL;
                        char ap[512];
                        if (last_a) build_child_prefix(child_prefix, 1, ap, sizeof(ap));
                        else build_child_prefix(child_prefix, 0, ap, sizeof(ap));
                        ast_print_expr_pref(a, ap, last_a);
                        a = a->prox;
                    }
                }
            } break;

            case CMD_COMPOSTO: {
                print_prefix_marker(child_prefix, 1);
                printf("BEGIN/END:\n");
                char bpref[512]; build_child_prefix(child_prefix, 1, bpref, sizeof(bpref));
                ast_print_bloco_pref(c->u.composto, bpref, 1);
            } break;
        }

        c = c->prox;
    }
}

static void ast_print_decls_pref(Decl* d, const char* prefix, int is_last) {
    if (!d) {
        print_prefix_marker(prefix, is_last);
        printf("(nenhuma)\n");
        return;
    }

    while (d) {
        int last = d->prox == NULL;
        print_prefix_marker(prefix, last);

        switch (d->tipo) {
            case DECL_VAR: {
                printf("DECL_VAR (Tipo: %s) IDs: ", tipo_semantico_to_string(d->u.var.tipo_var));
                print_idlist(d->u.var.ids);
                printf("\n");
            } break;

            case DECL_PROCEDURE: {
                printf("PROCEDURE %s\n", d->u.subrot.nome);
                char childp[512];
                build_child_prefix(prefix, last, childp, sizeof(childp));

                // Params
                print_prefix_marker(childp, 0);
                printf("Params:\n");
                if (d->u.subrot.params)
                    ast_print_param_decl_pref(d->u.subrot.params, childp, 1);
                else {
                    char p2[512]; build_child_prefix(childp, 1, p2, sizeof(p2));
                    print_prefix_marker(p2, 1);
                    printf("(nenhum)\n");
                }

                // Bloco
                print_prefix_marker(childp, 1);
                printf("Bloco:\n");
                char b2[512]; build_child_prefix(childp, 1, b2, sizeof(b2));
                ast_print_bloco_pref(d->u.subrot.bloco, b2, 1);
            } break;

            case DECL_FUNCTION: {
                printf("FUNCTION %s Ret %s\n", d->u.subrot.nome, tipo_semantico_to_string(d->u.subrot.tipo_retorno));
                char childp[512];
                build_child_prefix(prefix, last, childp, sizeof(childp));

                // Params
                print_prefix_marker(childp, 0);
                printf("Params:\n");
                if (d->u.subrot.params)
                    ast_print_param_decl_pref(d->u.subrot.params, childp, 1);
                else {
                    char p2[512]; build_child_prefix(childp, 1, p2, sizeof(p2));
                    print_prefix_marker(p2, 1);
                    printf("(nenhum)\n");
                }

                // Bloco
                print_prefix_marker(childp, 1);
                printf("Bloco:\n");
                char b2[512]; build_child_prefix(childp, 1, b2, sizeof(b2));
                ast_print_bloco_pref(d->u.subrot.bloco, b2, 1);
            } break;
        }

        d = d->prox;
    }
}

static void ast_print_bloco_pref(Bloco* b, const char* prefix, int is_last) {
    if (!b) {
        print_prefix_marker(prefix, is_last);
        printf("BLOCO_NULL\n");
        return;
    }

    print_prefix_marker(prefix, is_last);
    printf("BLOCO\n");

    char child_pref[512];
    build_child_prefix(prefix, is_last, child_pref, sizeof(child_pref));

    // Decls VAR
    print_prefix_marker(child_pref, 0);
    printf("Decls VAR:\n");
    if (b->decls_var) ast_print_decls_pref(b->decls_var, child_pref, 1);
    else {
        char p2[512]; build_child_prefix(child_pref, 1, p2, sizeof(p2));
        print_prefix_marker(p2, 1);
        printf("(nenhum)\n");
    }

    // Decls SUBROT
    print_prefix_marker(child_pref, 0);
    printf("Decls SUBROT:\n");
    if (b->decls_subrotinas) ast_print_decls_pref(b->decls_subrotinas, child_pref, 1);
    else {
        char p2[512]; build_child_prefix(child_pref, 1, p2, sizeof(p2));
        print_prefix_marker(p2, 1);
        printf("(nenhuma)\n");
    }

    // Comandos
    print_prefix_marker(child_pref, 1);
    printf("Comandos:\n");
    if (b->comandos) ast_print_cmds_pref(b->comandos, child_pref, 1);
    else {
        char p2[512]; build_child_prefix(child_pref, 1, p2, sizeof(p2));
        print_prefix_marker(p2, 1);
        printf("(nenhum)\n");
    }
}

// Safely compose a child prefix into `out`. Appends either "    " (if
// last child) or "│   " (if there are siblings after it).
static void build_child_prefix(const char* prefix, int is_last, char* out, size_t out_size) {
    out[0] = '\0';
    if (prefix && *prefix) {
        // copy prefix first
        strncpy(out, prefix, out_size - 1);
        out[out_size - 1] = '\0';
    }
    const char* add = is_last ? "    " : "│   ";
    strncat(out, add, out_size - strlen(out) - 1);
}

/* forward declaration removed (definition present above) */