/****parser.c****/
//xbohov01
//xkosti07
//xgrigo02

#include "ifj2017.h"

#ifdef DEBUG
#include "parser_test.h"
#endif

int cond_label = 0;
int while_cnt = 0;
//condition label format
//printf("LABEL $condition%d$end\n", cond_label);

//terminates all resources and program for error handling
void hard_exit(int code){
  fprintf(stderr, "HARD EXIT\n");
  //free resources
  #ifndef DEBUG
  free_sources();
  #endif

  free(currentToken.id);
  free(currentToken.value_string);
  free(params.content);

  if (func_table != NULL){
    hash_table_destroy(func_table);
  }
  if (var_table != NULL){
    hash_table_destroy(var_table);
  }

  //TODO destroy all stacks

  exit(code);
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
    "function",
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

  tmp_var_item->param_types = NULL;

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

    init_var(tmp_var_item->value_type, var_id);

    //free and return
    free(var_id);
    return end_of_lines();
  } else if (currentToken.token_type == EQ_O){
    //free(var_id);
    //evaluate expression
    printf("DEFVAR LF@_%s\n", var_id);
    eval_expr();
    save_result(var_id);
    free(var_id);
  } else {
    fprintf(stderr, "Syntax error: invalid sequence in variable declaration.\n");
    free(var_id);
    hard_exit(SYNT_ERR);
    //return SYNT_ERR;
  }

  return SUCCESS;
}

//<fncarg> -> <id>	as 	<type>
int fnc_arg(int pos){
  //copy identifier for later
  char *var_id = malloc(strlen(currentToken.id)*sizeof(char)+1);
  memcpy(var_id, currentToken.id, strlen(currentToken.id)+1);

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

    tmp_var_item->param_types = NULL;

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

    //tac
    if (func_definition == true){
      printf("DEFVAR LF@_%s\n", var_id);
      printf("MOVE LF@_%s LF@$_arg_%d\n", var_id, pos);
    }

    free(var_id);

    //gets next for condition in fnc_arglist
    get_token();
    return SUCCESS;
  }

  return SUCCESS;
}

