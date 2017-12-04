/*
** Prekladac jazyka IFJ2017
** Precedence syntax parser implementation
** Author: Danil Grigorev, xgrigo02
*/

#include "ifj2017.h"

// Feeds non terminal
T_NT_item *create_non_term(N_T_types input_type) {

    T_NT_item *temp_non_term = malloc(sizeof(T_NT_item));
    if (temp_non_term == NULL) {
        error_exit(INTERNAL_ERR);
    }

    temp_non_term->is_non_term = true;
    temp_non_term->data.NT_type = input_type;

    return temp_non_term;
}

void control_result_type_conform(int value_type) {
    switch(value_type) {
        case 0:
            switch(processing_stack->top->data.NT_type) {
                case INTEGER_NT:
                    // Is ok
                    break;
                case DOUBLE_NT:
                    retype_to_even_int();
                    processing_stack->top->data.NT_type = INTEGER_NT;
                    break;
                default:
                    fprintf(stderr, "Unexpected operand type, for int or float result value\n");
                    error_exit(TYPE_ERR);
                    break;
            }
            break;
        case 1:
            switch(processing_stack->top->data.NT_type) {
                case INTEGER_NT:
                    retype_stack(false, true);
                    processing_stack->top->data.NT_type = DOUBLE_NT;
                    break;
                case DOUBLE_NT:
                    // Is ok
                    break;
                default:
                    fprintf(stderr, "Unexpected operand type, for int or float result value\n");
                    error_exit(TYPE_ERR);
                    break;
            }
            break;
        case 2:
            switch(processing_stack->top->data.NT_type) {
                case STRING_NT:
                    // Is ok
                    break;
                default:
                    fprintf(stderr, "Unexpected operand type, for string result value\n");
                    error_exit(TYPE_ERR);
                    break;
            }
            break;
        default:
            fprintf(stderr, "Unexpected result variable type %d\n", value_type);
            error_exit(INTERNAL_ERR);
            break;
    }
    return;
}

void push_start_term(T_NT_stack *s) {
    Data_Term *temp_data = malloc(sizeof(T_NT_item));
    if (temp_data == NULL) {
        error_exit(INTERNAL_ERR);
    }

    temp_data->token_type = ENDL;

    push_T(s, temp_data);
    free(temp_data);
    return;
}

void check_psa_completion() {
    extern T_NT_stack *processing_stack;
    pop_T_NT(processing_stack);
    if (processing_stack->popped == NULL) {
        fprintf(stderr, "processing_stack is already free\n");
        error_exit(INTERNAL_ERR);
    }

    if (processing_stack->popped->is_non_term == true) {
        pop_T_NT(processing_stack);
        if ((processing_stack->popped->is_non_term == true) || (get_term_type(&(processing_stack->popped->data.Term)) != END)) {
            fprintf(stderr, "Stack is not free at the end\n");
            error_exit(INTERNAL_ERR);
        }
    }
    else {
        fprintf(stderr, "Expression is zero length\n");
        error_exit(SYNT_ERR);
    }
}

// <cond_expr> -> <expr> <cond_expr_2>
// <cond_expr2> -> eps
// <cond_expr_2> -> <rel> <expr>
void eval_cond_expr(bool is_do_while, int context, int end_block) {

    extern T_NT_stack *processing_stack;

    push_start_term(processing_stack);
    psa_operation(true);

    cond_jump(is_do_while, context, end_block);

    check_psa_completion();
    if (get_term_type(&(currentToken)) != END) {
        fprintf(stderr, "Unexpected token instead of end\n");
        error_exit(INTERNAL_ERR);
    }
    return;
}


