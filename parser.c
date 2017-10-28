/****parser.c****/

#include "parser.h"
#include "ifj2017lex.h"


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

//<deklaracie> ->	<deklaracia>	<endline>	<deklaracie>
int var_declr(){

}

//<paramlist> ->	<deklaracia>	,	<paramlist>
int fnc_arglist(){

}

//<funkcie>	-> <fnc> <endline> <funkcie>
//<fnc> -> <fncdeclr>
//<fnc> -> <fncdef>
//<fncdeclr>-> function <id> ( <paramlist> ) as	<type> <endline>
int functions(){
  //already got 'function' keyword
  NEXTT();

  //expecting identifier
  CHECKT(TT_ID);
  NEXTT();
  //expecting (
  CHECKT(TT_LEFTPAR);
  NEXTT();
  //expecting argument declarations or right parenthesis
  //<paramlist>	-> <fncarg>	,	<paramlist>
  //<fncarg> ->	<id>	as 	<type>

  }
}

int scope(){

}

//<s> -> <funkcie> <scope>
int start(){

  int result;

  switch (currentToken.type) {
    case TT_FUNC:
      //'function' keyword
      result = functions();
    case TT_SCOPE:
      //'scope' keyword
      result = scope();
    case TT_EOF:
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
