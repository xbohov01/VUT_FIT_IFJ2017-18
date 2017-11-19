CC=gcc
CFLAGS=-I.
DEPS = psa_parser.h psa_stack.h psa_stack_test.h ifj2017.h parser.c psa_parser.c psa_stack.c psa_stack_test.c scanner.c string_ops.c
OBJ = psa_parser.o psa_stack.o psa_stack_test.o scanner.o string_ops.o

%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

psa_parser: $(OBJ)
	gcc -g -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o psa_parser