/*
** PSA TAC commands
** Author: Danil Grigorev, xgrigo02
*/

#include "ifj2017.h"

// Init commands
// ========================================
void init_TAC_stack() {
    printf("DEFVAR GF@$_stack_temp\n");
    printf("DEFVAR GF@$_str_temp_1\n");
    printf("DEFVAR GF@$_str_temp_2\n");
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
            printf("MOVE LF@_%s string@\n", name);
            break;
        default:
            fprintf(stderr, "BAD VAR INIT TYPE\n");
            error_exit(INTERNAL_ERR);
    }
    return;
}

void start_program() {
    // header
    printf(".IFJcode17\n");
    printf("JUMP $$main\n");
    printf("\n");
}

void end_scope() {
    printf("POPFRAME\n");
    printf("JUMP $_end_program\n");
    printf("\n");
}

// =========================================

// Answer handlers
// =========================================

void save_to_temp() {
    printf("# Saving to temp for print statement\n");
    printf("POPS GF@$_stack_temp\n");
    printf("\n");
}

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

void retype_to_even_int() {
    printf("# Retype to int with even precedence\n");
    printf("FLOAT2R2EINTS\n");
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
            printf("Bad operator for stack\n");
            break;
    }
    printf("\n");
    return;
}

// String operations 
// ========================================
void str_arithm(PSA_Term_type op) {
    printf("# String arithmetics\n");
    switch(op) {
        case ADD:
            printf("POPS GF@$_str_temp_2\n");
            printf("POPS GF@$_str_temp_1\n");
            printf("CONCAT GF@$_str_temp_1 GF@$_str_temp_1 GF@$_str_temp_2\n");
            printf("PUSHS GF@$_str_temp_1\n");
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
    return;
}

// Print operations
// =========================================
void write_space() {
    printf("WRITE string@\\032\n");
}

void write_output() {
    printf("WRITE GF@$_stack_temp\n");
}

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
    printf("DEFVAR TF@$_arg_%d\n", arg_num);
    printf("POPS TF@$_arg_%d\n", arg_num);
    printf("\n");
    return;
}

void return_f(char *name) {
    printf("# Returning from '%s'\n", name);
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    return;
}

void f_call(char *name) {
    printf("# Calling %s\n", name);
    printf("CALL $_%s\n", name);
    if (strcmp(name, "chr") == 0) {
        add_build_in('C', false);
    }
    else if (strcmp(name, "asc") == 0) {
        add_build_in('A', false);
    }
    else if (strcmp(name, "substr") == 0) {
        add_build_in('S', false);
    }
    else if (strcmp(name, "length") == 0) {
        add_build_in('L', false);
    }
    printf("\n");
    return;
}

