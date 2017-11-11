#ifndef SCANNER_HEADER
#define SCANNER_HEADER

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

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
	BRA_P,// )
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

typedef struct {
	char *content;
	int len;
	int size;
} tBuffer;

FILE* file;

//buffer for identifiers
tBuffer buffer;
#define BUFFERSIZE 32

int esc;

//function declarations
int addchar(char n_char, tBuffer *str);
void delstr(tBuffer *str);
void free_sources();
int start_scanner(char *filename);
int get_token();
int str_init(tBuffer *str);

tToken currentToken;

#endif // SCANNER_HEADER included