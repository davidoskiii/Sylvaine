build: main.c codegen/codegen.c lexer/lexer.c ast/ast.c parser/parser.c
	gcc -o sylvaine -g main.c codegen/codegen.c lexer/lexer.c ast/ast.c parser/parser.c

clear:
	rm -f out *.o out.s

test: sylvaine
	./sylvaine inputs/input01.syl
	cc -o out out.s
	./out
	./sylvaine inputs/input02.syl
	cc -o out out.s
	./out
