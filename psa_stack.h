/*
**  PSA stack header file
**  Author: Danil Grigorev, xgrigo02
**          Samuel Bohovic, xbohov01
*/

#ifndef PSA_STACK_HEADER
#define PSA_STACK_HEADER

#include <stdlib.h>
#include "ifj2017.h"
#include "errors.h"


// Start of stack file
// ==============================

/* 
Technically non terminal is a list of 
terminals/non-terminals sorted for interpreter
conversion to IFJ_17 code.

Stack of stacks is a best representation
*/

// Possible types for non_terminals
// Used after rule application
typedef enum non_term_types {
    NT_ADD = 1, // 1: E + E
    NT_SUB,     // 2: E - E
    NT_MUL,     // 3: E * E
    NT_DIV,     // 4: E / E
    NT_MOD,     // 5: E \ E

    NT_PAR,     // 6: (E)

    NT_LT,      // 7: E < E
    NT_GT,      // 8: E > E
    NT_LE,      // 9: E <= E
    NT_GE       // 10:E >= E
} NT_types;

// Extended stack of terminals and non terminals (Basically a one-way list with pop function)
typedef struct t_nt_stack {
    struct t_nt_item *top;
    // Active item, mostly used for first terminal search and after item insertion
    struct t_nt_item *active;
    struct t_nt_item *popped; // Lastly popped item
} T_NT_stack;

// Non_terminal itself
typedef struct data_non_term
{
    NT_types NT_type;

} Data_NTerm; // Non-terminal data


// TODO: redo token for similar view
// typedef struct data_term {
//     T_token_type T_type;

//     union {
//         int int_val;
//         double double_var;
//         char* str_val;
//     } value;

// } Data_T; // Terminal data


// Style conversion of tToken structure
typedef tToken Data_Term;

// Data types of stack items
typedef union t_nt_data{
    Data_NTerm NTerm;
    Data_Term Term;
} T_NT_Data;

// Stack item structure
typedef struct t_nt_item {

    bool is_non_term;
    T_NT_Data data;
    struct t_nt_item *next_T_NT;

} T_NT_item;


void error_exit(T_NT_stack *T_NT_s, int code);

T_NT_stack *init_T_NT_stack();
void destroy_T_NT_stack(T_NT_stack *s);
T_NT_item *push_T_NT(T_NT_stack *s, bool is_non_term, T_NT_Data data);
T_NT_item *pop_T_NT(T_NT_stack *s); // Returns item for easy search

// Extended stack operations
void set_first_T_NT(T_NT_stack *s);
void next_T_NT(T_NT_stack *s);
bool active_T_NT(T_NT_stack *s);
T_NT_item *insert_after_T_NT(T_NT_stack *s, bool is_non_term, T_NT_Data data);


// =======================
// End of stack file

#endif // PSA_STACK_HEADER include