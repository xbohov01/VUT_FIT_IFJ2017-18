/*
Two_way_list main file 
Author: Danil Grigorev
*/

#include "token_list.h"

List *token_list_init()
{
    List *l;
    l = malloc(sizeof(List));
    if (l == NULL) {
        fprintf(stderr, "List malloc failed\n");
        return NULL;
    }
    l->active = NULL;
    l->first = NULL;
    l->last = NULL;
    return l;
} 

Item *token_init(int position, int line) {
    Item *itm;
    itm = (Item *) malloc(sizeof(Item));
    if (itm == NULL) {
        fprintf(stderr, "List item malloc failed\n");
        return NULL;
    }
    itm->data = (Data *) malloc(sizeof(Data));
    if (itm->data == NULL) {
        fprintf(stderr, "Data malloc failed\n");
        return NULL;
    }
    itm->data->content = (char *) malloc(MEM_ALLOC_CONST * sizeof(char));
    if (itm->data->content == NULL) {
        fprintf(stderr, "Content malloc failed\n");
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

int feed_word(Item *i, char* str) { // TODO: List *l
    assert(i != NULL);
    assert(i->data != NULL);
    if (strlen(str) + i->data->length > i->data->capacity) {
        printf("Reallocation from %d to %d\n", i->data->capacity, \
            i->data->capacity+i->data->capacity + MEM_ALLOC_CONST);
        realloc(i->data->content, i->data->capacity + MEM_ALLOC_CONST);
        if ( i->data->content == NULL) {
            fprintf(stderr, "Content realloc failed\n");
            return INTERNAL_ERR;
        }
    }
    strcat(i->data->content, str);
    printf("Now inside itm->data->content: |%s|\n", i->data->content);
    return SUCCESS;
}

void insert_first(List *l, Item *itm)
{
    itm->next = l->first;
    l->first = itm;
}

void insert_last(List *l, Item *itm)
{
    itm->prev = l->last;
    l->last = itm;
}

void first(List *l)
{
    l->active = l->first;
}

void last(List *l)
{
    l->active = l->last;
}

void insert_after(List *l, Item *itm)
{

}

bool active()
{
}

void data_free(List *l) {
    Item *i = l->first;
    Item *to_del;
    while (i != NULL) {
        to_del = i;
        i = to_del->next;
        if (to_del->data->content != NULL) {
            free(to_del->data->content);
        }
        if (to_del->data != NULL) {
            free(to_del->data);
        }
        free(to_del);
    }
    if (l != NULL) {
        free(l);
    }
    return;
}