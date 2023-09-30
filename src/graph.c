#include "graph.h"
#include "poisson.h"

static float calculate_radius(int num_points, int k) {
    return k / sqrt(num_points);
}

void init_graph(Graph* graph, bool directed, size_t num_vertices) {
    graph->directed = directed;
    graph->n_edges = 0;
    init_node_array(&graph->vertices);
    for (size_t i = 0; i < num_vertices; i++) {
        Node* node = new_node(i, -1, BLACK, 100);
        add_node(&graph->vertices, node);
    }

    // init vertices positions (for visualization)
    if (num_vertices > 0) {
        int radius = (int) calculate_radius(graph->vertices.count, 600);
        poisson_disk_sampling(&graph->vertices, radius, 30);
    }
}

void free_graph(Graph* graph) {
    free_node_array(&graph->vertices);
}

bool is_complete(Graph* graph) {
    return graph->n_edges * (graph->n_edges - 1) / 2 == graph->vertices.count;
}