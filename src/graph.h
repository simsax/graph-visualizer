#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"
#include "node.h"

typedef struct Graph {
    NodeArray vertices;
    bool directed;
} Graph;

void init_graph(Graph* graph, size_t num_vertices);
void free_graph(Graph* graph);

#endif // GRAPH_H