/* 
** PSA TAC commands
** Author: Danil Grigorev, xgrigo02
*/

#include "ifj2017.h"

// Init commands
// ========================================
void init_TAC_stack() {
    static bool was_init = false;
    if (!was_init) {
        printf("DEFVAR GF@$_stack_temp\n");
        was_init = true;
    }
    return;
}

void create_frame() {
    printf("CREATEFRAME\n");
}

void init_var(N_T_types type, char* name) {
    if (name == NULL) {
        fprintf(stderr, "NAME FOR INIT NOT SPECIFIED\n");
        error_exit(INTERNAL_ERR);
    }
    printf("# Var %s init\n", name);

    switch(type) {
        case DOUBLE_NT:
            printf("MOVE LF@_%s float@0.0\n", name);
            break;
        case INTEGER_NT:
            printf("MOVE LF@_%s int@0\n", name);
            break;
        case STRING_NT:
            printf("MOVE LF@_%s string@!\"\"\n", name);
            break;
        default:
            fprintf(stderr, "BAD VAR INIT TYPE\n");
            error_exit(INTERNAL_ERR);
    }
    return;
}

void create_label(char *name) {
    printf("LABEL %s\n", name);
    return;
}

// =========================================

// Answer handlers
// =========================================

void save_result(char *res_name) {
    printf("# Save final result\n");
    if (res_name == NULL) {
        fprintf(stderr, "Cant save variable to nothing\n");
        error_exit(INTERNAL_ERR);
    } else {
        printf("POPS LF@_%s\n", res_name);
    }  
    printf("\n");
    return;
}

// =========================================

// Number arithmetics
// =========================================
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
    else if (item->token_type == STRING) {
        printf("PUSHS string@%s\n", item->value_string);
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
        printf("POPS GF@$_stack_temp\n");
    }

    if (int2fl == true) {
        printf("INT2FLOATS\n");
    } else {
        printf("FLOAT2INTS\n");
    }

    if (second_operand == true) {
        printf("PUSHS GF@$_stack_temp\n");
    }
    printf("\n");
    return;
}

void arithm_stack(PSA_Term_type op) {
    printf("# Stack arithmetics:\n");
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

void clean_stack_TAC() {
    printf("CLEARS\n");
}

// =========================================

// Jumps or function calls
// =========================================
void cond_jump(bool is_while, int num) {
    printf("# Condition evaluation\n");
    printf("PUSHS bool@true\n");
    if (is_while) {
        printf("JUMPIFNEQS $end$while%d$label\n", num);
    } else {
        printf("JUMPIFNEQS $condition%d$end\n", num);
    }
    printf("\n");
}

void push_arg(int arg_num) {
    printf("# Add %d argument\n", arg_num);
    printf("POPS TF@$_arg_%d\n", arg_num);
    printf("\n");
    return;
}

void f_call(char *name) {
    printf("# Calling %s", name);
    printf("CALL $_%s\n", name);
    printf("\n");
    return;
}
// =========================================