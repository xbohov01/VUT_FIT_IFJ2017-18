/* 
** Precedense syntax parser implementation
** Author: Danil Grigorev, xgrigo02
*/

#include "psa_parser.h"


int main() {
    // // Simulate scanner actions 

    // // Was read already from scanner
    // Token last_token;

    // // Make stack init
    // T_NT_stack new_stack;
    // stack_init(&new_stack);
    //                         //   type, value
    // Token end_token = init_token(STR,  "$")
    // stack_push(&new_stack, end_token)
    // eval_expr()

    // Hash table test
    // +++++++++++++++
    hash_table_type *tmp = NULL;
    tmp = sym_tab_init(20);
    hash_table_insert(tmp, "new_id");
    printf("symbol name: %s\n", hash_table_search(tmp, "new_id")->symbol_name);
    hash_table_destroy(tmp);
    // +++++++++++++++
    return test();
}
