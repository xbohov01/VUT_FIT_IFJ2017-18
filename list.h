/*
Two_way_list header 
Author: Danil Grigorev
*/

#ifndef SCANNER
#include "scanner.h"
#endif

#ifndef ERRORS_HEADER
#define ERRORS_HEADER
#include "errors.h"
#endif

typedef struct item {
    struct item *next;
    Data *data;
    struct item *prev;
} Item;

typedef struct list {
    Item *active;
    Item *first;
    Item *last;
} List;

void token_list_init(List *l);
Item *insert_first(List *l, Item *i);
Item *token_init(int position, int line);
// TODO: correct
Data *feed_word(Item *i, char *str);
void data_free(List *l);



