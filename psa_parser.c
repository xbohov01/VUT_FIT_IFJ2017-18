/*
** Precedense syntax parser implementation
** Author: Danil Grigorev, xgrigo02
*/

#include "ifj2017.h"

// Feeds non terminal
Data_NTerm *create_non_term(N_T_rules input_rule, N_T_types input_type) {

    Data_NTerm *temp_non_term_data = malloc(sizeof(Data_NTerm));
    if (temp_non_term_data == NULL) {
        error_exit(INTERNAL_ERR);
    }

    temp_non_term_data->rule = input_rule;
    temp_non_term_data->type = input_type;

    return temp_non_term_data;
}

void control_result_type_conform(int value_type) {
    switch(value_type) {
        case 0:
            switch(processing_stack->top->data.NTerm.type) {
                case INTEGER_NT:
                    // Is ok
                    break;
                case DOUBLE_NT:
                    retype_to_even_int();
                    break;
                default:
                    fprintf(stderr, "Unexpected operand type, for int or float result value\n");
                    error_exit(TYPE_ERR);
                    break;
            }
            break;
        case 1:
            switch(processing_stack->top->data.NTerm.type) {
                case INTEGER_NT:
                    retype_stack(false, true);
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
            switch(processing_stack->top->data.NTerm.type) {
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
    Data_Term *temp_data = malloc(sizeof(Data_Term));
    if (temp_data == NULL) {
        error_exit(INTERNAL_ERR);
    }

    temp_data->token_type = ENDL;

    push_T_NT(s, temp_data, NULL);
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
void eval_cond_expr(bool is_do_while, int label_num) {

    extern T_NT_stack *processing_stack;

    push_start_term(processing_stack);
    psa_operation(true);

    cond_jump(is_do_while, label_num);

    check_psa_completion();
    if (get_term_type(&(currentToken)) != END) {
        fprintf(stderr, "Unexpected token instead of end\n");
        error_exit(INTERNAL_ERR);
    }
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
                else if (currentToken.token_type == RETURN_KEY) {
                // <CHECK_START_SYM> -> RETURN KEY
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
                if ((processing_stack->top->data.NTerm.type == INTEGER_NT) \
                    || (processing_stack->top->data.NTerm.type == DOUBLE_NT)) {
                    // write_space(); // TODO: maybe not needed
                }
                write_output();
                check_psa_completion();
                eval_ex_state = END_CHECK;
                break;
            case PRINT_CONT:
                get_token();
                // Can be ended at once
                if (currentToken.token_type == ENDL) {
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
                if ((processing_stack->top->data.NTerm.type == INTEGER_NT) \
                    || (processing_stack->top->data.NTerm.type == DOUBLE_NT)) {
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
        // Notification
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
    Data_NTerm *stopper = create_non_term(STOPPER, NONE_NT);

    find_first_term(s, &use_push);
    if (use_push)
        push_T_NT(s, NULL, stopper);
    else
        insert_after_T_NT(s, NULL, stopper);

    free(stopper);
    return;
}

void get_reversed_rule() {
    T_NT_item *temp;
    bool fin_reverse = false;

    while (fin_reverse == false) {
        temp = pop_T_NT(processing_stack);
        if (temp->is_non_term == true) {
            if (temp->data.NTerm.rule != STOPPER) {
                push_T_NT(evaluation_stack, NULL, &(temp->data.NTerm));
            }
            else {
                fin_reverse = true;
            }
        }
        else {
            push_T_NT(evaluation_stack, &(temp->data.Term), NULL);
        }
    }
    return;
}

Data_NTerm *id_R(hash_tab_symbol_type *found_var) {

    T_NT_item *look_ahead;
    Data_NTerm *used_rule;
    Data_Term *T;

    look_ahead = evaluation_stack->popped;
    T = &(look_ahead->data.Term);

    switch (T->token_type) {
        case INTEGER:
        case DOUBLE:
        case STRING:
            push_const_id(T);
            used_rule = create_non_term(NT_ID, map_NT_type(T->token_type));
            break;
        case IDENTIFICATOR:
            push_var_id(found_var->symbol_name);
            used_rule = create_non_term(NT_ID, found_var->value_type);
            break;
        default:
            fprintf(stderr, "Expected id or constant");
            error_exit(SEM_ERR);
    }

    look_ahead = pop_T_NT(evaluation_stack);
    T = &(look_ahead->data.Term);

    if ((look_ahead->is_non_term == true) || (get_term_type(T) != END)) {
        fprintf(stderr, "Unexpected item after id\n"); // Debug
        error_exit(SYNT_ERR);
    }

    return used_rule;
}

Data_NTerm *function_R(hash_tab_symbol_type *func) {

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
    int arg = 0;
    T_NT_item *look_ahead;
    Data_NTerm *used_rule;

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
                // '('
                look_ahead = pop_T_NT(evaluation_stack);
                if ((look_ahead == NULL) || (look_ahead->is_non_term == true) || (look_ahead->data.Term.token_type != PAR_L)) {
                    fprintf(stderr, "Expected '(' after function call\n");
                    error_exit(TYPE_ERR);
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
                // ')'
                look_ahead = pop_T_NT(evaluation_stack);
                if (look_ahead->is_non_term != false) {
                    fprintf(stderr, "Unexpected argument while calling %s\n", func->symbol_name);
                    error_exit(TYPE_ERR);
                }
                func_state = CALL_FUNC;
                break;
            case ONE_ARG:
                arg_pos--;
                // E
                look_ahead = pop_T_NT(evaluation_stack);
                if (look_ahead->is_non_term == false) {
                    fprintf(stderr, "Function %s got no argument on position %d\n", func->symbol_name, arg_pos);
                    error_exit(TYPE_ERR);
                }
                switch (map_arg_type(func->param_types[arg])) {
                    case INTEGER_NT:
                        if (look_ahead->data.NTerm.type == DOUBLE_NT) {
                            retype_to_even_int();
                        }
                        else if (look_ahead->data.NTerm.type == STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    case DOUBLE_NT:
                        if (look_ahead->data.NTerm.type == INTEGER_NT) {
                            retype_stack(false, true);
                        }
                        else if (look_ahead->data.NTerm.type == STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    case STRING_NT:
                        if (look_ahead->data.NTerm.type != STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    default:
                        fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos);
                        error_exit(TYPE_ERR);
                }
                arg++;
                push_arg(arg_pos);
                func_state = ZERO_ARG;
                break;
            case MORE_ARGS:
                arg_pos--;
                // E
                look_ahead = pop_T_NT(evaluation_stack);
                if (look_ahead->is_non_term == false) {
                    fprintf(stderr, "Function %s got no argument on position %d\n", func->symbol_name, arg_pos);
                    error_exit(TYPE_ERR);
                }
                switch (map_arg_type(func->param_types[arg])) {
                    case INTEGER_NT:
                        if (look_ahead->data.NTerm.type == DOUBLE_NT) {
                            retype_to_even_int();
                        }
                        else if (look_ahead->data.NTerm.type == STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    case DOUBLE_NT:
                        if (look_ahead->data.NTerm.type == INTEGER_NT) {
                            retype_stack(false, true);
                        }
                        else if (look_ahead->data.NTerm.type == STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    case STRING_NT:
                        if (look_ahead->data.NTerm.type != STRING_NT) {
                            fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos);
                            error_exit(TYPE_ERR);
                        }
                        break;
                    default:
                        fprintf(stderr, "Function %s, bad argument type on position %d\n", func->symbol_name, arg_pos);
                        error_exit(TYPE_ERR);
                        break;
                }
                // ','
                look_ahead = pop_T_NT(evaluation_stack);
                if ((look_ahead->is_non_term == true) || (get_term_type(&(look_ahead->data.Term)) != CM)) {
                    fprintf(stderr, "Expected ',' and %d argument \n", arg);
                    error_exit(TYPE_ERR);
                }
                arg++;
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
                look_ahead = pop_T_NT(evaluation_stack);
                if ((look_ahead->is_non_term == false) && (get_term_type(&(look_ahead->data.Term)) == END)) {
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
            used_rule = create_non_term(NT_FN, INTEGER_NT);
            break;
        case 1:
            used_rule = create_non_term(NT_FN, DOUBLE_NT);
            break;
        case 2:
            used_rule = create_non_term(NT_FN, STRING_NT);
            break;
        default:
            fprintf(stderr, "Unknown function return value type\n");
    }

    return used_rule;
}

Data_NTerm *arithm_R() {

    static enum {
        START_ARITHM_PSA,

        INT_AR_PSA,
        DOUBLE_AR_PSA,
        STRING_AR_PSA,
        AR_END_PSA,

        FINISHED_ARITHM_PSA
    } arithm_state;

    static char op_types[4][8] = {
        "integer",
        "float",
        "string",
        "none"
    };

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

    extern T_NT_stack *evaluation_stack;
    T_NT_item *look_ahead;
    Data_NTerm *used_rule;
    Data_NTerm *E1;
    Data_NTerm *E2;
    PSA_Term_type arithm_operand;

    look_ahead = evaluation_stack->popped;
    E2 = &(look_ahead->data.NTerm);

    look_ahead = pop_T_NT(evaluation_stack);
    if (look_ahead->is_non_term == true) {
        fprintf(stderr, "Expected operator\n");
        error_exit(SYNT_ERR);
    }
    arithm_operand = get_term_type(&(look_ahead->data.Term));

    look_ahead = pop_T_NT(evaluation_stack);
    if ((look_ahead->is_non_term == false) && (look_ahead->data.Term.token_type == ENDL)) {
        fprintf(stderr, "Expected operand, got nothing\n");
        error_exit(SYNT_ERR);
    }
    E1 = &(look_ahead->data.NTerm);

    arithm_state = START_ARITHM_PSA;

    while (arithm_state != FINISHED_ARITHM_PSA) {
        switch(arithm_state) {
            case START_ARITHM_PSA:
                if (E2->type == DOUBLE_NT) {
                    arithm_state = DOUBLE_AR_PSA;
                }
                else if (E2->type == INTEGER_NT) {
                    arithm_state = INT_AR_PSA;
                }
                else if (E2->type == STRING_NT) {
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
                        if (E1->type == INTEGER_NT) {
                            retype_stack(false, true);
                        }
                        else if (E1->type == DOUBLE_NT) {
                            ;
                        }
                        // Unexpected string type
                        else {
                            fprintf(stderr, "1) Unexpected arithmetic operand \"%s\" '%s' \"%s\"\n", op_types[E2->type], oper_types[arithm_operand], op_types[E1->type]);
                            error_exit(TYPE_ERR);
                        }
                        used_rule = create_non_term(map_NT_rule(arithm_operand), DOUBLE_NT);
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
                        if (E1->type == INTEGER_NT) {
                            used_rule = create_non_term(map_NT_rule(arithm_operand), INTEGER_NT);
                        }
                        else if (E1->type == DOUBLE_NT) {
                            retype_stack(true, true);
                            used_rule = create_non_term(map_NT_rule(arithm_operand), DOUBLE_NT);
                        }
                        // Unexpected string type
                        else {
                            fprintf(stderr, "2) Unexpected arithmetic operand \"%s\" '%s' \"%s\"\n", op_types[E2->type], oper_types[arithm_operand], op_types[E1->type]);
                            error_exit(TYPE_ERR);
                        }

                        arithm_stack(arithm_operand);
                        arithm_state = AR_END_PSA;
                        break;
                    case DIV:
                        if (E1->type == INTEGER_NT) {
                            retype_stack(true, true);
                            retype_stack(false, true);
                        }
                        else if (E1->type == DOUBLE_NT) {
                            retype_stack(true, true);
                        }
                        // Unexpected string type
                        else {
                            fprintf(stderr, "3) Unexpected arithmetic operand \"%s\" '%s' \"%s\"\n", op_types[E2->type], oper_types[arithm_operand], op_types[E1->type]);
                            error_exit(TYPE_ERR);
                        }
                        used_rule = create_non_term(NT_DIV, DOUBLE_NT);

                        arithm_stack(DIV);
                        arithm_state = AR_END_PSA;
                        break;
                    case IDIV:
                        if (E1->type != INTEGER_NT) {
                            fprintf(stderr, "Integer division works only with integers, got \"%s\" '%s' \"%s\"\n", op_types[E2->type], oper_types[arithm_operand], op_types[E1->type]);
                            error_exit(TYPE_ERR);
                        }
                        else {
                            retype_stack(true, true);
                            retype_stack(false, true);
                        }
                        used_rule = create_non_term(NT_IDIV, INTEGER_NT);
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
                        if (E1->type != STRING_NT) {
                            fprintf(stderr, "Expected string, got \"%s\" '%s' \"%s\"\n", op_types[E2->type], oper_types[arithm_operand], op_types[E1->type]);
                            error_exit(TYPE_ERR);
                        }
                        used_rule = create_non_term(map_NT_rule(arithm_operand), STRING_NT);
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
                look_ahead = pop_T_NT(evaluation_stack);
                if ((look_ahead->is_non_term == false) && (get_term_type(&look_ahead->data.Term) == END)) {
                    arithm_state = FINISHED_ARITHM_PSA;
                }
                else {
                    fprintf(stderr, "Unexpected item after arithmetics\n"); // Debug
                    error_exit(SYNT_ERR);
                }
                break;
            default: // Debug
                fprintf(stderr, "UNKNOWN STATE ARITHM"); // Debug
                error_exit(INTERNAL_ERR);
                break;
        }

    }
    return used_rule;
}

void reduce_by_rule() {

    T_NT_item *look_ahead;
    Data_NTerm *used_rule;
    Data_Term *T;

    push_start_term(evaluation_stack);
    get_reversed_rule();
    look_ahead = pop_T_NT(evaluation_stack);

    if (look_ahead->is_non_term == true) {
        used_rule = arithm_R();
    }
    else {
        T = &(look_ahead->data.Term);
        if (get_term_type(T) == PL) {
            // E
            look_ahead = pop_T_NT(evaluation_stack);
            used_rule = create_non_term(look_ahead->data.NTerm.rule, look_ahead->data.NTerm.type);

            // )
            look_ahead = pop_T_NT(evaluation_stack);

            // $
            look_ahead = pop_T_NT(evaluation_stack);

        }
        else if (get_term_type(T) == ID) {
            if (T->token_type == IDENTIFICATOR) {
                used_rule = id_R(hash_table_search(var_table, T->id));
            }
            else {
                used_rule = id_R(NULL);
            }
        }
        else if (T->token_type == FUNCTION) {
            used_rule = function_R(hash_table_search(func_table, T->id));
        }
        else {
            fprintf(stderr, "Unexpected token\n");
            error_exit(SYNT_ERR); // Debug
        }
    }

    push_T_NT(processing_stack, NULL, used_rule);
    free(used_rule);

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
                push_T_NT(processing_stack, &currentToken, NULL);
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
                if ((index_stack_top == ID) && (index_input == PAR_L)) {
                    fprintf(stderr, "Variable used as function\n");
                    error_exit(UNDEF_ERR);
                }
                else if ((index_stack_top == FNC) && (index_input != PAR_L)) {
                    fprintf(stderr, "Function used as variable\n"); 
                    error_exit(UNDEF_ERR);
                }
                else {
                    fprintf(stderr, "Psa syntax error.\n");
                    error_exit(SYNT_ERR);
                }
                break;
            // Only push
            case '=':
                push_T_NT(processing_stack, &currentToken, NULL);
                get_token();
                break;
            case '\0':
                if (allow_bool == false) {
                    psa_finished = true;
                }
                else {
                    fprintf(stderr, "End error.\n");
                    error_exit(SYNT_ERR);
                }
                break;
            default:
                error_exit(INTERNAL_ERR);
        }

    }

    return;
}
