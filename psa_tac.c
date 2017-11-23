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

void retype_stack(bool second_operand, bool int2fl) {
    printf("# Retype start\n");
    if (second_operand == true) {
        printf("POPS GF@$_stack_temp_1\n");
    }

    if (int2fl == true) {
        printf("INT2FLOATS\n");
    } else {
        printf("FLOAT2INTS\n");
    }

    if (second_operand == true) {
        printf("PUSHS GF@$_stack_temp_1\n");
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

void init_var(N_T_types type, char* name) {
    if (name == NULL) {
        fprintf(stderr, "NAME FOR INIT NOT SPECIFIED\n");
        error_exit(INTERNAL_ERR);
    }

    switch(type) {
        case DOUBLE_NT:
            printf("MOVE LF@%s float@0.0\n", name);
            break;
        case INTEGER_NT:
            printf("MOVE LF@%s int@0\n", name);
            break;
        case STRING_NT:
            printf("MOVE LF@%s string@!\"\"\n", name);
            break;
        default:
            fprintf(stderr, "BAD VAR INIT TYPE\n");
            error_exit(INTERNAL_ERR);
    }
    return;
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
            printf("DIVS\n");
            break;
        case IDIV:
            printf("DIVS\n");
            retype_stack(false, false);
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

void cond_jump(bool is_while, int num) {
    printf("# Condition evaluation\n");
    printf("PUSHS bool@true\n");
    if (is_while) {
        printf("JUMPIFNEQS $end$while%d$label\n", num);
    } else {
        printf("JUMPIFNEQS $condition%d$end\n", num);
    }
}