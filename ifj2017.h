/**** ifj2017.h ****/
//Global header file//
//xbohov01
//xgrigo02
//xberes01
//xkosti07

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "errors.h"

//====ERRORS====
void hard_exit(int code);

//====STRING====
typedef struct {
	char *content;
	int len;
	int size;
} tString;

int str_init(tString *str);
void delstr(tString *str);
int addchar(char n_char, tString *str);

//====SCANNER====
#define N_KEYWORDS 35
#define N_OPERATOR_CHARS 8
#define N_OPERATORS 12
#define N_DELIMITERS 7

//enumerator for token types
typedef enum {
	//operators begin
	ADD_O,// +
	ERR,
	SUB_O,// -
	MUL_O,// *
	DIV_O,// /
	MOD_O,
	LT_O,// <
	GT_O,// >
	LTE_O,// <=
	GTE_O,// >=
	EQ_O,// =
	NE_O,// <>
	//operators end

	//other chars begin
	PAR_R, // )
	PAR_L,// (
	COM,// ,
	SEM,// ;
	//other chars end

	//keywords begin
	AS_KEY,
	ASC_KEY,
	DECLARE_KEY,
	DIM_KEY,
	DO_KEY,
	DOUBLE_KEY,
	ELSE_KEY,
	END_KEY,
	CHR_KEY,
	FUNCTION_KEY,
	INPUT_KEY,
	INTEGER_KEY,
	LENGTH_KEY,
	LOOP_KEY,
	PRINT_KEY,
	RETURN_KEY,
	SCOPE_KEY,
	STRING_KEY,
	SUBSTR_KEY,
	THEN_KEY,
	WHILE_KEY,
	AND_KEY,
	BOOLEAN_KEY,
	CONTINUE_KEY,
	ELSEIF_KEY,
	EXIT_KEY,
	FALSE_KEY,
	FOR_KEY,
	NEXT_KEY,
	NOT_KEY,
	OR_KEY,
	SHARED_KEY,
	STATIC_KEY,
	TRUE_KEY,
	IF_KEY,
	//keywords end

	DOUBLE,
	INTEGER,
	STRING,
	IDENTIFICATOR,

	UNDEFINED,
	ERROR,
	ENDL,
	ENDF

} T_token_type;

typedef enum {
	BEGIN,

	POS_LT,
	POS_GT,

	ID_OR_KEY,

	POS_INT,
	POS_EXP,
	POS_EXP_S,
	POS_FL,
	POS_DOUBLE,

	POS_BEG_STRING,
	POS_STRING,
	ESCAPE,

	POS_BL_COMMENT,
	POS_BL_END_COMMENT,

	COM_OR_DIV,

	POS_LIN_COMMENT
} T_token_state;

//structure for token
typedef struct {
	int value_int;
	double value_double;
	char *value_string;
	T_token_type token_type;
	char *id;
} tToken;

FILE* file;

//buffer for identifiers
tString buffer;
#define BUFFERSIZE 32

int esc;

//function declarations
void free_sources();
int start_scanner(char *filename);
void get_token();

//====PSA_STACK====

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

T_NT_stack *init_T_NT_stack();
void destroy_T_NT_stack(T_NT_stack *s);
T_NT_item *push_T_NT(T_NT_stack *s, Data_Term *in_term, Data_NTerm *in_non_term);
T_NT_item *pop_T_NT(T_NT_stack *s); // Returns item for easy search

// Extended stack operations
T_NT_item *set_first_T_NT(T_NT_stack *s);
T_NT_item *set_next_T_NT(T_NT_stack *s);
bool active_T_NT(T_NT_stack *s);
T_NT_item* insert_after_T_NT(T_NT_stack *s, Data_Term *in_term, Data_NTerm *in_non_term);

// =========PSA_STACK_TESTS===========
void ps(T_NT_stack *T_NT_s); // Print stack for debug

//====PSA===
void eval_expr();
void psa_operation(bool first_call);
void reduce_by_rule(bool first_call);
void get_reversed_rule();

// TODO: create operations
Data_NTerm *id_or_function_R();
Data_NTerm *function_R();
Data_NTerm *parenthesis_R();
Data_NTerm *arithm_R();


void push_start_term(T_NT_stack *s);
T_NT_item *find_first_term(T_NT_stack *s, bool *is_first);
void insert_stopper(T_NT_stack *s);
Data_NTerm *create_non_term(N_T_rules in_rule, N_T_types in_type);

extern Data_Term currentToken;
T_NT_stack *processing_stack;
T_NT_stack *evaluation_stack;

// TODO: to be deleted
// For test, simulate rule usage and shows order
N_T_rules *right_order;

//====SYMTABLE====

/*

 typedef struct {
	symTab_hashTable_t *global;
	symTab_hashTable_t *local;

	//internal
	char *typeStringBuffer;
	size_t typeStringBufferSize;
	size_t typeStringBufferSizeWrittenChars;
} symTab_t;
*/


typedef struct hash_tab_symbol hash_tab_symbol_type;
struct hash_tab_symbol {
	hash_tab_symbol_type *next_symbol;

	//bool is_function;  // false = variable     true = function
	bool is_defined;

	int value_type;  // 0 = integer     1 = float     2 = string

	int value_int;
	float value_float;
	char *value_string;

	char *param_types;
	//int num_parameters;
	//int *type_parameters; // pole ukazatelov na int, budu tam tipy premennych
	char symbol_name[];	    // meno funkcie / premennej

};

typedef struct {
	unsigned table_size;
	hash_tab_symbol_type *list_items[];
} hash_table_type;

hash_table_type *func_table;
hash_table_type *var_table;
hash_tab_symbol_type *tmp_func_item;
hash_tab_symbol_type *tmp_var_item;

hash_table_type *sym_tab_init(unsigned size);
hash_tab_symbol_type *hash_table_insert(hash_table_type *hash_table, char *symbol_name);
hash_tab_symbol_type *hash_table_search(hash_table_type *hash_table, char *entry_key);
void hash_table_destroy(hash_table_type *hash_table);


//====PARSER====
tToken currentToken;
bool func_definition;

int start_parsing();
int start();
int scope();
int end_of_lines();
int var_declr();
int fnc_arg();
int fnc_arglist();
int fnc_stats();
int if_statements();
int statement();

tString params;

//====SCANNER====
