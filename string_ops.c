//*****string_ops.c******
//xbohov01
//xberes01
//xgrigo02

#include <ifj2017.h>

int addchar(char n_char, tString *str) //funkcia pridava znak do string
{
	if (str->len >= str->size - 1)
	{
		str->content = realloc(str->content, str->len + BUFFERSIZE); // zvacsenie alokovaneho priestoru
		if (str->content == NULL)
		{
			return INTERNAL_ERR;
		}
		str->size = str->size + BUFFERSIZE;
		for (int i = str->len; i < str->size - 1; i++) {
			str->content[i] = 0;
		}
	}

	str->content[str->len++] = n_char;
}

//deletes content of string
void delstr(tString *str)
{
	if (str->len >= 1)
	{
		memset(str->content, 0, str->len);
		str->len = 0;
	}
}

int str_init(tString *str) //funkcia inicializuje tBuffer
{

	size_t init_len = sizeof(char)*BUFFERSIZE;
	str->content = malloc(init_len); //alokovanie pamate
	if (str->content == NULL)
	{
		return INTERNAL_ERR;
	}
	else
	{
		memset(str->content, 0, init_len);
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
