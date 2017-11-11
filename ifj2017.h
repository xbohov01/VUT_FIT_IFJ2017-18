/**** ifj2017.h ****/
//Global header file//
//xbohov01
//xgrigo02
//xberes01
//xkosti07

#ifndef IFJ2017_HEADER
#define IFJ2017_HEADER

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

//====SCANNER====
//enumerator for token types
typedef enum {
	//operators begin
	ADD_O,// +
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
	LINE_COMMENT,
	BLOCK_COMMENT,
  UNDEFINED,
	ERROR,
	ENDF,
	EOL,
	TT_RIGHTPAR,
	TT_LEFTPAR,
	TT_COMMA,
	TT_SEMICOLON,
} T_token_type;

//structure for token
typedef struct {
	int value_int;
	double value_double;
	char *value_string;
	T_token_type token_type;
	char *id;
} tToken;


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

hash_table_type *sym_tab_init(unsigned size);
unsigned int hash_function(const char *str, unsigned hashTable_size);
hash_tab_symbol_type *hash_table_insert(hash_table_type *hash_table, char *symbol_name);
hash_tab_symbol_type *hash_table_search(hash_table_type *hash_table, char *entry_key);
void hash_table_destroy(hash_table_type *hash_table);
// ====END SYMTABlE====

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
int statement();
int if_statements();

//====SCANNER====


#endif // IFJ_2017 header
