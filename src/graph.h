#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"
#include "render.h"

typedef struct Node {
    uint32_t label;
    Text text;
    PointI position;
} Node;

typedef struct EdgeNode {
    uint32_t index;
    int weight;
    struct EdgeNode* next;
} EdgeNode;

typedef struct Graph {
    bool directed;
    size_t n_edges;
    size_t n_nodes;
    Node* nodes;
    EdgeNode** adj_list;
} Graph;

void init_random_graph(Graph* graph, bool directed, size_t num_vertices, size_t num_edges);
void free_graph(Graph* graph);

#endif // GRAPH_H