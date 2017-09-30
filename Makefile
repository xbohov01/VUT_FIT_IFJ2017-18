# Grigorev Danil
# xgrigo02
# Makefile for scanner

MAIN = scanner
LIST = token_list
EXEC = $(NAME)
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -pedantic

all :
	$(CC) $(MAIN).c -o $(MAIN).o -c
	$(CC) $(LIST).c -o $(LIST).o -c
	$(CC) -o $(MAIN) $(MAIN).o $(LIST).o $(CFLAGS) 
clean :
	rm -f *.o

