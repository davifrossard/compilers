
#include <stdio.h>
#include "interpreter.h"

#define _STACK_SIZE 500000
#define run_all(ast) for(int i = 0; i < get_child_count(ast); i++) rec_run_ast(get_child(ast, i));

stack_type _RES_STACK;
#define rpop() spop(&_RES_STACK)
#define rpush(x) spush(&_RES_STACK, x)

int _STACK[_STACK_SIZE];
int _FRAME_POINTER, _STACK_POINTER;
int _OFFSET_COUNTER, _LAST_FP, _LAST_SP;
SymTable *st, *current_st, *prev_st;
LitTable *lt;
FunTable *ft;

void run_ast(AST *ast, SymTable *pst, LitTable *plt, FunTable* pft) {
	screate(&_RES_STACK, 10);
	st = pst;
	lt = plt;
	ft = pft;
	current_st = get_sym_list_by_scope(pst, hash_fun_name("main"));
	rec_run_ast(ast);
}

void print_stack() {
	if(_STACK_POINTER>0) {
		fprintf(stderr, "FP:%d - SP:%d - CST:%d - PST:%d\n", _FRAME_POINTER, _STACK_POINTER, current_st, prev_st);
		for(int i=0; i<_STACK_POINTER; i++)
			fprintf(stderr,"%d ", _STACK[i]);
		fprintf(stderr,"\n");
	}
}

void rec_run_ast(AST *ast) {

	switch(get_kind(ast))
	{
		case FUNCTION_NODE:
		case BLOCK_NODE:
		case STMT_SEQ_NODE:
			run_all(ast);
			break;
		case FBODY_NODE:
			run_all(ast);
			current_st = _STACK[--_STACK_POINTER];
			_FRAME_POINTER = _STACK[--_STACK_POINTER];
		 	_STACK_POINTER = _STACK[--_STACK_POINTER];
			break;
		case FUNC_LIST_NODE:
			run_func_list(ast);
			break;
		case FUNCTION_CALL_NODE:
			_LAST_FP = _FRAME_POINTER;
			_LAST_SP = _STACK_POINTER;
			run_arg_list(get_child(ast, 0));
			_FRAME_POINTER = _STACK_POINTER;
			rec_run_ast(get_fun_node(ft, get_data(ast)));
			break;
		case FHEADER_NODE:
			prev_st = current_st;
			current_st = get_sym_list_by_scope(st, get_data(get_child(ast, 0)));
			rec_run_ast(get_child(ast, 2));
			break;
		case WRITE_NODE:
			run_write(ast);
			break;
		case VAR_DECL_NODE:
			run_var_decl(ast);
			break;
		case FPARAMETES_NODE:
			run_fparameters(ast);
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
		case VID_NODE:
			run_vid_val(ast);
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
		case RET_NODE:
			rec_run_ast(get_child(ast,0));
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
	rec_run_ast(main_node);
}


void run_arg_list(AST *ast) {
	for (int i = 0; i < get_child_count(ast); i++) {
		AST* child = get_child(ast, i);
		if(get_kind(child) == VID_NODE) {
			_STACK[_STACK_POINTER+i] = get_var_offset(current_st, get_data(child));
		} else {
			rec_run_ast(get_child(ast, i));
			_STACK[_STACK_POINTER+i] = rpop();
		}
	}
}

void run_fparameters(AST *ast) {
	_OFFSET_COUNTER = 0;
	for (int i = 0; i < get_child_count(ast); i++) {
		AST* id = get_child(ast, i);
		set_var_offset(current_st, get_data(id), _OFFSET_COUNTER);
		_OFFSET_COUNTER++;
	}
}

void run_var_decl(AST *ast) {
	for (int i = 0; i < get_child_count(ast); i++) {
		AST* id = get_child(ast, i);
		set_var_offset(current_st, get_data(id), _OFFSET_COUNTER);
		if(get_child_count(id) != 0) { /* vector */
			AST* len = get_child(id, 0);
			if(get_kind(len) == NUM_NODE)
				_OFFSET_COUNTER += get_data(len);
			else
				_OFFSET_COUNTER += _STACK[_FRAME_POINTER+get_var_offset(current_st, get_data(len))];
		} else { /* simple variable */
			_OFFSET_COUNTER++;
		}
	}
	_STACK_POINTER += _OFFSET_COUNTER;
	_STACK[_STACK_POINTER++] = _LAST_SP;
	_STACK[_STACK_POINTER++] = _LAST_FP;
	_STACK[_STACK_POINTER++] = prev_st;
	_OFFSET_COUNTER = 0;
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
	int sp;
	if(get_child_count(var) == 0)	{
		sp = _FRAME_POINTER + get_var_offset(current_st, get_data(var));
	}	else {
		rec_run_ast(get_child(var, 0));
		if(get_kind(var) == VID_NODE)
			sp = _STACK[_FRAME_POINTER + get_var_offset(current_st, get_data(var))] + rpop();
		else
			sp = _FRAME_POINTER + get_var_offset(current_st, get_data(var)) + rpop();
	}
	// Compute value
	rec_run_ast(get_child(ast, 1));
	// Assign
	_STACK[sp] = rpop();
}

void run_vid_val(AST *ast) {
	// Set stack pointer
	int sp;
	rec_run_ast(get_child(ast, 0));
	sp = _STACK[_FRAME_POINTER + get_var_offset(current_st, get_data(ast))] + rpop(); // Absolute indexing
	rpush(_STACK[sp]);
}

void run_id_val(AST *ast) {
	// Set stack pointer
	int sp;
	if(get_child_count(ast) == 0) {
		sp = _FRAME_POINTER + get_var_offset(current_st, get_data(ast));
	} else {
		rec_run_ast(get_child(ast, 0));
		sp = _FRAME_POINTER + get_var_offset(current_st, get_data(ast)) + rpop();
	}
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
