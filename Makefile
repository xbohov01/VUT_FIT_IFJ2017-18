parser_test: parser_test.c parser.c
	gcc -o parser_test parser_test.c parser.c

clean:
	rm parser_test
