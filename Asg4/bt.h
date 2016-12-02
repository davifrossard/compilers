#ifndef BT_H
#define BT_H

struct node;
typedef struct node BT;

#define BT_GROWTH 10 // Children leaves realloc factor

typedef enum {
  FUNC_LIST_NODE,
  FUNCTION_CALL_NODE,
  STMT_SEQ_NODE,
  IF_NODE,
  BLOCK_NODE,
  WHILE_NODE,
  ASSIGN_NODE,
  INPUT_NODE,
  OUTPUT_NODE,
  WRITE_NODE,
  PLUS_NODE,
  MINUS_NODE,
  TIMES_NODE,
  OVER_NODE,
  LT_NODE,
  LE_NODE,
  GT_NODE,
  GE_NODE,
  EQ_NODE,
  NEQ_NODE,
  NUM_NODE,
  VID_NODE,
  ID_NODE,
  RET_NODE,
  BOOL_EXPR_NODE,
  FUNCTION_NODE,
  FHEADER_NODE,
  FBODY_NODE,
  FPARAMETES_NODE,
  ARG_LIST_NODE,
  RET_INT_NODE,
  RET_VOID_NODE,
  VAR_DECL_NODE
} NodeKind;

BT* create_node(NodeKind kind);
BT* create_node_d(int data, NodeKind kind);
BT* add_children(BT* father, int num, ...);

void print_tree(BT *tree);
void free_tree(BT *tree);

NodeKind get_kind(BT *node);
int get_child_count(BT *node);
BT* get_child(BT *node, int i);
BT* adjust_type(BT *node, NodeKind kind);
int get_data(BT *node);

void print_dot(BT *tree);


#endif
