CC=gcc
CFLAGS=-I.
DEPS = psa_parser.h psa_stack.h psa_stack_test.h ifj2017.h
OBJ = psa_parser.o psa_stack.o psa_stack_test.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

psa_parser: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o psa_parser