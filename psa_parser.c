/* 
** Precedense syntax parser implementation
** Author: Danil Grigorev, xgrigo02
*/

#include "ifj2017.h"


// int main() {
//     // Tests
//     // +++++++++++++++
//     // hash_table_type *tmp = NULL;
//     // tmp = sym_tab_init(20);
//     // hash_table_insert(tmp, "new_id");
//     // printf("symbol name: %s\n", hash_table_search(tmp, "new_id")->symbol_name);
//     // hash_table_destroy(tmp);


//     // Started
//     start_scanner("hard_test_with_seg_fault.txt"); // TODO: delete -- test
//     // Parser did its work
//     get_token();

//     eval_expr();
//     // Parser continues
//     // End
//     free_sources();
//     return 0;
// }

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
        if ((processing_stack->popped->is_non_term == true) || (processing_stack->popped->data.Term.token_type != ENDL)) {
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
    extern T_NT_stack *evaluation_stack;
    processing_stack = init_T_NT_stack();
    evaluation_stack = init_T_NT_stack();

    init_TAC_stack();
    push_start_term(processing_stack);
    psa_operation(true);
    check_psa_completion();
    
    cond_jump(is_do_while, label_num);

    destroy_T_NT_stack(processing_stack);
    destroy_T_NT_stack(evaluation_stack);
    return;
}

