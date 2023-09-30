#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"
#include "node.h"

typedef struct Graph {
    bool directed;
    size_t n_edges;
    NodeArray vertices;
} Graph;

void init_graph(Graph* graph, bool directed, size_t num_vertices);
void free_graph(Graph* graph);

#endif // GRAPH_H