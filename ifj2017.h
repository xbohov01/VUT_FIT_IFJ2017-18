
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>




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

