#include "graph.h"

void init_graph(Graph* graph, size_t num_vertices) {
    init_node_array(&graph->vertices);
    for (size_t i = 0; i < num_vertices; i++) {
        Node* node = new_node(i, -1, BLACK, 100);
        add_node(&graph->vertices, node);
    }
}

void free_graph(Graph* graph) {
    free_node_array(&graph->vertices);
}