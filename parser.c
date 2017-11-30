/****parser.c****/
//xbohov01
//xkosti07
//xgrigo02

#include "ifj2017.h"

#ifdef DEBUG
#include "parser_test.h"
#endif

int while_cnt = 0;
bool returned = false;
//condition label format

//terminates all resources and program for error handling
void error_exit(int code){
  //free resources
  free_sources();

  if (processing_stack != NULL) {
    destroy_T_NT_stack(processing_stack);
  }
  
  free(currentToken.id);
  free(currentToken.value_string);
  free(params.content);

  if (func_table != NULL){
    hash_table_destroy(func_table);
  }
  if (var_table != NULL){
    hash_table_destroy(var_table);
  }

  exit(code);
}

void synt_error_print(int given, int expected){
  //array of all tokens
  const char *tokenList[] = {
    "+",
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
    error_exit(SYNT_ERR); \
  } \
}

int end_of_lines(){
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
    error_exit(SYNT_ERR);
  }

  //check for redeclaration
  tmp_var_item = hash_table_search(var_table, var_id);
  if (tmp_var_item != NULL){
    fprintf(stderr, "Syntax error: Variable %s previously declared.\n", var_id);
    free(var_id);
    error_exit(UNDEF_ERR);
  }
  //check if function with same name exists
  if(hash_table_search(func_table, var_id) != NULL){
    fprintf(stderr, "Syntax error: Function with name %s previously declared.\n", var_id);
    free(var_id);
    error_exit(UNDEF_ERR);
  }

  //insert into variable table
  tmp_var_item = hash_table_insert(var_table, var_id);
  if (tmp_var_item == NULL) {
    fprintf(stderr, "Inser hash table failed\n");
    error_exit(INTERNAL_ERR);
  }
  
  tmp_var_item->param_types = NULL;

  //adds type to params and var_table
  if (currentToken.token_type == INTEGER_KEY){
    tmp_var_item->value_type = 0;
  } else if (currentToken.token_type == DOUBLE_KEY){
    tmp_var_item->value_type = 1;
  } else {
    tmp_var_item->value_type = 2;
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
    //evaluate expression
    printf("DEFVAR LF@_%s\n", var_id);
    eval_expr();
    control_result_type_conform(tmp_var_item->value_type); // For correct result variable retyping
    check_psa_completion(); // Check that processing stack has only one non term on top and free
    save_result(var_id);
    free(var_id);
  } else {
    fprintf(stderr, "Syntax error: invalid sequence in variable declaration.\n");
    free(var_id);
    error_exit(SYNT_ERR);
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
    error_exit(SYNT_ERR);
    //return SYNT_ERR;
  } else {
    //check for redefinition
    tmp_var_item = hash_table_search(var_table, var_id);
    if (tmp_var_item != NULL){
      fprintf(stderr, "Syntax error: Variable %s was previously declared.\n", var_id);
      error_exit(UNDEF_ERR);
    }
    //check for parameter and function name collisions
    tmp_func_item = hash_table_search(func_table, var_id);
    if (tmp_func_item != NULL){
      fprintf(stderr, "Syntax error: Function %s was previously declared.\n", var_id);
      error_exit(UNDEF_ERR);
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
    //expecting identifier
    CHECKT(IDENTIFICATOR);
    if (fnc_arg(arg_pos++) != SUCCESS){
      fprintf(stderr, "Invalid parameter1\n");
      error_exit(SYNT_ERR);
      //return SYNT_ERR;
    }
    //expecting PAR_R or a comma
    if (currentToken.token_type == PAR_R){
      arg_pos = 0;
      return SUCCESS;
    } else if (currentToken.token_type == COM){
      get_token();
      continue;
    } else {
      fprintf(stderr, "Invalid parameter2\n");
      error_exit(SYNT_ERR);
    }
  }

  return SUCCESS;

}

//<statement> ->	if	<expr>	then	<endline>	<thenstats>	else	<endline>	<elsestats>	end 	if
int if_statements(hash_tab_symbol_type *tmp_func_item){
  // Function should return something in all cases
  returned = false;
  //last was ENDL so need to get next
  get_token();
  while (currentToken.token_type != END_KEY && currentToken.token_type != ELSE_KEY
        && currentToken.token_type != ELSEIF_KEY && currentToken.token_type != LOOP_KEY){
    if (statement(tmp_func_item) == SYNT_ERR){
      fprintf(stderr, "Error in conditional statement\n");
      error_exit(SYNT_ERR);
    }

  }
  if (currentToken.token_type == LOOP_KEY || currentToken.token_type == END_KEY){
    get_token();
  }
  return SUCCESS;
}

