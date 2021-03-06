/**** ifj2017.h ****/
//Prekladac jazyka IFJ2017
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
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "errors.h"
#include <time.h>


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
void free_string(tString *str);

//====SCANNER====
#define N_KEYWORDS 35
#define N_OPERATOR_CHARS 8
#define N_OPERATORS 12
#define N_DELIMITERS 7

//enumerator for token types
typedef enum {
	//operators begin
	ADD_O = 0,// +
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
	DECLARE_KEY,
	DIM_KEY,
	DO_KEY,
	DOUBLE_KEY,
	ELSE_KEY,
	END_KEY,
	FUNCTION_KEY,
	INPUT_KEY,
	INTEGER_KEY,
	LOOP_KEY,
	PRINT_KEY,
	RETURN_KEY,
	SCOPE_KEY,
	STRING_KEY,
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
	ENDF,
	ERR,

	FUNCTION

} T_token_type;

typedef enum {
	BEGIN = 0,

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

// Timeout
struct timeval timer;
fd_set rfds;

//buffer for identifiers
tString buffer;
#define BUFFERSIZE 32

int esc;

//function declarations
void free_sources();
int start_scanner();
void get_token();
char fgetc_extended();

//====PSA_STACK====


typedef enum {
    INTEGER_NT = 0,
    DOUBLE_NT,
    STRING_NT,
    STOPPER
} N_T_types;

typedef enum psa_term_type {
    ADD = 0,
    MUL,
    SUB,
    DIV,
    IDIV,

    // Relational operators
    LT,
    GT,
    LTE,
    GTE,
    EQ,
    NEQ,

    PL,
    PR,
    ID,
    FNC,
    CM,

    // End operator
    END

} PSA_Term_type;


// Extended stack of terminals and non terminals
typedef struct t_nt_stack {
    struct t_nt_item *top;
    struct t_nt_item *active;
    struct t_nt_item *popped;
} T_NT_stack;

// Style conversion of tToken structure
typedef tToken Data_Term;

// Data types of stack items
typedef union t_nt_data{
    N_T_types NT_type;
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
N_T_types map_NT_type(T_token_type in_token_type);
N_T_types map_arg_type(char arg_type);

T_NT_stack *init_T_NT_stack();
void clear_stack(T_NT_stack *s);
void destroy_T_NT_stack(T_NT_stack *s);
T_NT_item *push_T(T_NT_stack *s, Data_Term *in_term);
T_NT_item *push_NT(T_NT_stack *s, T_NT_item *in_non_term);
T_NT_item *pop_T_NT(T_NT_stack *s);

// Extended stack operations
T_NT_item *set_first_T_NT(T_NT_stack *s);
T_NT_item *set_next_T_NT(T_NT_stack *s);
bool active_T_NT(T_NT_stack *s);
T_NT_item* insert_after_NT(T_NT_stack *s, T_NT_item *in_non_term);

// =========PSA_STACK_TESTS===========
void ps(T_NT_stack *T_NT_s); // Print stack for debug
void print_item(T_NT_item *itm);

//====SYMTABLE====


typedef struct hash_tab_symbol hash_tab_symbol_type;
struct hash_tab_symbol {
	hash_tab_symbol_type *next_symbol;

	bool is_defined;

	int value_type;  // 0 = integer     1 = float     2 = string

	char *param_types;
	char symbol_name[];	    // jmeno funkcie / premennej

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

//====PSA===
void eval_expr();
void eval_return_expr(hash_tab_symbol_type *func);
void eval_cond_expr(bool is_do_while, int context, int end_block);
void psa_operation(bool allow_bool);
void reduce_by_rule();
Data_Term *get_item_before_stopper();
void check_psa_completion();

T_NT_item *id_R(hash_tab_symbol_type *var);
T_NT_item *function_R(hash_tab_symbol_type *func);
T_NT_item *arithm_R();
T_NT_item *parenthesis_R();

void push_start_term(T_NT_stack *s);
T_NT_item *find_first_term(T_NT_stack *s, bool *is_first);
void insert_stopper(T_NT_stack *s);
T_NT_item *create_non_term(N_T_types in_type);
void control_result_type_conform(int val_type);

T_NT_stack *processing_stack;
T_NT_stack *evaluation_stack;

// =======PSA_TAC========

// Init commands
void init_TAC_stack();
void create_frame();
void init_var(N_T_types type, char* name);
void start_program();
void end_scope();

// Answer handlers
void save_result(char *res_name);
void save_to_temp();
void save_to_temp();

// Number arithmetics
void push_var_id(char *name);
void push_const_id(Data_Term *item);
void retype_stack(bool second_operand, bool int2fl);
void retype_to_even_int();
void arithm_stack(PSA_Term_type op);

// String arithmetics
void str_arithm(PSA_Term_type op);

// Print operations
void write_space();
void write_output();

// Jumps or function calls
void cond_jump(bool is_while, int context, int next);
void push_arg(int arg_num);
void return_f(char *name);
void f_call(char *name);
void define_built_in_func();
bool *add_build_in(char which, bool only_return);

//====PARSER====
tToken currentToken;
bool func_definition;

int start_parsing();
int start();
int scope();
int functions();
int end_of_lines();
int var_declr();
int fnc_arg(int pos);
int fnc_arglist();
int fnc_stats(hash_tab_symbol_type *func);
int if_statements(hash_tab_symbol_type *func);
int statement(hash_tab_symbol_type *func); // func is not NULL only when
										   // is inside of function definition

tString params;

//====SCANNER====
