/* 
** Precedense syntax parser implementation
** Author: Danil Grigorev, xgrigo02
*/

#include "psa_parser.h"


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


    // // Simulate scanner actions 

    // result = eval_expr();
    NEXTT();

    // // Make stack init
    // T_NT_stack new_stack;
    // stack_init(&new_stack);
    //                         //   type, value
    // Token end_token = init_token(STR,  "$")
    // stack_push(&new_stack, end_token)
    // eval_expr()

    return 0;
}

// Feeds non terminal
// TODO: rename push_non_term and update
// T_NT_Data *non_term_create(N_T_rules input_type) {
//     T_NT_Data *temp_non_term_data = malloc(sizeof(T_NT_Data));
//     if (temp_non_term_data == NULL) {
//         return NULL;
//     }

//     Data_NTerm temp_non_term = temp_non_term_data->NTerm;

//     temp_non_term.NT_type = input_type;

//     temp_non_term_data->NTerm = temp_non_term;
//     return temp_non_term_data;
// }

// bool expecting_token(Data_Term *tok, PSA_Term_type expect) {
//     PSA_Term_type got = term_type(tok);
//     if (got == expect)
//         return true;
//     else if (got == ERR)
//         error_exit(processing_stack, SYNT_ERR);
//     else
//         return false;
// }

// void push_start_term(T_NT_stack *s) {
//     T_NT_Data *temp_data = malloc(sizeof(T_NT_Data));
//     if (temp_data == NULL) {
//         error_exit(s, INTERNAL_ERR);
//     }

//     temp_data->Term.token_type = ENDL;

//     push_T_NT(s, false, temp_data);
//     free(temp_data);
//     return;
// }


// PSA_Term_type term_type(Data_Term *in_term) {
//     switch(in_term->token_type) {
//         // Used in PSA table
//         //------------------
//         case ADD_O:
//             return ADD;
//         case SUB_O:
//             return SUB;
//         case MUL_O:
//             return MUL;
//         case DIV_O:
//             return DIV;
//         case MOD_O:
//             return MOD;
//         case PAR_R:
//             return PR;
//         case PAR_L:
//             return PL;
//         case DOUBLE:
//         case INTEGER:
//         case STRING:
//         case IDENTIFICATOR:
//             // TODO: can be const or function or variable,
//             // should look in hash_table
//             return ID;
//         case ENDL:
//             return END;

//         /*---------------------
//         * Relational operators */

//         case LT_O:
//             return LT;
//         case GT_O:
//             return GT;
//         case LTE_O:
//             return LTE;
//         case GTE_O:
//             return GTE;
//         case EQ_O:
//             return EQ;
//         case NE_O:
//             return NEQ;
//         default:
//             // Bad token type
//             return ERR;
//     }
// }


// int eval_expr() {

    

//     T_NT_item *item1;
//     item1 = psa_operation(&last_token, &processing_stack)
    
    
// }

// T_NT_item *find_first_term(T_NT_stack *s, bool *is_first) {
//     set_first_T_NT(s);
//     *is_first = false;

//     // In case that top item is term, should return it
//     // and notification to do push instead of insert_after
//     if (s->active.is_non_term) {
//         // Notification
//         *is_first = true;
//         return s->active;
//     }

//     while (!s->active->next_T_NT.is_non_term)
//         next_T_NT(s);

//     return s->active->next_T_NT;
// }

// void insert_stopper(T_NT_stack *s) {
//     bool use_push;
//     T_NT_Data *stopper = non_term_create(STOPPER);

//     find_first_term(s, &use_push);
//     if (use_push)
//         push_T_NT(s, true, *stopper);
//     else
//         insert_after_T_NT(s, true, *stopper);

//     free(stopper);
//     return;
// }

// PSA_state reduce_by_rule(T_NT_stack *s, Data_Term *input, PSA_state final_state) {

//     while (final_state != ACCEPT_RULE_PSA) {
//         //----RULES----
//         // NT_ADD, // 1: E + E
//         // NT_SUB, // 2: E - E
//         // NT_MUL, // 3: E * E
//         // NT_DIV, // 4: E / E
//         // NT_MOD, // 5: E \ E

//         // NT_PAR, // 6: (E)

//         // NT_LT,  // 7: E < E
//         // NT_GT,  // 8: E > E
//         // NT_LE,  // 9: E <= E
//         // NT_GE,  // 10:E >= E



//     }

//     return final_state;
// }

// void psa_operation(Data_Term *last_token, T_NT_stack *s) {
//     char table_psa[11][11] = {
//     //        | ADD | MUL| SUB| DIV| MOD| LP | RP | ID | FNC|COMM| END|
//     //        |   + |  * |  - |  / |  \ |  ( |  ) |  i |  f |  , |  $ |
//     //----------------------------------------------------------------    
//     /* ADD| + |*/'>', '<', '>', '<', '<', '<', '>', '<', '<', '>', '>', 
//     /* MUL| * |*/'>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>', 
//     /* SUB| - |*/'>', '<', '>', '<', '<', '<', '>', '<', '<', '>', '>',
//     /* DIV| / |*/'>', '>', '>', '>', '>', '<', '>', '<', '<', '>', '>',
//     /* MOD| \ |*/'>', '<', '>', '<', '>', '<', '>', '<', '<', '>', '>',
//     /* LP | ( |*/'<', '<', '<', '<', '<', '<', '=', '<', '<', '=', '#', 
//     /* RP | ) |*/'>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>', 
//     /* ID | i |*/'>', '>', '>', '>', '>', '#', '>', '#', '#', '>', '>', 
//     /* FNC| f |*/'#', '#', '#', '#', '#', '=', '#', '#', '#', '#', '#',
//     /* COM| , |*/'<', '<', '<', '<', '<', '<', '=', '<', '<', '=', '#',
//     /* END| $ |*/'<', '<', '<', '<', '<', '<', '#', '<', '<', '#', '\0'
//     };

//     PSA_Term_type index_stack_top;
//     PSA_Term_type index_input;
//     T_NT_Data first_term_data;
//     bool use_push;

//     PSA_state current_state;

    

//     // Main logic

//     processing_stack = init_T_NT_stack();
//     push_start_term(processing_stack);
    
//     current_state = START_PSA;

//     while (current_state != FINISHED_PSA) {

//         first_term_data = find_first_term(processing_stack, &use_push)->data;
//         index_stack_top = term_type(&first_term_data);
//         index_input = term_type(&currentToken);

//         // Syntax control part:
//         // ----------------------------------------------
//         switch(table_psa[index_stack_top][index_input]) {
//             // Insert stopper before first terminal and push currentToken to stack
//             case '<':
//                 insert_stopper(processing_stack);
//                 push_T_NT(processing_stack, false, &currentToken);
//                 break;
//             // Reduce by rule
//             // 
//             // -------------------Semantic control part-----------------------
//             case '>':
//                 current_state
//                 while()
//                 break;
//             // --------------------End semantic control-----------------------
//             // Syntax error
//             case '#':
//                 error_exit(processing_stack, SYNT_ERR);
//                 break;
//             // Only push
//             case '=':
//                 push_T_NT(processing_stack, false, &currentToken);
//                 break;
//             //
//             default:
//                 if (current_state == ACCEPT_RULE_PSA)
//                     current_state = FINISHED_PSA;
//                 else
//                     error_exit(processing_stack, SYNT_ERR);

//         }
//     }

//     // Final free
//     destroy_T_NT_stack(processing_stack);
//     return 0;
// }
