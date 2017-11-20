/*
** Precedense syntax parser implementation
** Author: Danil Grigorev, xgrigo02
*/

//#include "psa_parser.h"
#include "ifj2017.h"

/*
int main() {
    // Tests
    // +++++++++++++++
    // hash_table_type *tmp = NULL;
    // tmp = sym_tab_init(20);
    // hash_table_insert(tmp, "new_id");
    // printf("symbol name: %s\n", hash_table_search(tmp, "new_id")->symbol_name);
    // hash_table_destroy(tmp);

    // test();
    // +++++++++++++++
    right_order = malloc(100);
    memset(right_order, 0, 100);
    eval_expr();
    printf("\n");
    printf("right_order: %s\n", right_order);
    printf("Result: 0\n");
    return 0;
}
*/

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


// <expr> -> <PSA> <PSA_second>
void eval_expr() {

    // Init
    PSA_Term_type relational_op;
    extern T_NT_stack *processing_stack;
    extern T_NT_stack *evaluation_stack;
    processing_stack = init_T_NT_stack();
    evaluation_stack = init_T_NT_stack();
    //start_scanner("hard_test_with_seg_fault.txt"); // TODO: delete -- test

    // TODO: delete -- test
    // id
    get_token();
    // =
    get_token();
    // smth
    get_token();
    // TODO: delete -- test ^

    // <PSA>
    psa_operation();
    // --END-- <PSA>

    // <PSA_second>
    relational_op = get_term_type(&currentToken);

    // <PSA_second> -> epsilon
    if (relational_op == END) {
        free_sources(); // TODO: delete -- test
        destroy_T_NT_stack(processing_stack);
        destroy_T_NT_stack(evaluation_stack);
        return;
    }
    // --END-- <PSA_second> -> epsilon
    // ++++++++++++++++++++++++++++++++++
    // <PSA_second> -> <rel> <PSA>
    else {
        if (relational_op > END)
        get_token();
        // <PSA>
        psa_operation();
        // --END-- <PSA>

        // <rel>
        switch (relational_op) {

            // <rel> -> '<'
            case LT:
                // TODO: add instruction
                break;
            // ++++++++++++++++++++++++++
            // <rel> -> '>'
            case GT:
                // TODO: add instruction
                break;
            // ++++++++++++++++++++++++++
            // <rel> -> '<='
            case LTE:
                // TODO: add instruction
                break;
            // ++++++++++++++++++++++++++
            // <rel> -> '>='
            case GTE:
                // TODO: add instruction
                break;
            // ++++++++++++++++++++++++++
            // <rel> -> '='
            case EQ:
                // TODO: add instruction
                break;
            // ++++++++++++++++++++++++++
            // <rel> -> '<>'
            case NEQ:
                // TODO: add instruction
                break;
            default:
                break;
        }
        // --END-- <rel>
    }
    // --END-- <PSA_second> -> <rel> <PSA>

    // --END-- <PSA_second>


    // Final free
    free_sources(); // TODO: delete -- test
    destroy_T_NT_stack(processing_stack);
    destroy_T_NT_stack(evaluation_stack);

    return;
}
// --END-- <expr> -> <PSA> <PSA_second>

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
    // TODO: undone
    static enum {
        START_ID,
        ID_OR_FUNC,
        END_CONTROL_ID, // TODO: delete
        FINISHED_ID
    } id_or_f_state;

    T_NT_item *look_ahead;
    Data_NTerm *used_rule;
    Data_Term *T;

    id_or_f_state = START_ID;

    while (id_or_f_state != FINISHED_ID) {
        switch(id_or_f_state) {
            case START_ID:
                look_ahead = evaluation_stack->popped;
                T = &(look_ahead->data.Term);
                switch (T->token_type) {
                    case INTEGER:
                        // TODO: push
                        used_rule = create_non_term(NT_ID, INTEGER_NT);
                        look_ahead = pop_T_NT(evaluation_stack);
                        id_or_f_state = END_CONTROL_ID;
                        break;
                    case DOUBLE:
                        // TODO: push
                        used_rule = create_non_term(NT_ID, DOUBLE_NT);
                        look_ahead = pop_T_NT(evaluation_stack);
                        id_or_f_state = END_CONTROL_ID;
                        break;
                    case STRING:
                        // TODO: some magic?
                        used_rule = create_non_term(NT_ID, STRING_NT);
                        look_ahead = pop_T_NT(evaluation_stack);
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
                T = &(look_ahead->data.Term);
                // TODO: hash_search
                // if id
                used_rule = create_non_term(NT_ID, INTEGER_NT); // TODO: get type from hash table
                look_ahead = pop_T_NT(evaluation_stack);
                id_or_f_state = END_CONTROL_ID; // TODO: delete
                // else if function
                // used_rule = function_R();
                break;
            case END_CONTROL_ID:
                T = &(look_ahead->data.Term);
                if (get_term_type(T) == END) {
                    id_or_f_state = FINISHED_ID;
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


Data_NTerm *arithm_R(int i) {

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
                            // TODO: generate stack sum
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: generate stack sum
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_ADD, DOUBLE_NT);
                        right_order[i] = '1';
                        printf("1");
                        break;
                    case SUB:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: int to float stack second
                            // TODO: generate stack sub
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: generate stack sub
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_SUB, DOUBLE_NT);
                        right_order[i] = '2';
                        printf("2");
                        break;
                    case MUL:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: int to float stack second
                            // TODO: generate stack mul
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: generate stack mul
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_MUL, DOUBLE_NT);
                        right_order[i] = '3';
                        printf("3");
                        break;
                    case DIV:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: int to float stack second
                            // TODO: generate stack float div
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: generate stack float div
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_DIV, DOUBLE_NT);
                        right_order[i] = '4';
                        printf("4");
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
                            // TODO: generate stack add
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_ADD, INTEGER_NT);
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: int to float stack first
                            // TODO: generate stack add
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_ADD, DOUBLE_NT);
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        right_order[i] = '1';
                        printf("1");
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
                            // TODO: generate stack sub
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_SUB, DOUBLE_NT);
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        right_order[i] = '2';
                        printf("2");
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
                            // TODO: generate stack mul
                            arithm_state = AR_END_PSA;
                            used_rule = create_non_term(NT_MUL, DOUBLE_NT);
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        right_order[i] = '3';
                        printf("3");
                        break;
                    case DIV:
                        look_ahead = pop_T_NT(evaluation_stack);
                        E = &(look_ahead->data.NTerm);
                        if (E->type == INTEGER_NT) {
                            // TODO: int to float stack first
                            // TODO: int to float stack second
                            // TODO: generate stack float div
                            arithm_state = AR_END_PSA;
                        }
                        else if (E->type == DOUBLE_NT) {
                            // TODO: int to float stack first
                            // TODO: generate stack float div
                            arithm_state = AR_END_PSA;
                        }
                        // Unexpected string type
                        else {
                            error_exit(SEM_ERR);
                        }
                        used_rule = create_non_term(NT_DIV, DOUBLE_NT);
                        right_order[i] = '4';
                        printf("4");
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
                        right_order[i] = '5';
                        printf("5");
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
                right_order[i] = '1';
                printf("1");
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
    static int i = -1;
    i++;
    T_NT_item *look_ahead;
    Data_NTerm *used_rule;

    extern T_NT_stack *evaluation_stack;

    push_start_term(evaluation_stack);
    get_reversed_rule();
    look_ahead = pop_T_NT(evaluation_stack);

    if (look_ahead->is_non_term == true) {
        used_rule = arithm_R(i);
    }
    else {
        if (get_term_type(&(look_ahead->data.Term)) == ID) {
            used_rule = id_or_function_R(i);
            right_order[i] = '7';
            printf("7");
        }
        else if (get_term_type(&(look_ahead->data.Term)) == PL) {
            // E
            look_ahead = pop_T_NT(evaluation_stack);
            used_rule = create_non_term(look_ahead->data.NTerm.rule, look_ahead->data.NTerm.type);

            // )
            look_ahead = pop_T_NT(evaluation_stack);

            // $
            look_ahead = pop_T_NT(evaluation_stack);
            right_order[i] = '6';
            printf("6");
        }
        else {
            error_exit(INTERNAL_ERR); // Debug
        }
    }

    push_T_NT(processing_stack, NULL, used_rule);
    free(used_rule);
    return;
}

