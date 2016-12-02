#ifndef _STACK_H
#define _STACK_H
#include "bt.h"

typedef void* entry_type;

typedef struct {
  entry_type *contents;
  int maxSize;
  int top;
} stack_type;


void screate(stack_type *stack, int size);

void sfree(stack_type *stack);

void spush(stack_type *stack, entry_type element);
entry_type spop(stack_type *stack);

#endif
