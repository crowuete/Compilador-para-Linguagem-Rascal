all: calc

calc.tab.c calc.tab.h: calc.y 
	bison -d calc.y

lex.yy.c: calc.l calc.tab.h
	flex calc.l

calc: calc.tab.c lex.yy.c
	gcc lex.yy.c calc.tab.c -o calc

clean: 
	rm -f calc.tab.* lex.yy.c calc

.PHONY: all clean