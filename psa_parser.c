/* 
** Precedense syntax parser implementation
** Author: Danil Grigorev, xgrigo02
*/
// #include "scanner.h"
#include "psa_parser.h"
// #include "ial.c"


// TODO: to be removed
// Tests part
// =======================

#include <string.h>


int main() {
    // // Simulate scanner actions 

    // // Was read already from scanner
    // Token last_token;

    // // Make stack init
    // T_NT_stack new_stack;
    // stack_init(&new_stack);
    //                         //   type, value
    // Token end_token = init_token(STR,  "$")
    // stack_push(&new_stack, end_token)
    // eval_expr()
    return test();
}


void print_stack() {
    static int call_num = 1;
    char non_term_type_names[10][10] = {
    "NT_ADD", // 1: E + E
    "NT_SUB",     // 2: E - E
    "NT_MUL",     // 3: E * E
    "NT_DIV",     // 4: E / E
    "NT_MOD",     // 5: E \ E
    "NT_PAR",     // 6: (E)
    "NT_LT",      // 7: E < E
    "NT_GT",      // 8: E > E
    "NT_LE",      // 9: E <= E
    "NT_GE"       // 10:E >= E
    };

    T_NT_item *active_temp = processing_stack->active;

    T_NT_item *act;
    set_first_T_NT();

    printf("Test %d:\n", call_num);
    printf("=================\n\n");

    while (active_T_NT()) {
        act = processing_stack->active;
        printf("is_non_term: %d\t", act->is_non_term);
        if (act->is_non_term) {
            printf("Non-term type: %s\n", non_term_type_names[act->data.NTerm.NT_type]);
        }
        else {
            printf("Name: %s\t", act->data.Term.id);
            switch(act->data.Term.token_type) {
                case INTEGER:
                    printf("Int_value: %d\n", act->data.Term.value_int);
                    break;
                case DOUBLE:
                    printf("Double_value: %lf\n", act->data.Term.value_double);
                    break;
                case STRING:
                    printf("String value: %s\n", act->data.Term.value_string);
                    break;
                default:
                    printf("Unknown\n");
            }
        }

        printf("\t|\n");
        next_T_NT();
    }
    if (processing_stack->active == NULL) {
        printf("NULL\n");
    }

    printf("\n=================\n");
    printf("End test %d\n", call_num++);

    processing_stack->active = active_temp;
    return;
}



// Simulate token
// ++++++++++++++++++++
// STRING type
T_NT_Data *str_fill_data_token(T_token_type type, char* id, char* str) {
    T_NT_Data *temp_data = malloc(sizeof(T_NT_Data));
    if (temp_data == NULL) {
        error_exit(INTERNAL_ERR);
    }

    Data_Term temp_term = temp_data->Term;

    temp_term.id = malloc(20);
    if (temp_term.id == NULL) {
        error_exit(INTERNAL_ERR);
    }

    temp_term.value_string = malloc(20);
    if (temp_term.value_string == NULL) {
        error_exit(INTERNAL_ERR);
    }

    memcpy(temp_term.id, id, sizeof(id) + 1);
    memcpy(temp_term.value_string, str, sizeof(str) + 1);

    temp_term.token_type = STRING;

    temp_data->Term = temp_term;
    return temp_data;
} // End STRING type

// DOUBLE type
T_NT_Data *double_fill_data_token(T_token_type type, char* id, double d_val) {
    T_NT_Data *temp_data = malloc(sizeof(T_NT_Data));
    if (temp_data == NULL) {
        error_exit(INTERNAL_ERR);
    }

    Data_Term temp_term = temp_data->Term;

    temp_term.id = malloc(20);
    if (temp_term.id == NULL) {
        error_exit(INTERNAL_ERR);
    }

    memcpy(temp_term.id, id, sizeof(id) + 1);
    temp_term.value_double = d_val;

    temp_term.token_type = DOUBLE;

    temp_data->Term = temp_term;
    return temp_data;
} // End DOUBLE type

// INTEGER type
T_NT_Data *int_fill_data_token(T_token_type type, char* id, int int_val) {
    T_NT_Data *temp_data = malloc(sizeof(T_NT_Data));
    if (temp_data == NULL) {
        error_exit(INTERNAL_ERR);
    }

    Data_Term temp_term = temp_data->Term;

    temp_term.id = malloc(20);
    if (temp_term.id == NULL) {
        error_exit(INTERNAL_ERR);
    }

    memcpy(temp_term.id, id, sizeof(id) + 1);
    temp_term.value_int = int_val;

    temp_term.token_type = INTEGER;

    temp_data->Term = temp_term;
    return temp_data;
} // end INTEGER type

