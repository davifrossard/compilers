
#ifndef TABLES_H
#define TABLES_H

// Literals Table
// ----------------------------------------------------------------------------

// Opaque structure.
struct lit_table;
typedef struct lit_table LitTable;

// Creates an empty literal table.
LitTable* create_lit_table();

// Adds the given string to the table without repetitions.
// String 's' is copied internally.
// Returns the index of the string in the table.
int add_literal(LitTable* lt, char* s);

// Returns a pointer to the string stored at index 'i'.
char* get_literal(LitTable* lt, int i);

// Prints the given table to stdout.
void print_lit_table(LitTable* lt);

// Clear the allocated structure.
void free_lit_table(LitTable* lt);


// Symbols Table
// ----------------------------------------------------------------------------

// Opaque structure.
struct sym_table;
typedef struct sym_table SymTable;

// Creates an empty symbol table.
SymTable* create_sym_table();

// Add new scope to list
SymTable* add_new_list(SymTable* root, char* scope);

// Adds a fresh var to the table.
// No check is made by this function, so make sure to call 'lookup_var' first.
// Returns the index where the variable was inserted.
int add_var(SymTable* st, char* s, int line, char* scope);

// Returns the index where the given variable is stored or -1 otherwise.
int lookup_var(SymTable* st, char* s, char* scope);

// Returns the variable name stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
char* get_var_name(SymTable* st, int i, char* scope);

// Returns the declaration line of the variable stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
int get_var_line(SymTable* st, int i, char* scope);

// Prints the given table to stdout.
void print_sym_table(SymTable* st);

// Clear the allocated structure.
void free_sym_table(SymTable* st);


// Functions Table
// ----------------------------------------------------------------------------

// Opaque structure.
struct fun_table;
typedef struct fun_table FunTable;

// Creates an empty function table.
FunTable* create_fun_table();

// Adds a fresh function to the table.
// No check is made by this function, so make sure to call 'lookup_var' first.
// Returns the index where the variable was inserted.
int add_fun(FunTable* st, char* s, int line, int arity);

// Returns the index where the given function is stored or -1 otherwise.
int lookup_fun(FunTable* st, char* s);

// Returns the function name stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
char* get_fun_name(FunTable* st, int i);

// Returns the declaration line of the function stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
int get_fun_line(FunTable* st, int i);

// Returns the arity of the function stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
int get_fun_arity(FunTable* ft, int i);

// Prints the given table to stdout.
void print_fun_table(FunTable* st);

// Clear the allocated structure.
void free_fun_table(FunTable* st);

#endif // TABLES_H
