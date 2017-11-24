	/****scanner.c****/
//ifj2017
//xberes01
//xbohov01

#include "ifj2017.h"

void free_sources() //funkcia uvolnuje pouzite zdroje
{
	free(buffer.content);
	// if (file != NULL){
	// 		fclose(file);
	// }

}

T_token_type get_key() //funkcia zistuje ci retazec znakov v bufferi je klucove slovo
{
	if (strcmp(buffer.content, "as") == 0)
	{
		return AS_KEY;
	}
	else if (strcmp(buffer.content, "asc") == 0)
	{
		return ASC_KEY;
	}
	else if (strcmp(buffer.content, "declare") == 0)
	{
		return DECLARE_KEY;
	}
	else if (strcmp(buffer.content, "dim") == 0)
	{
		return DIM_KEY;
	}
	else if (strcmp(buffer.content, "do") == 0)
	{
		return DO_KEY;
	}
	else if (strcmp(buffer.content, "double") == 0)
	{
		return DOUBLE_KEY;
	}
	else if (strcmp(buffer.content, "else") == 0)
	{
		return ELSE_KEY;
	}
	else if (strcmp(buffer.content, "end") == 0)
	{
		return END_KEY;
	}
	else if (strcmp(buffer.content, "chr") == 0)
	{
		return CHR_KEY;
	}
	else if (strcmp(buffer.content, "function") == 0)
	{
		return FUNCTION_KEY;
	}
	else if (strcmp(buffer.content, "if") == 0)
	{
		return IF_KEY;
	}
	else if (strcmp(buffer.content, "input") == 0)
	{
		return INPUT_KEY;
	}
	else if (strcmp(buffer.content, "integer") == 0)
	{
		return INTEGER_KEY;
	}
	else if (strcmp(buffer.content, "length") == 0)
	{
		return LENGTH_KEY;
	}
	else if (strcmp(buffer.content, "loop") == 0)
	{
		return LOOP_KEY;
	}
	else if (strcmp(buffer.content, "print") == 0)
	{
		return PRINT_KEY;
	}
	else if (strcmp(buffer.content, "return") == 0)
	{
		return RETURN_KEY;
	}
	else if (strcmp(buffer.content, "scope") == 0)
	{
		return SCOPE_KEY;
	}
	else if (strcmp(buffer.content, "string") == 0)
	{
		return STRING_KEY;
	}
	else if (strcmp(buffer.content, "substr") == 0)
	{
		return SUBSTR_KEY;
	}
	else if (strcmp(buffer.content, "then") == 0)
	{
		return THEN_KEY;
	}
	else if (strcmp(buffer.content, "while") == 0)
	{
		return WHILE_KEY;
	}
	else
	{
		return IDENTIFICATOR; //pokial sa retazec nezhoduje zo ziadnym kv je to id
	}
}

