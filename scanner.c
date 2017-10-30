#include "scanner.h"
#include "errors.h"

int addchar(char n_char, tBuffer *str) //funkcia pridava znak do bufferu
{
	if (str->len+1 > str->size)
	{
		char *b_char = realloc(str->content, str->len + BUFFERSIZE); // zvacsenie alokovaneho priestoru
			if (str->content == NULL)
			{
				return LEX_ERR;
			}
			str->content = b_char;
			str->size = str->size + BUFFERSIZE;
	}

		str->content[str->len] = n_char;
		str->len++;
}


void delstr(tBuffer *str) //funkcia uvolnuje tBuffer
{
	if (str->len >= 1)
	{
		memset(str->content, 0, str->len);
		str->len = 0;
	}
}

int str_init(tBuffer *str) //funkcia inicializuje tBuffer
{

	str->content = malloc(sizeof(char)*BUFFERSIZE); //alokovanie pamate
	if (str->content == NULL)
	{
		return LEX_ERR;
	}
	else
	{
		str->content[0] = '\0';
		str->len = 0;
		str->size = BUFFERSIZE;
	}
	return SUCCESS;
}

void free_sources(FILE *file, tBuffer *str) //funkcia uvolnuje pouzite zdroje
{
	free(str->content);
	fclose(file);
}

T_token_type get_key(char *str) //funkcia zistuje ci retazec znakov v bufferi je klucove slovo
{
	if (strcmp(&(buffer->content), "as") == 0)
	{
		return AS_KEY;
	}
	else if (strcmp(&(buffer->content), "asc") == 0)
	{
		return ASC_KEY;
	}
	else if (strcmp(&(buffer->content), "declare") == 0)
	{
		return DECLARE_KEY;
	}
	else if (strcmp(&(buffer->content), "dim") == 0)
	{
		return DIM_KEY;
	}
	else if (strcmp(&(buffer->content), "do") == 0)
	{
		return DO_KEY;
	}
	else if (strcmp(&(buffer->content), "double") == 0)
	{
		return DOUBLE_KEY;
	}
	else if (strcmp(&(buffer->content), "else") == 0)
	{
		return ELSE_KEY;
	}
	else if (strcmp(&(buffer->content), "end") == 0)
	{
		return END_KEY;
	}
	else if (strcmp(&(buffer->content), "chr") == 0)
	{
		return CHR_KEY;
	}
	else if (strcmp(&(buffer->content), "function") == 0)
	{
		return FUNCTION_KEY;
	}
	else if (strcmp(&(buffer->content), "if") == 0)
	{
		return IF_KEY;
	}
	else if (strcmp(&(buffer->content), "input") == 0)
	{
		return INPUT_KEY;
	}
	else if (strcmp(&(buffer->content), "integer") == 0)
	{
		return INTEGER_KEY;
	}
	else if (strcmp(&(buffer->content), "length") == 0)
	{
		return LENGTH_KEY;
	}
	else if (strcmp(&(buffer->content), "loop") == 0)
	{
		return LOOP_KEY;
	}
	else if (strcmp(&(buffer->content), "print") == 0)
	{
		return PRINT_KEY;
	}
	else if (strcmp(&(buffer->content), "return") == 0)
	{
		return RETURN_KEY;
	}
	else if (strcmp(&(buffer->content), "scope") == 0)
	{
		return SCOPE_KEY;
	}
	else if (strcmp(&(buffer->content), "string") == 0)
	{
		return STRING_KEY;
	}
	else if (strcmp(&(buffer->content), "substr") == 0)
	{
		return SUBSTR_KEY;
	}
	else if (strcmp(&(buffer->content), "then") == 0)
	{
		return THEN_KEY;
	}
	else if (strcmp(&(buffer->content), "while") == 0)
	{
		return WHILE_KEY;
	}
	else
	{
		return IDENTIFICATOR; //pokial sa retazec nezhoduje zo ziadnym kv je to id
	}
}

