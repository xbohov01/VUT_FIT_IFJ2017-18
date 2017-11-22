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
void eval_cond_expr(int label_num) {

    PSA_Term_type relational_op;
    extern T_NT_stack *processing_stack;
    extern T_NT_stack *evaluation_stack;
    processing_stack = init_T_NT_stack();
    evaluation_stack = init_T_NT_stack();

    init_TAC_stack();
    push_start_term(processing_stack);
    psa_operation(false);
    check_psa_completion();

    // <cond_expr2> -> eps
    relational_op = get_term_type(&currentToken);
    if (relational_op == END) {
        destroy_T_NT_stack(processing_stack);
        destroy_T_NT_stack(evaluation_stack);
        printf("PUSHS int@0\n");
        arithm_TAC(EQ);
    }
    else {
        // <cond_expr_2> -> <rel> <expr>
        get_token();
        push_start_term(processing_stack);
        psa_operation(false);
        check_psa_completion();

        if (get_term_type(&currentToken) != END) {
            fprintf(stderr, "More than one relational operator in expression\n");
            error_exit(SYNT_ERR);
        }
        // <rel>
        switch (relational_op) {
            case LT:
            case GT:
            case LTE:
            case GTE:
            case EQ:
            case NEQ:
                arithm_TAC(relational_op);
                break;
            default:
                fprintf(stderr, "Bad relational operator after psa\n");
                error_exit(INTERNAL_ERR);
                break;
        }
    }
    printf("# Condition evaluation\n");
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $condition%d$end\n", label_num);
    printf("\n");

    destroy_T_NT_stack(processing_stack);
    destroy_T_NT_stack(evaluation_stack);
    // printf("Result =  0\n"); // Tests
    // printf("=====END_EVAL_COND\n");
    return;
}

