/* posfixo.l */
%{
#include "posfixo.tab.h"
%}

%option noyywrap

%%

[\n\t ]  ;

\+       { return MAIS; }
\-       { return MENOS; }
\*       { return ASTERISCO; }
\/       { return DIV; }
a        { return IDENT; }

%%