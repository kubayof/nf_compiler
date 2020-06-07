all: test

clean:
	rm app bin/*.o

test: compile
	./app test.nf

compile:
	gcc -o app main.c lexer/Lexer.c errors/Errors.c parser/Parser.c binder/Binder.c symbols/SymbolTable.c

link: bin/main.o bin/Lexer.o bin/Parser.o bin/Errors.o
	gcc -o app bin/*.o

bin/main.o: main.c
	gcc -o bin/main.o -c main.c

bin/Lexer.o: lexer/Lexer.c
	gcc -o bin/Lexer.o -c lexer/Lexer.c

bin/Parser.o: parser/Parser.c
	gcc -o bin/Parser.o -c parser/Parser.c

bin/Errors.o: errors/Errors.c
	gcc -o bin/Errors.o -c errors/Errors.c