// <expr> -> <CHECK_START_SYM> <VAR_INIT>
// <expr> -> <CHECK_START_SYM> <VAR_DEFINITION>
// <expr> -> <CHECK_START_SYM> <NO_EFFECT_STATEMENT>
void eval_expr() {

    hash_tab_symbol_type *result_variable;
    enum {
        CHECK_START_SYM,
        VAR_INIT,
        VAR_DEFINITION,
        NO_EFFECT_STATEMENT,
        END_CHECK,
        FINISHED_EXPR
    } eval_ex_state;

    extern T_NT_stack *processing_stack;
    extern T_NT_stack *evaluation_stack;
    processing_stack = init_T_NT_stack();
    evaluation_stack = init_T_NT_stack();
    init_TAC_stack();

    eval_ex_state = CHECK_START_SYM;

    while (eval_ex_state != FINISHED_EXPR) {
        switch (eval_ex_state) {
            case CHECK_START_SYM:
                if (currentToken.token_type == IDENTIFICATOR) {
                // <CHECK_START_SYM> -> 'id' <rel> 
                    result_variable = hash_table_search(var_table, currentToken.id);
                    if (result_variable == NULL) {
                        error_exit(UNDEF_ERR);
                    }

                    // 'id'
                    push_start_term(processing_stack);
                    psa_operation(true);
                    // <rel>
                    if (currentToken.token_type == EQ_O) {
                        eval_ex_state = VAR_DEFINITION;
                    }
                    else if ((get_term_type(&currentToken) <= IDIV) || (get_term_type(&currentToken) == END)) {
                        // Relational operator or some of ending tokens
                        eval_ex_state = NO_EFFECT_STATEMENT;
                    }
                    else {
                        fprintf(stderr, "PSA error after first token eval\n");
                        error_exit(INTERNAL_ERR);
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
            // <VAR_INIT> -> = <PSA_EXPR>
                get_token();
                push_start_term(processing_stack);
                psa_operation(false);
                check_psa_completion();
                // Result is NOT defined
                pop_to_result(NULL);
                eval_ex_state = END_CHECK;
                break;
            case VAR_DEFINITION:
            // <VAR_DEFINITION> -> <PSA_EXPR>
                clear_stack(processing_stack);
                // Read next after '='
                get_token();

                push_start_term(processing_stack);
                psa_operation(false);
                // Result is defined
                pop_to_result(result_variable->symbol_name);
                check_psa_completion();
                eval_ex_state = END_CHECK;
                break;
            case NO_EFFECT_STATEMENT:
            // <CHECK_START_SYM> -> eps
                psa_operation(false);
                check_psa_completion();
                // Result is NOT defined
                pop_to_result(NULL);
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

Data_NTerm *id_or_function_R() {
    static enum {
        START_ID,
        ID_OR_FUNC,
        VAR_FOUND,
        FUNC_FOUND,
        END_CONTROL_ID, // TODO: delete
        FINISHED_ID_OR_FUNC
    } id_or_f_state;

    T_NT_item *look_ahead;
    Data_NTerm *used_rule;
    Data_Term *T;
    hash_tab_symbol_type *found_var;
    hash_tab_symbol_type *found_func;

    id_or_f_state = START_ID;

    while (id_or_f_state != FINISHED_ID_OR_FUNC) {
        switch(id_or_f_state) {
            case START_ID:
                look_ahead = evaluation_stack->popped;
                T = &(look_ahead->data.Term);
                switch (T->token_type) {
                    case INTEGER:
                        push_const_id(T);
                        used_rule = create_non_term(NT_ID, INTEGER_NT);
                        id_or_f_state = END_CONTROL_ID;
                        break;
                    case DOUBLE:
                        push_const_id(T);
                        used_rule = create_non_term(NT_ID, DOUBLE_NT);
                        id_or_f_state = END_CONTROL_ID;
                        break;
                    case STRING:
                        // TODO: some magic?
                        used_rule = create_non_term(NT_ID, STRING_NT);
                        id_or_f_state = END_CONTROL_ID;
                        break;
                    case IDENTIFICATOR:
                        id_or_f_state = ID_OR_FUNC;
                        break;
                    default:
                        printf("UNEXPECTED TOKEN AFTER PSA");
                        error_exit(INTERNAL_ERR);
                }
                break;
            case ID_OR_FUNC:
                found_var = hash_table_search(var_table, T->id);
                found_func = hash_table_search(func_table, T->id);
                if ((found_var == NULL) && (found_func == NULL)) {
                    printf("Variable was not declared\n");
                    error_exit(UNDEF_ERR); // TODO: Really this error?
                }
                else if (found_var != NULL) {
                    id_or_f_state = VAR_FOUND;
                }
                else {
                    id_or_f_state = FUNC_FOUND;
                }
                break;
            case VAR_FOUND:
                push_var_id(found_var->symbol_name);
                used_rule = create_non_term(NT_ID, found_var->value_type);
                id_or_f_state = END_CONTROL_ID;
                break;
            case FUNC_FOUND:
                used_rule = function_R();
                id_or_f_state = FINISHED_ID_OR_FUNC;
                break;
            case END_CONTROL_ID:
                look_ahead = pop_T_NT(evaluation_stack);
                T = &(look_ahead->data.Term);
                if (get_term_type(T) == END) {
                    id_or_f_state = FINISHED_ID_OR_FUNC;
                }
                else {
                    printf("UNEXPECTED ITEM AFTER PSA\n"); // Debug
                    error_exit(INTERNAL_ERR);
                }
                break;
            }
    }

    return used_rule;
}

Data_NTerm *function_R() {
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
                switch (get_term_type(&look_ahead->data.Term)) {
                    case ADD:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: int to float stack second
                            // second_operand, int2fl, round_to_even
                            retype_stack(false, true, false);
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_ADD, DOUBLE_NT);
                        
                        arithm_TAC(ADD);
                        // printf("1")
                        break;
                    case SUB:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: int to float stack second
                            retype_stack(false, true, false);
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_SUB, DOUBLE_NT);
                        
                        swap_stack();
                        arithm_TAC(SUB);
                        // printf("2")
                        break;
                    case MUL:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: int to float stack second
                            retype_stack(false, true, false);
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_MUL, DOUBLE_NT);
                        
                        arithm_TAC(MUL);
                        // printf("3")
                        break;
                    case DIV:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: int to float stack second
                            retype_stack(false, true, false);
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_DIV, DOUBLE_NT);
                        
                        swap_stack();
                        arithm_TAC(DIV);
                        // printf("4")
                        break;
                    case IDIV:
                        error_exit(TYPE_ERR);
                        break;
                    default:
                        printf("UNEXPECTED SIGN AFTER PSA\n");
                        error_exit(SEM_ERR);
                        break;
                }
                break;
            case INT_AR_PSA:
                look_ahead = pop_T_NT(evaluation_stack);
                switch(get_term_type(&look_ahead->data.Term)) {
                    case ADD:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_ADD, INTEGER_NT);
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: int to float stack first
                            retype_stack(false, true, false);
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_ADD, DOUBLE_NT);
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        
                        arithm_TAC(ADD);
                        // printf("1")
                        break;
                    case SUB:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: generate stack sub
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_SUB, INTEGER_NT);
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: int to float stack first
                            retype_stack(false, true, false);
                            // TODO: generate stack sub
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_SUB, DOUBLE_NT);
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }

                        swap_stack();
                        arithm_TAC(SUB);
                        // printf("2")
                        break;
                    case MUL:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: generate stack mul
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_MUL, INTEGER_NT);
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: int to float stack first
                            retype_stack(false, true, false);
                            // TODO: generate stack mul
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_MUL, DOUBLE_NT);
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        
                        arithm_TAC(MUL);
                        // printf("3")
                        break;
                    case DIV:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: int to float stack first
                            retype_stack(true, true, false);
                            // TODO: int to float stack second
                            retype_stack(false, true, false);
                            // TODO: generate stack float div
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: int to float stack first
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_DIV, DOUBLE_NT);
                        
                        swap_stack();
                        arithm_TAC(DIV);
                        // printf("4")
                        break;
                    case IDIV:
                        printf("OK IDIV\n");
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: generate stack integer div
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected double or string type
                        else {
                            error_exit(TYPE_ERR);
                        }
                        used_rule = create_non_term(NT_IDIV, INTEGER_NT);
                        
                        swap_stack();
                        arithm_TAC(IDIV);
                        // printf("5")
                        break;
                    default:
                        printf("UNEXPECTED SIGN AFTER PSA\n");
                        error_exit(SEM_ERR);
                }
                break;
            case STRING_AR_PSA:
                look_ahead = pop_T_NT(evaluation_stack);
                if (get_term_type(&look_ahead->data.Term) == ADD) {
                    look_ahead = pop_T_NT(evaluation_stack);
                    E = &(look_ahead->data.NTerm);
                    if (E->type == STRING_NT) {
                        // TODO: Concat str_result str_temp_1 str_temp_2 
                        arithm_state = AR_END_PSA;
                    }
                    else {
                        error_exit(TYPE_ERR);
                    }
                }
                else {
                    error_exit(SEM_ERR);
                }
                used_rule = create_non_term(NT_ADD, STRING_NT);
                
                // printf("1")
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


        // PARENTHESIS_END_PSA,

        // FUNC_PSA,
        // FUNC_EXTEND_PSA,
        // FUNC_END_PSA,

        // CONSTANT_PSA,
        // ID_PSA,

        // ACCEPT_RULE_PSA,
    T_NT_item *look_ahead;
    Data_NTerm *used_rule;
   
    extern T_NT_stack *evaluation_stack;
    
    push_start_term(evaluation_stack);
    get_reversed_rule();
    look_ahead = pop_T_NT(evaluation_stack);

    if (look_ahead->is_non_term == true) {
        used_rule = arithm_R();
    }
    else {
        if (get_term_type(&(look_ahead->data.Term)) == ID) {
            used_rule = id_or_function_R();
            
            // printf("7") // TODO: function call
        }
        else if (get_term_type(&(look_ahead->data.Term)) == PL) {
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

void psa_operation(bool needed_definition) {
    char table_psa[11][11] = { \
        //        | ADD | MUL| SUB| DIV|IDIV| PL | PR | ID | FNC| CM | END|
        //        |   + |  * |  - |  / |  \ |  ( |  ) |  i |  f |  , |  $ |
        //----------------------------------------------------------------    
    {   /* ADD| + |*/'>', '<', '>', '<', '<', '<', '>', '<', '<', '>', '>' }, 
    {   /* MUL| * |*/'>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>' }, 
    {   /* SUB| - |*/'>', '<', '>', '<', '<', '<', '>', '<', '<', '>', '>' }, 
    {   /* DIV| / |*/'>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>' }, 
    {   /* IDI| \ |*/'>', '<', '>', '<', '>', '<', '>', '<', '<', '>', '>' }, 
    {   /* PL | ( |*/'<', '<', '<', '<', '<', '<', '=', '<', '<', '=', '#' }, 
    {   /* PR | ) |*/'>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>' }, 
    {   /* ID | i |*/'>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>' }, 
    {   /* FNC| f |*/'#', '#', '#', '#', '#', '=', '#', '#', '#', '#', '#' }, 
    {   /* CM | , |*/'<', '<', '<', '<', '<', '<', '=', '<', '<', '=', '#' }, 
    {   /* END| $ |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '\0'}
    };

    PSA_Term_type index_stack_top;
    PSA_Term_type index_input;
    Data_Term first_term_data;
    bool use_push;
    // In case of relational operator will be set on true

    bool psa_finished;

    // Main logic

    psa_finished = false;

    while (psa_finished != true) {
        first_term_data = find_first_term(processing_stack, &use_push)->data.Term;
        index_stack_top = get_term_type(&first_term_data);
        index_input = get_term_type(&currentToken);
        if (index_input > END) {
        // Got relational operator
            index_input = END;
        }
        
        // Syntax control part:
        // ----------------------------------------------
        switch(table_psa[index_stack_top][index_input]) {
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
                if (needed_definition == true) {
                    return; // Temporary stop for variable reading
                }
                reduce_by_rule(needed_definition);
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
