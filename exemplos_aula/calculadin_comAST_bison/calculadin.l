%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calculadin.tab.h"

%}

/* opções do flex */
%option noyywrap yylineno

ESPACO      [ \t\r\n]+
ID          [a-z][a-z0-9]*
NUM         [0-9]+(\.[0-9]+)?

%%

"iniciar"          { return INICIAR; }
"finalizar"        { return FINALIZAR; }
"calculadin"       { return CALCULADIN; }
"input"            { return INPUT; }
"output"           { return OUTPUT; }

{NUM}               { yylval.dval = atof(yytext); return NUM; }
{ID}                { yylval.sval = strdup(yytext); return ID; }

"("                 { return '('; }
")"                 { return ')'; }
";"                 { return ';'; }
":"                 { return ':'; }
"."                 { return '.'; }
"="                 { return '='; }
"+"                 { return '+'; }
"-"                 { return '-'; }
"*"                 { return '*'; }
"/"                 { return '/'; }

{ESPACO}            { /* ignora */ }

.                   { printf("ERRO LÉXICO na linha %d: símbolo ilegal %c\n", yylineno, yytext[0]); }

%%