void get_token() //hlavna funkcia sluziaca na ziskanie tokenu
{
	int n_char;
	currentToken.token_type = UNDEFINED;
	T_token_state token_state = BEGIN;
	delstr(&buffer);

	while (currentToken.token_type == UNDEFINED)
	{
		n_char = fgetc(stdin); //nacitanie znaku

		switch (token_state)
		{
			case BEGIN:
			if (n_char == ' ')
			{
				continue;
			}

			//operatory a specialne znaky
			else if (n_char == EOF)
			{
			  currentToken.token_type = ENDF;
			}
			else if (n_char == '\n')
			{
			  currentToken.token_type = ENDL;
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
				token_state = POS_LT;
			}
			else if (n_char == '>')
			{
			  token_state = POS_GT;
			}
			else if (n_char == '=')
			{
			  currentToken.token_type = EQ_O;
			}
			else if (n_char == ')')
			{
			  currentToken.token_type = PAR_R;
			}
			else if (n_char == '(')
			{
			  currentToken.token_type = PAR_L;
			}
			else if (n_char == ',')
			{
			  currentToken.token_type = COM;
			}
			else if (n_char == ';')
			{
			  currentToken.token_type = SEM;
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

			else if (isalpha(n_char) || n_char == '_')
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
					ungetc(n_char, stdin);
					currentToken.token_type = LT_O;
					token_state = BEGIN;
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
					ungetc(n_char, stdin);
					currentToken.token_type = GT_O;
					token_state = BEGIN;
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
				ungetc(n_char, stdin);
				currentToken.token_type = DIV_O;
				token_state = BEGIN;
			}
			break;

			case POS_BL_COMMENT:
			if (n_char == '\'')
			{
				token_state = POS_BL_END_COMMENT;
			}
			break;

			case POS_BL_END_COMMENT:
			if (n_char == '/')
			{
				token_state = BEGIN;
			}
			break;

			case POS_LIN_COMMENT:
			if (n_char == '\n')
			{
				ungetc(n_char, stdin);
				token_state = BEGIN;
			}
			break;

			case POS_BEG_STRING:
			if (n_char == '\"')
			{
				token_state = POS_STRING;
			}
			else
			{
				currentToken.token_type = ERROR;
				hard_exit(LEX_ERR);
			}
			break;

			case POS_STRING:
			if (n_char == '\"')
			{
				currentToken.token_type = STRING;
				currentToken.value_string = realloc(currentToken.value_string, strlen(buffer.content)*sizeof(char)+1);
				memcpy(currentToken.value_string, buffer.content, strlen(buffer.content)+1);
			}
			else if (n_char == '\\')
			{
				addchar(n_char, &buffer);
				//int esc = 0;
				token_state = ESCAPE;
			}
			else if (isspace(n_char)) //prevod bielych znakov v stringu do escape
			{
				addchar('\\', &buffer);
				addchar('0', &buffer);
				char a = n_char / 10 + 48;
				addchar(a, &buffer);
				char b = n_char % 10 + 48;
				addchar(b, &buffer);
			}
			else if (n_char > 31)
			{
				addchar(n_char, &buffer);
			}
			else
			{
				currentToken.token_type = ERROR;
				hard_exit(LEX_ERR);
			}
			break;

			case ESCAPE:
			{
				if (n_char == '"')
				{
					addchar('0', &buffer);
					addchar('3', &buffer);
					addchar('4', &buffer);
					token_state = POS_STRING;
				}
				else if (n_char == 'n')
				{
					addchar('0', &buffer);
					addchar('1', &buffer);
					addchar('0', &buffer);
					token_state = POS_STRING;
				}
				else if (n_char == 't')
				{
					addchar('0', &buffer);
					addchar('0', &buffer);
					addchar('9', &buffer);
					token_state = POS_STRING;
				}
				else if (n_char == '\\')
				{
					addchar('0', &buffer);
					addchar('9', &buffer);
					addchar('2', &buffer);
					token_state = POS_STRING;
				}
				else if (isdigit(n_char))
				{
					ungetc(n_char, stdin);
					for (int i_e = 0; i_e <= 2; i_e++)
					{
						n_char = fgetc(stdin);
						if (isdigit(n_char))
						{
							addchar(n_char, &buffer);
						  esc = (esc * 10) + (n_char - 48);
						}
						else
						{
							currentToken.token_type = ERROR;
							hard_exit(LEX_ERR);
						}
					}
					if ((esc >= 1) && (esc <= 255))
					{
						token_state = POS_STRING;
					}
					else
					{
						currentToken.token_type = ERROR;
						hard_exit(LEX_ERR);
					}
				}
				else
				{
					currentToken.token_type = ERROR;
					hard_exit(LEX_ERR);
				}
			}
			break;

			case POS_INT:
			if (isdigit(n_char))
			{
				addchar(n_char, &buffer);
			}
			else if ((n_char == 'E')||(n_char == 'e'))
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
				ungetc(n_char, stdin);
				currentToken.token_type = INTEGER;
				currentToken.value_int = atoi(buffer.content);
				token_state = BEGIN;
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
				currentToken.token_type = ERROR;
				hard_exit(LEX_ERR);
			}
			break;

			case POS_EXP_S:
			if (isdigit(n_char))
			{
				addchar(n_char, &buffer);
			}
			else
			{
				ungetc(n_char, stdin);
				currentToken.token_type = DOUBLE;
				currentToken.value_double = atoi(buffer.content);
				token_state = BEGIN;
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
				currentToken.token_type = ERROR;
				hard_exit(LEX_ERR);
			}
			break;

			case POS_DOUBLE:
			if (isdigit(n_char))
			{
				addchar(n_char, &buffer);
			}
			else if ((n_char == 'E')||(n_char == 'e'))
			{
				addchar(n_char, &buffer);
				token_state = POS_EXP;
			}
			else
			{
				ungetc(n_char, stdin);
				currentToken.token_type = DOUBLE;
				currentToken.value_double = atoi(buffer.content);
				token_state = BEGIN;
			}
			break;

			case ID_OR_KEY:
			if ((n_char != '_') && (!isalnum(n_char)))
			{
				ungetc(n_char, stdin);
				currentToken.token_type = get_key(buffer.content);
				if (currentToken.token_type == IDENTIFICATOR)
				{
					currentToken.id = realloc(currentToken.id, strlen(buffer.content)*sizeof(char)+1);
					strcpy(currentToken.id, buffer.content);
				}
				token_state = BEGIN;
			}
			else
			{
				n_char = tolower(n_char);
				addchar(n_char, &buffer);
			}
			break;

			default:
				currentToken.token_type = ERROR;
				hard_exit(LEX_ERR);
			break;

		}
	}
}

int start_scanner(char *filename)
{
	// if (filename != NULL)
	// {
	// 	file = fopen(filename, "r");
	// 	if (file == NULL)
	// 	{
	// 		fprintf(stderr, "File %s cannot be opened.\n", filename);
	// 		hard_exit(INTERNAL_ERR);
	// 	}
	// }

	if (str_init(&buffer) != SUCCESS)
	{
		//fclose(file);
		hard_exit(INTERNAL_ERR);
	}

	currentToken.id = malloc(1);
	currentToken.value_string = malloc(1);

	return SUCCESS;
}

/*
int main() //aby bol prekladac spoko, testy zmazane
{}*/
