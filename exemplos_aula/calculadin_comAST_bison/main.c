#include <stdio.h>
#include <stdlib.h>
#include "calculadin.tab.h"
#include "ast_calculadin.h" // inclui a declaração da extern 'raiz_ast'

extern int yylineno;

int main() {

    if (yyparse() == 0 && raiz_ast != NULL) {
        printf("Imprimindo AST:\n");
        ast_print(raiz_ast, stdout);
        ast_free(raiz_ast);
    } else {
        fprintf(stderr, "Parsing falhou.\n");
    }
    return 0;
}