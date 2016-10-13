#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bt.h"

#define N_CHILD 7

struct node {
    char *data;
    int num_child;
    struct node *sons[N_CHILD];
};

BT* create_node(char* data) {
    BT* node = malloc(sizeof * node);
    node->num_child = 0;
    node->data = malloc(sizeof(char) * strlen(data));
    strcpy(node->data, data);
    return node;
}

BT* add_son(BT* father, BT* son)
{
    int num_son = father->num_child;
    father->sons[num_son] = son;
    father->num_child++;
    return father;
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
