#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "bt.h"

struct node {
    char *data;
    int num_child;
    int max_child;
    struct node **sons;
};

BT* create_node(char* data) {
    BT* node = malloc(sizeof * node);
    node->num_child = 0;
    node->max_child = 0;
    node->data = data;
    node->sons = NULL;
    return node;
}

BT* add_children(BT* father, int num, ... )
{
    va_list children;
    va_start(children, num);
    int num_children = father->num_child;
    int max_children = father->max_child;
    int demand = num_children + num;
    if(max_children < demand)
    {
      int new_size = demand + (BT_GROWTH - demand % BT_GROWTH);
      father->sons = realloc(father->sons, sizeof(BT*) * new_size);
    }
    for(int i=num_children; i<(num_children+num); i++)
    {
      father->sons[i] = va_arg(children, BT*);
      father->num_child++;
    }
    return father;
    va_end(children);
}


void print_node(BT *node, int level) {
    printf("%d: Node -- Addr: %p -- Data: %s -- Sons: ",
           level, node, node->data);
    int i;
    for(i=0;i<node->num_child;i++)
        printf("%p ", node->sons[i]);
    printf("\n");
}

void print_bt(BT *tree, int level) {
    print_node(tree, level);
    int i;
    for(i=0; i<tree->num_child;i++)
        print_node(tree->sons[i], level+1);
}

void print_tree(BT *tree) {
    print_bt(tree, 0);
}

void free_tree(BT *tree) {
    if (tree != NULL) {
        int i;
        for(i=0;i<tree->num_child;i++)
            free_tree(tree->sons[i]);
        free(tree->data);
        free(tree);
    }
}

// Dot output.
int nr;

int print_node_dot(BT *node) {
    int my_nr = nr++;
    printf("node%d[label=\"%s\"];\n", my_nr, node->data);
    int i;
    for(i=0; i<node->num_child; i++)
    {
        int l_nr = print_node_dot(node->sons[i]);
        printf("node%d -> node%d;\n", my_nr, l_nr);
    }
    return my_nr;
}

void print_dot(BT *tree) {
    nr = 0;
    printf("digraph {\ngraph [ordering=\"out\"];\n");
    print_node_dot(tree);
    printf("}\n");
}
