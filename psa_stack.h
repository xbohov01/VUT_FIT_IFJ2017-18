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

// Possible types for non_terminals
// Used after rule application
typedef enum non_term_types {
    NT_ADD, // 1: E -> E + E
    NT_SUB, // 2: E -> E - E
    NT_MUL, // 3: E -> E * E
    NT_DIV, // 4: E -> E / E
    NT_IDIV, // 5: E -> E \ E

    NT_PAR, // 6: E -> (E)

    NT_ID,  // 7: E -> id
    NT_FN,  // 8: E -> id(eps/E/E,...E)
    
    STOPPER // '<'
} N_T_rules;

typedef enum {
    DOUBLE_NT,
    INTEGER_NT,
    STRING_NT,
    NONE_NT
} N_T_types;

typedef enum psa_term_type{
    ADD,
    MUL,
    SUB,
    DIV,
    IDIV,
    PL,
    PR,
    ID,
    FNC,
    CM,
    END,

    // Relational operators
    LT,
    GT,
    LTE,
    GTE,
    EQ,
    NEQ,

    PSA_ERR
} PSA_Term_type;


// Extended stack of terminals and non terminals
typedef struct t_nt_stack {
    struct t_nt_item *top;
    struct t_nt_item *active;
    struct t_nt_item *popped;
} T_NT_stack;

// Non_terminal itself
typedef struct data_non_term
{
    N_T_rules rule;
    N_T_types type;
} Data_NTerm; // Non-terminal data

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


void error_exit(int code);
PSA_Term_type get_term_type(Data_Term *in_term);
void copy_term(Data_Term *from, Data_Term *to);

T_NT_stack *init_T_NT_stack();
void destroy_T_NT_stack(T_NT_stack *s);
T_NT_item *push_T_NT(T_NT_stack *s, Data_Term *in_term, Data_NTerm *in_non_term);
T_NT_item *pop_T_NT(T_NT_stack *s); // Returns item for easy search

// Extended stack operations
T_NT_item *set_first_T_NT(T_NT_stack *s);
T_NT_item *set_next_T_NT(T_NT_stack *s);
bool active_T_NT(T_NT_stack *s);
T_NT_item* insert_after_T_NT(T_NT_stack *s, Data_Term *in_term, Data_NTerm *in_non_term);

// =======================
// End of stack file

#endif // PSA_STACK_HEADER include