#ifndef BT_H
#define BT_H

struct node; // Opaque structure to ensure encapsulation.

typedef struct node BT;

BT* create_node(char* data);
BT* add_son(BT* father, BT* son);

void print_tree(BT *tree);
void free_tree(BT *tree);

void print_dot(BT *tree);


#endif
