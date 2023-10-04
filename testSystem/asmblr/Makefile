CC=g++

CFLAGS=-g -w

buildASM:
	$(CC) $(CFLAGS) src/*.cpp -o bin/asmblr.out -I../../

test:
	./bin/asmblr.out ./testAsmCode/test.asm ./testAsmCode/test.tok