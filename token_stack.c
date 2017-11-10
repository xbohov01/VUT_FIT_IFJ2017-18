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
    itm->data.content = (char *) malloc(MEM_ALLOC_CONST * sizeof(char));
    if (itm->data.content == NULL) {
        fprintf(stderr, "Content malloc failed\n");
        return NULL;
    }
    itm->next = NULL;
    itm->prev = NULL;
    itm->data.position = position;
    itm->data.line = line;
    itm->data.length = 0;
    itm->data.capacity = MEM_ALLOC_CONST;
    return itm;
}

int feed_word(Item *i, char* str, size_t len) { // TODO: List *l
    assert(i != NULL);
    assert(i->data.content != NULL);
    if (len + i->data.length > i->data.capacity) {
        printf("Reallocation from %d to %d\n", i->data.capacity, \
            i->data.capacity + MEM_ALLOC_CONST);
        realloc(i->data.content, i->data.capacity + MEM_ALLOC_CONST);
        if ( i->data.content == NULL) {
            fprintf(stderr, "Content realloc failed\n");
            return INTERNAL_ERR;
        }
    }
    strncat(i->data.content, str, len);
    printf("Now inside itm->data->content: |%s|\n", i->data.content);
    return SUCCESS;
}

Item *insert_first(List *l, int position, int line) /* scanner related TODO: make universal*/
{
    Item *itm = token_init(position, line);
    itm->next = l->first;
    if (active(l->first) == true) {
        l->first->prev = itm;  
    }
    l->first = itm;
    if (active(l->last) == false) {
        l->last = itm;
    }
    return itm;
}

Item *insert_last(List *l, int position, int line) /* scanner related TODO: make universal*/
{
    Item *itm = token_init(position, line);
    itm->prev = l->last;
    if (active(l->last) == true) {
        l->last->next = itm;
    }
    l->last = itm;
    if (active(l->first) == false) {
        l->first = itm;
    }
    return itm;
}

Item *insert_after(List *l, int position, int line) /* scanner related TODO: make universal*/
{
    if (active(l->active) == false || active(l->last) == false || l->active == l->last) {
        return insert_last(l, position, line);
    }
    else {
        Item *itm = token_init(position, line);
        itm->next = l->active->next;
        itm->prev = l->active;
        l->active->next->prev = itm;
        l->active->next = itm;
        return itm;
    }
}

Item *insert_before(List *l, int position, int line) /* scanner related TODO: make universal*/
{
    if (active(l->active) == false || active(l->first) == false || l->active == l->first) {
        return insert_first(l, position, line);
    }
    else {
        Item *itm = token_init(position, line);
        itm->prev = l->active->prev;
        itm->next = l->active;
        l->active->prev->next = itm;
        l->active->prev = itm;
        return itm;
    }
}

bool active(Item *itm) { return (itm != NULL); }
void first(List *l) { l->active = l->first; }
void last(List *l) { l->active = l->last; }

void next(List *l)
{
    if (active(l->active) == true) {
        l->active = l->active->next;
    }
}

void prev(List *l)
{
    if (active(l->active) == true) {
        l->active = l->active->prev;
    }
}

void data_free(List *l) {
    if (l != NULL) {
        first(l);
        Item *next;
        do {
            next = l->active->next;
            if (l->active->data.content != NULL) {
                printf("%s\n", l->active->data.content);
                free(l->active->data.content);
            }
            if (l->active != NULL) {
                free(l->active);
            }
            l->active = next;
        } while (next->next != NULL);
        printf("\n\n%s\n", l->active->data.content);
        free(l->active->data.content);
        if (l != NULL) {
            free(l);
        }
    }
    return;
}