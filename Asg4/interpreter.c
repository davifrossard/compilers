
#include <stdio.h>
#include "interpreter.h"

#define _STACK_SIZE 500000
#define run_all(ast) for(int i = 0; i < get_child_count(ast); i++) rec_run_ast(get_child(ast, i));
#define run_all_inv(ast)  for(int i = get_child_count(ast)-1; i >= 0 ; i--) rec_run_ast(get_child(ast, i));

stack_type _RES_STACK;
#define rpop() spop(&_RES_STACK)
#define rpush(x) spush(&_RES_STACK, x)

int _STACK[_STACK_SIZE];
int _FRAME_POINTER, _STACK_POINTER;
int _PREV_SCOPE, _CURRENT_SCOPE, _EXPECT_RET;
SymTable *st, *current_st;
LitTable *lt;
FunTable *ft;

void run_ast(AST *ast, SymTable *pst, LitTable *plt, FunTable* pft) {
	screate(&_RES_STACK, 10);
	st = pst;
	lt = plt;
	ft = pft;
	rec_run_ast(ast);
}

void rec_run_ast(AST *ast) {
	switch(get_kind(ast))
	{
		case FUNC_LIST_NODE:
			run_func_list(ast);
			break;
		case BLOCK_NODE:
		case FUNCTION_NODE:
		case STMT_SEQ_NODE:
			run_all(ast);
			break;
		case FHEADER_NODE:
			_PREV_SCOPE = _CURRENT_SCOPE;
			_CURRENT_SCOPE = get_data(get_child(ast, 0));
			current_st = get_sym_list_by_scope(st, _CURRENT_SCOPE);
			_EXPECT_RET = get_kind(get_child(ast, 1)) == RET_VOID_NODE ? 0 : 1;
			rec_run_ast(get_child(ast, 2));
			break;
		case FBODY_NODE:
			run_all(ast);
			break;
		case WRITE_NODE:
			run_write(ast);
			break;
		case VAR_DECL_NODE:
			run_var_decl(ast);
			break;
		case FPARAMETES_NODE:
			run_all_inv(ast);
			break;
		case ASSIGN_NODE:
			run_assign(ast);
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
		case BOOL_EXPR_NODE:
			run_bool_expr(ast);
			break;
		case OVER_NODE:
			run_over(ast);
			break;
		case NUM_NODE:
			rpush(get_data(ast));
			break;
		case ID_NODE:
			run_id_val(ast);
			break;
		case OUTPUT_NODE:
			run_output(ast);
			break;
		case INPUT_NODE:
			run_input(ast);
			break;
		case IF_NODE:
			run_if(ast);
			break;
		case WHILE_NODE:
			run_while(ast);
			break;
		default:
			printf("%s NOT IMPLEMENTED!\n", kind2str(get_kind(ast)));
			return;
	}
}

void run_func_list(AST *ast) {
	AST* main_node;
	int main_hash = hash_fun_name("main");
	for(int i = 0; i < get_child_count(ast); i++) {
		AST* func = get_child(ast, i);
		int key = get_data(func);
		set_fun_node(ft, key, func);
		if(key == main_hash)
			main_node = func;
	}
	_CURRENT_SCOPE = main_hash;
	rec_run_ast(main_node);
}

void run_var_decl(AST *ast) {
	int offset_counter = 0;
	for (int i = 0; i < get_child_count(ast); i++) {
		AST* id = get_child(ast, i);
		set_var_offset(current_st, get_data(id), offset_counter);
		if(get_child_count(id) != 0) { /* vector */
			AST* len = get_child(id, 0);
			if(get_kind(len) == NUM_NODE)
				offset_counter += get_data(len);
			else
				offset_counter += _STACK[_FRAME_POINTER+get_var_offset(current_st, get_data(len))];
		} else { /* simple variable */
			offset_counter += 1;
		}
	}
	_STACK_POINTER += offset_counter;
}

void run_if(AST *ast) {
		rec_run_ast(get_child(ast,0));
		if(rpop())
			rec_run_ast(get_child(ast,1));
		else if(get_child_count(ast)>2)
			rec_run_ast(get_child(ast,2));
}

void run_while(AST *ast) {
		rec_run_ast(get_child(ast,0));
		int cond = rpop();
		while(cond)
		{
			rec_run_ast(get_child(ast,1));
			rec_run_ast(get_child(ast,0));
			cond = rpop();
		}
}


void run_input(AST *ast) {
	int res;
	scanf("\n%d", &res);
	rpush(res);
}

void run_assign(AST *ast) {
	// Set stack pointer
	AST* var = get_child(ast, 0);
	int sp = _FRAME_POINTER + get_var_offset(current_st, get_data(var));
	// Compute value
	rec_run_ast(get_child(ast, 1));
	// Assign
	_STACK[sp] = rpop();
}

void run_id_val(AST *ast) {
	// Set stack pointer
	int sp = _FRAME_POINTER + get_var_offset(current_st, get_data(ast));
	rpush(_STACK[sp]);
}

void run_output(AST *ast) {
	rec_run_ast(get_child(ast, 0));
	printf("%d", rpop());
}

void run_plus(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = rpop();
	int l = rpop();
	rpush(l+r);
}



void run_bool_expr(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,2));
	int r = rpop();
	int l = rpop();
	switch(get_kind(get_child(ast,1)))
	{
		case GT_NODE:
			rpush(l>r);
			break;
		case GE_NODE:
			rpush(l>=r);
			break;
		case LT_NODE:
			rpush(l<r);
			break;
		case LE_NODE:
			rpush(l<=r);
			break;
		case EQ_NODE:
			rpush(l==r);
			break;
		case NEQ_NODE:
			rpush(l!=r);
			break;
		default:
			printf("%s NOT IMPLEMENTED!\n", kind2str(get_kind(ast)));
			rpush(0);
			break;
	}
}

void run_minus(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = rpop();
	int l = rpop();
	rpush(l-r);

}
void run_times(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = rpop();
	int l = rpop();
	rpush(l*r);
}

void run_over(AST *ast) {
	rec_run_ast(get_child(ast,0));
	rec_run_ast(get_child(ast,1));
	int r = rpop();
	int l = rpop();
  rpush(l/r);
}

void run_write(AST *ast) {
	char* string = get_literal(lt, get_data(ast));
	int len = strlen(string)-1;
	for(int i=1; i<len; i++) {
		if(string[i] == '\\' && i+1 <= len) {
			switch(string[i+1])
			{
				case 'n': putchar('\n'); break;
				case 't': putchar('\t'); break;
				case '\\': putchar('\\'); break;
				default: printf("\\%c", string[i+1]); break;
			}
			i++;
		} else {
			putchar(string[i]);
		}
	}
}
