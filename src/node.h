#ifndef NODE_H
#define NODE_H

#include "common.h"
#include "render.h"

typedef struct Node {
    uint32_t label;
    Text text;
    int weight;
    struct Node* next;
    PointI position; // gonna need it when drawing the edges of the graph
} Node;
// number of edges should be a parameter (of a future ui, with real time updates)

typedef struct NodeArray {
    size_t count;
    size_t capacity;
    Node** nodes;
} NodeArray;

Node* new_node(uint32_t label, int weight, uint32_t label_color, int label_size);
void init_node_array(NodeArray* node_array);
void add_node(NodeArray* node_array, Node* node);
void free_node_array(NodeArray* node_array);

#endif // NODE_H
