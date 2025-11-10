all: calc

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

calc: parser.tab.c lex.yy.c
	gcc parser.tab.c lex.yy.c -o $@

clean:
	rm -f calc lex.yy.c parser.tab.c parser.tab.h

.PHONY: all clean