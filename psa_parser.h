/*
**  PSA symtable for parser
**  Author: Danil Grigorev, xgrigo02
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "errors.h"
#include "ifj2017.h"
#include "ial.c"
#include "scanner.h"
#include "psa_stack.h"
// #include "psa_stack_test.h" TESTS


typedef enum psa_states {
    START_RULE_PSA,

    INT_ARITHM_PSA,
    DOUBLE_ARITHM_PSA,
    STRING_ARITHM_PSA,

    PARENTHESES_PSA,

    IDENTIFIER_PSA,
    CONSTANT_PSA,
    FUNCTION_PSA,

    ACCEPT_RULE_PSA,
    FINISHED_PSA,
    ERROR_PSA
} PSA_state;

typedef enum psa_term_type{
    ERR = -1,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    PR,
    PL,
    ID,
    END,

    // Relational operators
    // which are not used in PSA table,
    // but logically are included
    LT,
    GT,
    LTE,
    GTE,
    EQ,
    NEQ
} PSA_Term_type;

// TODO: create operations 
// int psa_parse();

// void push_start_term(T_NT_stack *s);
// bool expected_token(Data_Term *tok, PSA_Term_type expect);
// T_NT_item *find_first_term(T_NT_stack *s, bool *is_first);
// void insert_stopper(T_NT_stack *s);
// void psa_compare(Data_Term *tok, T_NT_stack *s);
// PSA_state reduce_by_rule(T_NT_stack *s, Data_Term *input, PSA_state now);
// T_NT_Data *non_term_create(N_T_rules type);

// PSA_Term_type term_type(Data_Term *term);

// Data_Term currentToken; // TODO: make extern
// T_NT_stack *processing_stack;

// // TODO: to be deleted
// // For test, simulate rule usage and shows order
// NT_types *right_order