void psa_operation() {

    char table_psa[11][11] = {
    //        | ADD | MUL| SUB| DIV|IDIV| PL | PR | ID | FNC| CM | END|
    //        |   + |  * |  - |  / |  \ |  ( |  ) |  i |  f |  , |  $ |
    //----------------------------------------------------------------
    /* ADD| + |*/'>', '<', '>', '<', '<', '<', '>', '<', '<', '>', '>',
    /* MUL| * |*/'>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>',
    /* SUB| - |*/'>', '<', '>', '<', '<', '<', '>', '<', '<', '>', '>',
    /* DIV| / |*/'>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>',
    /* IDI| \ |*/'>', '<', '>', '<', '>', '<', '>', '<', '<', '>', '>',
    /* PL | ( |*/'<', '<', '<', '<', '<', '<', '=', '<', '<', '=', '#',
    /* PR | ) |*/'>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>',
    /* ID | i |*/'>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>',
    /* FNC| f |*/'#', '#', '#', '#', '#', '=', '#', '#', '#', '#', '#',
    /* CM | , |*/'<', '<', '<', '<', '<', '<', '=', '<', '<', '=', '#',
    /* END| $ |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '\0'
    };

    PSA_Term_type index_stack_top;
    PSA_Term_type index_input;
    Data_Term first_term_data;
    bool use_push;
    // In case of relational operator will be set on true
    bool got_relational_op = false;

    bool psa_finished;

    // Main logic

    push_start_term(processing_stack);
    psa_finished = false;

    while (psa_finished != true) {
        first_term_data = find_first_term(processing_stack, &use_push)->data.Term;
        index_stack_top = get_term_type(&first_term_data);
        index_input = get_term_type(&currentToken);
        // Got relational operator
        if (index_input > END) {
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
                reduce_by_rule(processing_stack);
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
                    if (pop_T_NT(processing_stack)->is_non_term == false)
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
