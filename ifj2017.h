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
	BRA_R,// )
	BRA_L,// (
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
int get_token();

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

	bool is_function;  // false = variable     true = function

	int value_type;  // 0 = integer     1 = float     2 = string

	int integer_t;
	float float_t;
	char *char_t;

	int num_parameters;
	int *type_parameters; // pole ukazatelov na int, budu tam tipy premennych
	char symbol_name[];	    // meno funkcie / premennej

};

typedef struct {
	unsigned table_size;
	hash_tab_symbol_type *list_items[];
} hash_table_type;

//====PARSER====
tToken currentToken;

int start_parsing();
int start();
int scope();
int end_of_lines();
int var_declr();
int fnc_arg();
int fnc_arglist();
int fnc_stats();
int if_statements();
int statements();

//====SCANNER====