//<statement>	<id>	='	<expr>
//<statement>	input	<id>
//<statement>	print	<id>	;	<print_list>
//<statement>	if	<expr>	then	<endline>	<thenstats>	else	<endline>	<elsestats>	end 	if
//<statement>	do	while	<expr>	<endline>	<teloprogramu>	loop
//<statement>	<id>	='	<id>	(	<args>	)
//<statement>	return	<expr>
int statement(hash_tab_symbol_type *tmp_func_item){
  static int if_context = 0;
  static int while_context = 0;
  int while_cnt;
  int if_cond_end;
  int if_cond_next;

  //expecting one of the above
  switch (currentToken.token_type) {
    case ENDL :
      get_token();
      return SUCCESS;

    case DIM_KEY :
      //is variable declaration
      if (var_declr() != SUCCESS){
        error_exit(SYNT_ERR);
      }
      return end_of_lines();

    case IDENTIFICATOR:
      eval_expr();
      return end_of_lines();
      break;
    case RETURN_KEY :
      //implicit return handle
      returned = true;
      //resolve expression or function call or return
      eval_return_expr(tmp_func_item);
      return end_of_lines();
      break;
    case INPUT_KEY :
      get_token();
      //expecting identifier
      CHECKT(IDENTIFICATOR);

      //print the !"? "
      printf("WRITE string@?\\032\n");

      //check if symbol exists
      tmp_var_item = hash_table_search(var_table, currentToken.id);
      if (tmp_var_item == NULL){
        fprintf(stderr, "Syntax error: Variable %s doesn't exist in this context.\n", currentToken.id);
        error_exit(UNDEF_ERR);
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

      eval_expr(); // WRITE to output is now included

      //expecting ;
      while (currentToken.token_type == SEM){
        // get_token();

        eval_expr(); // WRITE to output is now included
      }
      //expecting ENDLs
      return end_of_lines();

    case IF_KEY :
      if_cond_end = if_context;
      if_context++;
      if_cond_next = 1;

      get_token();
      //creates jump to next condition inside of one if group
      eval_cond_expr(false, if_cond_end, if_cond_next);

      CHECKT(THEN_KEY);
      //expecting end of line
      get_token();
      CHECKT(ENDL);
      //if block
      if (if_statements(tmp_func_item) != SUCCESS){
        error_exit(SYNT_ERR);
      }

      printf("JUMP $if_end_%d\n", if_cond_end);
      printf("LABEL $if_%d_%d\n", if_cond_end, if_cond_next++);

      // else if block
      if (currentToken.token_type == ELSEIF_KEY){
        while (currentToken.token_type == ELSEIF_KEY) {

          get_token();
          //creates jump to next condition inside of one if group
          eval_cond_expr(false, if_cond_end, if_cond_next);

          CHECKT(THEN_KEY);
          get_token();
          CHECKT(ENDL); 
          //if block
          if (if_statements(tmp_func_item) != SUCCESS){
            error_exit(SYNT_ERR);
          }
          printf("JUMP $if_end_%d\n", if_cond_end);
          printf("LABEL $if_%d_%d\n", if_cond_end, if_cond_next++);
        }
      }

      //else block
      if (currentToken.token_type == ELSE_KEY){
        if (if_statements(tmp_func_item) != SUCCESS){
          error_exit(SYNT_ERR);
        }
      }
      else {
        if (returned == true) {
          returned = false;
        }
      }

      //has end
      //check end if
      CHECKT(IF_KEY);
      get_token();

      printf("LABEL $if_end_%d\n", if_cond_end);

      return SUCCESS;

    case DO_KEY :
      //has do
      get_token();
      //expecting while
      CHECKT(WHILE_KEY);
      get_token();

      while_cnt = while_context++;
      printf("LABEL $repeat_while_%d\n", while_cnt);
      eval_cond_expr(true, 0, while_cnt);

      //expecting ENDL after expr
      CHECKT(ENDL);

      //while block
      if (if_statements(tmp_func_item) != SUCCESS){
        error_exit(SYNT_ERR);
        //return SYNT_ERR;
      }
      printf("JUMP $repeat_while_%d\n", while_cnt);
      printf("LABEL $end_while_%d\n", while_cnt);
      while_cnt++;

      return SUCCESS;

    default :
      fprintf(stderr, "Syntax error in statement\n");
      error_exit(SYNT_ERR);
  }

  return SUCCESS;

}

//<fncstats>	<statement>	<endline>	<fncstats>
//<fncstats>	epsilon
int fnc_stats(hash_tab_symbol_type *tmp_func_item){
  //expecting statements inside a function or end of function
  get_token();
  while (currentToken.token_type != END_KEY){
    if (statement(tmp_func_item) != SUCCESS){
      fprintf(stderr, "Invalid statement inside function\n");
      error_exit(SYNT_ERR);
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

  //necessary to check if implicit return is needed
  returned = false;

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
      error_exit(SYNT_ERR);
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
      error_exit(SYNT_ERR);
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
        error_exit(SYNT_ERR);
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
    error_exit(SYNT_ERR);
  }

  //check if function is in symtable
  tmp_func_item = hash_table_search(func_table, identifier);

  if (tmp_func_item == NULL){
    tmp_func_item = hash_table_insert(func_table, identifier);
    tmp_func_item->value_type = return_type;
    if (definition){
      tmp_func_item->is_defined = true;
    } else {
      tmp_func_item->is_defined = false;
    }
    if (params.len != 0){
      tmp_func_item->param_types = malloc(sizeof(char)*params.len+1);
      if (tmp_func_item->param_types == NULL) {
	error_exit(INTERNAL_ERR);
      }
      addchar('\0', &params);
      memcpy(tmp_func_item->param_types, params.content, strlen(params.content)+1);
    } else {
      tmp_func_item->param_types = NULL;
    }
  } else {
    if (tmp_func_item->is_defined == true){
      fprintf(stderr, "Syntax error: Function %s is already defined\n", identifier);
      free(identifier);
      error_exit(UNDEF_ERR);
    }
    if (tmp_func_item->value_type != return_type){
      fprintf(stderr, "Syntax error: Invalid fuction return type\n");
      free(identifier);
      error_exit(UNDEF_ERR);
    }
    if (params.len != 0){
      if (strcmp(tmp_func_item->param_types, params.content) != 0){
        fprintf(stderr, "Function definition is different from declaration.\n");
        error_exit(UNDEF_ERR);
      }
    }
  }

  //delete params
  delstr(&params);

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
    error_exit(SYNT_ERR);
    //return SYNT_ERR;
  } else {
    if (fnc_stats(tmp_func_item) != SUCCESS){
      error_exit(SYNT_ERR);
    }

    //implicit return is required
    if (returned == false){
      printf("# Implicit return\n");
      switch (tmp_func_item->value_type) {
        case 0 :
          printf("PUSHS int@0\n");
          break;
        case 1 :
          printf("PUSHS float@0.0\n");
          break;
        case 2 :
          printf("PUSHS string@\n");
          break;
      }
      printf("POPFRAME\n");
      printf("RETURN\n\n\n");
    }
    //has end expecting function
    get_token();
    CHECKT(FUNCTION_KEY);
    get_token();

    free(identifier);


    //destroy variable table
    hash_table_destroy(var_table);
    return end_of_lines();
  }

  return SUCCESS;

}

int scope(){
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
          error_exit(SYNT_ERR);
        }
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

        return SUCCESS;

      case ENDL :
        get_token();
        continue;

      case RETURN_KEY :
        fprintf(stderr, "Syntax error: Return outside of function.\n");
        error_exit(SYNT_ERR);
        break;
      default :
        //expecting statement
        if (statement(NULL) != SUCCESS){
          fprintf(stderr, "Invalid statement\n");
          error_exit(SYNT_ERR);
        }
        continue;
    }
    get_token();
  }

  return SUCCESS;
}

