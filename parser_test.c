#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "errors.h"

int get_token();

//which test should begin testing
int testing = 0;

//result of test
int result = 2;

//position in test array
int pos = 0;

//prints current token
void print_curr_token(){
  //array of all tokens
  const char *tokenList[] = {
    "+",
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
    "line comment",
    "BLOCK_COMMENT",
    "UNDEFINED",
    "end of file",
  };
  //token to print
  char *printToken;
  printf("Current token -> %s\n", tokenList[currentToken.token_type]);
}

T_token_type *currentTest = NULL;

//test for empty scope
T_token_type test1[] = {
  SCOPE_KEY,
  END_KEY,
  SCOPE_KEY,
  ENDF,
};

//test with one variable
T_token_type test2[] = {
  ENDF,

};

//total number of tests
int tests = 2;

int expectedResult[] = {
  SUCCESS,
  SUCCESS,
};

//replacement for scanner
//gives parser "fake" tokens from array
int get_token(){

  if (testing == 0){
    currentTest = test1;
  } else if (testing == 1){
    currentTest = test2;
  }

  currentToken.token_type = currentTest[pos];
  print_curr_token();
  pos++;

  return SUCCESS;

}

int main(int argc, char *argv[]){
  if (argc != 1){
    testing =   strtol(argv[1], NULL, 10);
  }


  for (int i = 0; i < tests; i++){
    printf("===>>> Starting TEST%d <<<===\n", i+1);
    pos = 0;
    result = start_parsing();
    printf(">>> TEST%d result: %d || expected: %d\n", i+1, result, expectedResult[i]);
    printf("==================================\n");
    testing++;

  }

}
