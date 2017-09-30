/****** scanner.h ******/

#ifndef MAIN_LIBS
#define MAIN_LIBS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#endif

#define SCANNER 1 

#define N_KEYWORDS 35
#define N_OPERATOR_CHARS 8
#define N_OPERATORS 12
#define N_DELIMITERS 7
#define MEM_ALLOC_CONST 32

typedef struct token {
    int position;
    int line;
    int capacity;
    int length;
    char *content;
} Token;

typedef Token Data;

//function declarations
// Read file until EOF and return 0 on success
int start_scanner(char *file_name, Token *lines);
int stop_scanner();
int get_token();
