#		  Grigorev Danil
#			  xgrigo02
# Makefile pro proj2 z předmětu IOS
# 		Synchronizace procesu

NAME = hello_world
EXEC = $(NAME)
CC = gcc
WERR = 
CFLAGS = -std=gnu99 -Wall -Wextra $(WERR) -pedantic

all : $(EXEC)
%.o : %.c

	$(CC) $(NAME).c -o $(NAME) $(CFLAGS) $(LDFLAGS)

clean :
	rm -f *.o