//<s> -> <funkcie> <scope>
int start(){

  start_program();

  int result = 0;

  while (result != SYNT_ERR){
    switch (currentToken.token_type) {
      case FUNCTION_KEY:
        //'function' keyword
      case DECLARE_KEY:
        //'declare' keyword
        result = functions();
        continue;

      case SCOPE_KEY:
        //'scope' keyword
        result = scope();
        end_scope();
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
        result = SYNT_ERR;
        return result;
    }
    return result;
  }

  return result;
}

int start_parsing(){

  srand(time(NULL));

  int result = SYNT_ERR;

  str_init(&params);

  processing_stack = init_T_NT_stack();

  //get first token
  get_token();
  if (currentToken.token_type == ENDL) {
    end_of_lines();
  }
  //check if empty program
  if (currentToken.token_type == ENDF){
    fprintf(stderr, "Empty file on input\n");
    error_exit(SYNT_ERR);
  }

  //begin parsing
  result = start();

  define_built_in_func(); // Will add all of used functions to TAC

  free(params.content);

  return result;

}

int main(){
  int result;

  //init symtables
  func_table = sym_tab_init(64);
  if (func_table == NULL) {
    error_exit(INTERNAL_ERR);
  }

  //start scanner
  start_scanner();

  //add built-in functions to symtable
  tmp_func_item = hash_table_insert(func_table, "length");
  tmp_func_item->is_defined = true;
  tmp_func_item->param_types = malloc(strlen("s")*sizeof(char)+1);
  memcpy(tmp_func_item->param_types, "s", strlen("s")*sizeof(char)+1);
  tmp_func_item->value_type = 0;

  tmp_func_item = hash_table_insert(func_table, "substr");
  tmp_func_item->is_defined = true;
  tmp_func_item->param_types = malloc(strlen("sii")*sizeof(char)+1);
  memcpy(tmp_func_item->param_types, "sii", strlen("sii")*sizeof(char)+1);
  tmp_func_item->value_type = 2;

  tmp_func_item = hash_table_insert(func_table, "asc");
  tmp_func_item->is_defined = true;
  tmp_func_item->param_types = malloc(strlen("si")*sizeof(char)+1);
  memcpy(tmp_func_item->param_types, "si", strlen("si")*sizeof(char)+1);
  tmp_func_item->value_type = 0;

  tmp_func_item = hash_table_insert(func_table, "chr");
  tmp_func_item->is_defined = true;
  tmp_func_item->param_types = malloc(strlen("i")*sizeof(char)+1);
  memcpy(tmp_func_item->param_types, "i", strlen("i")*sizeof(char)+1);
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
