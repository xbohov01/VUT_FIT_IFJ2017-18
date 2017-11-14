/****parser.c****/

#include "ifj2017.h"
#include "errors.h"
#include <stdio.h>

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

  if (file != NULL){
    fclose(file);
  }

  //TODO destroy all stacks

  exit(code);
}

void print_token(){
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

  fprintf(stderr, "Syntax Error: %s was expected but %s was given.\n", tokenList[expected], tokenList[given]);
}

//some macros
//getting next token with error check
#define NEXTT()  {  \
  if (get_token() != SUCCESS){  \
    return LEX_ERR; \
  } \
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
//TODO add to symtable
//TODO expr eval
int var_declr(){
  //already has dim keyword
  get_token();
  //expecting identifier
  CHECKT(IDENTIFICATOR);
  get_token();
  //expecting as keyword
  CHECKT(AS_KEY);
  get_token();
  //expecting type
  if (currentToken.token_type != INTEGER_KEY && currentToken.token_type != DOUBLE_KEY && currentToken.token_type != STRING_KEY){
    fprintf(stderr, "Syntax error: expecting variable type in variable declaration.\n");
    return SYNT_ERR;
  }
  get_token();
  //expecting ENDL or variable initialization
  if (currentToken.token_type == ENDL){
    return end_of_lines();
  } else if (currentToken.token_type == EQ_O){
    //call expression evaluation;
  } else {
    fprintf(stderr, "Syntax error: invalid sequence in variable declaration.\n");
    hard_exit(SYNT_ERR);
    //return SYNT_ERR;
  }

}

//<fncarg> -> <id>	as 	<type>
//TODO add to symtable
int fnc_arg(){
  //already got identifier
  get_token();

  //expecting as keyword
  CHECKT(AS_KEY);
  get_token();

  //expecting type
  if (currentToken.token_type != (STRING_KEY || INTEGER_KEY || DOUBLE_KEY)){
    fprintf(stderr, "Invalid type\n");
    hard_exit();
    //return SYNT_ERR;
  } else {
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
    get_token();
    //expecting identifier
    CHECKT(IDENTIFICATOR);
    if (fnc_arg() != SUCCESS){
      fprintf(stderr, "Invalid argument\n");
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    }
    get_token();
    //expecting PAR_R or a comma
    if (currentToken.token_type == PAR_R){
      return SUCCESS;
    } else if (currentToken.token_type == COM){
      continue;
    } else {
      fprintf(stderr, "Invalid argument\n");
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
int statement(){
  //expecting one of the above
  switch (currentToken.token_type) {
    case ENDL :
      get_token();

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
  int definition = 0;

  //if function is being declared one more NEXTT has to be called
  if (currentToken.token_type == DECLARE_KEY){
    get_token();
    CHECKT(FUNCTION_KEY);
    //is not definition
    definition = 1;
  }
  //already got 'function' keyword
  get_token();

  //expecting identifier
  CHECKT(IDENTIFICATOR);
  get_token();
  //expecting (
  CHECKT(PAR_L);
  get_token();
  //expecting argument declarations or right parenthesis
  //<paramlist>	-> <fncarg>	,	<paramlist>
  //<fncarg> ->	<id>	as 	<type>
  if (currentToken.token_type == PAR_R){
    //expecting ENDL(s) before next statement
    if (definition != 0){
        return end_of_lines();
    }
  } else if (currentToken.token_type == IDENTIFICATOR){
    //expecting argument declaration
    if (fnc_arglist() != SUCCESS){
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    } else {
      CHECKT(PAR_R);
      //expecting ENDL(s) before next statement
      if (definition != 0){
          return end_of_lines();
      }
    }
  } else {
    fprintf(stderr, "Syntax error in function declaration/definition. Expecting ')' or variables, %d was given.\n", currentToken.token_type);
    hard_exit(SYNT_ERR);
    //return SYNT_ERR;
  }
  //if function is also defined
  //expecting ENDL(s) before next statement
  if (end_of_lines() != SUCCESS){
    hard_exit(SYNT_ERR);
    //return SYNT_ERR;
  } else {
    if (fnc_stats() != SUCCESS){
      hard_exit(SYNT_ERR);
      //return SYNT_ERR;
    }
    return end_of_lines();
  }

}

int scope(){
  //int result = SYNT_ERR;
  get_token();
  CHECKT(ENDL);
  get_token();

  while (currentToken.token_type != ENDF){
    switch (currentToken.token_type){
      case DIM_KEY :
        //is variable declaration
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
      case SCOPE_KEY:
        //'scope' keyword
        result = scope();
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

  return result;

}

int main(int argc, char *argv[]){
  int result;
  if (argc != 2){
    fprintf(stderr, "Incorrect arguments\n");
    return INTERNAL_ERR;
  }

  //init symtables
  func_table = sym_tab_init(64);
  var_table = sym_tab_init(64);
  //TODO check for correct init

  //start scanner
  start_scanner(argv[1]);

  //start parsing
  result = start_parsing();

  printf("%d\n", result);

  return result;

}