void eval_return_expr(hash_tab_symbol_type *func) {
    // set expr_start
    get_token();
    if (get_term_type(&(currentToken)) == END) {
        fprintf(stderr, "Function can't return void\n");
        error_exit(SYNT_ERR);
    }
    push_start_term(processing_stack);
    psa_operation(false);
    control_result_type_conform(func->value_type);
    check_psa_completion();
    if (currentToken.token_type != ENDL) {
        fprintf(stderr, "Unexpected item after return\n");
        error_exit(SYNT_ERR);
    }

    return_f(func->symbol_name);
    return;
}
// <expr> -> <CHECK_START_SYM> <VAR_INIT>
// <expr> -> <CHECK_START_SYM> <VAR_DEFINITION>
// <expr> -> <CHECK_START_SYM> <FUNC_CALL>
void eval_expr() {

    enum {
        CHECK_START_SYM,
        PRINT_START,
        PRINT_CONT,
        VAR_INIT,
        VAR_DEFINITION,
        CALC_PSA,
        END_CHECK,
        FINISHED_EXPR
    } eval_ex_state;

    hash_tab_symbol_type *result_variable;

    eval_ex_state = CHECK_START_SYM;

    while (eval_ex_state != FINISHED_EXPR) {
        switch (eval_ex_state) {
            case CHECK_START_SYM:
                if (currentToken.token_type == IDENTIFICATOR) {
                    result_variable = hash_table_search(var_table, currentToken.id);
                    if (result_variable == NULL) {
                        fprintf(stderr, "Variable %s was not declared\n", currentToken.id);
                        error_exit(UNDEF_ERR);
                    }
                    // <CHECK_START_SYM> -> 'id'
                    else {
                        eval_ex_state = VAR_DEFINITION;
                    }
                }
                else if (currentToken.token_type == EQ_O) {
                // <CHECK_START_SYM> -> '='
                    eval_ex_state = VAR_INIT;
                }
                else if (currentToken.token_type == PRINT_KEY) {
                    eval_ex_state = PRINT_START;
                }
                else if (currentToken.token_type == SEM) {
                    eval_ex_state = PRINT_CONT;
                }
                else {
                    fprintf(stderr, "Bad variable on input of eval_expr\n");
                    error_exit(INTERNAL_ERR);
                }
                break;
            case VAR_INIT:
                // set expr_start
                get_token();
                if (get_term_type(&(currentToken)) == END) {
                    fprintf(stderr, "Expression zero length\n");
                    error_exit(SYNT_ERR);
                }
                push_start_term(processing_stack);
                psa_operation(false);
                eval_ex_state = END_CHECK;
                break;
            case PRINT_START:
                get_token();
                // CANT be ended at once
                if (get_term_type(&currentToken) == END) {
                    fprintf(stderr, "Expression zero length after print\n");
                    error_exit(SYNT_ERR);
                }
                push_start_term(processing_stack);
                psa_operation(false);
                // Expected ';'
                if (currentToken.token_type != SEM) {
                    fprintf(stderr, "Expecting ';' after first print expression\n");
                    error_exit(SYNT_ERR);
                }
                // Write output part
                save_to_temp();
                if ((processing_stack->top->data.NT_type == INTEGER_NT) \
                    || (processing_stack->top->data.NT_type == DOUBLE_NT)) {
                    // write_space(); // TODO: maybe not needed
                }
                write_output();
                check_psa_completion();
                eval_ex_state = END_CHECK;
                break;
            case PRINT_CONT:
                get_token();
                // Can be ended at once
                if (get_term_type(&currentToken) == END) {
                    eval_ex_state = FINISHED_EXPR;
                    break;
                }
                push_start_term(processing_stack);
                psa_operation(false);
                // Expected ';'
                if (currentToken.token_type != SEM) {
                    fprintf(stderr, "Expecting ';' after first print expression\n");
                    error_exit(SYNT_ERR);
                }
                // Write output part
                save_to_temp();
                if ((processing_stack->top->data.NT_type == INTEGER_NT) \
                    || (processing_stack->top->data.NT_type == DOUBLE_NT)) {
                    // write_space(); // TODO: maybe not needed
                }
                write_output();
                check_psa_completion();
                eval_ex_state = END_CHECK;
                break;
            case VAR_DEFINITION:
                // set '='
                get_token();
                if (currentToken.token_type != EQ_O) {
                    fprintf(stderr, "Expected '='\n");
                    error_exit(SYNT_ERR);
                }
                // set expr_start
                get_token();
                push_start_term(processing_stack);
                psa_operation(false);
                control_result_type_conform(result_variable->value_type);
                save_result(result_variable->symbol_name);
                check_psa_completion();
                eval_ex_state = END_CHECK;
                break;
            case END_CHECK:
                if (get_term_type(&(currentToken)) != END) {
                    fprintf(stderr, "Unexpected token instead of end\n");
                    error_exit(SYNT_ERR);
                }
                eval_ex_state = FINISHED_EXPR;
                break;
            default:
                fprintf(stderr, "State machine error.\n");
                break;
        }
    }
    return;
}

