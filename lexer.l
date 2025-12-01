%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

%}

%option noyywrap yylineno

ESPACO      [ \t\n]+
ID          [a-zA-Z][a-zA-Z0-9_]*
NUM         [0-9]+

%%

"program"           { return TK_PROGRAM; }
"procedure"         { return PROCEDURE; }
"function"          { return FUNCTION; }
"begin"             { return KW_BEGIN; }
"end"               { return END; }
"var"               { return VAR; }
"if"                { return IF; }
"then"              { return THEN; }
"else"              { return ELSE; }
"while"             { return WHILE; }
"do"                { return DO; }
"read"              { return READ; }
"write"             { return WRITE; }
"integer"           { return INTEGER; }
"boolean"           { return BOOLEAN; }
"true"              { return TRUE; }
"false"             { return FALSE; }
"not"               { return NOT; }
"or"                { return OR; }
"and"               { return AND; }
"div"               { return DIV; }

{NUM}               { yylval.ival = atoi(yytext); return NUM; }
{ID}                { yylval.sval = strdup(yytext); return ID; }

"("                 { return '('; }
")"                 { return ')'; }
";"                 { return ';'; }
":"                 { return ':'; }
"."                 { return '.'; }
","                 { return ','; }
":="                { return ATRIB; }
"="                 { return IGUAL; }
"<>"                { return DIF; }
"<"                 { return MENOR; }
"<="                { return MENOR_IGUAL; }
">"                 { return MAIOR; }
">="                { return MAIOR_IGUAL; }
"+"                 { return '+'; }
"-"                 { return '-'; }
"*"                 { return '*'; }

{ESPACO}            { /* ignora */ }

.                   { printf("ERRO LÉXICO na linha %d: símbolo ilegal %c\n", yylineno, yytext[0]); return 0; }

%%