void define_built_in_func() {
    if (add_build_in(0, true)[0] == true) {
        //inbuilt function
        printf(
            "LABEL $_length\n"
            "PUSHFRAME\n"
            "STRLEN GF@$_stack_temp LF@$_arg_0\n"
            "PUSHS GF@$_stack_temp\n"
            "POPFRAME\n"
            "RETURN\n\n"
            );
    }
    if (add_build_in(0, true)[1] == true) {
        // SUBSTRING function
        printf(
            "LABEL $_substr\n"
            "PUSHFRAME\n"
            "STRLEN GF@$_str_temp_2 LF@$_arg_0\n"
            "JUMPIFEQ $_assert_zero GF@$_str_temp_2 int@0\n"
            "\n"
            "LT GF@$_str_temp_1 LF@$_arg_1 int@1\n"
            "JUMPIFEQ $_assert_zero GF@$_str_temp_1 bool@true\n"
            "\n"
            "LT GF@$_str_temp_1 LF@$_arg_2 int@0\n"
            "JUMPIFEQ $_truncate_N GF@$_str_temp_1 bool@true\n"
            "\n"
            "SUB GF@$_str_temp_1 GF@$_str_temp_2 LF@$_arg_1\n"
            "GT GF@$_str_temp_1 LF@$_arg_2 GF@$_str_temp_1\n"
            "JUMPIFEQ $_truncate_N GF@$_str_temp_1 bool@true\n"
            "JUMP $_cycle_start\n"
            "LABEL $_truncate_N\n"
            "SUB GF@$_str_temp_1 GF@$_str_temp_2 LF@$_arg_1\n"
            "ADD GF@$_str_temp_1 GF@$_str_temp_1 int@1\n"
            "MOVE LF@$_arg_2 GF@$_str_temp_1\n"
            "\n"
            "LABEL $_cycle_start\n"
            "SUB LF@$_arg_1 LF@$_arg_1 int@1\n"
            "MOVE GF@$_stack_temp string@\n"
            "LABEL $_compare_next\n"
            "JUMPIFEQ $_all_done LF@$_arg_2 int@0\n"
            "GETCHAR GF@$_str_temp_1 LF@$_arg_0 LF@$_arg_1\n"
            "CONCAT GF@$_stack_temp GF@$_stack_temp GF@$_str_temp_1\n"
            "ADD LF@$_arg_1 LF@$_arg_1 int@1\n"
            "SUB LF@$_arg_2 LF@$_arg_2 int@1\n"
            "JUMP $_compare_next\n"
            "\n"
            "LABEL $_assert_zero\n"
            "MOVE GF@$_stack_temp string@\n"
            "\n"
            "LABEL $_all_done\n"
            "PUSHS GF@$_stack_temp\n"
            "POPFRAME\n"
            "RETURN\n\n"
            );
    }
    if (add_build_in(0, true)[2] == true) {
        // ASC function
        printf(
            "LABEL $_asc\n"
            "PUSHFRAME\n"
            "STRLEN GF@$_stack_temp LF@$_arg_0\n"
            "JUMPIFEQ $_assert_zero_end GF@$_stack_temp int@0\n"
            "\n"
            "SUB GF@$_stack_temp GF@$_stack_temp int@1\n"
            "LT GF@$_str_temp_1 LF@$_arg_1 int@0\n"
            "JUMPIFEQ $_assert_zero_end GF@$_str_temp_1 bool@true\n"
            "\n"
            "GT GF@$_str_temp_1 LF@$_arg_1 GF@$_stack_temp\n"
            "JUMPIFEQ $_assert_zero_end GF@$_str_temp_1 bool@true\n"
            "\n"
            "STRI2INT GF@$_stack_temp LF@$_arg_0 LF@$_arg_1\n"
            "JUMP $_asc_end\n"
            "\n"
            "LABEL $_assert_zero_end\n"
            "MOVE GF@$_stack_temp int@0\n"
            "LABEL $_asc_end\n"
            "PUSHS GF@$_stack_temp\n"
            "POPFRAME\n"
            "RETURN\n\n"
            );
    }
    if (add_build_in(0, true)[3] == true) {
        // CHR function
        printf(
            "LABEL $_chr\n"
            "PUSHFRAME\n"
            "INT2CHAR GF@$_stack_temp LF@$_arg_0\n"
            "PUSHS GF@$_stack_temp\n"
            "POPFRAME\n"
            "RETURN\n\n"
            );
    }

    // All finished
    printf("LABEL $_end_program\n");
    printf("\n");
    return;
}

bool *add_build_in(char which, bool only_return) {
    static bool builtin_calls[4] = {0, 0, 0, 0};
                              //  L  S  A  C
    if (only_return == true) {
        return builtin_calls;
    }
    else {
        switch(which) {
            case 'L':
                builtin_calls[0] = true;
                break;
            case 'S':
                builtin_calls[1] = true;
                break;
            case 'A':
                builtin_calls[2] = true;
                break;
            case 'C':
                builtin_calls[3] = true;
                break;
            default:
                fprintf(stderr, "Unknown built-in call\n");
                error_exit(INTERNAL_ERR);
        }
        return builtin_calls;
    }
}
// =========================================