T_NT_item *find_first_term(T_NT_stack *s, bool *is_first) {
    set_first_T_NT(s);
    if (!active_T_NT(s)) {
        fprintf(stderr, "List has no items\n");
        error_exit(INTERNAL_ERR);
    }
    *is_first = false;

    // In case that top item is term, should return it
    // and notification to do push instead of insert_after
    if (s->active->is_non_term == false) {
        *is_first = true;
        return s->active;
    }

    bool found = false;
    while ((found == false) && (s->active->next_T_NT != NULL)) {
        if (s->active->next_T_NT->is_non_term == false)
            found = true;
        else
            set_next_T_NT(s);
    }

    if (found == false) {
        fprintf(stderr, "List contains zero terminals\n");
        error_exit(INTERNAL_ERR);
    }

    return s->active->next_T_NT;
}

void insert_stopper(T_NT_stack *s) {
    bool use_push;
    T_NT_item *stopper;
    stopper = create_non_term(STOPPER);
    find_first_term(s, &use_push);
    if (use_push)
        push_NT(s, stopper);
    else
        insert_after_NT(s, stopper);
    free(stopper);
    return;
}

Data_Term *get_item_before_stopper() {
    T_NT_item *tmp = processing_stack->top;
    set_first_T_NT(processing_stack);
    while ((processing_stack->active->is_non_term != true) || (processing_stack->active->data.NT_type != STOPPER)) {
        tmp = processing_stack->active;
        set_next_T_NT(processing_stack);
    }
    return &(tmp->data.Term);
}

T_NT_item *id_R(hash_tab_symbol_type *found_var) {

    T_NT_item *look_ahead;
    T_NT_item *non_term;
    Data_Term *T;
    // id
    look_ahead = processing_stack->popped;
    T = &(look_ahead->data.Term);

    switch (T->token_type) {
        case INTEGER:
        case DOUBLE:
        case STRING:
            push_const_id(T);
            non_term = create_non_term(map_NT_type(T->token_type));
            break;
        case IDENTIFICATOR:
            push_var_id(found_var->symbol_name);
            non_term = create_non_term(found_var->value_type);
            break;
        default:
            fprintf(stderr, "Expected id or constant");
            error_exit(SEM_ERR);
    }

    // Stopper
    look_ahead = pop_T_NT(processing_stack);
    if ((look_ahead->is_non_term == false) || (look_ahead->data.NT_type != STOPPER)) {
        fprintf(stderr, "Unexpected item after id\n"); // Debug
        error_exit(SYNT_ERR);
    }


    return non_term;
}

