/****parser.c****/

#include "parser.h"
#include "parser_test.h"
//#include "scanner.h"
#include "errors.h"
#include <stdio.h>


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
    fprintf(stderr, "Syntax Error: %d was expected but %d was given.\n", expected, currentToken.token_type);  \
    return SYNT_ERR; \
  } \
}

int end_of_lines(){
  NEXTT();
  CHECKT(EOL);
  while (currentToken.token_type == EOL){
    NEXTT();
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
  NEXTT();
  //expecting identifier
  CHECKT(IDENTIFICATOR);
  NEXTT();
  //expecting as keyword
  CHECKT(AS_KEY);
  NEXTT();
  //expecting type
  if (currentToken.token_type != INTEGER_KEY && currentToken.token_type != DOUBLE_KEY && currentToken.token_type != STRING_KEY){
    fprintf(stderr, "Syntax error: expecting variable type in variable declaration.\n");
    return SYNT_ERR;
  }
  NEXTT();
  //expecting EOL or variable initialization
  if (currentToken.token_type == EOL){
    return end_of_lines();
  } else if (currentToken.token_type == EQ_O){
    //call expression evaluation;
  } else {
    fprintf(stderr, "Syntax error: invalid sequence in variable declaration.\n");
    return SYNT_ERR;
  }

}

//<fncarg> -> <id>	as 	<type>
//TODO add to symtable
int fnc_arg(){
  //already got identifier
  NEXTT()

  //expecting as keyword
  CHECKT(AS_KEY);
  NEXTT();

  //expecting type
  if (currentToken.token_type != (STRING_KEY || INTEGER_KEY || DOUBLE_KEY)){
    return SYNT_ERR;
  } else {
    //gets next for condition in fnc_arglist
    NEXTT();
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
  if (currentToken.token_type == TT_RIGHTPAR){
    return SUCCESS;
  }
  //some arguments
  while (currentToken.token_type != TT_RIGHTPAR){
    //one argument
    NEXTT();
    //expecting identifier
    CHECKT(IDENTIFICATOR);
    if (fnc_arg() != SUCCESS){
      return SYNT_ERR;
    }
    NEXTT()
    //expecting TT_RIGHTPAR or a comma
    if (currentToken.token_type == TT_RIGHTPAR){
      return SUCCESS;
    } else if (currentToken == TT_COMMA){
      continue;
    } else {
      return SYNT_ERR;
    }
  }
/*
  while (currentToken.token_type != TT_RIGHTPAR){
    //has identifier or comma
    if (currentToken.token_type == IDENTIFICATOR){
      if (fnc_arg() != SUCCESS){
        return SYNT_ERR;
      }
    } else if (currentToken.token_type == TT_COMMA){
      NEXTT();
    } else {
      return SYNT_ERR;
    }
  }
*/
}

//<statement> ->	if	<expr>	then	<endline>	<thenstats>	else	<endline>	<elsestats>	end 	if
int if_statements(){
  while (currentToken.token_type != END_KEY || currentToken.token_type != ELSE_KEY || currentToken.token_type != ELSEIF_KEY){
    if (statements() == SYNT_ERR){
      return SYNT_ERR;
    } else {
      return SUCCESS;
    }
  }
}

//<statement>	<expr>
//<statement>	<id>	='	<expr>
//<statement>	input	<id>
//<statement>	print	<id>	;	<print_list>
//<statement>	if	<expr>	then	<endline>	<thenstats>	else	<endline>	<elsestats>	end 	if
//<statement>	do	while	<expr>	<endline>	<teloprogramu>	loop
//<statement>	<id>	='	<id>	(	<args>	)
//<statement>	return	<expr>
//TODO if statement
//TODO while statement
//TODO return psa call
int statement(){
  //expecting one of the above
  switch (currentToken.token_type) {
    case INTEGER :
    case DOUBLE :
    case STRING :
    case IDENTIFICATOR:
    case RETURN_KEY :
      //resolve expression or function call or return
      //TODO add psa call
      return end_of_lines();

    case INPUT_KEY :
      NEXTT();
      //expecting identifier
      CHECKT(IDENTIFICATOR);
      return end_of_lines();

    case PRINT_KEY :
      NEXTT();
      //expecting identifier
      CHECKT(IDENTIFICATOR);
      NEXTT();
      //expecting ; or EOL
      while (currentToken.token_type == TT_SEMICOLON){
        NEXTT();
        //expecting identifier
        CHECKT(IDENTIFICATOR);
      }
      //EOLs
      return end_of_lines();

    case IF_KEY :

      //has if
      NEXTT();
      //TODO check if expression

      NEXTT();
      CHECKT(THEN_KEY);
      //expecting end of line
      NEXTT();
      CHECKT(EOL);
      //if block
      if (if_statements() != SUCCESS){
        return SYNT_ERR;
      }
      //else if block
      if (currentToken.token_type == ELSEIF_KEY){
        while (currentToken.token_type == ELSEIF_KEY){
          NEXTT();
          //expecting then
          CHECKT(THEN_KEY);
          //checking statements
          if (if_statements() != SUCCESS){
            return SYNT_ERR;
          }
        }
      }
      //else block
      if (currentToken.token_type == ELSE_KEY){
        if (if_statements() != SUCCESS){
          return SYNT_ERR;
        }
      }
      //check end if
      if (currentToken.token_type == END_KEY){
        NEXTT();
        //expecting if
        CHECKT(IF_KEY);
      } else {
        //TODO maybe remove this? probably dead code
        return SYNT_ERR;
      }

    case DO_KEY :
      //has do
      NEXTT();
      //expecting while
      CHECKT(WHILE_KEY);
      //TODO expression eval

      //expecting EOL after expr TODO maybe not get next token
      NEXTT();
      CHECKT(EOL);

      //while block
      if (if_statements() != SUCCESS){
        return SYNT_ERR;
      }

      //expecting loop
      CHECKT(LOOP_KEY);

    default :
      return SYNT_ERR;
  }

}

//<fncstats>	<statement>	<endline>	<fncstats>
//<fncstats>	epsilon
int fnc_stats(){
  //expecting statements inside a function or end of function
  NEXTT()
  while (currentToken.token_type != END_KEY){
    if (statement() != SUCCESS){
      return SYNT_ERR;
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
    NEXTT();
    CHECKT(FUNCTION_KEY);
    //is not definition
    definition = 1;
  }
  //already got 'function' keyword
  NEXTT();

  //expecting identifier
  CHECKT(IDENTIFICATOR);
  NEXTT();
  //expecting (
  CHECKT(TT_LEFTPAR);
  NEXTT();
  //expecting argument declarations or right parenthesis
  //<paramlist>	-> <fncarg>	,	<paramlist>
  //<fncarg> ->	<id>	as 	<type>
  if (currentToken.token_type == TT_RIGHTPAR){
    //expecting EOL(s) before next statement
    if (definition != 0){
        return end_of_lines();
    }
  } else if (currentToken.token_type == IDENTIFICATOR){
    //expecting argument declaration
    if (fnc_arglist() != SUCCESS){
      return SYNT_ERR;
    } else {
      CHECKT(TT_RIGHTPAR);
      //expecting EOL(s) before next statement
      if (definition != 0){
          return end_of_lines();
      }
    }
  } else {
    fprintf(stderr, "Syntax error in function declaration/definition. Expecting ')' or variables, %d was given.\n", currentToken.token_type);
    return SYNT_ERR;
  }
  //if function is also defined
  //expecting EOL(s) before next statement
  if (end_of_lines() != SUCCESS){
    return SYNT_ERR;
  } else {
    if (fnc_stats() != SUCCESS){
      return SYNT_ERR;
    }
    return end_of_lines();
  }

}

int scope(){
  //int result = SYNT_ERR;
  NEXTT();

  while (currentToken.token_type != ENDF){
    switch (currentToken.token_type){
      case DIM_KEY :
        //is variable declaration
        return var_declr();

      case END_KEY :
        //expecting end scope
        NEXTT();
        CHECKT(SCOPE_KEY);

        //expecting EOF
        NEXTT()
        CHECKT(ENDF);
        return SUCCESS;

      default :
        //expecting statement
        return statement();
    }
  }

}

//<s> -> <funkcie> <scope>
int start(){

  int result;

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
      default:
        return SYNT_ERR;
    }
  }
}

//TODO init symtable
//TODO init scanner
int start_parsing(){

  int result = SYNT_ERR;

  //get first token
  if (get_token() != SUCCESS){
    return LEX_ERR;
  }
  //begin parsing
  result = start();
  if (result == SUCCESS){
    return SUCCESS;
  } else {
    return SYNT_ERR;
  }

}
