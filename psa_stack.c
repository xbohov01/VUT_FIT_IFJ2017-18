/*
**  PSA symtable stack operations
**  Author: Danil Grigorev, xgrigo02
**          Samuel Bohovic, xbohov01
*/

#include "ifj2017.h"

// Additional options
// =======================
PSA_Term_type get_term_type(Data_Term *in_term) {
    switch(in_term->token_type) {
        // Used in PSA table
        //------------------
        case ADD_O:
            return ADD;
        case SUB_O:
            return SUB;
        case MUL_O:
            return MUL;
        case DIV_O:
            return DIV;
        case MOD_O:
            return IDIV;
        case PAR_R:
            return PR;
        case PAR_L:
            return PL;
        case DOUBLE:
        case INTEGER:
        case STRING:
        case IDENTIFICATOR:
            // TODO: can be const or function or variable,
            // should look in hash_table
            return ID;
        case ENDL:
            return END;

        /*---------------------
        * Relational operators */

        case LT_O:
            return LT;
        case GT_O:
            return GT;
        case LTE_O:
            return LTE;
        case GTE_O:
            return GTE;
        case EQ_O:
            return EQ;
        case NE_O:
            return NEQ;
        default:
            error_exit(SYNT_ERR);
    }
}
// =======================


// Stack operations
// =======================

T_NT_stack *init_T_NT_stack() {
    T_NT_stack *s = malloc(sizeof(T_NT_stack));
    if (s == NULL) {
        error_exit(INTERNAL_ERR);
    }

    s->top = NULL;
    s->active = NULL;
    s->popped = NULL;
    return s;
}

void destroy_T_NT_stack(T_NT_stack *s) {

    if (s != NULL) {
        while(pop_T_NT(s) != NULL);
        free(s);
    }
    return;
}

void copy_term(Data_Term *from, Data_Term *to) {
    T_token_type input_type = from->token_type;
    size_t alloc_len;
    switch(input_type) {
        case INTEGER:
            to->value_int = from->value_int;
            break;
        case DOUBLE:
            to->value_double = from->value_double;
            break;
        case STRING:
            // TODO: delete after string implementation
            to->value_string = malloc(strlen(from->value_string) + 1);
            if (to->value_string == NULL) {
                error_exit(INTERNAL_ERR);
            }
            strcpy(to->value_string, from->value_string);
            break;
        case IDENTIFICATOR:
            // TODO: delete after string implementation
            to->id = malloc(strlen(from->id) + 1);
            if (to->id == NULL) {
                error_exit(INTERNAL_ERR);
            }
            strcpy(to->id, from->id);
            // TODO:
            // Hash search and value copy
        default:
            break;
    }
    to->token_type = input_type;
}

T_NT_item *push_T_NT(T_NT_stack *s, Data_Term *in_term, Data_NTerm *in_non_term) {
    T_NT_item *new_top = malloc(sizeof(T_NT_item));
    if (new_top == NULL) {
        error_exit(INTERNAL_ERR);
    }

    new_top->next_T_NT = s->top;

    if (in_non_term == NULL && in_term != NULL) {
        new_top->is_non_term = false;
        copy_term(in_term, &(new_top->data.Term));
    }
    else if (in_term == NULL && in_non_term != NULL) {
        new_top->is_non_term = true;
        new_top->data.NTerm.rule = in_non_term->rule;
    }
    else {
        error_exit(INTERNAL_ERR);
    }

    s->top = new_top;
    return new_top;
}

T_NT_item *pop_T_NT(T_NT_stack *s) {

    if (s->popped != NULL) {
        if (!s->popped->is_non_term) {
            if (s->popped->data.Term.token_type == IDENTIFICATOR) {
                free(s->popped->data.Term.id);
            }
            if (s->popped->data.Term.token_type == STRING) {
                free(s->popped->data.Term.value_string); // TODO: use string lib
            }
        }
        free(s->popped);
        s->popped = NULL;
    }
    if (s->active == s->top) {
        s->active == NULL;
    }
    if (s->top == NULL) {
        return NULL;
    }

    s->popped = s->top;
    s->top = s->top->next_T_NT;
    s->popped->next_T_NT = NULL;

    return s->popped;
}

// Extended stack operations
// =======================

T_NT_item *set_first_T_NT(T_NT_stack *s) {
    s->active = s->top;
    return s->active;
}

T_NT_item *set_next_T_NT(T_NT_stack *s) {
    if (s->active != NULL) {
        s->active = s->active->next_T_NT;
    }
    return s->active;
}

bool active_T_NT(T_NT_stack *s) {
    return s->active != NULL;
}

T_NT_item* insert_after_T_NT(T_NT_stack *s, Data_Term *in_term, Data_NTerm *in_non_term) {
    if (s->active == NULL) {
        return NULL;
    }

    T_NT_item *item_after = malloc(sizeof(T_NT_item));
    if (item_after == NULL) {
        error_exit(INTERNAL_ERR);
    }

    item_after->next_T_NT = s->active->next_T_NT;
    if (in_non_term == NULL && in_term != NULL) {
        item_after->is_non_term = false;
        copy_term(in_term, &(item_after->data.Term));
    }
    else if (in_term == NULL && in_non_term != NULL) {
        item_after->is_non_term = true;
        item_after->data.NTerm.rule = in_non_term->rule;
    }
    else {
        error_exit(INTERNAL_ERR);
    }

    s->active->next_T_NT = item_after;
    return item_after;
}

// TODO: extend this function
void error_exit(int code) {
    extern T_NT_stack *processing_stack;
    extern T_NT_stack *evaluation_stack;
    if (code == INTERNAL_ERR) {
        printf("CHYBA PAMETI\n");
    }
    free_sources();
    destroy_T_NT_stack(processing_stack);
    destroy_T_NT_stack(evaluation_stack);
    exit(code);
}

// =======================
// End stack operations
