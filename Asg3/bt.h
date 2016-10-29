#ifndef BT_H
#define BT_H

struct node;
typedef struct node BT;

#define BT_GROWTH 10 // Children leaves realloc factor

BT* create_node(char* data);
BT* add_children(BT* father, int num, ...);

void print_tree(BT *tree);
void free_tree(BT *tree);

void print_dot(BT *tree);


#endif
