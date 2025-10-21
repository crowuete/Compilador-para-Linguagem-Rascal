/* posfixo2.l */
%{
#include "posfixo2.tab.h"
#include <string.h>
%}

%option noyywrap

%%

[\n\t ]  ;

and      { return AND; }
or       { return OR; }

\+       { return MAIS; }
\-       { return MENOS; }
\*       { return ASTERISCO; }
\/       { return DIV; }

a|b      { yylval.sval = strdup(yytext); return IDENT; }

%%
