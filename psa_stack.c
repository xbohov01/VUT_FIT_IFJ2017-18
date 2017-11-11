/*
**  PSA symtable stack operations
**  Author: Danil Grigorev, xgrigo02
**          Samuel Bohovic, xbohov01
*/

#include "psa_stack.h"

// Stack operations
// =======================

T_NT_stack *init_T_NT_stack() {
    T_NT_stack *s = malloc(sizeof(T_NT_stack));
    if (s == NULL) {
        error_exit(s, INTERNAL_ERR);
    }

    s->top = NULL;
    s->active = NULL;
    s->popped = NULL;
    return s;
}

void destroy_T_NT_stack(T_NT_stack *s) {
    while(pop_T_NT(s) != NULL)
        ;

    if (s != NULL) {
        free(s);
    }
    return;
}

T_NT_item *push_T_NT(T_NT_stack *s, bool non_term, T_NT_Data input_data) {
    T_NT_item *new_top = malloc(sizeof(T_NT_item));
    if (new_top == NULL) {
        error_exit(s, INTERNAL_ERR);
    }

    new_top->next_T_NT = s->top;
    new_top->is_non_term = non_term;
    new_top->data = input_data;

    s->top = new_top;
    return new_top;
}

T_NT_item *pop_T_NT(T_NT_stack *s) {

    if (s->popped != NULL) {
        if (!s->popped->is_non_term) {
            free(s->popped->data.Term.id);
            if (s->popped->data.Term.token_type == STRING) {
                free(s->popped->data.Term.value_string);
            }
        }
        free(s->popped);
        s->popped = NULL;
    }
    if (s->active == s->top) {
        s->active == NULL;
    }
    if (s->top == NULL) {
        return s->popped;
    }
    s->popped = s->top;
    s->top = s->top->next_T_NT;

    return s->popped;
}


// Extended stack operations
// =======================

void set_first_T_NT(T_NT_stack *s) {
    s->active = s->top;
}

void next_T_NT(T_NT_stack *s) {
    if (s->active != NULL) {
        s->active = s->active->next_T_NT;
    }
}

bool active_T_NT(T_NT_stack *s) {
    return s->active != NULL;
}

T_NT_item* insert_after_T_NT(T_NT_stack *s, bool non_term, T_NT_Data input_data) {
    if (s->active == NULL) {
        return NULL;
    }

    T_NT_item *item_after = malloc(sizeof(T_NT_item));
    if (item_after == NULL) {
        error_exit(s, INTERNAL_ERR);
    }

    item_after->next_T_NT = s->active->next_T_NT;
    item_after->is_non_term = non_term;
    item_after->data = input_data;

    s->active->next_T_NT = item_after;
    return item_after;
}

// TODO: extend this function 
void error_exit(T_NT_stack *s, int code) {
    destroy_T_NT_stack(s);
    exit(code);
}

// =======================
// End stack operations