// <expr> -> <CHECK_START_SYM> <VAR_INIT>
// <expr> -> <CHECK_START_SYM> <VAR_DEFINITION>
// <expr> -> <CHECK_START_SYM> <FUNC_CALL>
void eval_expr() {

    enum {
        CHECK_START_SYM,
        VAR_INIT,
        VAR_DEFINITION,
        CALC_PSA,
        FUNC_CALL,
        END_CHECK,
        FINISHED_EXPR
    } eval_ex_state;

    extern T_NT_stack *processing_stack;
    extern T_NT_stack *evaluation_stack;
    hash_tab_symbol_type *result_variable;
    hash_tab_symbol_type *function_variable;
    processing_stack = init_T_NT_stack();
    evaluation_stack = init_T_NT_stack();
    init_TAC_stack();

    eval_ex_state = CHECK_START_SYM;

    while (eval_ex_state != FINISHED_EXPR) {
        switch (eval_ex_state) {
            case CHECK_START_SYM:
                if (currentToken.token_type == IDENTIFICATOR) {
                    result_variable = hash_table_search(var_table, currentToken.id);
                    function_variable = hash_table_search(func_table, currentToken.id); 
                    if ((result_variable == NULL) && (function_variable == NULL)) {
                        error_exit(UNDEF_ERR);
                    }
                    // <CHECK_START_SYM> -> 'func'
                    else if (function_variable != NULL) {
                        eval_ex_state = FUNC_CALL;
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
                else {
                    fprintf(stderr, "Bad variable on input of eval_expr\n");
                    error_exit(INTERNAL_ERR);
                }
                break;
            case VAR_INIT:
                // set expr_start
                get_token();
                push_start_term(processing_stack);
                psa_operation(false);
                temporary_save();
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
                temporary_save();
                save_result(result_variable->symbol_name);
                check_psa_completion();
                eval_ex_state = END_CHECK;
                break;
            case FUNC_CALL:
                psa_operation(false);
                check_psa_completion();
                eval_ex_state = END_CHECK;
                break;
            case END_CHECK:
                if (currentToken.token_type != ENDL) {
                    fprintf(stderr, "Unexpected token instead of end\n");
                }
                eval_ex_state = FINISHED_EXPR;
                break;
            default:
                fprintf(stderr, "State machine error.\n");
                break;   
        }
    }

    destroy_T_NT_stack(processing_stack);
    destroy_T_NT_stack(evaluation_stack);
    return;
}

T_NT_item *find_first_term(T_NT_stack *s, bool *is_first) {
    set_first_T_NT(s);
    if (!active_T_NT(s)) {
        printf("List has no items\n");
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
        printf("List contains zero terminals\n");
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
            printf("UNEXPECTED TOKEN AFTER PSA");
            error_exit(INTERNAL_ERR);
    }

    look_ahead = pop_T_NT(evaluation_stack);
    T = &(look_ahead->data.Term);

    if (get_term_type(T) != END) {
        printf("UNEXPECTED ITEM AFTER PSA\n"); // Debug
        error_exit(INTERNAL_ERR);
    }

    return used_rule;
}

Data_NTerm *function_R(hash_tab_symbol_type *func) {
    // TODO   
    return create_non_term(NT_FN, INTEGER_NT);
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

    extern T_NT_stack *evaluation_stack;
    T_NT_item *look_ahead;
    Data_NTerm *used_rule;
    Data_NTerm *E;
    PSA_Term_type arithm_operand;

    look_ahead = evaluation_stack->popped;
    arithm_state = START_ARITHM_PSA;


    while (arithm_state != FINISHED_ARITHM_PSA) {
        switch(arithm_state) {
            case START_ARITHM_PSA:
                E = &(look_ahead->data.NTerm);
                if (E->type == DOUBLE_NT) {
                    arithm_state = DOUBLE_AR_PSA;
                }
                else if (E->type == INTEGER_NT) {
                    arithm_state = INT_AR_PSA;
                }
                else {
                    arithm_state = STRING_AR_PSA;
                }
                break;
            case DOUBLE_AR_PSA:
                look_ahead = pop_T_NT(evaluation_stack);
                arithm_operand = get_term_type(&look_ahead->data.Term);
                look_ahead = pop_T_NT(evaluation_stack);
                E = &(look_ahead->data.NTerm);
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
                        if (E->type == INTEGER_NT) {
                            retype_stack(false, true);
                        }
                        else if (E->type == DOUBLE_NT) {
                            ;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(map_NT_rule(arithm_operand), DOUBLE_NT);
                        arithm_stack(arithm_operand);
                        arithm_state = AR_END_PSA;
                        break;
                    case IDIV:
                        error_exit(TYPE_ERR);
                        break;
                    default:
                        fprintf(stderr, "UNEXPECTED SIGN AFTER PSA\n");
                        error_exit(INTERNAL_ERR);
                        break;
                }
                break;
            case INT_AR_PSA:
                look_ahead = pop_T_NT(evaluation_stack);
                arithm_operand = get_term_type(&look_ahead->data.Term);
                look_ahead = pop_T_NT(evaluation_stack);
                E = &(look_ahead->data.NTerm);
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
                        if (E->type == INTEGER_NT) {
                            used_rule = create_non_term(NT_MUL, INTEGER_NT);
                        }
                        else if (E->type == DOUBLE_NT) {
                            retype_stack(true, true);
                            used_rule = create_non_term(NT_MUL, DOUBLE_NT);
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        
                        arithm_stack(arithm_operand);
                        arithm_state = AR_END_PSA;
                        break;
                    case DIV:
                        if (E->type == INTEGER_NT) {
                            retype_stack(true, true);
                            retype_stack(false, true);
                        }
                        else if (E->type == DOUBLE_NT) {
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_DIV, DOUBLE_NT);
                        
                        // printf("4")
                        arithm_stack(DIV);
                        arithm_state = AR_END_PSA;
                        break;
                    case IDIV:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type != INTEGER_NT) {
                            error_exit(TYPE_ERR);
                        }

                        used_rule = create_non_term(NT_IDIV, INTEGER_NT);
                        // printf("5")
                        arithm_stack(IDIV);
                        arithm_state = AR_END_PSA;
                        break;
                    default:
                        fprintf(stderr, "UNEXPECTED SIGN AFTER PSA\n");
                        error_exit(INTERNAL_ERR);
                        break;
                }
                break;
            case STRING_AR_PSA:
                look_ahead = pop_T_NT(evaluation_stack);
                arithm_operand = get_term_type(&look_ahead->data.Term);
                look_ahead = pop_T_NT(evaluation_stack);
                E = &(look_ahead->data.NTerm);
                switch(arithm_operand) {
                    case ADD:
                    case NEQ:
                        if (E->type != STRING_NT) {
                            error_exit(TYPE_ERR);
                        }
                        used_rule = create_non_term(map_NT_rule(arithm_operand), STRING_NT);
                        // printf("1")
                        arithm_stack(arithm_operand);
                        arithm_state = AR_END_PSA;
                        break;
                    case LT:
                    case GT:
                    case LTE:
                    case GTE:
                    case EQ:
                    case SUB:
                    case MUL:
                    case DIV:
                    case IDIV:
                        fprintf(stderr, "UNEXPECTED OPERATION. MAYBE YOU MENT '+'?\n");
                        error_exit(SYNT_ERR);
                        break;
                    default:
                        fprintf(stderr, "UNEXPECTED SIGN AFTER PSA\n");
                        error_exit(INTERNAL_ERR);
                        break;

                }
                
                
                break;
            case AR_END_PSA:
                look_ahead = pop_T_NT(evaluation_stack);
                if ((look_ahead->is_non_term == false) && (get_term_type(&look_ahead->data.Term) == END)) {
                    arithm_state = FINISHED_ARITHM_PSA;
                }
                else {
                    printf("UNEXPECTED ITEM AFTER PSA\n"); // Debug
                    error_exit(INTERNAL_ERR);
                }
                break;
            default: // Debug
                printf("UNKNOWN STATE ARITHM"); // Debug
                error_exit(INTERNAL_ERR);
                break;
        }
        
    }

    return used_rule;
}

void reduce_by_rule() {

        // FUNC_PSA,
        // FUNC_EXTEND_PSA,
        // FUNC_END_PSA,

    T_NT_item *look_ahead;
    Data_NTerm *used_rule;
    Data_Term *T;
    hash_tab_symbol_type *found_var;
    hash_tab_symbol_type *found_func;

    extern T_NT_stack *evaluation_stack;
    
    push_start_term(evaluation_stack);
    get_reversed_rule();
    look_ahead = pop_T_NT(evaluation_stack);
    T = &(look_ahead->data.Term);

    if (look_ahead->is_non_term == true) {
        used_rule = arithm_R();
    }
    else {
        if (get_term_type(T) == ID) {
            if (T->token_type == IDENTIFICATOR) {
                found_var = hash_table_search(var_table, T->id);
                found_func = hash_table_search(func_table, T->id);
                if ((found_var == NULL) && (found_func == NULL)) {
                    printf("Variable was not declared\n");
                    error_exit(UNDEF_ERR);
                }
                else if (found_var != NULL) {
                    used_rule = id_R(found_var);
                    // printf("7") // TODO: function call
                }
                else {
                    used_rule = function_R(found_func);
                }
            }
            else {
                used_rule = id_R(NULL);
            }
        }
        else if (get_term_type(T) == PL) {
            // E
            look_ahead = pop_T_NT(evaluation_stack);
            used_rule = create_non_term(look_ahead->data.NTerm.rule, look_ahead->data.NTerm.type);

            // )
            look_ahead = pop_T_NT(evaluation_stack);

            // $
            look_ahead = pop_T_NT(evaluation_stack);
            
            // printf("6")
        }
        else {
            printf("Unexpected token after PSA\n");
            error_exit(INTERNAL_ERR); // Debug 
        }
    }

    push_T_NT(processing_stack, NULL, used_rule);
    free(used_rule);
    return;
}

void psa_operation(bool allow_bool) {
    char table_psa[17][17] = { 
        //        | ADD | MUL| SUB| DIV|IDIV| PL | PR | ID | FNC| CM | LT | GT | LTE| GTE| EQ | NEQ| END|
        //        |   + |  * |  - |  / |  \ |  ( |  ) |  i |  f |  , |  < |  > | <= | >= |  = | <> |   $|
        //-----------------------------------------------------------------------------------------------
    {   /* ADD| + |*/'>', '<', '>', '<', '<', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>' }, 
    {   /* MUL| * |*/'>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>' }, 
    {   /* SUB| - |*/'>', '<', '>', '<', '<', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>' }, 
    {   /* DIV| / |*/'>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>' }, 
    {   /* IDI| \ |*/'>', '<', '>', '<', '>', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>' }, 
    {   /* PL | ( |*/'<', '<', '<', '<', '<', '<', '=', '<', '<', '=', '#', '#', '#', '#', '#', '#', '#' }, 
    {   /* PR | ) |*/'>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>', '>', '>', '>', '>', '>', '>' }, 
    {   /* ID | i |*/'>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>', '>', '>', '>', '>', '>', '>' }, 
    {   /* FNC| f |*/'#', '#', '#', '#', '#', '=', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' }, 
    {   /* CM | , |*/'<', '<', '<', '<', '<', '<', '=', '<', '<', '=', '#', '#', '#', '#', '#', '#', '#' }, 
    {   /* LT | < |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '#', '#', '#', '#', '#', '#', '>' },
    {   /* GT | > |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '#', '#', '#', '#', '#', '#', '>' },
    {   /* LTE|<= |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '#', '#', '#', '#', '#', '#', '>' },
    {   /* GTE|>= |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '#', '#', '#', '#', '#', '#', '>' },
    {   /* EQ | = |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '#', '#', '#', '#', '#', '#', '>' },
    {   /* NEQ|<> |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '#', '#', '#', '#', '#', '#', '>' },
    {   /* END| $ |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '<', '<', '<', '<', '<', '<', '\0'},
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
                error_exit(SYNT_ERR);
                break;
            // Only push
            case '=':
                push_T_NT(processing_stack, &currentToken, NULL);
                get_token();
                break;
            case '\0':
                if ((index_input == END) && (index_stack_top == END)) {
                    psa_finished = true;
                }
                else {
                    error_exit(SYNT_ERR);
                }
                break;
            default:
                error_exit(INTERNAL_ERR);
        }

    }

    return;
}
