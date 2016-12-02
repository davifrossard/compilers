
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tables.h"


// Adler Hash Function
uint hash_fun(char* buf, int len, int SZ) {
  uint s1 = 1;
  uint s2 = 0;
  for(int i = 0; i < len; i++) {
    s1 = (s1 + buf[i]) % 65521;
    s2 = (s1 + s2) % 65521;
  }
  return ((s2 << 16) | s1) % SZ;
}

// Literals Table
// ----------------------------------------------------------------------------

#define LITERALS_HASH_SIZE 1013

struct lit_table {
  char *t[LITERALS_HASH_SIZE];
  int size;
};

LitTable* create_lit_table() {
  LitTable *lt = malloc(sizeof * lt);
  memset(lt->t, 0, LITERALS_HASH_SIZE);
  lt->size = 0;
  return lt;
}

int add_literal(LitTable* lt, char* s) {
  int lit_len = strlen(s);
  char* value = malloc(sizeof(char) * (lit_len+1));
  strcpy(value, s);

  uint key = hash_fun(s, lit_len, LITERALS_HASH_SIZE);
  lt->t[key] = value;
  lt->size++;
  return key;
}

char* get_literal(LitTable* lt, int i) {
  return lt->t[i];
}

void print_lit_table(LitTable* lt) {
  printf("Literals table:\n");
  for(int i = 0; i < LITERALS_HASH_SIZE; i++) {
    if(lt->t[i] != 0)
    {
      printf("Entry %d -- %s\n", i, lt->t[i]);
    }
  }
}

void free_lit_table(LitTable* lt) {
  for (int i = 0; i < LITERALS_HASH_SIZE; i++) {
    if(lt->t[i] != 0)
      free(lt->t[i]);
  }
  free(lt);
}

// Variables Table
// ----------------------------------------------------------------------------

#define SYMBOLS_HASH_SIZE 1013
#define FUNCTIONS_HASH_SIZE 1013
#define NUM_SCOPES 5

typedef struct {
  char *name;
  int line;
  int offset;
} Entry;

struct sym_table {
  Entry *t[SYMBOLS_HASH_SIZE];
  int scope;
  SymTable *next;
};

SymTable* create_sym_table() {
  SymTable *st = malloc(sizeof * st);
  memset(st->t, 0, SYMBOLS_HASH_SIZE);
  st->scope = hash_fun("main", 4, FUNCTIONS_HASH_SIZE);
  st->next = NULL;
  return st;
}

int lookup_var(SymTable* st, char* s, int scope) {
  uint key = hash_fun(s, strlen(s), SYMBOLS_HASH_SIZE);
  SymTable* aux;
  for(aux = st; aux != NULL && aux->scope != scope; aux = aux->next);
  if(aux == NULL) {
    return -2;
  }
  if(aux->t[key] != 0 && strcmp(aux->t[key]->name, s) == 0)
    return key;
  return -1;
}

SymTable* get_sym_list_by_scope(SymTable* st, int scope) {
  SymTable* aux;
  for(aux = st; aux != NULL && aux->scope != scope; aux = aux->next);
  if(aux == NULL) {
    return -1;
  }
  else
    return aux;
}

SymTable* add_new_list(SymTable* root, int scope)
{
  // Create new list
  SymTable *st = malloc(sizeof * st);
  memset(st->t, 0, SYMBOLS_HASH_SIZE);
  st->scope = scope;
  st->next = NULL;

  // Apend it to structure
  SymTable* aux;
  for(aux = root; aux->next != NULL; aux = aux->next);
  aux->next = st;

  return root;
}

int add_var(SymTable* st, char* s, int line, int scope) {
  int sym_len = strlen(s);
  char* value = malloc(sizeof(char) * (sym_len+1));
  strcpy(value, s);
  Entry* e = malloc(sizeof(Entry));
  e->name = value;
  e->line = line;

  uint key = hash_fun(s, sym_len, SYMBOLS_HASH_SIZE);
  SymTable* aux;
  for(aux = st; aux != NULL && aux->scope != scope; aux = aux->next);
  aux->t[key] = e;
  return key;
}

