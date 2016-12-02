#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "bt.h"

struct node {
    int data;
    int num_child;
    int max_child;
    NodeKind kind;
    struct node **sons;
};

BT* create_node(NodeKind kind) {
    BT* node = malloc(sizeof * node);
    node->num_child = 0;
    node->max_child = 0;
    node->data = -1;
    node->sons = NULL;
    node->kind = kind;
    return node;
}

BT* create_node_d(int data, NodeKind kind) {
    BT* node = malloc(sizeof * node);
    node->num_child = 0;
    node->max_child = 0;
    node->data = data;
    node->sons = NULL;
    node->kind = kind;
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

NodeKind get_kind(BT *node)
{
  return node->kind;
}

int get_child_count(BT *node)
{
  return node->num_child;
}

BT* get_child(BT *node, int i)
{
  return node->sons[i];
}

int get_data(BT *node)
{
  return node->data;
}

void print_node(BT *node, int level) {
    printf("%d: Node -- Addr: %p -- Kind %d -- Data: %d -- Sons: ",
           level, node, node->kind, node->data);
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

char* kind2str(NodeKind kind) {
    switch(kind) {
      case FUNC_LIST_NODE: return "FUNC_LIST_NODE";
      case STMT_SEQ_NODE: return "STMT_SEQ_NODE";
      case IF_NODE: return "IF_NODE";
      case BLOCK_NODE: return "BLOCK_NODE";
      case WHILE_NODE: return "WHILE_NODE";
      case ASSIGN_NODE: return "ASSIGN_NODE";
      case INPUT_NODE: return "INPUT_NODE";
      case OUTPUT_NODE: return "OUTPUT_NODE";
      case WRITE_NODE: return "WRITE_NODE";
      case PLUS_NODE: return "PLUS_NODE";
      case MINUS_NODE: return "MINUS_NODE";
      case TIMES_NODE: return "TIMES_NODE";
      case OVER_NODE: return "OVER_NODE";
      case LT_NODE: return "LT_NODE";
      case LE_NODE: return "LE_NODE";
      case GT_NODE: return "GT_NODE";
      case GE_NODE: return "GE_NODE";
      case EQ_NODE: return "EQ_NODE";
      case NEQ_NODE: return "NEQ_NODE";
      case NUM_NODE: return "NUM_NODE";
      case ID_NODE: return "ID_NODE";
      case RET_NODE: return "RET_NODE";
      case BOOL_EXPR_NODE: return "BOOL_EXPR_NODE";
      case FUNCTION_NODE: return "FUNCTION_NODE";
      case FHEADER_NODE: return "FHEADER_NODE";
      case FBODY_NODE: return "FBODY_NODE";
      case FPARAMETES_NODE: return "FPARAMETES_NODE";
      case ARG_LIST_NODE: return "ARG_LIST_NODE";
      case RET_INT_NODE: return "RET_INT_NODE";
      case RET_VOID_NODE: return "RET_VOID_NODE";
      case VAR_DECL_NODE: return "VAR_DECL_NODE";
      default: return "ERROR!!";
    }
}

int print_node_dot(BT *node) {
    int my_nr = nr++;
    printf("node%d[label=\"%s,%d\"];\n", my_nr, kind2str(node->kind),node->data);
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
