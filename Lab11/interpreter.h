#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "stack.h"
#include "tables.h"

void run_ast(AST *ast, SymTable *st);

void rec_run_ast(AST *ast);

void run_assign(AST *ast);

void run_read(AST *ast);

void run_write(AST *ast);

void run_plus(AST *ast);
void run_minus(AST *ast);
void run_times(AST *ast);
void run_over(AST *ast);
void run_lt(AST *ast);
void run_eq(AST *ast);

void run_repeat(AST *ast);

#endif
