compile: main.c lexer/lexer.c ast/ast.c parser/parser.c
	gcc -o sylvaine -g main.c lexer/lexer.c ast/ast.c parser/parser.c

clean:
	rm -f sylvaine *.o

test: sylvaine
	-(./sylvaine inputs/input01.syl; \
	 ./sylvaine inputs/input02.syl; \
	 ./sylvaine inputs/input03.syl; \
	 ./sylvaine inputs/input04.syl; \
	 ./sylvaine inputs/input05.syl)
