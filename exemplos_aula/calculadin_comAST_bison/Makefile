all: calculadin

calculadin.tab.c calculadin.tab.h: calculadin.y
	bison -d calculadin.y

lex.yy.c: calculadin.l calculadin.tab.h
	flex calculadin.l

calculadin: calculadin.tab.c lex.yy.c ast_calculadin.c main.c
	gcc calculadin.tab.c lex.yy.c ast_calculadin.c main.c -o $@

runOK:
	./calculadin <exemplo.calc

runErro:
	./calculadin <exemplo.erro.calc

clean:
	rm -f calculadin lex.yy.c calculadin.tab.c calculadin.tab.h

.PHONY: all clean runErro runOK
