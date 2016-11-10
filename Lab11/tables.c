
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tables.h"

// Symbols Table
// ----------------------------------------------------------------------------

#define SYMBOL_MAX_SIZE 128
#define SYMBOL_TABLE_MAX_SIZE 100

typedef struct {
  char name[SYMBOL_MAX_SIZE];
  int line;
  int val;
} Entry;

struct sym_table {
    Entry t[SYMBOL_TABLE_MAX_SIZE];
    int size;
};

SymTable* create_sym_table() {
    SymTable *st = malloc(sizeof * st);
    st->size = 0;
    return st;
}

int get_size(SymTable* st) {
    return st->size;
}

int add_fresh(SymTable* st, char* s, int line) {
    strcpy(st->t[st->size].name, s);
    st->t[st->size].line = line;
    int old_side = st->size;
    st->size++;
    return old_side;
}

int add_or_look(SymTable* st, char* s, int line) {
    for (int i = 0; i < st->size; i++) {
        if (strcmp(st->t[i].name, s) == 0)  {
            return i;
        }
    }
    return add_fresh(st, s, line);
}

char* get_name(SymTable* st, int i) {
    return st->t[i].name;
}

int get_line(SymTable* st, int i) {
    return st->t[i].line;
}

int get_val(SymTable* st, int i) {
	return st->t[i].val;
}

void put_val(SymTable* st, int i, int ival) {
	st->t[i].val = ival;
}

void print_var_table(SymTable* st) {
    fprintf(stderr, "Variables table:\n");
    for (int i = 0; i < st->size; i++) {
         fprintf(stderr, "Entry %d -- name: %s, line: %d\n",
                 i, get_name(st, i), get_line(st, i));
    }
}

void free_sym_table(SymTable* st) {
    free(st);
}
