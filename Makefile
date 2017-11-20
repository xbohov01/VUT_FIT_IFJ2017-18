test: parser.c
	gcc -o ptest parser.c scanner.c symtable.c string_ops.c -I.

clean:
	rm ptest
