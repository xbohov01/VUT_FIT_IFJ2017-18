/*
** PSA symtable stack tests header
** Author: Danil Girgorev, xgrigo02
*/

#include <stdlib.h>
#include <string.h>

#include "ifj2017.h"
#include "psa_stack.h"


// TESTS
// =======================
int test();
void print_stack(T_NT_stack *T_NT_s);

T_NT_Data *str_fill_data_token(T_token_type type, char* id, char* str);
T_NT_Data *int_fill_data_token(T_token_type type, char* id, int int_val);
T_NT_Data *double_fill_data_token(T_token_type type, char* id, double d_val);

// =======================
// TODO: End tests