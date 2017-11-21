CC=gcc
CFLAGS=-I.
DEPS = psa_parser.c ifj2017.h string_ops.c psa_stack.h symtable.c parser.c psa_stack_test.c scanner.c 
OBJ = psa_parser.o string_ops.o psa_stack.o symtable.o parser.o psa_stack_test.o scanner.o 

%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

ifj2017: $(OBJ)
	gcc -g -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o psa_parser