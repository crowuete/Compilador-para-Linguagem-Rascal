# Constantes
CC    = gcc
BISON = bison
FLEX  = flex

# Regras 
%.tab.c %.tab.h: %.y
	$(BISON) -d $<

lex-%.yy.c: %.l %.tab.h
	$(FLEX) -o $@ $<

%: %.tab.c lex-%.yy.c
	$(CC) $^ -o $@

# Diretiva que impede que o make apague os intermediários
.PRECIOUS: %.tab.c %.tab.h lex-%.yy.c

# 'make all' para construir todas as versões
all: posfixo posfixo1 posfixo2 posfixo3

# 'make clean' para apagar tudo menos os .y e .l
clean:
	rm -f posfixo posfixo1 posfixo2 posfixo3 *.tab.c *.tab.h lex-*.yy.c

.PHONY: all clean