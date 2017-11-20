CC=gcc
CFLAGS=-I.
DEPS = psa_parser.c ifj2017.h psa_stack.h symtable.c parser.c psa_stack_test.c scanner.c string_ops.c
OBJ = psa_parser.o psa_stack.o symtable.o parser.o psa_stack_test.o scanner.o string_ops.o

%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

psa_parser: $(OBJ)
	gcc -g -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o psa_parser