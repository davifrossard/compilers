
#include <stdio.h>
#include "interpreter.h"


stack_type _RES_STACK;
SymTable *vt;

void run_ast(AST *ast, SymTable *st) {
	screate(&_RES_STACK, 10);
	vt = st;
	rec_run_ast(ast);
}
	
void rec_run_ast(AST *ast) {
	int idx;
	switch(get_kind(ast))
	{
		case STMT_SEQ_NODE:
			for (int i = 0; i < get_child_count(ast); i++)
			{	
				rec_run_ast(get_child(ast, i));
			}
			break;
		/*
		case IF_NODE:
			run_num(ast);
			break;
		*/
		case REPEAT_NODE:
			run_repeat(ast);
			break;
		case ASSIGN_NODE:
			run_assign(ast);
			break;
		case READ_NODE:
			run_read(ast);
			break;
		case WRITE_NODE:
			run_write(ast);
			break;
		case PLUS_NODE:
			run_plus(ast);
			break;
		case MINUS_NODE:
			run_minus(ast);
			break;
		case TIMES_NODE:
			run_times(ast);
			break;
		case OVER_NODE:
			run_over(ast);
			break;
		case LT_NODE:
			run_lt(ast);
			break;
		case EQ_NODE:
			run_eq(ast);
			break;
		case NUM_NODE:
			spush(&_RES_STACK, get_data(ast));
			break;
		case ID_NODE:
			idx = get_data(ast);
			spush(&_RES_STACK, get_val(vt, idx));
			break;
		default:
			break;
	}
}

void run_assign(AST *ast) {
	int idx = get_data(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int res = spop(&_RES_STACK);
	put_val(vt, idx, res);
}

void run_read(AST *ast) {
	int res;
	printf("DIGITA AE: ");
	scanf("%d", &res);
	int idx = get_data(get_child(ast,0));
	put_val(vt, idx, res);
}

void run_write(AST *ast) {
	rec_run_ast(get_child(ast,0));
	int res = spop(&_RES_STACK);
	printf("%d\n", res);
}

void run_plus(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = spop(&_RES_STACK);
	int l = spop(&_RES_STACK);
	spush(&_RES_STACK, l+r);
}

void run_minus(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = spop(&_RES_STACK);
	int l = spop(&_RES_STACK);
	spush(&_RES_STACK, l-r);
}

void run_times(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = spop(&_RES_STACK);
	int l = spop(&_RES_STACK);
	spush(&_RES_STACK, l*r);
}

void run_over(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = spop(&_RES_STACK);
	int l = spop(&_RES_STACK);
	spush(&_RES_STACK, l/r);
}

void run_lt(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = spop(&_RES_STACK);
	int l = spop(&_RES_STACK);
	spush(&_RES_STACK, l<r);
}

void run_eq(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = spop(&_RES_STACK);
	int l = spop(&_RES_STACK);
	spush(&_RES_STACK, l==r);
}

void run_repeat(AST *ast) {
	AST *stmt_seq = get_child(ast, 0);
	AST *repeat_expr = get_child(ast, 1);
	do {
		rec_run_ast(stmt_seq);
		rec_run_ast(repeat_expr);
	} while(!spop(&_RES_STACK));
}

