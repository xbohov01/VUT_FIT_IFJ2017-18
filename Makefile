ifj2017: parser.c
	gcc -o ifj2017 parser.c ial.c scanner.c string_ops.c

test: parser_test.c parser.c
	gcc -o parser_test parser_test.c parser.c ial.c string_ops.c

rmtest:
	rm parser_test

clean:
	rm ifj2017
