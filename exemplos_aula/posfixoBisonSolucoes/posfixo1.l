/* posfixo1.l */
%{
#include "posfixo1.tab.h"
#include <string.h>
%}

%option noyywrap

%%

[\n\t ]  ;

\+       { return MAIS; }
\-       { return MENOS; }
\*       { return ASTERISCO; }
\/       { return DIV; }
a|b      { yylval.sval = strdup(yytext); return IDENT; }

%%
