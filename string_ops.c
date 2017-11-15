//*****string_ops.c******
//xbohov01
//xberes01

#include "ifj2017.h"
#include "errors.h"

int addchar(char n_char, tString *str) //funkcia pridava znak do string
{
	if (str->len+1 > str->size)
	{
		char *b_char = realloc(str->content, str->len + BUFFERSIZE); // zvacsenie alokovaneho priestoru
			if (str->content == NULL)
			{
				return INTERNAL_ERR;
			}
			str->content = b_char;
			str->size = str->size + BUFFERSIZE;
	}

		str->content[str->len] = n_char;
		str->len++;
}

//deletes content of string
void delstr(tString *str)
{
	if (str->len >= 1)
	{
		memset(str->content, 0, 0);
		str->len = 0;
	}
}

int str_init(tString *str) //funkcia inicializuje tBuffer
{

	str->content = malloc(sizeof(char)*BUFFERSIZE); //alokovanie pamate
	if (str->content == NULL)
	{
		return INTERNAL_ERR;
	}
	else
	{
		str->content[0] = '\0';
		str->len = 0;
		str->size = BUFFERSIZE;
	}
	return SUCCESS;
}

void free_string(tString *str){
  free(str->content);
  str->len = 0;
  str->size = 0;
}
