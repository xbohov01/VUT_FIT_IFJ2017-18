CC=gcc
CFLAGS=-I.
DEPS = psa_parser.h psa_stack.h psa_stack_test.h ifj2017.h scanner.h parser.c psa_parser.c  psa_stack.c psa_stack_test.c scanner.c
OBJ = psa_parser.o psa_stack.o psa_stack_test.o scanner.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

psa_parser: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o psa_parser