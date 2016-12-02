%output "parser.c"
%defines "parser.h"
%define parse.error verbose
%define parse.lac full
%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bt.h"
#include "stack.h"
#include "tables.h"
#include "interpreter.h"
#define GET_MACRO(_1,_2,NAME,...) NAME
#define NN(...) GET_MACRO(__VA_ARGS__, NN2, NN1)(__VA_ARGS__)
#define NN1(type) create_node(type)
#define NN2(type,data) create_node_d(data,type)

extern int yylineno;
extern char* yytext;
int yylex(void);
void yyerror (char const *s)
{
	printf("PARSE ERROR (%d): %s\n", yylineno,s);
	exit(0);
}

BT *tree;
BT *_LAST_PARENT = NULL;
stack_type _NODE_STACK;
LitTable *lits_table;
FunTable *funs_table;
SymTable *syms_table;
int param_count;
char* current_scope;

int safe_add_fun(char* funname, int lineno, int arity)
{
	int key = lookup_fun(funs_table, funname);
  if(key == -1) {
    return add_fun(funs_table, funname, lineno, arity);
  } else {
    printf("SEMANTIC ERROR (%d): function '%s' already declared at line %d.\n",
					 lineno, funname, get_fun_line(funs_table, key));
		exit(0);
	}
}

int check_call(char* funname, int lineno, int arity)
{
	int key = lookup_fun(funs_table, funname);
  if(key == -1) {
		printf("SEMANTIC ERROR (%d): function '%s' was not declared.\n",
					 lineno, funname);
		exit(0);
  } else {
		int fun_arity = get_fun_arity(funs_table, key);
		if(fun_arity != arity) {
			printf("SEMANTIC ERROR (%d): function '%s' was called with %d arguments but declared with %d parameters.\n",
						 lineno, funname, arity, fun_arity);
			exit(0);
		}
	}
	return key;
}

int safe_add_var(char* varname, int lineno, int scope)
{
	int key = lookup_var(syms_table, varname, scope);
  if(key == -1) {
    return add_var(syms_table, varname, lineno, scope);
  } if(key == -2) {
    add_new_list(syms_table, scope);
		return add_var(syms_table, varname, lineno, scope);
  } else {
		printf("SEMANTIC ERROR (%d): variable '%s' already declared at line %d.\n",
					 lineno, varname, get_var_line(syms_table, key, scope));
    exit(0);
	}
}

int check_var(char* varname, int lineno, int scope)
{
	int key = lookup_var(syms_table, varname, scope);
  if(key < 0) {
		printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n",
					 lineno, varname);
		exit(0);
	}
	return key;
}

%}
%define api.value.type {char*}
%token ELSE IF INPUT INT OUTPUT RETURN VOID WHILE WRITE PLUS MINUS TIMES OVER LT LE GT GE EQ NEQ ASSIGN SEMI COMMA LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE NUM STRING ID
%left LT LE GT GE EQ NEQ
%left PLUS MINUS
%left TIMES OVER
%left LPAREN RPAREN

%%
program: %empty
| func_decl_list
;

func_decl_list: func_decl { add_children(tree, 1, $1); }
| func_decl_list func_decl { add_children(tree, 1, $2); }
;

func_decl: func_header func_body { $$ = NN(FUNCTION_NODE, current_scope); add_children($$, 2, $1, $2); }
;

func_header: ret_type ID LPAREN { param_count = 0; current_scope = safe_add_fun($2, yylineno, param_count); } params RPAREN { $$ = NN(FHEADER_NODE); add_children($$, 3, NN(ID_NODE, current_scope), $1, $5); }
;

func_body: LBRACE opt_var_decl opt_stmt_list RBRACE {$$ = NN(FBODY_NODE); add_children($$, 2, $2, $3); }
;

ret_type: INT { $$ = NN(RET_INT_NODE); }
| VOID { $$ = NN(RET_VOID_NODE); }
;

params: VOID { $$ = NN(FPARAMETES_NODE); }
| {
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = NN(FPARAMETES_NODE);
		_LAST_PARENT = $$;
	} param_list {
		_LAST_PARENT = spop(&_NODE_STACK);
	}
;

param_list: param	{ add_children(_LAST_PARENT, 1, $1); param_count++; }
| param_list COMMA param  { add_children(_LAST_PARENT, 1, $3); param_count++; }
;

param: INT ID	 { $$ = NN(ID_NODE, safe_add_var($2, yylineno, current_scope));  }
| INT ID LBRACK RBRACK { $$ = NN(ID_NODE, safe_add_var($2, yylineno, current_scope));  }
;

opt_var_decl: %empty { $$ = NN(VAR_DECL_NODE); }
| {
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = NN(VAR_DECL_NODE);
		_LAST_PARENT = $$;
	} var_decl_list {
		_LAST_PARENT = spop(&_NODE_STACK);
	}
;

var_decl_list: var_decl { add_children(_LAST_PARENT, 1, $1); }
| var_decl_list var_decl { add_children(_LAST_PARENT, 1, $2); }
;

var_decl: INT ID SEMI   { $$ = NN(ID_NODE, safe_add_var($2, yylineno, current_scope)); }
| INT ID LBRACK NUM RBRACK SEMI   { $$ = NN(ID_NODE, safe_add_var($2, yylineno, current_scope)); add_children($$, 1, NN(NUM_NODE, atoi($4))); }
;

