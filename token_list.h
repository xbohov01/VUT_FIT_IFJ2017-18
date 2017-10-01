/*
Two_way_list header 
Author: Danil Grigorev
*/
#include "errors.h"

#ifndef MAIN_LIBS
#define MAIN_LIBS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#endif

#include <assert.h>
#include "scanner_structures.h"

#define MEM_ALLOC_CONST 32

typedef struct item {
    struct item *next;
    Data *data;
    struct item *prev;
} Item;

// TODO:Change to static
typedef struct list {
    Item *active;
    Item *first;
    Item *last;
} List;

List *token_list_init();
Item *token_init(int position, int line);

void insert_first(List *l, Item *i);
void insert_last(List *l, Item *i);
void insert_after(List *l, Item *i);
void insert_before(List *l, Item *i);

int feed_word(Item *i, char *str);
void data_free(List *l);



