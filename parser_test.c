#include <stdio.h>
#include <stdlib.h>
#include "ifj2017.h"
#include "errors.h"

void get_token();

//which test should begin testing
int testing = 0;

//result of test
int result = 55;

//position in test array
int pos = 0;

tString tid;

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
  printf("%d NEXTT used. Current token -> %s\n", pos, tokenList[currentToken.token_type]);
}

T_token_type *currentTest = NULL;

//test for empty scope
T_token_type test1[] = {
  SCOPE_KEY,
  ENDL,
  END_KEY,
  SCOPE_KEY,
  ENDF,
};

//test just end of file
T_token_type test2[] = {
  ENDF,
};

//test with scope and one variable
T_token_type test3[] = {
  SCOPE_KEY,
  ENDL,
  DIM_KEY,
  IDENTIFICATOR,
  AS_KEY,
  INTEGER_KEY,
  ENDL,
  END_KEY,
  SCOPE_KEY,
  ENDF,
};

T_token_type test4[] = {
  ENDL, SCOPE_KEY, ENDL,
  DIM_KEY, IDENTIFICATOR, AS_KEY, INTEGER_KEY, ENDL,
  DIM_KEY, IDENTIFICATOR, AS_KEY, INTEGER_KEY, ENDL,
	PRINT_KEY, STRING, SEM, ENDL,
  INPUT_KEY, IDENTIFICATOR, ENDL,
  IF_KEY, IDENTIFICATOR, LT_O, INTEGER, THEN_KEY, ENDL,
  PRINT_KEY, STRING, SEM, ENDL, ELSE_KEY, ENDL,
	IDENTIFICATOR, EQ_O, INTEGER, ENDL,
  DO_KEY, WHILE_KEY, IDENTIFICATOR, GT_O, INTEGER, ENDL,
  IDENTIFICATOR, EQ_O, IDENTIFICATOR, MUL_O, IDENTIFICATOR, ENDL,
  IDENTIFICATOR, EQ_O, IDENTIFICATOR, SUB_O, INTEGER, ENDL,
  LOOP_KEY, ENDL,
  PRINT_KEY, STRING, SEM, IDENTIFICATOR, SEM, STRING, SEM, ENDL,
  END_KEY, IF_KEY, ENDL,
  END_KEY, SCOPE_KEY, ENDL,
  ENDF,
};

T_token_type test5[] = {
  DECLARE_KEY, FUNCTION_KEY, IDENTIFICATOR, PAR_L, IDENTIFICATOR, AS_KEY, INTEGER_KEY, PAR_R, AS_KEY, INTEGER_KEY, ENDL,
  ENDF,
};

//total number of tests
int tests = 5;

int expectedResult[] = {
  SUCCESS,
  SUCCESS,
  SUCCESS,
  SUCCESS,
  SUCCESS,
};

//replacement for scanner
//gives parser "fake" tokens from array
void get_token(){

  if (testing == 0){
    currentTest = test1;
  } else if (testing == 1){
    currentTest = test2;
  } else if (testing == 2){
    currentTest = test3;
  } else if (testing == 3){
    currentTest = test4;
  } else if (testing == 4){
    currentTest = test5;
  }

  currentToken.token_type = currentTest[pos];
  if (currentToken.token_type == IDENTIFICATOR){
    addchar('d', &tid);
    currentToken.id = tid.content;
  }
  print_curr_token();
  pos++;

}

void free_sources() //funkcia uvolnuje pouzite zdroje
{
	free(params.content);

}

int main(int argc, char *argv[]){
  if (argc != 1){
    testing =   strtol(argv[1], NULL, 10);
  }

  //str_init(&params);
  file == NULL;

  str_init(&tid);


  for (int i = 0; i < tests; i++){
    printf("===>>> Starting TEST%d <<<===\n", i+1);
    pos = 0;
    func_table = sym_tab_init(64);
    result = start_parsing();

    hash_table_destroy(func_table);
    free_sources();

    printf(">>> TEST%d result: %d || expected: %d\n", i+1, result, expectedResult[i]);
    printf("==================================\n");
    testing++;

  }

  free(tid.content);

}