T_NT_item *function_R(hash_tab_symbol_type *func) {

    enum {
        START_FUNC,
        ZERO_ARG,
        ONE_ARG,
        MORE_ARGS,
        CALL_FUNC,
        END_CHECK,
        FIN_FUNC
    } func_state;

    int arg_pos = 0;
    T_NT_item *look_ahead;
    T_NT_item *non_term;

    func_state = START_FUNC;

    while (func_state != FIN_FUNC) {
        switch(func_state) {
            case START_FUNC:
                // Get number of function arguments
                if ((func->param_types != NULL) && (func->param_types[0] != '\0')) {
                    while(func->param_types[arg_pos] != 0) {
                        arg_pos++;
                    }
                }
                create_frame();

                if (arg_pos == 0) {
                    func_state = ZERO_ARG;
                }
                else if (arg_pos == 1) {
                    func_state = ONE_ARG;
                }
                else {
                    func_state = MORE_ARGS;
                }
                break;
            case ZERO_ARG:
                // '('
                look_ahead = pop_T_NT(processing_stack);
                if ((look_ahead == NULL) || (look_ahead->is_non_term == true) || (look_ahead->data.Term.token_type != PAR_L)) {
                    fprintf(stderr, "Expected '(' after function name\n");
                    error_exit(TYPE_ERR);
                }
                func_state = CALL_FUNC;
                break;
            case ONE_ARG:
                arg_pos--;
                // E
                look_ahead = pop_T_NT(processing_stack);
                if (look_ahead->is_non_term == false) {
                    fprintf(stderr, "Function %s got no argument on position %d\n", func->symbol_name, arg_pos+1);
                    error_exit(TYPE_ERR);
                }
                switch (map_arg_type(func->param_types[arg_pos])) {
                    case INTEGER_NT:
                        if (look_ahead->data.NT_type == DOUBLE_NT) {
                            retype_to_even_int();
                        }
                        else if (look_ahead->data.NT_type == STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos+1);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    case DOUBLE_NT:
                        if (look_ahead->data.NT_type == INTEGER_NT) {
                            retype_stack(false, true);
                        }
                        else if (look_ahead->data.NT_type == STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos+1);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    case STRING_NT:
                        if (look_ahead->data.NT_type != STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos+1);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    default:
                        fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos+1);
                        error_exit(TYPE_ERR);
                }
                push_arg(arg_pos);
                func_state = ZERO_ARG;
                break;
            case MORE_ARGS:
                arg_pos--;
                // E
                look_ahead = pop_T_NT(processing_stack);
                if (look_ahead->is_non_term == false) {
                    fprintf(stderr, "Function %s got no argument on position %d\n", func->symbol_name, arg_pos+1);
                    error_exit(TYPE_ERR);
                }
                switch (map_arg_type(func->param_types[arg_pos])) {
                    case INTEGER_NT:
                        if (look_ahead->data.NT_type == DOUBLE_NT) {
                            retype_to_even_int();
                        }
                        else if (look_ahead->data.NT_type == STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos+1);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    case DOUBLE_NT:
                        if (look_ahead->data.NT_type == INTEGER_NT) {
                            retype_stack(false, true);
                        }
                        else if (look_ahead->data.NT_type == STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos+1);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    case STRING_NT:
                        if (look_ahead->data.NT_type != STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos+1);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    default:
                        fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos+1);
                        error_exit(TYPE_ERR);
                        break;
                }
                // ','
                look_ahead = pop_T_NT(processing_stack);
                if ((look_ahead->is_non_term == true) || (get_term_type(&(look_ahead->data.Term)) != CM)) {
                    fprintf(stderr, "Expected ',' before %d argument\n", arg_pos+1);
                    error_exit(TYPE_ERR);
                }
                push_arg(arg_pos);
                if (arg_pos > 1) {
                    func_state = MORE_ARGS;
                }
                else {
                    func_state = ONE_ARG;
                }
                break;
            case CALL_FUNC:
                f_call(func->symbol_name);
                func_state = END_CHECK;
                break;
            case END_CHECK:
                look_ahead = pop_T_NT(processing_stack);
                if ((look_ahead->is_non_term == false) && (get_term_type(&(look_ahead->data.Term)) == FNC)) {
                    look_ahead = pop_T_NT(processing_stack);
                    if ((look_ahead->is_non_term == false) || (look_ahead->data.NT_type != STOPPER)) {
                        fprintf(stderr, "Expected end of expression\n");
                        error_exit(SYNT_ERR);
                    }
                    func_state = FIN_FUNC;
                }
                else if (look_ahead->is_non_term == true) {
                    fprintf(stderr, "Unexpected argument after function call\n");
                    error_exit(TYPE_ERR);
                }
                else {
                    fprintf(stderr, "Unexpected item after function call\n");
                    error_exit(SYNT_ERR);
                }
                break;
            default:
                fprintf(stderr, "Bad function state\n");
                error_exit(INTERNAL_ERR);
        }
    }

    switch(func->value_type) {
        case 0:
            non_term = create_non_term(INTEGER_NT);
            break;
        case 1:
            non_term = create_non_term(DOUBLE_NT);
            break;
        case 2:
            non_term = create_non_term(STRING_NT);
            break;
        default:
            fprintf(stderr, "Unknown function return value type\n");
    }

    return non_term;
}

