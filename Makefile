scanner: scanner.c
	gcc -o scanner scanner.c -I.

clean:
	rm scanner
