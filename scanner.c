#include "errors.h"
#include "ifj2017.h"

void free_sources() //funkcia uvolnuje pouzite zdroje
{
	free(buffer.content);
	fclose(file);
}

T_token_type get_key(char *str) //funkcia zistuje ci retazec znakov v bufferi je klucove slovo
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

int get_token() //hlavna funkcia sluziaca na ziskanie tokenu
{
	int n_char;
	currentToken.token_type = UNDEFINED;
	T_token_state token_state = BEGIN;

	while (currentToken.token_type == UNDEFINED)
	{
		n_char = fgetc(file); //nacitanie znaku

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
			//pridane
			else if (n_char == '}')
			{
			  currentToken.token_type = BRA_R;
			}
			else if (n_char == '{')
			{
			  currentToken.token_type = BRA_L;
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
				//ungetc
				currentToken.token_type = DIV_O;
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
				ungetc(n_char, file);
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
			}
			break;

			case POS_STRING:
			if (n_char == '\"')
			{
				currentToken.token_type = STRING;
				currentToken.value_string = buffer.content;
			}
			else if (n_char > 31)
			{
				addchar(n_char, &buffer);
			}
			else if (n_char == '\\')
			{
				int esc = 0;
				token_state = ESCAPE;
			}
			else
			{
				currentToken.token_type = ERROR;
			}
			break;

			case ESCAPE:
			{
				if (n_char == '"')
				{
					addchar('"', &buffer);
					token_state = POS_STRING;
				}
				else if (n_char == 'n')
				{
					addchar('\n', &buffer);
					token_state = POS_STRING;
				}
				else if (n_char == 't')
				{
					addchar('\t', &buffer);
					token_state = POS_STRING;
				}
				else if (n_char == '\\')
				{
					addchar('\\', &buffer);
					token_state = POS_STRING;
				}
				else if (isdigit(n_char))
				{
					int i_e = 0;
					while (i_e < 3)
					{
						if (isdigit(n_char))
						{
							n_char = fgetc(file);
						 esc = (esc * 10) + n_char;
						}
						else
						{
							currentToken.token_type = ERROR;
						}
						i_e++;
					}
					if ((esc >= 1) && (esc <= 255))
					{
						addchar(esc, &buffer);
						token_state = POS_STRING;
					}
					else
					{
						currentToken.token_type = ERROR;
					}
				}
				else
				{
					currentToken.token_type = ERROR;
				}
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
				ungetc(n_char, file);
				currentToken.token_type = ERROR;
				token_state = BEGIN;
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
				ungetc(n_char, file);
				currentToken.token_type = ERROR;
				token_state = BEGIN;
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
				currentToken.token_type = DOUBLE;
				currentToken.value_double = atoi(buffer.content);
				token_state = BEGIN;
			}
			break;

			case ID_OR_KEY:
			if ((n_char != '_') && (!isalnum(n_char)))
			{
				ungetc(n_char, file);
				currentToken.token_type = get_key(buffer.content);
				if (currentToken.token_type == IDENTIFICATOR)
				{
					currentToken.id = buffer.content;
				}
				token_state = BEGIN;
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
		file = fopen(filename, "r");
		if (file == NULL)
		{
			fprintf(stderr, "File %s cannot be opened.\n", filename);
			return LEX_ERR;
		}
	}

	if (str_init(&buffer) != SUCCESS)
	{
		fclose(file);
		return LEX_ERR;
	}
}

//prints current token - testing only
void print_curr_token(){
  //array of all tokens
  const char *tokenList[] = {
    "+",
		"err",
    "-",
    "*",
    "/",
    "modulo",
    "<",
    ">",
    "<=",
    ">=",
    "=",
    "<>",
		"}",
		"{",
		")",
		"(",
		",",
		";",
    "as",
    "asc",
    "declare",
    "dim",
    "do",
    "double",
    "else",
    "end",
    "chr",
    "function",
    "input",
    "integer",
    "length",
    "loop",
    "print",
    "return",
    "scope",
    "string",
    "substr",
    "then",
    "while",
    "and",
    "boolean",
    "continue",
    "elseif",
    "exit",
    "false",
    "for",
    "next",
    "not",
    "or",
    "shared",
    "static",
    "true",
    "if",
    "double_val",
    "integer_val",
    "string_val",
    "identifier",
    "UNDEFINED",
		"ERROR",
		"end of line",
    "end of file",
  };
  //token to print
  char *printToken;
  printf("Current token -> %s\n", tokenList[currentToken.token_type]);
}

// int main() //aby bol prekladac spoko a tiez na testovanie
// {
// 	int i = 0;
// 	char inputf1[] = "test2.txt";
// 	T_token_type test_tokens1[] = {
// 	//test1:
// 	/*ENDL, SCOPE_KEY, ENDL, DIM_KEY, IDENTIFICATOR, AS_KEY, INTEGER_KEY, ENDL, DIM_KEY, IDENTIFICATOR, AS_KEY, INTEGER_KEY, ENDL,
// 	PRINT_KEY, STRING, SEM, ENDL, INPUT_KEY, IDENTIFICATOR, ENDL, IF_KEY, IDENTIFICATOR, LT_O, INTEGER, THEN_KEY, ENDL, PRINT_KEY, STRING, SEM, ENDL, ELSE_KEY, ENDL,
// 	IDENTIFICATOR, EQ_O, INTEGER, ENDL, DO_KEY, WHILE_KEY, IDENTIFICATOR, GT_O, INTEGER, ENDL, IDENTIFICATOR, EQ_O, IDENTIFICATOR, MUL_O, IDENTIFICATOR, ENDL,
// 	IDENTIFICATOR, EQ_O, IDENTIFICATOR, SUB_O, INTEGER, ENDL, LOOP_KEY, ENDL, PRINT_KEY, STRING, SEM, IDENTIFICATOR, SEM, STRING, SEM, ENDL, END_KEY, IF_KEY, ENDL, END_KEY, SCOPE_KEY, ENDL, ENDF*/
// 	//test2:
// 	DECLARE_KEY, FUNCTION_KEY, IDENTIFICATOR, PAR_L, IDENTIFICATOR, AS_KEY, INTEGER_KEY, PAR_R, AS_KEY, INTEGER_KEY, ENDL,
// 	FUNCTION_KEY, IDENTIFICATOR, PAR_L, IDENTIFICATOR, AS_KEY, INTEGER_KEY, PAR_R, AS_KEY, INTEGER_KEY, ENDL,
// 		DIM_KEY, IDENTIFICATOR, AS_KEY, INTEGER_KEY, ENDL,
// 		DIM_KEY, IDENTIFICATOR, AS_KEY, INTEGER_KEY, ENDL,
// 		DIM_KEY, IDENTIFICATOR, AS_KEY, INTEGER_KEY, ENDL,
// 		IF_KEY, IDENTIFICATOR, LT_O, INTEGER, THEN_KEY, ENDL,
// 			IDENTIFICATOR, EQ_O, INTEGER, ENDL,
// 		ELSE_KEY, ENDL,
// 			IDENTIFICATOR, EQ_O, IDENTIFICATOR, SUB_O, INTEGER, ENDL,
// 			IDENTIFICATOR, EQ_O, IDENTIFICATOR, PAR_L, IDENTIFICATOR, PAR_R, ENDL,
// 			IDENTIFICATOR, EQ_O, IDENTIFICATOR, MUL_O, IDENTIFICATOR, ENDL,
// 		END_KEY, IF_KEY, ENDL,
// 		RETURN_KEY, IDENTIFICATOR, ENDL,
// 	END_KEY, FUNCTION_KEY, ENDL,
// 	SCOPE_KEY, ENDL,
// 		DIM_KEY, IDENTIFICATOR, AS_KEY, INTEGER_KEY, ENDL,
// 		DIM_KEY, IDENTIFICATOR, AS_KEY, INTEGER_KEY, ENDL,
// 		PRINT_KEY, STRING, SEM, ENDL,
// 		INPUT_KEY, IDENTIFICATOR, ENDL,
// 		IF_KEY, IDENTIFICATOR, LT_O, INTEGER, THEN_KEY, ENDL,
// 			PRINT_KEY, STRING, SEM, ENDL,
// 		ELSE_KEY, ENDL,
// 			IDENTIFICATOR, EQ_O, IDENTIFICATOR, PAR_L, IDENTIFICATOR, PAR_R, ENDL,
// 			PRINT_KEY, STRING, SEM, IDENTIFICATOR, SEM, STRING, SEM, ENDL,
// 		END_KEY, IF_KEY, ENDL,
// 	END_KEY, SCOPE_KEY, ENDL, ENDF
// 	};

// 	start_scanner("test2.txt");
// 	while(currentToken.token_type != ENDF)
// 	{
// 		if (get_token() == LEX_ERR){
// 			printf("LEX ERR %s\n", buffer);
// 		}
// 		else if(currentToken.token_type == test_tokens1[i])
// 		{
// 			printf("OK %d ", i);
// 			printf("%s ", buffer);
// 		}
// 		else
// 		{
// 			printf("WRONG %d ",i);
// 			printf("%s ", buffer);
// 		}
// 		print_curr_token();
// 		i++;
// 	}
// 	free_sources();
	// tString test_str;
	// char *bad_var = NULL;
	// str_init(&test_str);

	// char *test = "-------|-------|-------|-------|-------|test-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|";
	// for (int i = 0; test[i] != '\0'; i++) {
	// 	addchar(test[i], &test_str);
	// }
	// printf("%s\n", test_str);
	// make_str_copy(test_str.content, &bad_var);
	// printf("STR: %s\n", bad_var);
	// make_str_copy(test_str.content, &bad_var);

	// free_string(&test_str);
	// free(bad_var);

	// return 0;
// }
