/*
** PSA symtable stack tests implementation
** Author: Danil Girgorev, xgrigo02
*/

// Tests part
// =======================

#include "ifj2017.h"

void ps(T_NT_stack *s) {
    static int call_num = 1;
    


    if (s == NULL) {
        fprintf(stderr, "Stack is null\n");
        return;
    }

    T_NT_item *active_temp = s->active;

    T_NT_item *act;
    set_first_T_NT(s);

    fprintf(stderr, "Test %d:\n\n", call_num++);

    while (active_T_NT(s)) {
        act = s->active;
        print_item(act);
        fprintf(stderr, "\t|\n");
        set_next_T_NT(s);
    }
    if (s->active == NULL) {
        fprintf(stderr, "NULL\n\n");
    }
    if (s->popped != NULL) {
        fprintf(stderr, "Popped:\t");
        print_item(s->popped);
    }

    fprintf(stderr, "\n=================\n\n");

    s->active = active_temp;
    return;
}

void print_item(T_NT_item *act) {

    static char term_type_names[18][10] = {
        "+",    // "ADD",
        "*",    // "MUL",
        "-",    // "SUB",
        "/",    // "DIV",
        "\\",   // "MOD",
        "<",    // "LT",
        ">",    // "GT",
        "<=",   // "LTE",
        ">=",   // "GTE",
        "==",   // "EQ",
        "<>",   // "NEQ",
        "(",    // "PL",
        ")",    // "PR",
        "i",    // "ID",
        "f",    // "FNC",
        ",",    // "CM",
        "$"     // "END"
    };

    static char N_T_type_names[4][11] = {
        "INTEGER_NT",
        "DOUBLE_NT",
        "STRING_NT",
        "STOPPER"
    };

    hash_tab_symbol_type *found_func;
    hash_tab_symbol_type *found_var;

    if (act->is_non_term) {
        fprintf(stderr, "Non-term type: %s\n", N_T_type_names[act->data.NT_type]);
    }
    else {
        switch(act->data.Term.token_type) {
            case INTEGER:
                fprintf(stderr, "\tInt value: %d\n", act->data.Term.value_int);
                break;
            case DOUBLE:
                fprintf(stderr, "\tDouble value: %lf\n", act->data.Term.value_double);
                break;
            case STRING:
                fprintf(stderr, "\tString value: %s\n", act->data.Term.value_string);
                break;
            default:
                fprintf(stderr, "Term type: %s", term_type_names[get_term_type(&(act->data.Term))]);
                if (act->data.Term.token_type == IDENTIFICATOR) {
                    if ((found_var = hash_table_search(var_table, act->data.Term.id)) != NULL)
                        fprintf(stderr, "\tName: %s Type: %s\n", act->data.Term.id, N_T_type_names[found_var->value_type]);
                    else {
                        fprintf(stderr, "\tUnknown variable %s\n", act->data.Term.id);
                    }
                }
                else if (act->data.Term.token_type == FUNCTION) {
                    if ((found_func = hash_table_search(func_table, act->data.Term.id)) != NULL)
                        fprintf(stderr, "\tName: %s Return val: %s\t Params: %s\n", act->data.Term.id, N_T_type_names[found_func->value_type], found_func->param_types);
                    else {
                        fprintf(stderr, "\tUnknown function %s\n", act->data.Term.id);
                    }
                }
                else {
                    fprintf(stderr, "\n");
                }
                break;
        }
    }
    return;
}


