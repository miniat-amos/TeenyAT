all: lemon
	./lemon tnasm.y
	flex tnasm.lex
	gcc -o tnasm blocks.c lexer.c list.c listing.c symbol_tables.c tnasm.c token.c util.c

lemon: lemon.c lempar.c
	gcc -o lemon lemon.c

clean:
	rm -rf tnasm lemon
	rm -rf tnasm.c tnasm.h tnasm.out
	rm -rf lexer.c
