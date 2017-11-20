CC=gcc
CFLAGS=-I.
DEPS = ifj2017.h string_ops.c scanner.c symtable.c parser.c psa_parser.c psa_stack.c
OBJ = string_ops.o scanner.o symtable.o parser.o psa_parser.o psa_stack.o

%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

ifj2017: $(OBJ)
	gcc -g -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o ifj2017
