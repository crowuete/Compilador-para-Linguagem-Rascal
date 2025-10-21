/* calc.l  */
%option noyywrap nodefault
%{
#include <stdlib.h>
#include "calc.tab.h"
%}

PULA   [ \t\r]+
NUM [0-9]+(\.[0-9]+)?

%%

{PULA}            ;
\n                return '\n';

{NUM}             { yylval.val = strtod(yytext, NULL); return NUM; }

"+"               return '+';
"-"               return '-';
"*"               return '*';
"/"               return '/';
"("               return '(';
")"               return ')';

.                 return yytext[0];

%%