//<paramlist> ->	<fncarg>	<paramlist2>
//<paramlist>	-> )
//<paramlist2> ->	,	<fncarg>	<paramlist2>
//<paramlist2> ->	)
int fnc_arglist(){
  static int arg_pos = 0;

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
    if (fnc_arg(arg_pos++) != SUCCESS){
      fprintf(stderr, "Invalid parameter1\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    }
    //get_token();
    //expecting PAR_R or a comma
    if (currentToken.token_type == PAR_R){
      arg_pos = 0;
      return SUCCESS;
    } else if (currentToken.token_type == COM){
      get_token();
      continue;
    } else {
      fprintf(stderr, "Invalid parameter2\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    }
  }

  return SUCCESS;

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
int statement(){
  int if_cnt = cond_label;

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
      eval_expr();
      // do {
      //   get_token();
      // } while(currentToken.token_type != ENDL);
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

      //get next for correct end
      get_token();

      return end_of_lines();

    case PRINT_KEY :

      //get_token();

      eval_expr();
      save_result("stack_temp");

      printf("WRITE GF@_stack_temp\n");

      /*
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
      */
      get_token();
      //expecting ; or ENDL
      while (currentToken.token_type == SEM){
        //get_token();
        eval_expr();
        save_result("stack_temp");

        printf("WRITE GF@_stack_temp\n");
        /*
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
          printf("WRITE int@%d\n", currentToken.value_int);
        } else if (currentToken.token_type == DOUBLE){
          printf("WRITE float@%f\n", currentToken.value_double);
        } else if (currentToken.token_type == STRING){
          printf("WRITE string@%s\n", currentToken.value_string);
        }
        //has thing to print, move to next token
        get_token();*/
      }
      //ENDLs
      return end_of_lines();

    case IF_KEY :

      //has if
      get_token();

      eval_cond_expr(false, cond_label);
      //creates jump to else

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

      printf("JUMP $end$%dif\n", if_cnt);
      //else if block
      if (currentToken.token_type == ELSEIF_KEY){
        while (currentToken.token_type == ELSEIF_KEY){
          //elseif condition
          eval_cond_expr(false, cond_label);

          //get_token();
          //expecting then
          CHECKT(THEN_KEY);
          //checking statements
          if (if_statements() != SUCCESS){
            hard_exit(SYNT_ERR);
            //return SYNT_ERR;
          }
          //end of conditional statements
          printf("LABEL $condition%d$end\n", cond_label);
          cond_label++;
        }
      }
      //else block
      if (currentToken.token_type == ELSE_KEY){
        printf("LABEL $condition%d$end\n", cond_label);
        cond_label++;
        if (if_statements() != SUCCESS){
          hard_exit(SYNT_ERR);
          //return SYNT_ERR;
        }
        //end of conditional statements
        printf("LABEL $condition%d$end\n", cond_label);
        cond_label++;
      }

      printf("LABEL $end$%dif\n", if_cnt);

      //has end
      //check end if
      CHECKT(IF_KEY);
      get_token();

      return SUCCESS;

    case DO_KEY :
      //has do
      get_token();
      //expecting while
      CHECKT(WHILE_KEY);
      get_token();
      printf("LABEL $while%d$label\n", while_cnt);
      eval_cond_expr(true, while_cnt);
      // do {
      //   get_token();
      //   printf("IGNORING EXPR\n");
      // } while(currentToken.token_type != ENDL);

      //expecting ENDL after expr TODO maybe not get next token
      //get_token();
      CHECKT(ENDL);

      //while block
      if (if_statements() != SUCCESS){
        hard_exit(SYNT_ERR);
        //return SYNT_ERR;
      }
      printf("JUMP $while%d$label\n", while_cnt);
      printf("LABEL $end$while%d$label\n", while_cnt);
      while_cnt++;

      //expecting loop
      //removed to fix multiple if stat blocks
      //CHECKT(LOOP_KEY);

      return SUCCESS;

    default :
      fprintf(stderr, "Syntax error in statement\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
  }

  return SUCCESS;

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
int functions(){
  char *identifier;
  int return_type;
  bool definition = true;
  func_definition = true;

  //init variable table
  var_table = sym_tab_init(64);

  //if function is being declared one more NEXTT has to be called
  if (currentToken.token_type == DECLARE_KEY){
    get_token();
    CHECKT(FUNCTION_KEY);
    //is not definition
    definition = false;
    func_definition = false;
  }
  //already got 'function' keyword
  get_token();

  //expecting identifier
  CHECKT(IDENTIFICATOR);
  //save identifier for hash
  identifier= malloc(strlen(currentToken.id)*sizeof(char)+1);
  memcpy(identifier, currentToken.id, strlen(currentToken.id)+1);

  if (definition){
    //tac
    printf("LABEL $_%s\n", identifier);
    printf("PUSHFRAME\n");

    //return value room
    //printf("DEFVAR LF@$_RETVAL\n");
  }

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
  if (tmp_func_item == NULL || (tmp_func_item != NULL && tmp_func_item->is_defined == false)){
    //add entry into functions table
    //tmp_func_item points to new table item
    tmp_func_item = hash_table_insert(func_table, identifier);
    //set return type
    tmp_func_item->value_type = return_type;
    //is defined
    if (definition){
      tmp_func_item->is_defined = true;
    } else {
      tmp_func_item->is_defined = false;
    }
    //set parameter types
    if (params.len != 0){
      tmp_func_item->param_types = malloc(sizeof(char)*params.len+1);
      //TODO add malloc check
      addchar('\0', &params);
      strcpy(tmp_func_item->param_types, params.content);
    } else {
      tmp_func_item->param_types = NULL;
    }
  } else {
    fprintf(stderr, "Syntax error: Function %s is already defined\n", identifier);
    free(identifier);
    hard_exit(UNDEF_ERR);
  }

  //if function is only declared
  //expecting ENDL(s) before next statement
  if (definition == false){
    //destroy variable table
    hash_table_destroy(var_table);
    get_token();
    free(identifier);
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

    free(identifier);

    //tac
    printf("POPFRAME\n");
    printf("RETURN\n\n\n");

    //destroy variable table
    hash_table_destroy(var_table);
    return end_of_lines();
  }

  return SUCCESS;

}

int scope(){
  //int result = SYNT_ERR;
  get_token();
  CHECKT(ENDL);
  get_token();

  //tac
  printf("LABEL $$main\n");
  printf("CREATEFRAME\n");
  printf("PUSHFRAME\n");
  //init global stack
  init_TAC_stack();

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

  return SUCCESS;
}

//<s> -> <funkcie> <scope>
int start(){

  //header
  printf(".IFJcode17\n");
  //jump to main
  printf("JUMP $$main\n");

  //inbuilt function
  printf("LABEL $substring\n");
  printf("PUSHFRAME\n\
DEFVAR LF@int0\n\
DEFVAR LF@bool0\n\
DEFVAR LF@bool1\n\
DEFVAR LF@bool2\n\
DEFVAR LF@bool3\n\
DEFVAR LF@strl\n\
DEFVAR LF@char0\n\
DEFVAR LF@string0\n\
\
STRLEN LF@strl LF@$_arg_0\n\
\
LT LF@bool1 LF@_arg_2 int@1\n\
\
JUMPIFEQ $$konec0 LF@bool1 bool@TRUE\n\
\
EQ LF@bool1 LF@strl int@0\n\
JUMPIFEQ $$konec0 LF@bool1 bool@TRUE\n\
\
LT LF@bool1 LF@_arg_3 int@0\n\
JUMPIFEQ $$konec1 LF@bool1 bool@TRUE \n\
\
LABEL $$cyklus1\n\
EQ LF@bool0 LF@_arg_2 LF@_arg_2\n\
LT LF@bool1 LF@_arg_1 LF@strl\n\
\
OR LF@bool3 LF@bool0 LF@bool1\n\
\
JUMPIFEQ $$cyklus0 LF@bool3 bool@TRUE\n\
\
GETCHAR LF@char0 LF@_arg_0 LF@_arg_2\n\
CONCAT LF@string0 LF@string0 LF@char0\n\
\
MUL LF@vysl LF@vysl LF@a\n\
ADD LF@_arg_2 LF@_arg_2 int@1\n\
\
JUMP $$cyklus1\n\
\
LABEL $$cyklus0\n\
\
LABEL $$konec0\n\n\n");

  int result = 0;

  while (result != SYNT_ERR){
    switch (currentToken.token_type) {
      case FUNCTION_KEY:
        //'function' keyword
      case DECLARE_KEY:
        //'declare' keyword
        result = functions();
        //tac
        //printf("POPFRAME\n");
        //printf("RETURN\n");
        continue;

      case SCOPE_KEY:
        //'scope' keyword
        result = scope();
        //tac
        printf("POPFRAME\n");
        hash_table_destroy(var_table);
        break;
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

  return result;
}

int start_parsing(){

  int result = SYNT_ERR;

  str_init(&params);

  extern T_NT_stack *processing_stack;
  extern T_NT_stack *evaluation_stack;
  processing_stack = NULL; // Init psa_parser stacks to NULL for
  evaluation_stack = NULL; // correct error exit (if needed)

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

  //init symtables
  func_table = sym_tab_init(64);
  //TODO check for correct init

  //start scanner
  start_scanner(argv[1]);

  //add built-in functions to symtable
  tmp_func_item = hash_table_insert(func_table, "length");
  tmp_func_item->param_types = malloc(strlen("s")*sizeof(char)+1);
  strcpy(tmp_func_item->param_types, "s");
  tmp_func_item->value_type = 0;

  tmp_func_item = hash_table_insert(func_table, "substr");
  tmp_func_item->param_types = malloc(strlen("sin")*sizeof(char)+1);
  strcpy(tmp_func_item->param_types, "sin");
  tmp_func_item->value_type = 2;

  tmp_func_item = hash_table_insert(func_table, "asc");
  tmp_func_item->param_types = malloc(strlen("si")*sizeof(char)+1);
  strcpy(tmp_func_item->param_types, "si");
  tmp_func_item->value_type = 0;

  tmp_func_item = hash_table_insert(func_table, "chr");
  tmp_func_item->param_types = malloc(strlen("i")*sizeof(char)+1);
  strcpy(tmp_func_item->param_types, "i");
  tmp_func_item->value_type = 2;

  //start parsing
  result = start_parsing();

  //destroy function table
  hash_table_destroy(func_table);
  //free buffer
  free(buffer.content);

  free(currentToken.id);
  free(currentToken.value_string);

  printf("\n# RESULT %d\n", result);

  return result;

}