char* get_var_name(SymTable* st, int i, int scope) {
  SymTable* aux;
  for(aux = st; aux != NULL && aux->scope != scope; aux = aux->next);
  if(aux != NULL)
    return aux->t[i]->name;
  else
    return -1;
}

int get_var_offset(SymTable* st, int i) {
  return st->t[i]->offset;
}

void set_var_offset(SymTable* st, int i, int offset) {
  st->t[i]->offset = offset;
}

int get_var_line(SymTable* st, int i, int scope) {
  SymTable* aux;
  for(aux = st; aux != NULL && aux->scope != scope; aux = aux->next);
  if(aux != NULL)
    return aux->t[i]->line;
  else
    return -1;
}

void print_sym_table(SymTable* st) {
  printf("Variables table:\n");
  SymTable* aux;
  int scope;

  for(aux = st; aux != NULL; aux = aux->next) {
    scope = aux->scope;
    for(int i = 0; i < SYMBOLS_HASH_SIZE; i++) {
      if(aux->t[i] != 0)
      {
        printf("Entry %d -- name: %s, line: %d, scope: %d, offset: %d\n",
               i, get_var_name(st, i, scope), get_var_line(st, i, scope), scope, get_var_offset(aux, i));
      }
    }
  }
}

void free_sym_table(SymTable* st) {
  SymTable* aux;
  for(aux = st; aux != NULL;) {
    for (int i = 0; i < SYMBOLS_HASH_SIZE; i++) {
      if(aux->t[i] != 0) {
        free(aux->t[i]->name);
        free(aux->t[i]);
      }
    }
    SymTable* aux2 = aux->next;
    free(aux);
    aux = aux2;
  }
}

// Functions Table
// ----------------------------------------------------------------------------

typedef struct {
  char *name;
  int line;
  int arity;
  BT* node;
} fun_entry;

struct fun_table {
  fun_entry *t[FUNCTIONS_HASH_SIZE];
  int size;
};

FunTable* create_fun_table()
{
  FunTable *ft = malloc(sizeof * ft);
  memset(ft, 0, FUNCTIONS_HASH_SIZE);
  ft->size = 0;
  return ft;
}

int add_fun(FunTable* ft, char* s, int line, int arity)
{
  int fun_len = strlen(s);
  char* value = malloc(sizeof(char) * (fun_len+1));
  strcpy(value, s);
  fun_entry* fe = malloc(sizeof(fun_entry));
  fe->name = value;
  fe->line = line;
  fe->arity = arity;

  uint key = hash_fun(s, fun_len, FUNCTIONS_HASH_SIZE);
  ft->t[key] = fe;
  ft->size++;
  return key;
}

int hash_fun_name(const char* name) {
  return hash_fun(name, strlen(name), FUNCTIONS_HASH_SIZE);
}

int lookup_fun(FunTable* ft, char* s) {
  uint key = hash_fun(s, strlen(s), FUNCTIONS_HASH_SIZE);
  if(ft->t[key] != 0 && strcmp(ft->t[key]->name, s) == 0)
    return key;
  return -1;
}

char* get_fun_name(FunTable* ft, int i) {
  return ft->t[i]->name;
}

void set_fun_node(FunTable* ft, int i, BT* node)
{
  ft->t[i]->node = node;
}

BT* get_fun_node(FunTable* ft, int i)
{
  return ft->t[i]->node;
}

int get_fun_line(FunTable* ft, int i) {
  return ft->t[i]->line;
}

int get_fun_arity(FunTable* ft, int i) {
  return ft->t[i]->arity;
}

void print_fun_table(FunTable* ft) {
  printf("Functions table:\n");
  for(int i = 0; i < SYMBOLS_HASH_SIZE; i++) {
    if(ft->t[i] != 0) {
      printf("Entry %d -- name: %s, line: %d, arity: %d, node: %p\n",
             i, get_fun_name(ft, i), get_fun_line(ft, i), get_fun_arity(ft, i), get_fun_node(ft, i));
    }
  }
}

void free_fun_table(FunTable* ft) {
  for (int i = 0; i < FUNCTIONS_HASH_SIZE; i++) {
    if(ft->t[i] != 0) {
      free(ft->t[i]->name);
      free(ft->t[i]);
    }
  }
  free(ft);
}
