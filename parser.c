/****parser.c****/

#include "parser.h"
#include "scanner.h"
#include "errors.h"


//some macros
//getting next token with error check
#define NEXTT()  {  \
  if (get_token() != SUCCESS){  \
    return LEX_ERR; \
  } \
}

//check if token is correct
#define CHECKT(expected){ \
  if (currentToken.type != expected){ \
    return SYNT_ERR; \
  } \
}

int end_of_lines(){
  while (currentToken.type == EOL){
    NEXTT();
  }
  return SUCCESS;

}

//<deklaracie> ->	<deklaracia>	<endline>	<deklaracie>
int var_declr(){

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
  if (currentToken.type != (STRING_KEY || INTEGER_KEY || DOUBLE_KEY)){
    return SYNT_ERR;
  } else {
    //gets next for condition in fnc_arglist
    NEXTT();
    return SUCCESS;
  }

}

//<paramlist> ->	<fncarg>	,	<paramlist>
int fnc_arglist(){

  while (currentToken.type != TT_RIGHTPAR){
    //has identifier or comma
    if (currentToken.type == IDENTIFICATOR){
      if (fnc_arg() != SUCCESS){
        return SYNT_ERR;
      }
    } else if (currentToken.type == TT_COMMA){
      NEXTT();
    } else {
      return SYNT_ERR;
    }
  }

}

//<funkcie>	-> <fnc> <endline> <funkcie>
//<fnc> -> <fncdeclr>
//<fnc> -> <fncdef>
//<fncdeclr>-> function <id> ( <paramlist> ) as	<type> <endline>
int functions(){
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
  if (currentToken.type == TT_RIGHTPAR){
    //expecting EOL(s) before next statement
    return end_of_lines();
  } else if (currentToken.type == IDENTIFICATOR){
    //expecting argument declaration
    if (fnc_arglist() != SUCCESS){
      return SYNT_ERR;
    } else {
      CHECKT(TT_RIGHTPAR);
      //expecting EOL(s) before next statement
      return end_of_lines();
    }
  } else {
    return SYNT_ERR;
  }

  }
}

int scope(){

}

//<s> -> <funkcie> <scope>
int start(){

  int result;

  switch (currentToken.type) {
    case FUNCTION_KEY:
      //'function' keyword
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
