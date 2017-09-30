/*
Two_way_list main file 
Author: Danil Grigorev
*/

#include "list.h"

void token_list_init(List *l)
{
    l->active = NULL;
    l->first = NULL;
    l->last = NULL;
} 

Item* token_init(int position, int line) {
    Item *itm;
    itm = (Item *) malloc(sizeof(Item));
    if (itm == NULL) {
        return NULL;
    }
    itm->data = (Data *) malloc(sizeof(Data));
    if (itm->data == NULL) {
        return NULL;
    }
    itm->data->content = malloc(MEM_ALLOC_CONST * sizeof(char));
    if (itm->data->content == NULL) {
        return NULL;
    }
    itm->next = NULL;
    itm->prev = NULL;
    itm->data->position = position;
    itm->data->line = line;
    itm->data->length = 0;
    itm->data->capacity = MEM_ALLOC_CONST;
    return itm;
}

Data* feed_word(Item *i, char* str) { // TODO: List *l
    if (strlen(str) + i->data->length > i->data->capacity) {
        printf("Reallocation from %d to %d\n", i->data->capacity, \
            i->data->capacity+i->data->capacity + MEM_ALLOC_CONST);
        void *mem;
        if ((mem = realloc(i, i->data->capacity + MEM_ALLOC_CONST)) == NULL) {
            return NULL;
        }
    }
    strcpy(i->data->content, str);
    printf("Now inside itm->data->content: |%s|\n", i->data->content);
    return i->data;
}

Item *insert_first(List *l, Item *itm)
{
    printf("%d %d\n", l, itm);
    itm->next = l->first;
    printf("first is ok\n");
    l->first = itm;
    printf("second is ok\n");

    return itm;
}

void data_free(List *l) {
    for (Item *i = l->first; i != NULL; i = i->next) {
        free(i->data->content);
        free(i->data);
        free(i);
    }
    free(l);
    return;
}