T_NT_item *parenthesis_R() {
    T_NT_item *look_ahead;
    T_NT_item *non_term;
    // E
    look_ahead = pop_T_NT(processing_stack);
    if (look_ahead->is_non_term == false) {
        fprintf(stderr, "Expected id\n");
        error_exit(SYNT_ERR);
    }

    non_term = create_non_term(look_ahead->data.NT_type);

    // (
    look_ahead = pop_T_NT(processing_stack);
    if ((look_ahead->is_non_term == true) || (look_ahead->data.Term.token_type != PAR_L)) {
        fprintf(stderr, "Unexpected id, expected '('\n");
        error_exit(SYNT_ERR);
    }

    // STOPPER
    look_ahead = pop_T_NT(processing_stack);
    if ((look_ahead->is_non_term == false) || (look_ahead->data.NT_type != STOPPER)) {
        fprintf(stderr, "Expected end of expression\n");
        error_exit(SYNT_ERR);
    }
    return non_term;
}

T_NT_item *arithm_R() {

    static enum {
        START_ARITHM_PSA,

        INT_AR_PSA,
        DOUBLE_AR_PSA,
        STRING_AR_PSA,
        AR_END_PSA,

        FINISHED_ARITHM_PSA
    } arithm_state;

    static char oper_types[12][3] = {
        "+",
        "*",
        "-",
        "/",
        "\\",
        "<",
        ">",
        "<=",
        ">=",
        "=",
        "<>",
        "??"
    };

    T_NT_item *look_ahead;
    T_NT_item *non_term;
    T_NT_Data *E1;
    T_NT_Data *E2;
    PSA_Term_type arithm_operand;

    look_ahead = processing_stack->popped;
    E2 = &(look_ahead->data);

    look_ahead = pop_T_NT(processing_stack);
    if (look_ahead->is_non_term == true) {
        fprintf(stderr, "Expected operator\n");
        error_exit(SYNT_ERR);
    }
    arithm_operand = get_term_type(&(look_ahead->data.Term));

    look_ahead = pop_T_NT(processing_stack);
    if (look_ahead->is_non_term == false || look_ahead->data.NT_type == STOPPER) {
        fprintf(stderr, "Expected operand.\n");
        error_exit(SYNT_ERR);
    }
    E1 = &(look_ahead->data);

    arithm_state = START_ARITHM_PSA;

    while (arithm_state != FINISHED_ARITHM_PSA) {
        switch(arithm_state) {
            case START_ARITHM_PSA:
                if (E2->NT_type == DOUBLE_NT) {
                    arithm_state = DOUBLE_AR_PSA;
                }
                else if (E2->NT_type == INTEGER_NT) {
                    arithm_state = INT_AR_PSA;
                }
                else if (E2->NT_type == STRING_NT) {
                    arithm_state = STRING_AR_PSA;
                }
                else {
                    fprintf(stderr, "\"Void\" type is unsupported\n");
                    error_exit(TYPE_ERR);
                }
                break;
            case DOUBLE_AR_PSA:
                switch (arithm_operand) {
                    case ADD:
                    case SUB:
                    case MUL:
                    case DIV:
                    case LT:
                    case GT:
                    case LTE:
                    case GTE:
                    case EQ:
                    case NEQ:
                        if (E1->NT_type == INTEGER_NT) {
                            retype_stack(true, true);
                        }
                        else if (E1->NT_type == DOUBLE_NT) {
                            ;
                        }
                        // Unexpected string type
                        else {
                            fprintf(stderr, "1) Unexpected arithmetic operand '%s'\n", oper_types[arithm_operand]);
                            error_exit(TYPE_ERR);
                        }
                        non_term = create_non_term(DOUBLE_NT);
                        arithm_stack(arithm_operand);
                        arithm_state = AR_END_PSA;
                        break;
                    case IDIV:
                        fprintf(stderr, "Integer division works only with integers\n");
                        error_exit(TYPE_ERR);
                        break;
                    default:
                        fprintf(stderr, "Unexpected operator\n");
                        error_exit(SYNT_ERR);
                        break;
                }
                break;
            case INT_AR_PSA:
                switch(arithm_operand) {
                    case ADD:
                    case SUB:
                    case MUL:
                    case LT:
                    case GT:
                    case LTE:
                    case GTE:
                    case EQ:
                    case NEQ:
                        if (E1->NT_type == INTEGER_NT) {
                            non_term = create_non_term(INTEGER_NT);
                        }
                        else if (E1->NT_type == DOUBLE_NT) {
                            retype_stack(false, true);
                            non_term = create_non_term(DOUBLE_NT);
                        }
                        // Unexpected string type
                        else {
                            fprintf(stderr, "2) Unexpected arithmetic operand '%s'\n", oper_types[arithm_operand]);
                            error_exit(TYPE_ERR);
                        }

                        arithm_stack(arithm_operand);
                        arithm_state = AR_END_PSA;
                        break;
                    case DIV:
                        if (E1->NT_type == INTEGER_NT) {
                            retype_stack(true, true);
                            retype_stack(false, true);
                        }
                        else if (E1->NT_type == DOUBLE_NT) {
                            retype_stack(false, true);
                        }
                        // Unexpected string type
                        else {
                            fprintf(stderr, "3) Unexpected arithmetic operand '%s'\n", oper_types[arithm_operand]);
                            error_exit(TYPE_ERR);
                        }
                        non_term = create_non_term(DOUBLE_NT);

                        arithm_stack(DIV);
                        arithm_state = AR_END_PSA;
                        break;
                    case IDIV:
                        if (E1->NT_type != INTEGER_NT) {
                            fprintf(stderr, "Integer division works only with integers\n");
                            error_exit(TYPE_ERR);
                        }
                        else {
                            retype_stack(true, true);
                            retype_stack(false, true);
                        }
                        non_term = create_non_term(INTEGER_NT);
                        arithm_stack(IDIV);
                        arithm_state = AR_END_PSA;
                        break;
                    default:
                        fprintf(stderr, "Unexpected operator\n");
                        error_exit(SYNT_ERR);
                        break;
                }
                break;
            case STRING_AR_PSA:
                switch(arithm_operand) {
                    case ADD:
                    case NEQ:
                    case LT:
                    case GT:
                    case LTE:
                    case GTE:
                    case EQ:
                        if (E1->NT_type != STRING_NT) {
                            fprintf(stderr, "Expected string\n");
                            error_exit(TYPE_ERR);
                        }
                        non_term = create_non_term(STRING_NT);
                        str_arithm(arithm_operand);
                        arithm_state = AR_END_PSA;
                        break;
                    case SUB:
                    case MUL:
                    case DIV:
                    case IDIV:
                        fprintf(stderr, "Unexpected string operator '%s'\n", oper_types[arithm_operand]);
                        error_exit(TYPE_ERR);
                        break;
                    default:
                        fprintf(stderr, "Unexpected operator\n");
                        error_exit(SYNT_ERR);
                        break;

                }
                break;
            case AR_END_PSA:
                look_ahead = pop_T_NT(processing_stack);
                if ((look_ahead->is_non_term == true) && (look_ahead->data.NT_type == STOPPER)) {
                    arithm_state = FINISHED_ARITHM_PSA;
                }
                else {
                    fprintf(stderr, "Unexpected item after arithmetics\n"); // Debug
                    error_exit(SYNT_ERR);
                }
                break;
            default:
                fprintf(stderr, "UNKNOWN STATE ARITHM"); // Debug
                error_exit(INTERNAL_ERR);
                break;
        }

    }
    return non_term;
}

