/*
** PSA symtable stack tests implementation
** Author: Danil Girgorev, xgrigo02
*/

// Tests part
// =======================

#include "psa_stack_test.h"

void print_stack(T_NT_stack *s) {
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

    T_NT_item *active_temp = s->active;

    T_NT_item *act;
    set_first_T_NT(s);

    printf("Test %d:\n", call_num);
    printf("=================\n\n");

    while (active_T_NT(s)) {
        act = s->active;
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
        next_T_NT(s);
    }
    if (s->active == NULL) {
        printf("NULL\n");
    }

    printf("\n=================\n");
    printf("End test %d\n", call_num++);

    s->active = active_temp;
    return;
}



// Simulate token
// ++++++++++++++++++++
// STRING type
T_NT_Data *str_fill_data_token(T_token_type type, char* id, char* str) {
    T_NT_Data *temp_data = malloc(sizeof(T_NT_Data));
    if (temp_data == NULL) {
        return NULL;
    }

    Data_Term temp_term = temp_data->Term;

    temp_term.id = malloc(20);
    if (temp_term.id == NULL) {
        return NULL;
    }

    temp_term.value_string = malloc(20);
    if (temp_term.value_string == NULL) {
        return NULL;
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
        return NULL;
    }

    Data_Term temp_term = temp_data->Term;

    temp_term.id = malloc(20);
    if (temp_term.id == NULL) {
        return NULL;
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
        return NULL;
    }

    Data_Term temp_term = temp_data->Term;

    temp_term.id = malloc(20);
    if (temp_term.id == NULL) {
        return NULL;
    }

    memcpy(temp_term.id, id, sizeof(id) + 1);
    temp_term.value_int = int_val;

    temp_term.token_type = INTEGER;

    temp_data->Term = temp_term;
    return temp_data;
} // end INTEGER type

// ++++++++++++++++
// End simulate token


int test() {

    T_NT_stack *processing_stack;
    T_NT_Data *temp_data;
    processing_stack = init_T_NT_stack();

    temp_data = non_term_create(NT_GT);
    push_T_NT(processing_stack, true, *temp_data);
    free(temp_data);
    print_stack(processing_stack);


    pop_T_NT(processing_stack);
    print_stack(processing_stack);

    temp_data = non_term_create(NT_SUB);
    push_T_NT(processing_stack, true, *temp_data);
    free(temp_data);
    temp_data = non_term_create(NT_ADD);
    push_T_NT(processing_stack, true, *temp_data);
    free(temp_data);
    temp_data = int_fill_data_token(INTEGER, "x", 42);
    push_T_NT(processing_stack, false, *temp_data);
    free(temp_data);
    print_stack(processing_stack);

    pop_T_NT(processing_stack);
    print_stack(processing_stack);

    temp_data = double_fill_data_token(DOUBLE, "double", 3.14);
    push_T_NT(processing_stack, false, *temp_data);
    free(temp_data);
    temp_data = str_fill_data_token(STRING, "some", "string");
    push_T_NT(processing_stack, false, *temp_data);
    free(temp_data);
    print_stack(processing_stack);

    set_first_T_NT(processing_stack);
    next_T_NT(processing_stack);
    temp_data = str_fill_data_token(STRING, "string", "inside");
    insert_after_T_NT(processing_stack, false, *temp_data);
    free(temp_data);
    print_stack(processing_stack);

    pop_T_NT(processing_stack);
    pop_T_NT(processing_stack);
    print_stack(processing_stack);

    destroy_T_NT_stack(processing_stack);
    return 0;
}

// =======================
// End tests
