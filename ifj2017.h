
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
	char symbol_name[];	
	/*union {
		symTab_functionData_t functionData;
		symTab_variableData_t variableData;
	};*/

};

typedef struct {
	unsigned table_size;
	hash_tab_symbol_type *list_items[];
} hash_table_type;

