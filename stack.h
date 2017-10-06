/******stack.h******
*
* Stack header file
* Samuel Bohovic, xbohov01
* Header file for stack implementation
*
*/

//TODO maybe create a return value data type or just return int

typedef struct tdata{
  int value;
} tData;

typedef struct tstackelement{
  tData data;
  struct tstackelement *nextItem;
} tStackElement;

typedef struct tstack{
  tStackElement *top;
} tStack;

int stack_push(tStack *s, tData data);
int stack_pop(tStack *s);
tData stack_top(tStack *s);
int stack_free(tStack *s);
