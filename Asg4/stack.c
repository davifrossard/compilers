#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

void screate(stack_type *stack, int size)
{
  entry_type *vals;
  vals = malloc(sizeof(entry_type) * size);
  stack->contents = vals;
  stack->maxSize = size;
  stack->top = -1;
}

void sfree(stack_type *stack)
{
  free(stack->contents);
  stack->contents = NULL;
  stack->maxSize = 0;
  stack->top = -1;
}

void spush(stack_type *stack, entry_type element)
{
  if(stack->top >= stack->maxSize - 1)
  {
    int size = stack->maxSize;
    stack->contents = realloc(stack->contents, sizeof(entry_type) * size * 2);
    stack->maxSize = size * 2;
  }
  stack->contents[++stack->top] = element;
}

entry_type spop(stack_type *stack)
{
  if(stack->top == -1)
  {
    printf("Stack is empty\n");
    exit(0);
  }
  return stack->contents[stack->top--];
}
