#include <stdio.h>
#include "ast.h"

// Declarado pelo Bison
int yyparse(void);
extern FILE *yyin;

int main(int argc, char **argv) {

    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Erro ao abrir arquivo de entrada");
            return 1;
        }
    }

    printf("Iniciando parsing...\n");

    if (yyparse() == 0) {
        printf("Parsing concluído com sucesso!\n\n");

        if (raiz_ast) {
            ast_print_program(raiz_ast);
            prog_free(raiz_ast);
        } else {
            printf("ATENÇÃO: raiz_ast == NULL (o parser não construiu a AST)\n");
        }

    } else {
        printf("Erros encontrados durante o parsing.\n");
    }

    return 0;
}