// Non terminal type
T_NT_Data *non_term_create(NT_types input_type) {
    T_NT_Data *temp_non_term_data = malloc(sizeof(T_NT_Data));
    if (temp_non_term_data == NULL) {
        error_exit(INTERNAL_ERR);
    }

    Data_NTerm temp_non_term = temp_non_term_data->NTerm;

    temp_non_term.NT_type = input_type;

    temp_non_term_data->NTerm = temp_non_term;
    return temp_non_term_data;
} // end Non terminal type

// ++++++++++++++++
// End simulate token



int test() {
    init_T_NT_stack();
    
    push_T_NT(true, *non_term_create(NT_SUB));
    push_T_NT(true, *non_term_create(NT_ADD));
    push_T_NT(false, *int_fill_data_token(INTEGER, "x", 42));
    print_stack();

    pop_T_NT();
    print_stack();

    push_T_NT(false, *double_fill_data_token(DOUBLE, "double", 3.14));
    push_T_NT(false, *str_fill_data_token(STRING, "some", "string"));
    print_stack();

    set_first_T_NT();
    next_T_NT();
    insert_after_T_NT(false, *str_fill_data_token(STRING, "string", "inside"));
    print_stack();

    pop_T_NT();
    pop_T_NT();
    print_stack();

    destroy_T_NT_stack();
    printf("Hello world\n");
    return 0;
}

// =======================
// End tests

// TODO: add to separate file
// Stack operations
// =======================

void init_T_NT_stack() {
    processing_stack = malloc(sizeof(T_NT_stack));
    if (processing_stack == NULL) {
        error_exit(INTERNAL_ERR);
    }

    processing_stack->top = NULL;
    processing_stack->active = NULL;

    return;
}

void destroy_T_NT_stack() {
    T_NT_item *temp = processing_stack->top;

    while (temp != NULL) {
        processing_stack->top = processing_stack->top->next_T_NT;
        if (!temp->is_non_term) {
            free(temp->data.Term.id);
                if (temp->data.Term.token_type == STRING) {
                    free(temp->data.Term.value_string);
            }
        }
        free(temp);
        temp = processing_stack->top;
    }

    if (processing_stack != NULL) {
        free(processing_stack);
    }
    return;
}

T_NT_item *push_T_NT(bool non_term, T_NT_Data input_data) {
    T_NT_item *new_top = malloc(sizeof(T_NT_item));
    if (new_top == NULL) {
        error_exit(INTERNAL_ERR);
    }

    new_top->next_T_NT = processing_stack->top;
    new_top->is_non_term = non_term;
    new_top->data = input_data;

    processing_stack->top = new_top;
    return new_top;
}

T_NT_item* pop_T_NT() {
    if (processing_stack->top == NULL) {
        return NULL;
    }

    // TODO: Possible wont work
    T_NT_item* popped;
    popped = malloc(sizeof(T_NT_item));
    if (popped == NULL) {
        error_exit(INTERNAL_ERR);
    }

    popped->is_non_term = processing_stack->top->is_non_term;
    popped->data = processing_stack->top->data;
    popped->next_T_NT = NULL; // Does not need this

    T_NT_item* temp = processing_stack->top;
    processing_stack->top = processing_stack->top->next_T_NT;

    if (!temp->is_non_term) {
        free(temp->data.Term.id);
        if (temp->data.Term.token_type == STRING) {
            free(temp->data.Term.value_string);
        }
    }
    free(temp);

    return popped;
}


// Extended stack operations
// =======================

void set_first_T_NT() {
    processing_stack->active = processing_stack->top;
}

void next_T_NT() {
    if (processing_stack->active != NULL) {
        processing_stack->active = processing_stack->active->next_T_NT;
    }
}

static inline bool active_T_NT() {
    return processing_stack->active != NULL;
}

T_NT_item* insert_after_T_NT(bool non_term, T_NT_Data input_data) {
    if (processing_stack->active == NULL) {
        return NULL;
    }

    T_NT_item *item_after = malloc(sizeof(T_NT_item));
    if (item_after == NULL) {
        error_exit(INTERNAL_ERR);
    }

    item_after->next_T_NT = processing_stack->active->next_T_NT;
    item_after->is_non_term = non_term;
    item_after->data = input_data;

    processing_stack->active->next_T_NT = item_after;
    return item_after;
}

// =======================
// End stack operations

// TODO: extend this function 
void error_exit(int code) {
    destroy_T_NT_stack(&processing_stack);
    exit(code);
}