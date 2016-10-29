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
#define N(x) create_node(x)

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

void safe_add_fun(char* funname, int lineno, int arity)
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

void check_call(char* funname, int lineno, int arity)
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
}

void safe_add_var(char* varname, int lineno, char* scope)
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

void check_var(char* varname, int lineno, char* scope)
{
	int key = lookup_var(syms_table, varname, scope);
  if(key < 0) {
		printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n",
					 lineno, varname);
		exit(0);
	}
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

func_decl: func_header func_body { $$ = create_node("function"); add_children($$, 2, $1, $2); }
;

func_header: ret_type ID LPAREN { param_count = 0; current_scope = $2; } params RPAREN { $$ = create_node("function_header"); add_children($$, 3, N($2), $1, $5); safe_add_fun($2, yylineno, param_count); }
;

func_body: LBRACE opt_var_decl opt_stmt_list RBRACE {$$ = create_node("function_body"); add_children($$, 2, $2, $3); }
;

ret_type: INT { $$ = create_node("return_type"); add_children($$, 1, N($1)); }
| VOID { $$ = create_node("return_type"); add_children($$, 1, N($1)); }
;

params: VOID { $$ = create_node("function_parameters"); }
| {
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = create_node("function_parameters");
		_LAST_PARENT = $$;
	} param_list {
		_LAST_PARENT = spop(&_NODE_STACK);
	}
;

param_list: param	{ add_children(_LAST_PARENT, 1, $1); param_count++; }
| param_list COMMA param  { add_children(_LAST_PARENT, 1, $3); param_count++; }
;

param: INT ID	 { $$ = N($2); safe_add_var($2, yylineno, current_scope); }
| INT ID LBRACK RBRACK { $$ = N($2); safe_add_var($2, yylineno, current_scope); }
;

opt_var_decl: %empty { $$ = create_node("var_decl_list"); }
| {
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = create_node("var_decl_list");
		_LAST_PARENT = $$;
	} var_decl_list {
		_LAST_PARENT = spop(&_NODE_STACK);
	}
;

var_decl_list: var_decl { add_children(_LAST_PARENT, 1, $1); }
| var_decl_list var_decl { add_children(_LAST_PARENT, 1, $2); }
;

var_decl: INT ID SEMI   { $$ = N($1); add_children($$, 1, N($2)); safe_add_var($2, yylineno, current_scope); }
| INT ID LBRACK NUM RBRACK SEMI   { $$ = create_node("array_declaration"); $1 = add_children(N($1), 2, N($2), N($4)); add_children($$, 1, $1); safe_add_var($2, yylineno, current_scope); }
;

opt_stmt_list: %empty { $$ = create_node("statement_list"); }
| {
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = create_node("statement_list");
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

assign_stmt: lval ASSIGN arith_expr SEMI  { $$ = create_node("assign_stmt"); add_children($$, 2, $1, $3); }
;

lval: ID	{ $$ = N($1); check_var($1, yylineno, current_scope); }
| ID LBRACK NUM RBRACK { $$ = create_node("array_access"); add_children($$, 2, N($1), N($3)); check_var($1, yylineno, current_scope); }
| ID LBRACK ID RBRACK { $$ = create_node("array_access"); add_children($$, 2, N($1), N($3)); check_var($1, yylineno, current_scope); check_var($3, yylineno, current_scope); }
;

if_stmt: IF LPAREN bool_expr RPAREN block  { $$ = create_node("if_stmt"); add_children($$, 3, N($1), $3, $5); }
| IF LPAREN bool_expr RPAREN block ELSE block  { $$ = create_node("if_stmt"); add_children($$, 5, N($1), $3, $5, N($6), $7); }
;

block:
	{
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = create_node("block");
	} LBRACE opt_stmt_list RBRACE {
		add_children($1, 1, $3);
		_LAST_PARENT = spop(&_NODE_STACK);
		$$ = $1;
	}
;

while_stmt:
	 WHILE LPAREN bool_expr RPAREN block { $$ = create_node("while_stmt"); add_children($$, 3, N($1), $3, $5); }
;

return_stmt: RETURN SEMI   { $$ = create_node("return_stmt"); add_children($$, 2, N($1), N($2)); }
| RETURN arith_expr SEMI   { $$ = create_node("return_stmt"); add_children($$, 2, N($1), $2); }
;

func_call: output_call   { $$ = $1; }
| write_call   { $$ = $1; }
| user_func_call { $$ = $1; }
;

input_call: INPUT LPAREN RPAREN   { $$ = create_node("input_call"); }
;

output_call: OUTPUT LPAREN arith_expr RPAREN   { $$ = create_node("output_call"); add_children($$, 2, N($1), $3); }
;

write_call: WRITE LPAREN STRING RPAREN   { $$ = create_node("write_call"); add_literal(lits_table, $3); }
;

user_func_call: ID LPAREN {param_count = 0;} opt_arg_list RPAREN { $$ = create_node("func_call"); add_children($$, 2, N($1), $4); check_call($1, yylineno, param_count); }
;

opt_arg_list: %empty { $$ = create_node("arg_list"); }
| {
		spush(&_NODE_STACK, _LAST_PARENT);
		$$ = create_node("arg_list");
		_LAST_PARENT = $$;
	} arg_list {
		_LAST_PARENT = spop(&_NODE_STACK);
	}
;

arg_list: arith_expr { add_children(_LAST_PARENT, 1, $1); param_count++; }
| arg_list COMMA arith_expr   { add_children(_LAST_PARENT, 1, $3); param_count++; }
;

bool_expr: arith_expr bool_op arith_expr { $$ = create_node("bool_expr"); add_children($$, 3, $1, $2, $3); }
;

bool_op: LT { $$ = N($1); }
| LE { $$ = N($1); }
| GT { $$ = N($1); }
| GE { $$ = N($1); }
| EQ { $$ = N($1); }
| NEQ { $$ = N($1); }
;

arith_expr: NUM 	{ $$ = N($1); }
| user_func_call { $$ = $1; }
| input_call { $$ = $1; }
| lval { $$ = $1; }
| LPAREN arith_expr RPAREN 	{ $$ = create_node("arith_expr"); add_children($$, 3, N($1), $2, N($3)); }
| arith_expr PLUS arith_expr { $$ = create_node("arith_expr"); add_children($$, 3, $1, N($2), $3); }
| arith_expr MINUS arith_expr { $$ = create_node("arith_expr"); add_children($$, 3, $1, N($2), $3); }
| arith_expr TIMES arith_expr { $$ = create_node("arith_expr"); add_children($$, 3, $1, N($2), $3); }
| arith_expr OVER arith_expr { $$ = create_node("arith_expr"); add_children($$, 3, $1, N($2), $3); }
;


%%

int main() {
	tree = create_node("program");
	screate(&_NODE_STACK, 10);
	lits_table = create_lit_table();
	funs_table = create_fun_table();
	syms_table = create_sym_table();
	yyparse();
  printf("PARSE SUCESSFUL!\n");

	/*print_lit_table(lits_table);
  printf("\n\n");
	free_lit_table(lits_table);

	print_sym_table(syms_table);
	printf("\n\n");
	free_sym_table(syms_table);

	print_fun_table(funs_table);
	printf("\n\n");
	free_fun_table(funs_table);

	freopen ("out.dot","w+",stdout);
	print_dot(tree);*/
  return 0;
}
