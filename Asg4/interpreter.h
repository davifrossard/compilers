#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "bt.h"
#include "stack.h"
#include "tables.h"

typedef BT AST;

void run_ast(AST *ast, SymTable *st, LitTable *lt, FunTable* ft);

#endif