void reduce_by_rule() {

    T_NT_item *look_ahead;
    Data_Term *func_or_left_p;
    T_NT_item *non_term;
    Data_Term *T;

    look_ahead = pop_T_NT(processing_stack);

    if (look_ahead->is_non_term == true) {
        non_term = arithm_R();
    }
    else {
        T = &(look_ahead->data.Term);
        if (get_term_type(T) == PR) {
            func_or_left_p = get_item_before_stopper(); // TODO: rename get item before stopper
            if (get_term_type(func_or_left_p) == FNC) {
                non_term = function_R(hash_table_search(func_table, func_or_left_p->id));
            }
            else if (get_term_type(func_or_left_p) == PL) {
                non_term = parenthesis_R();
            }
            else {
                fprintf(stderr, "Unknown token before stopper\n");
                error_exit(INTERNAL_ERR);
            }
        }
        else if (get_term_type(T) == ID) {
            if (T->token_type == IDENTIFICATOR) {
                non_term = id_R(hash_table_search(var_table, T->id));
            }
            else {
                non_term = id_R(NULL);
            }
        }
        else {
            fprintf(stderr, "Unexpected token\n");
            error_exit(SYNT_ERR); // Debug
        }
    }

    push_NT(processing_stack, non_term);
    free(non_term);

    return;
}

