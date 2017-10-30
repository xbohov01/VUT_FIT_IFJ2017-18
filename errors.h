/****** errors.h ******
*
* FIT VUT IFJ 2017/18
* By Samuel Bohovic, xbohov01
* Contains definitions for error code constants used in application.
*
*/

//Check if error messages were included (shouldn't happen but feature included just in case)
#ifndef ERROR_MSGS
#define ERROR_MSGS
  //No problem encountered
  #define SUCCESS 0
  
  //Original assignment error codes
  //Problem in lexical analysis
  #define LEX_ERR 1
  //Problem in syntax analysis
  #define SYNT_ERR 2
  //Semantical error. Undefined function/variable, attempt to redifine function/variable
  #define UNDEF_ERR 3
  //Semantical error. Type compatibility in expressions or incorrect type or number of arguments in function call
  #define TYPE_ERR 4
  //Semantical error that doesn't fall into above categories i.e. other semantical error
  #define SEM_ERR 6
  //Internal error (memory allocation, etc)
  #define INTERNAL_ERR 99

  //Developer made error messages

#endif
