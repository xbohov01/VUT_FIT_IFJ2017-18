/****parser.c****/
//xbohov01
//xkosti07
//xgrigo02

#include "ifj2017.h"

#ifdef DEBUG
#include "parser_test.h"
#endif

//terminates all resources and program for error handling
void hard_exit(int code){
  fprintf(stderr, "HARD EXIT\n");
  //free resources
  #ifndef DEBUG
  free_sources();
  #endif

  if (func_table != NULL){
    hash_table_destroy(func_table);
  }
  if (var_table != NULL){
    hash_table_destroy(var_table);
  }
  /*
  if (file != NULL){
    fclose(file);
  }
*/
  //TODO destroy all stacks

  exit(code);
}

//prints current token
void print_curr_token(){
  //array of all tokens
  const char *tokenList[] = {
    "+",
    "lex err",
    "-",
    "*",
    "/",
    "mod",
    "<",
    ">",
    "<=",
    ">=",
    "=",
    "<>",
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


void synt_error_print(int given, int expected){
  //array of all tokens
  const char *tokenList[] = {
    "+",
    "lex err",
    "-",
    "*",
    "/",
    "mod",
    "<",
    ">",
    "<=",
    ">=",
    "=",
    "<>",
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

  fprintf(stderr, "Syntax Error: %s was expected but %s was given.\n", tokenList[expected], tokenList[given]);
}

//check if token is correct
#define CHECKT(expected){ \
  if (currentToken.token_type != expected){ \
    synt_error_print(currentToken.token_type, expected);  \
    hard_exit(SYNT_ERR); \
  } \
}

int end_of_lines(){
  //get_token();
  //CHECKT(ENDL);
  while (currentToken.token_type == ENDL){
    get_token();
  }
  return SUCCESS;

}

//<deklaracie> ->	<deklaracia>	<endline>	<deklaracie>
//<deklaracia> -> dim	<id>	as	<type>	<varinit>
//<varinit> -> = <expr>
//<varinit> -> epsilon
int var_declr(){
  //already has dim keyword
  get_token();
  //expecting identifier
  CHECKT(IDENTIFICATOR);
  char *var_id = malloc(strlen(currentToken.id)*sizeof(char)+1);
  memcpy(var_id, currentToken.id, strlen(currentToken.id)+1);

  get_token();
  //expecting as keyword
  CHECKT(AS_KEY);
  get_token();
  //expecting type
  if (currentToken.token_type != INTEGER_KEY && currentToken.token_type != DOUBLE_KEY && currentToken.token_type != STRING_KEY){
    fprintf(stderr, "Syntax error: expecting variable type in variable declaration.\n");
    free(var_id);
    hard_exit(SYNT_ERR);
  }


  //check for redeclaration
  //TODO can be declared and defined separately?
  tmp_var_item = hash_table_search(var_table, var_id);
  if (tmp_var_item != NULL){
    fprintf(stderr, "Syntax error: Variable %s previously declared.\n", var_id);
    free(var_id);
    hard_exit(UNDEF_ERR);
  }
  //check if function with same name exists
  if(hash_table_search(func_table, var_id) != NULL){
    fprintf(stderr, "Syntax error: Function with name %s previously declared.\n", var_id);
    free(var_id);
    hard_exit(UNDEF_ERR);
  }

  //insert into variable table
  tmp_var_item = hash_table_insert(var_table, var_id);
  //TODO check for correct insert

  //adds type to params and var_table
  if (currentToken.token_type == INTEGER_KEY){
    addchar('i', &params);
    tmp_var_item->value_type = 0;
    tmp_var_item->value_int = currentToken.value_int;
  } else if (currentToken.token_type == DOUBLE_KEY){
    addchar('d', &params);
    tmp_var_item->value_type = 1;
    tmp_var_item->value_float = currentToken.value_double;
  } else {
    addchar('s', &params);
    tmp_var_item->value_type = 2;
    tmp_var_item->value_string = malloc(strlen(currentToken.value_string)+1);
    memcpy(tmp_var_item->value_string, currentToken.value_string, strlen(currentToken.value_string)+1);
  }

  get_token();
  //expecting ENDL or variable initialization
  if (currentToken.token_type == ENDL){
    //generate TAC
    printf("DEFVAR LF@_%s\n", var_id);

    //free and return
    free(var_id);
    return end_of_lines();
  } else if (currentToken.token_type == EQ_O){
    free(var_id);
    //TODO expr
    //TODO tac
  } else {
    fprintf(stderr, "Syntax error: invalid sequence in variable declaration.\n");
    free(var_id);
    hard_exit(SYNT_ERR);
    //return SYNT_ERR;
  }

}

//<fncarg> -> <id>	as 	<type>
int fnc_arg(){
  //copy identifier for later
  char *var_id = currentToken.id;
  //already got identifier
  get_token();

  //expecting as keyword
  CHECKT(AS_KEY);
  get_token();

  //expecting type
  if (currentToken.token_type != STRING_KEY && currentToken.token_type != INTEGER_KEY && currentToken.token_type != DOUBLE_KEY){
    fprintf(stderr, "Invalid param type\n");
    hard_exit(SYNT_ERR);
    //return SYNT_ERR;
  } else {
    //check for redefinition
    tmp_var_item = hash_table_search(var_table, var_id);
    if (tmp_var_item != NULL){
      fprintf(stderr, "Syntax error: Variable %s previously declared.\n", var_id);
      hard_exit(SYNT_ERR);
    }
    //add variable to var_table
    tmp_var_item = hash_table_insert(var_table, var_id);

    //adds type to params and var_table
    if (currentToken.token_type == INTEGER_KEY){
      addchar('i', &params);
      tmp_var_item->value_type = 0;
    } else if (currentToken.token_type == DOUBLE_KEY){
      addchar('d', &params);
      tmp_var_item->value_type = 1;
    } else {
      addchar('s', &params);
      tmp_var_item->value_type = 2;
    }

    //TODO tac

    //gets next for condition in fnc_arglist
    get_token();
    return SUCCESS;
  }

}

//<paramlist> ->	<fncarg>	<paramlist2>
//<paramlist>	-> )
//<paramlist2> ->	,	<fncarg>	<paramlist2>
//<paramlist2> ->	)
int fnc_arglist(){

  //already has (
  //no arguments
  if (currentToken.token_type == PAR_R){
    return SUCCESS;
  }
  //some arguments
  while (currentToken.token_type != PAR_R){
    //one argument
    //get_token();
    //expecting identifier
    CHECKT(IDENTIFICATOR);
    if (fnc_arg() != SUCCESS){
      fprintf(stderr, "Invalid parameter1\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    }
    //get_token();
    //expecting PAR_R or a comma
    if (currentToken.token_type == PAR_R){
      return SUCCESS;
    } else if (currentToken.token_type == COM){
      continue;
    } else {
      fprintf(stderr, "Invalid parameter2\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    }
  }

}

//<statement> ->	if	<expr>	then	<endline>	<thenstats>	else	<endline>	<elsestats>	end 	if
int if_statements(){
  //last was ENDL so need to get next
  get_token();
  while (currentToken.token_type != END_KEY && currentToken.token_type != ELSE_KEY
        && currentToken.token_type != ELSEIF_KEY && currentToken.token_type != LOOP_KEY){
    if (statement() == SYNT_ERR){
      fprintf(stderr, "Error in conditional statement\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    }

  }
  if (currentToken.token_type == LOOP_KEY || currentToken.token_type == END_KEY){
    get_token();
  }
  return SUCCESS;
}

//<statement>	<expr>
//<statement>	<id>	='	<expr>
//<statement>	input	<id>
//<statement>	print	<id>	;	<print_list>
//<statement>	if	<expr>	then	<endline>	<thenstats>	else	<endline>	<elsestats>	end 	if
//<statement>	do	while	<expr>	<endline>	<teloprogramu>	loop
//<statement>	<id>	='	<id>	(	<args>	)
//<statement>	return	<expr>
//TODO return psa call
//TODO var_table checks
int statement(){
  //expecting one of the above
  switch (currentToken.token_type) {
    case ENDL :
      get_token();
      return SUCCESS;

    case DIM_KEY :
      //is variable declaration
      if (var_declr() != SUCCESS){
        hard_exit(SYNT_ERR);
      }
      return end_of_lines();

    case INTEGER :
    case DOUBLE :
    case STRING :
    case IDENTIFICATOR:
    case RETURN_KEY :
      //resolve expression or function call or return
      //TODO add psa call
      do {
        get_token();
      } while(currentToken.token_type != ENDL);
      return end_of_lines();

    case INPUT_KEY :
      get_token();
      //expecting identifier
      CHECKT(IDENTIFICATOR);

      //check if symbol exists
      tmp_var_item = hash_table_search(var_table, currentToken.id);
      if (tmp_var_item == NULL){
        fprintf(stderr, "Syntax error: Variable %s doesn't exist in this context.\n", currentToken.id);
        hard_exit(UNDEF_ERR);
      }

      char* types[3] = {
        "INT",
        "FLOAT",
        "STRING",
      };
      //tac
      printf("READ LF@_%s %s\n", currentToken.id, types[tmp_var_item->value_type]);

      return end_of_lines();

    case PRINT_KEY :
      get_token();

      //expecting value or identifier
      if (currentToken.token_type != STRING && currentToken.token_type != INTEGER
          && currentToken.token_type != DOUBLE && currentToken.token_type != IDENTIFICATOR){
            fprintf(stderr, "Identifier or expression expected\n");
            hard_exit(SYNT_ERR);
            //return SYNT_ERR;
          }

      //tac
      if (currentToken.token_type == IDENTIFICATOR){
        tmp_var_item = hash_table_search(var_table, currentToken.id);
        if (tmp_var_item == NULL){
          fprintf(stderr, "Syntax error: Variable %s not declared.\n", currentToken.id);
          hard_exit(UNDEF_ERR);
        }
        printf("WRITE LF@_%s\n", currentToken.id);
      } else if (currentToken.token_type == INTEGER){
        printf("WRITE int@%d\n", currentToken.value_int);
      } else if (currentToken.token_type == DOUBLE){
        printf("WRITE float@%f\n", currentToken.value_double);
      } else if (currentToken.token_type == STRING){
        printf("WRITE string@%s\n", currentToken.value_string);
      }


      get_token();
      //expecting ; or ENDL
      while (currentToken.token_type == SEM){
        get_token();
        //expecting value or identifier or EOL
        if (currentToken.token_type != STRING && currentToken.token_type != INTEGER
            && currentToken.token_type != DOUBLE && currentToken.token_type != IDENTIFICATOR){
              if (currentToken.token_type == ENDL){
                return end_of_lines();
              }
              fprintf(stderr, "Identifier or expression expected\n");
              hard_exit(SYNT_ERR);
              //return SYNT_ERR;
            }

        //tac
        if (currentToken.token_type == IDENTIFICATOR){
          tmp_var_item = hash_table_search(var_table, currentToken.id);
          if (tmp_var_item == NULL){
            fprintf(stderr, "Syntax error: Variable %s not declared.\n", currentToken.id);
            hard_exit(UNDEF_ERR);
          }
          printf("WRITE LF@_%s\n", currentToken.id);
        } else if (currentToken.token_type == INTEGER){
          printf("WRTIE int@%d\n", currentToken.value_int);
        } else if (currentToken.token_type == DOUBLE){
          printf("WRTIE float@%f\n", currentToken.value_double);
        } else if (currentToken.token_type == STRING){
          printf("WRTIE string@%s\n", currentToken.value_string);
        }
      }
      //ENDLs
      return end_of_lines();

    case IF_KEY :

      //has if
      get_token();
      //TODO check if expression
      do {
        get_token();
      } while (currentToken.token_type != THEN_KEY);

      //get_token();
      CHECKT(THEN_KEY);
      //expecting end of line
      get_token();
      CHECKT(ENDL);
      //if block
      if (if_statements() != SUCCESS){
        hard_exit(SYNT_ERR);
        //return SYNT_ERR;
      }
      //else if block
      if (currentToken.token_type == ELSEIF_KEY){
        while (currentToken.token_type == ELSEIF_KEY){
          get_token();
          //expecting then
          CHECKT(THEN_KEY);
          //checking statements
          if (if_statements() != SUCCESS){
            hard_exit(SYNT_ERR);
            //return SYNT_ERR;
          }
        }
      }
      //else block
      if (currentToken.token_type == ELSE_KEY){
        if (if_statements() != SUCCESS){
          hard_exit(SYNT_ERR);
          //return SYNT_ERR;
        }
      }

      //check end if
      CHECKT(IF_KEY);
      get_token();

      return SUCCESS;

    case DO_KEY :
      //has do
      get_token();
      //expecting while
      CHECKT(WHILE_KEY);
      //TODO expression eval
      do {
        get_token();
        printf("IGNORING EXPR\n");
      } while(currentToken.token_type != ENDL);

      //expecting ENDL after expr TODO maybe not get next token
      //get_token();
      CHECKT(ENDL);

      //while block
      if (if_statements() != SUCCESS){
        hard_exit(SYNT_ERR);
        //return SYNT_ERR;
      }

      //expecting loop
      //removed to fix multiple if stat blocks
      //CHECKT(LOOP_KEY);

      return SUCCESS;

    default :
      fprintf(stderr, "Syntax error in statement\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
  }

}

//<fncstats>	<statement>	<endline>	<fncstats>
//<fncstats>	epsilon
int fnc_stats(){
  //expecting statements inside a function or end of function
  get_token();
  while (currentToken.token_type != END_KEY){
    if (statement() != SUCCESS){
      fprintf(stderr, "Invalid statement inside function\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    }
  }
  return SUCCESS;
}

//<funkcie>	-> <fnc> <endline> <funkcie>
//<fnc> -> <fncdeclr>
//<fnc> -> <fncdef>
//<fncdeclr>-> function <id> ( <paramlist> ) as	<type> <endline>
//<fncdef>	function	<id>	(	<paramlist>	) 	as	<type>	<endline>	<fncstats>	end	function
//TODO add to symtable
int functions(){
  char *identifier;
  int return_type;
  bool definition = true;

  //if function is being declared one more NEXTT has to be called
  if (currentToken.token_type == DECLARE_KEY){
    get_token();
    CHECKT(FUNCTION_KEY);
    //is not definition
    definition = false;
  }
  //already got 'function' keyword
  get_token();

  //expecting identifier
  CHECKT(IDENTIFICATOR);
  //save identifier for hash
  identifier= malloc(strlen(currentToken.id)*sizeof(char)+1);
  memcpy(identifier, currentToken.id, strlen(currentToken.id)+1);

  //tac
  printf("LABEL %s\n", identifier);
  printf("PUSHFRAME\n");

  get_token();
  //expecting (
  CHECKT(PAR_L);
  get_token();
  //expecting argument declarations or right parenthesis
  //<paramlist>	-> <fncarg>	,	<paramlist>
  //<fncarg> ->	<id>	as 	<type>
  if (currentToken.token_type == PAR_R){
    //expecting ENDL(s) before next statement
    get_token();
    CHECKT(AS_KEY);
    get_token();
    //expecting type
    if (currentToken.token_type != STRING_KEY && currentToken.token_type != INTEGER_KEY && currentToken.token_type != DOUBLE_KEY){
      fprintf(stderr, "Invalid return type\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    }
    //set return type
    if (currentToken.token_type == INTEGER_KEY){
      return_type = 0;
    } else if (currentToken.token_type == DOUBLE_KEY){
      return_type = 1;
    } else {
      return_type = 2;
    }

  } else if (currentToken.token_type == IDENTIFICATOR){
    //init variable table
    var_table = sym_tab_init(64);
    //expecting argument declaration
    if (fnc_arglist() != SUCCESS){
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    } else {
      //checking for return type
      //) as	<type> <endline>
      get_token();
      CHECKT(AS_KEY);
      get_token();
      //expecting type
      if (currentToken.token_type != STRING_KEY && currentToken.token_type != INTEGER_KEY && currentToken.token_type != DOUBLE_KEY){
        fprintf(stderr, "Invalid type\n");
        hard_exit(SYNT_ERR);
        //return SYNT_ERR;
      }

      //set return type
      if (currentToken.token_type == INTEGER_KEY){
        return_type = 0;
      } else if (currentToken.token_type == DOUBLE_KEY){
        return_type = 1;
      } else {
        return_type = 2;
      }

    }
  } else {
    fprintf(stderr, "Syntax error in function declaration/definition. Expecting ')' or variables, %d was given.\n", currentToken.token_type);
    hard_exit(SYNT_ERR);
    //return SYNT_ERR;
  }

  //check if function is in symtable
  tmp_func_item = hash_table_search(func_table, identifier);
  //no entry found
  if (tmp_func_item == NULL){
    //add entry into functions table
    //tmp_func_item points to new table item
    tmp_func_item = hash_table_insert(func_table, identifier);
    //set return type
    tmp_func_item->value_type = return_type;
    //set parameter types
    if (params.len != 0){
      tmp_func_item->param_types = malloc(sizeof(char)*params.len+1);
      //TODO add malloc check
      strcpy(tmp_func_item->param_types, params.content);
    } else {
      tmp_func_item->param_types = NULL;
    }
  } else {
    fprintf(stderr, "Syntax error: Function %s is already defined\n", identifier);
    hard_exit(UNDEF_ERR);
  }

  //if function is only declared
  //expecting ENDL(s) before next statement
  if (definition == false){
    //destroy variable table
    hash_table_destroy(var_table);
    get_token();
    return end_of_lines();
  }

  //if function is also defined
  //expecting ENDL(s) before next statement
  if (end_of_lines() != SUCCESS){
    hard_exit(SYNT_ERR);
    //return SYNT_ERR;
  } else {
    if (fnc_stats() != SUCCESS){
      hard_exit(SYNT_ERR);
    }
    //has end expecting function
    get_token();
    CHECKT(FUNCTION_KEY);
    get_token();
    //destroy variable table
    hash_table_destroy(var_table);
    return end_of_lines();
  }

}

int scope(){
  //int result = SYNT_ERR;
  get_token();
  CHECKT(ENDL);
  get_token();

  //tac
  printf("CREATEFRAME\n");
  printf("PUSHFRAME\n");

  //init var_table
  var_table = sym_tab_init(64);

  while (currentToken.token_type != ENDF){
    switch (currentToken.token_type){
      case DIM_KEY :
        //is variable declaration
        //ready for global variable
        if (var_declr() != SUCCESS){
          hard_exit(SYNT_ERR);
          //return SYNT_ERR;
        }
        //return var_declr();
        continue;

      case END_KEY :
        //expecting end scope
        get_token();
        CHECKT(SCOPE_KEY);

        //expecting EOF
        get_token();
        //could have empty line
        end_of_lines();
        CHECKT(ENDF);

        //hash_table_destroy(var_table);

        return SUCCESS;

      case ENDL :
        get_token();
        continue;

      default :
        //expecting statement
        if (statement() != SUCCESS){
          fprintf(stderr, "Invalid statement\n");
          hard_exit(SYNT_ERR);
          //return SYNT_ERR;
        }
        continue;
    }
    get_token();
  }

}

//<s> -> <funkcie> <scope>
int start(){

  int result = 0;

  while (result != SYNT_ERR){
    switch (currentToken.token_type) {
      case FUNCTION_KEY:
        //'function' keyword
      case DECLARE_KEY:
        //'declare' keyword
        result = functions();
        //tac
        printf("POPFRAME\n");
        printf("RETURN\n");
        continue;

      case SCOPE_KEY:
        //'scope' keyword
        result = scope();
        //tac
        printf("POPFRAME\n");
        hash_table_destroy(var_table);
      case ENDF:
        //<s> -> EOF
        return SUCCESS;
      case ENDL :
        get_token();
        continue;
      default:
        fprintf(stderr, "Syntax error in start\n");
        return result;
    }
    return result;
  }
}

int start_parsing(){

  int result = SYNT_ERR;

  str_init(&params);

  //get first token
  get_token();
  /*if (get_token() != SUCCESS){
    printf("sss\n");
    return LEX_ERR;
  }*/
  //begin parsing
  result = start();

  free(params.content);

  return result;

}

int main(int argc, char *argv[]){
  int result;
  if (argc != 2){
    fprintf(stderr, "Too few arguments.\n");
    return INTERNAL_ERR;
  }

  //init symtables
  func_table = sym_tab_init(64);
  //TODO check for correct init

  //start scanner
  start_scanner(argv[1]);

  //start parsing
  result = start_parsing();

  //close file
  fclose(file);
  //destroy function table
  hash_table_destroy(func_table);
  //free buffer
  free(buffer.content);

  printf("RESULT %d\n", result);

  return result;

}
