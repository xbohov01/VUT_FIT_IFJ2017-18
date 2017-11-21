/*
** PSA symtable stack tests implementation
** Author: Danil Girgorev, xgrigo02
*/

// Tests part
// =======================

#include "ifj2017.h"

void print_stack(T_NT_stack *s) {
    static int call_num = 1;
    char non_term_type_names[10][10] = {
    "NT_ADD", // 1: E + E
    "NT_SUB",     // 2: E - E
    "NT_MUL",     // 3: E * E
    "NT_DIV",     // 4: E / E
    "NT_IDIV",     // 5: E \ E
    "NT_PAR",     // 6: (E)
    "NT_ID",      // 7: id
    "NT_FN",      // 8: function
    "STOPPER"     // 8: <
    };

    char term_type_names[18][10] = {
        "+",    // "ADD",
        "*",    // "MUL",
        "-",    // "SUB",
        "/",    // "DIV",
        "\\",   // "MOD",
        "(",    // "PL",
        ")",    // "PR",
        "i",    // "ID",
        "f",    // "FNC",
        ",",    // "CM",
        "$",    // "END",

        "<",    // "LT",
        ">",    // "GT",
        "<=",   // "LTE",
        ">=",   // "GTE",
        "<>",   // "EQ",
        "==",   // "NEQ",
        "ER",   // "PSA_ERR"
    };

    if (s == NULL) {
        printf("Stack is null\n");
        return;
    }

    T_NT_item *active_temp = s->active;

    T_NT_item *act;
    set_first_T_NT(s);

    printf("Test %d:\n", call_num);
    printf("=================\n\n");

    while (active_T_NT(s)) {
        act = s->active;
        printf("is_non_term: %d\t", act->is_non_term);
        if (act->is_non_term) {
            printf("Non-term type: %s\n", non_term_type_names[act->data.NTerm.rule]);
        }
        else {
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
                    printf("Term type: %s", term_type_names[get_term_type(&(act->data.Term))]);
                    if (act->data.Term.token_type == IDENTIFICATOR) {
                        printf("\tName: %s\n", act->data.Term.id);
                    }
                    else {
                        printf("\n");
                    }
            }
        }
        printf("\t|\n");
        set_next_T_NT(s);
    }
    if (s->popped != NULL) {
        printf("Popped:\t");
        switch(s->popped->data.Term.token_type) {
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
                printf("Term type: %s", term_type_names[get_term_type(&(act->data.Term))]);
                if (act->data.Term.token_type == IDENTIFICATOR) {
                    printf("\tName: %s\n", act->data.Term.id);
                }
                else {
                    printf("\n");
                }
        }
        printf("\t|\n");
    }
    if (s->active == NULL) {
        printf("NULL\n");
    }

    printf("\n=================\n");
    printf("End test %d\n", call_num++);

    s->active = active_temp;
    return;
}


