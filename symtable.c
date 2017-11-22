#include "ifj2017.h"

//Inicializuje hashovaciu tabulku, alokuje miesto
//vracia ukazatel na hashovaciu tabulku
//v pirpade chyby NULL


hash_table_type *sym_tab_init(unsigned size)
{

	int hash_table_size = sizeof(hash_table_type) + (sizeof(hash_tab_symbol_type) * size);

	hash_table_type *hash_table = (hash_table_type *)malloc(hash_table_size);

	if (hash_table == NULL)
	{
		fprintf(stderr, "SymbolTable: Failed to malloc hash table\n");
		return NULL;
	}

	hash_table->table_size = size;

	for (unsigned int i = 0; i < (hash_table -> table_size) ; ++i)
	{


		hash_table -> list_items[i] = NULL;
	}


	return hash_table;
}



unsigned int hash_function(const char *str, unsigned hashTable_size) {
	unsigned int h=0;
	const unsigned char *p;
	for(p=(const unsigned char*)str; *p!='\0'; p++)
		h = 65599*h + *p;
	//printf("%d\n", h % hashTable_size);
	return h % hashTable_size;
}

//vlozi dany symbol do hashovacej tabulky
//vrati ukazatel na dany symbol
//v pripade chyby NULL

hash_tab_symbol_type *hash_table_insert(hash_table_type *hash_table, char *symbol_name)
{

    if (hash_table == NULL || symbol_name == NULL)
    {
    	fprintf(stderr,"Hash_table: Invalid symbol.\n");
    	return NULL;
    }

	int lenght_of_name = strlen(symbol_name) + 1;
	hash_tab_symbol_type *new_symbol = (hash_tab_symbol_type *)malloc(sizeof(hash_tab_symbol_type) + lenght_of_name);

	if (new_symbol == NULL) {
		fprintf(stderr, "SymbolTable: Failed to alloc new symbol\n");
		return NULL;
	}


	strcpy(new_symbol->symbol_name, symbol_name);
	//newSymbol->symbolType = st_undefined;
	
	unsigned key = hash_function(symbol_name, hash_table->table_size);
	new_symbol->next_symbol = hash_table->list_items[key];
	hash_table->list_items[key] = new_symbol;


	return new_symbol;

}

// najde hladany symbol
// vrati ukazatel na symbol -- ak nenajde NULL
hash_tab_symbol_type *hash_table_search(hash_table_type *hash_table, char *entry_key)
{
    if (hash_table == NULL || entry_key == NULL)
    {
    	fprintf(stderr,"Hash_table: Invalid key.\n");
    	return NULL;
    }


	unsigned hash_key = hash_function(entry_key, hash_table ->table_size);

	hash_tab_symbol_type *tmp_symbol;

	for (tmp_symbol = hash_table->list_items[hash_key]; tmp_symbol != NULL ; tmp_symbol = tmp_symbol->next_symbol)
	{
		if (tmp_symbol -> symbol_name && (strcmp(tmp_symbol->symbol_name, entry_key) == 0))
		{
			return tmp_symbol;
		}
	}

	return NULL;
}

// dealokuje hashovaciu tabulku
void hash_table_destroy(hash_table_type *hash_table) {
	if (hash_table == NULL)
		return;

		hash_tab_symbol_type *tmp;

	for (unsigned int i = 0; i < (hash_table -> table_size) ; ++i)
	{
		tmp = hash_table -> list_items[i];

		while(tmp != NULL)
		{
			hash_tab_symbol_type *next = tmp -> next_symbol;

			if (tmp->param_types != NULL){
					free(tmp->param_types);
			}

			free(tmp);  // postupne dealokuje cely spojity zoznam

			tmp = next;
		}

		hash_table -> list_items[i] = NULL;
	}

	free(hash_table);

	return;
}


/*
int main(int argc, char const *argv[])
{

hash_table_type *tmp = NULL;
tmp = sym_tab_init(20);



hash_table_insert(tmp, "ahoj");
hash_table_insert(tmp, "if");
hash_table_insert(tmp, "premmene");
hash_table_insert(tmp, "in");
hash_table_insert(tmp, "ko");
hash_table_insert(tmp, "ok");
hash_table_insert(tmp, "ahj");
hash_table_insert(tmp, "ifk");
hash_table_insert(tmp, "prekmmene");
hash_table_insert(tmp, "ink");
hash_table_insert(tmp, "kok");
hash_table_insert(tmp, "okk");
hash_table_insert(tmp, "anhoj");
hash_table_insert(tmp, "inf");
hash_table_insert(tmp, "pnremmene");
hash_table_insert(tmp, "inn");
hash_table_insert(tmp, "kno");
hash_table_insert(tmp, "onk");


printf("fasdfas\n");
hash_tab_symbol_type * num = hash_table_search(tmp, "inf");
if (num != NULL)
	printf("%s", num->next_symbol->symbol_name);


num = hash_table_search(tmp, "inf");
if (num != NULL)
	printf("Nasiel som \n");


hash_table_destroy(tmp);

//free(tmp);
	return 0;
}*/
