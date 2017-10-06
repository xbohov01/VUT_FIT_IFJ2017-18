/******stack.c******
*
* Stack header file
* Samuel Bohovic, xbohov01
* Stack implementation
*
*/

#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

//initialize stack
tStack *stack_init (){
  tStack *stack = malloc(sizeof(tStack));

  if (stack == NULL){
    //problem with malloc
    return NULL;
  }

  //set first item to NULL
  stack->top = NULL;

  return stack;
}

//push into stack
int stack_push(tStack *s, tData data){
  //check if stack is empty
  if (s == NULL){
    printf("stack was empty\n");
    return 1;
  }

  //allocate memory for the item
  tStackElement *newElement = (tStackElement *) malloc(sizeof(tStackElement));
  //check malloc result
  if (newElement == NULL){
    printf("element alloc failed\n");
    return 1;
  }

  //place data into new item
  newElement->data = data;

  //link new item
  newElement->nextItem = s->top;
  s->top = newElement;

  return 0;
}

//pop stack
int stack_pop(tStack *s){
  tStackElement *item;

  //check if stack is empty
  if (s->top != NULL){
    //move pointers
    item = s->top;
    s->top = item->nextItem;
    //free popped item
    free(item);
  } else {
    return 1;
  }
  return 0;
}

//return top of stack
tData stack_top(tStack *s){
  //check stack
  if (s != NULL){
    return s->top->data;
  }
}

//empty and free stack
int stack_free(tStack *s){
  if (s != NULL){
    if (s->top == NULL){
      //if stack is empty free it
      free(s);
    } else
    //if not empty
    {
      while (s->top != NULL){
        stack_pop(s);
      }
      free(s);
    }
  } else {
    return 1;
  }

  return 0;
}

//main for self debug
int main(){
  int i = 0;
  //tests on one item
  printf("====STARTING SINGLE ITEM TESTS====\n");
  tStack *test = stack_init();
  if (test == NULL){
    printf("init failed\n");
  } else {
    printf("init done\n");
  }

  tData testData;
  testData.value = 50;

  if (stack_push(test, testData) != 0){
    printf("push failed\n");
  } else {
    printf("push done\n");
  }

  testData = stack_top(test);
  if (testData.value == 0){
    printf("top failed\n");
  } else {
    printf("top done\n");
    printf("value: %d\n", testData.value);
  }

  if (stack_pop(test) != 0){
    printf("pop failed\n");
  } else {
    printf("pop done\n");
  }

  if (stack_free(test) != 0){
    printf("free failed\n");
  } else {
    printf("free done\n");
  }


  printf("====STARTING MULTIPLE ITEM TESTS====\n");
  //test on multiple item
  test = stack_init();
  if (test == NULL){
    printf("init failed\n");
  } else {
    printf("init done\n");
  }

  for (i = 0; i<=42; i++){
    testData.value = i;
    if (stack_push(test, testData) != 0){
      printf("%d: push failed\n", i);
    } else {
      printf("%d: push done\n", i);
    }
  }

  for (i = 0; i<=42; i++){
    testData = stack_top(test);
    if (testData.value == 0){
      printf("%d: top failed\n", i);
    } else {
      printf("%d: value: %d\n", i, testData.value);
    }
    if (stack_pop(test) != 0){
      printf("pop failed\n");
    } else {
      printf("pop done\n");
    }
  }

  for (i = 0; i<=42; i++){
    testData.value = i;
    if (stack_push(test, testData) != 0){
      printf("%d: push failed\n", i);
    } else {
      printf("%d: push done\n", i);
    }
  }

  if (stack_free(test) != 0){
    printf("free failed\n");
  } else {
    printf("free done\n");
  }

  return 0;
}
