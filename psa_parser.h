/*
**  PSA symtable for parser
**  Author: Danil Grigorev, xgrigo02
*/

#ifndef PSA_PARSER_INCLUDED
#define PSA_PARSER_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "errors.h"
#include "ifj2017.h"
//#include "psa_stack.h"
//#include "psa_stack_test.h" // TESTS


void eval_expr();
void psa_operation();
void get_reversed_rule();

// TODO: create operations
Data_NTerm *id_or_function_R();
Data_NTerm *function_R();
Data_NTerm *arithm_R(int i);


void push_start_term(T_NT_stack *s);
T_NT_item *find_first_term(T_NT_stack *s, bool *is_first);
void insert_stopper(T_NT_stack *s);
void psa_compare(Data_Term *tok, T_NT_stack *s);
void reduce_by_rule();
Data_NTerm *create_non_term(N_T_rules in_rule, N_T_types in_type);

extern Data_Term currentToken;
T_NT_stack *processing_stack;
T_NT_stack *evaluation_stack;

// TODO: to be deleted
// For test, simulate rule usage and shows order
char *right_order;

#endif // PSA_PARSER_INCLUDED end
