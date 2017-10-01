/****** scanner.h ******/

#include "errors.h"

#ifndef MAIN_LIBS
#define MAIN_LIBS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#endif

#define STRUCTURES
#include "token_list.h"

#define N_KEYWORDS 35
#define N_OPERATOR_CHARS 8
#define N_OPERATORS 12
#define N_DELIMITERS 7
#define MEM_ALLOC_CONST 32


//function declarations
/* Read file until EOF and return tokens
Returns pointer to structure which will contain all file tokens */
List *start_scanner(char *file_name);
