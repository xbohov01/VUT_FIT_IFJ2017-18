/* 
** PSA TAC commands
** Author: Danil Grigorev, xgrigo02
*/

#include "ifj2017.h"

void init_TAC_stack() {
    static bool was_init = false;
    if (!was_init) {
        printf("DEFVAR GF@$_stack_temp_1\n");
        printf("DEFVAR GF@$_stack_temp_2\n");
        was_init = true;
    }
    return;
}

void swap_stack() {
    printf("# Swap\n");
    printf("POPS GF@$_stack_temp_1\n");
    printf("POPS GF@$_stack_temp_2\n");
    printf("PUSHS GF@$_stack_temp_1\n");
    printf("PUSHS GF@$_stack_temp_2\n");
    printf("\n");
}

void push_var_id(char *name) {
    printf("PUSHS LF@_%s\n", name);
    return;
}

void push_const_id(Data_Term *item) {
    if (item->token_type == INTEGER) {
        printf("PUSHS int@%d\n", item->value_int);
    }
    else if (item->token_type == DOUBLE) {
        printf("PUSHS float@%lf\n", item->value_double);
    }
    else {
        fprintf(stderr, "Item is not constant, cant push\n");
        error_exit(INTERNAL_ERR);
    }
    return;
}

void retype_stack(bool second_operand, bool int2fl, bool round_to_even) {
    printf("# Retype start\n");
    if (second_operand == true) {
        printf("POPS GF@$_stack_temp_1\n");
    }

    if (int2fl == true) {
        printf("INT2FLOATS\n");
    } else {
        if (round_to_even == true) {
            printf("FLOAT2R2EINTS\n");
        } else {
            printf("FLOAT2INTS\n");
        }
    }

    if (second_operand = true) {
        printf("PUSHS GF@_stack_temp_1\n");
    }
    printf("\n");
    return;
}

void pop_to_result(char *res_name) {
    printf("# Save result\n");
    if (res_name == NULL) {
        printf("POPS GF@$_stack_temp_1\n");
    } else {
        printf("POPS LF@_%s\n", res_name);
    }
    printf("\n");
    return;
}

void clean_stack_TAC() {
    printf("CLEARS\n");
}

void arithm_TAC(PSA_Term_type op) {
    printf("# Arithmetics:\n");
    switch (op) {
        case ADD:
            printf("ADDS\n");
            break;
        case SUB:
            printf("SUBS\n");
            break;
        case MUL:
            printf("MULS\n");
            break;
        case DIV:
            printf("DIV\n");
            break;
        case IDIV:
            printf("DIV\n");
            retype_stack(false, false, true);
            break;

        case LT:
            printf("LTS\n");
            break;
        case GT:
            printf("GTS\n");
            break;
        case EQ:
            printf("EQS\n");
            break;
        case NEQ:
            printf("EQS\n");
            printf("NOTS\n");
            break;
        case LTE:
            printf("GTS\n");
            printf("NOTS\n");
            break;
        case GTE:
            printf("LTS\n");
            printf("NOTS\n");
            break;
        default:
            printf("Bad operator for TAC\n");
            break;
    }
    printf("\n");
    return;
}

void create_label(char *name) {
    printf("LABEL %s\n", name);
    return;
}