int get_token() //hlavna funkcia sluziaca na ziskanie tokenu
{
	int row = 0;
	int n_char;
	currentToken.token_type = UNDEFINED;
	T_token_state token_state = BEGIN;

	while (currentToken.token_type == UNDEFINED)
	{
		n_char = fgetc(file); //nacitanie znaku

		if (n_char == '\n')
		row++;

		switch (token_state)
		{
			case BEGIN:
			if (isspace(n_char))
			{
				continue;
			}

			//operatory a specialne znaky
			else if (n_char == EOF)
			{
			  currentToken.token_type = ENDF;
			}
			else if (n_char == '+')
			{
			  currentToken.token_type = ADD_O;
			}
			else if (n_char == '-')
			{
			  currentToken.token_type = SUB_O;
			}
			else if (n_char == '*')
			{
			  currentToken.token_type = MUL_O;
			}
			else if (n_char == '/')
			{
			  token_state = COM_OR_DIV;
			}
			else if (n_char == '\\')
			{
				currentToken.token_type = MOD_O;
			}
			else if (n_char == '<')
			{
			  currentToken.token_type = POS_LT;
			}
			else if (n_char == '>')
			{
			  currentToken.token_type = POS_GT;
			}
			else if (n_char == '=')
			{
			  currentToken.token_type = EQ_O;
			}

			//cislo
			else if (isdigit(n_char))
			{
				delstr(&buffer);
				addchar(n_char, &buffer);
				token_state = POS_INT;
			}


			else if (n_char == '!')
			{
				token_state = POS_BEG_STRING;
			}

			else if (n_char == '\'')
			{
				token_state = POS_LIN_COMMENT;
			}

			else if (isalpha(n_char))
			{
				n_char = tolower(n_char);
				delstr(&buffer);
				addchar(n_char, &buffer);
				token_state = ID_OR_KEY;
			}
			break;

			case POS_LT:
			{
				if (n_char == '=')
				{
					currentToken.token_type = LTE_O;
				}
				else if (n_char == '>')
				{
					currentToken.token_type = NE_O;
				}
				else
				{
					currentToken.token_type = LT_O;
				}
			}
			break;

			case POS_GT:
			{
				if (n_char == '=')
				{
					currentToken.token_type = GTE_O;
				}
				else
				{
					currentToken.token_type = GT_O;
				}
			}
			break;

			case COM_OR_DIV:
			if (n_char == '\'')
			{
				delstr(&buffer);
				token_state = POS_BL_COMMENT;
			}
			else
			{
				currentToken.token_type = DIV_O;
			}
			break;

			case POS_BL_COMMENT:
			if (n_char == '\'')
			{
				token_state = POS_BL_END_COMMENT;
			}
			else
			{
				addchar(n_char, &buffer);
			}
			break;

			case POS_BL_END_COMMENT:
			if (n_char == '/')
			{
				currentToken.token_type = BLOCK_COMMENT;
			}
			else
			{
				addchar(n_char, &buffer);
			}
			break;

			case POS_LIN_COMMENT:
			if (n_char == '\n')
			{
				currentToken.token_type = LINE_COMMENT;
			}
			else
			{
				addchar(n_char, &buffer);
			}

			case POS_BEG_STRING:
			if (n_char == '\"')
			{
				token_state = POS_STRING;
			}
			else
			{
				currentToken.token_type = ERROR;
			}
			break;

			case POS_STRING:
			if (n_char == '\"')
			{
				currentToken.token_type = STRING;
			}
			else if (n_char > 31)
			{
				addchar(n_char, &buffer);
			}
			else
			{
				currentToken.token_type = ERROR;
			}
			break;

			case POS_INT:
			if (isdigit(n_char))
			{
				addchar(n_char, &buffer);
			}
			if ((n_char == 'E')||(n_char == 'e'))
			{
				addchar(n_char, &buffer);
				token_state = POS_EXP;
			}
			else if (n_char == '.')
			{
				addchar(n_char, &buffer);
				token_state = POS_FL;
			}
			else
			{
				ungetc(n_char, file);
				if (n_char == '\n')
				{
					row--;
				}
				currentToken.token_type = INTEGER;
				currentToken.value_int = atoi(&buffer->content);
			}
			break;

			case POS_EXP:
			if (isdigit(n_char)||((n_char == '+')||(n_char == '-')))
			{
				addchar(n_char, &buffer);
				token_state = POS_EXP_S;
			}
			else
			{
				ungetc(n_char, file);
				if (n_char == '\n')
				{
					row--;
				}
				currentToken.token_type = ERROR;
			}
			break;

			case POS_EXP_S:
			if (isdigit(n_char))
			{
				addchar(n_char, &buffer);
			}
			else
			{
				ungetc(n_char, file);
				if (n_char == '\n')
				{
					row--;
				}
				currentToken.token_type = DOUBLE;
				currentToken.value_double = atoi(&buffer->content);
			}
			break;

			case POS_FL:
			if (isdigit(n_char))
			{
				addchar(n_char, &buffer);
				token_state = POS_DOUBLE;
			}
			else
			{
				ungetc(n_char, file);
				if (n_char == '\n')
				{
					row--;
				}
				currentToken.token_type = ERROR;
			}
			break;

			case POS_DOUBLE:
			if (isdigit(n_char))
			{
				addchar(n_char, &buffer);
			}
			if ((n_char == 'E')||(n_char == 'e'))
			{
				addchar(n_char, &buffer);
				token_state = POS_EXP;
			}
			else
			{
				ungetc(n_char, file);
				if (n_char == '\n')
				{
					row--;
				}
				currentToken.token_type = DOUBLE;
				currentToken.value_double = atoi(&buffer->content);
			}
			break;

			case ID_OR_KEY:
			if ((n_char != '_') && (!isalnum(n_char)))
			{
				ungetc(n_char, file);
				if (n_char == '\n')
				{
					row--;
				}
				currentToken.token_type = get_key(&buffer->content);
				if (currentToken.token_type == IDENTIFICATOR)
				{
					currentToken.id = &buffer->content;
				}
			}
			else
			{
				n_char = tolower(n_char);
				addchar(n_char, &buffer);
			}
			break;

		}
	}
}

int start_scanner(char *filename)
{
	if (filename != NULL)
	{
		input = fopen(filename, "r");
		if (input == NULL)
		{
			fprintf(stderr, "File %s cannot be opened.\n", filename);
			return LEX_ERR;
		}
	}

	if (str_init(buffer) != SUCCESS)
	{
		fclose(file);
		return LEX_ERR;
	}
}

int main(){} //aby bol prekladac spoko