void psa_operation(bool allow_bool) {
    static char table_psa[17][17] = {
        //        | ADD | MUL| SUB| DIV|IDIV| LT | GT | LTE| GTE| EQ | NEQ| PL | PR | ID | FNC| CM | END|
        //        |   + |  * |  - |  / |  \ |  < |  > | <= | >= |  = | <> |  ( |  ) |  i |  f |  , |   $|
        //-----------------------------------------------------------------------------------------------
    {   /* ADD| + |*/'>', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>' },
    {   /* MUL| * |*/'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>' },
    {   /* SUB| - |*/'>', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>' },
    {   /* DIV| / |*/'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>' },
    {   /* IDI| \ |*/'>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>' },
    {   /* LT | < |*/'<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '#', '>' },
    {   /* GT | > |*/'<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '#', '>' },
    {   /* LTE|<= |*/'<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '#', '>' },
    {   /* GTE|>= |*/'<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '#', '>' },
    {   /* EQ | = |*/'<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '#', '>' },
    {   /* NEQ|<> |*/'<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '#', '>' },
    {   /* PL | ( |*/'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '=', '#' },
    {   /* PR | ) |*/'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>' },
    {   /* ID | i |*/'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>' },
    {   /* FNC| f |*/'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '=', '#', '#', '#', '#', '#' },
    {   /* CM | , |*/'<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '=', '<', '<', '=', '#' },
    {   /* END| $ |*/'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '\0'},
    };

    PSA_Term_type index_stack_top;
    PSA_Term_type index_input;
    Data_Term first_term_data;
    char what_to_do;
    bool use_push;
    bool psa_finished;

    psa_finished = false;

    while (psa_finished != true) {
        first_term_data = find_first_term(processing_stack, &use_push)->data.Term;
        index_stack_top = get_term_type(&first_term_data);
        index_input = get_term_type(&currentToken);

        if (currentToken.token_type == IDENTIFICATOR) {
            if (hash_table_search(var_table, currentToken.id) == NULL) {
                if (hash_table_search(func_table, currentToken.id) == NULL) {
                    fprintf(stderr, "Variable '%s' was not declared\n", currentToken.id);
                    error_exit(UNDEF_ERR);
                }
                index_input = FNC;
                currentToken.token_type = FUNCTION;
            }
        }

        if (allow_bool == false) {
            switch(index_input) {
                case LT:
                case GT:
                case LTE:
                case GTE:
                case EQ:
                case NEQ:
                    what_to_do = '#'; // Bool not allowed, error
                    break;
                default:
                    what_to_do = table_psa[index_stack_top][index_input];
                    break;
            }
        } else {
            what_to_do = table_psa[index_stack_top][index_input];
        }

        // Syntax control part:
        // ----------------------------------------------
        switch(what_to_do) {
            // Insert stopper before first terminal and push currentToken to stack
            case '<':
                if (allow_bool == true) {
                    switch(index_input) {
                            case LT:
                            case GT:
                            case LTE:
                            case GTE:
                            case EQ:
                            case NEQ:
                                allow_bool = false;
                                break;
                            default:
                                break;
                    }
                }
                insert_stopper(processing_stack);
                push_T(processing_stack, &currentToken);
                get_token();
                break;
            // Reduce by rule
            //
            // -------------------Semantic control part-----------------------
            case '>':
                reduce_by_rule();
                break;
            // --------------------End semantic control-----------------------
            // Syntax error
            case '#':
                if ((index_stack_top == ID) && (index_input == PL)) {
                    fprintf(stderr, "Variable '%s' used as function\n", first_term_data.id);
                    error_exit(UNDEF_ERR);
                }
                else if ((index_stack_top == FNC) && (index_input != PL)) {
                    fprintf(stderr, "Function '%s' used as variable\n", first_term_data.id);
                    error_exit(UNDEF_ERR);
                }
                else {
                    fprintf(stderr, "Psa syntax error.\n");
                    error_exit(SYNT_ERR);
                }
                break;
            // Only push
            case '=':
                push_T(processing_stack, &currentToken);
                get_token();
                break;
            case '\0':
                if (allow_bool == false) {
                    psa_finished = true;
                }
                else {
                    fprintf(stderr, "Expected boolean expression\n");
                    error_exit(TYPE_ERR);
                }
                break;
            default:
                error_exit(INTERNAL_ERR);
        }

    }

    return;
}