opt_stmt_list: %empty { $$ = NN(STMT_SEQ_NODE); }
| {
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = NN(STMT_SEQ_NODE);
		_LAST_PARENT = $$;
	} stmt_list {
		_LAST_PARENT = spop(&_NODE_STACK);
	}
;

stmt_list: stmt { add_children(_LAST_PARENT, 1, $1); }
| stmt_list stmt { add_children(_LAST_PARENT, 1, $2); }
;

stmt: assign_stmt   { $$ = $1; }
| if_stmt   { $$ = $1; }
| while_stmt   { $$ = $1; }
| return_stmt   { $$ = $1; }
| func_call SEMI { $$ = $1; }
;

assign_stmt: lval ASSIGN arith_expr SEMI  { $$ = NN(ASSIGN_NODE); add_children($$, 2, $1, $3); }
;

lval: ID	{ $$ = NN(ID_NODE, check_var($1, yylineno, current_scope));  }
| ID LBRACK NUM RBRACK { $$ = NN(ID_NODE, check_var($1, yylineno, current_scope)); }
| ID LBRACK ID RBRACK { $$ = NN(ID_NODE, check_var($1, yylineno, current_scope)); }
;

if_stmt: IF LPAREN bool_expr RPAREN block  { $$ = NN(IF_NODE); add_children($$, 2, $3, $5); }
| IF LPAREN bool_expr RPAREN block ELSE block  { $$ = NN(IF_NODE); add_children($$, 3, $3, $5, $7); }
;

block:
	{
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = NN(BLOCK_NODE);
	} LBRACE opt_stmt_list RBRACE {
		add_children($1, 1, $3);
		_LAST_PARENT = spop(&_NODE_STACK);
		$$ = $1;
	}
;

while_stmt:
	 WHILE LPAREN bool_expr RPAREN block { $$ = NN(WHILE_NODE); add_children($$, 2, $3, $5); }
;

return_stmt: RETURN SEMI   { $$ = NN(RET_NODE); }
| RETURN arith_expr SEMI   { $$ = NN(RET_NODE); add_children($$, 1, $2); }
;

func_call: output_call   { $$ = $1; }
| write_call   { $$ = $1; }
| user_func_call { $$ = $1; }
;

input_call: INPUT LPAREN RPAREN   { $$ = NN(INPUT_NODE); }
;

output_call: OUTPUT LPAREN arith_expr RPAREN   { $$ = NN(OUTPUT_NODE); add_children($$, 1, $3); }
;

write_call: WRITE LPAREN STRING RPAREN   { $$ = NN(WRITE_NODE, add_literal(lits_table, $3)); }
;

user_func_call: ID LPAREN {param_count = 0;} opt_arg_list RPAREN { $$ = NN(FUNCTION_NODE, check_call($1, yylineno, param_count)); add_children($$, 1, $4); }
;

opt_arg_list: %empty { $$ = NN(ARG_LIST_NODE); }
| {
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = NN(ARG_LIST_NODE);
		_LAST_PARENT = $$;
	} arg_list {
		_LAST_PARENT = spop(&_NODE_STACK);
	}
;

arg_list: arith_expr { add_children(_LAST_PARENT, 1, $1); param_count++; }
| arg_list COMMA arith_expr   { add_children(_LAST_PARENT, 1, $3); param_count++; }
;

bool_expr: arith_expr bool_op arith_expr { $$ = NN(BOOL_EXPR_NODE); add_children($$, 3, $1, $2, $3); }
;

bool_op: LT { $$ = NN(LT_NODE); }
| LE { $$ = NN(LE_NODE); }
| GT { $$ = NN(GT_NODE); }
| GE { $$ = NN(GE_NODE); }
| EQ { $$ = NN(EQ_NODE); }
| NEQ { $$ = NN(NEQ_NODE); }
;

arith_expr: NUM 	{ $$ = NN(NUM_NODE, atoi($1)); }
| user_func_call { $$ = $1; }
| input_call { $$ = $1; }
| lval { $$ = $1; }
| LPAREN arith_expr RPAREN 	{ $$ = $2; }
| arith_expr PLUS arith_expr { $$ = NN(PLUS_NODE); add_children($$, 2, $1, $3); }
| arith_expr MINUS arith_expr { $$ = NN(MINUS_NODE); add_children($$, 2, $1, $3); }
| arith_expr TIMES arith_expr { $$ = NN(TIMES_NODE); add_children($$, 2, $1, $3); }
| arith_expr OVER arith_expr { $$ = NN(OVER_NODE); add_children($$, 2, $1, $3); }
;


%%

int main() {
	tree = NN(FUNC_LIST_NODE);
	screate(&_NODE_STACK, 10);
	lits_table = create_lit_table();
	funs_table = create_fun_table();
	syms_table = create_sym_table();
	yyparse();
	stdin = fopen(ctermid(NULL), "r");
	run_ast(tree, syms_table, lits_table, funs_table);

	print_lit_table(lits_table);
	printf("\n\n");
	free_lit_table(lits_table);

	print_sym_table(syms_table);
	printf("\n\n");
	free_sym_table(syms_table);

	print_fun_table(funs_table);
	printf("\n\n");
	free_fun_table(funs_table);

	freopen ("out.dot","w+",stdout);
	print_dot(tree);
  return 0;
}
