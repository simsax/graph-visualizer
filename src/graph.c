#include "graph.h"
#include "poisson.h"


static void init_node(Node* node, uint32_t label, uint32_t label_color, int label_size) {
    char label_str[5];
    node->label = label;
    sprintf(&label_str[0], "%d", node->label);
    init_text(&node->text, &label_str[0], label_color, label_size);
    node->position = (PointI){-1, -1};
}

static float calculate_radius(int num_points, int k) {
    return k / sqrt(num_points);
}

static bool is_complete(size_t num_edges, size_t num_vertices) {
    return num_edges * (num_edges - 1) / 2 >= num_vertices;
}

// bool is_complete(Graph* graph) {
//     return graph->n_edges * (graph->n_edges - 1) / 2 == graph->vertices.count;
// }

static EdgeNode* new_edge_node(uint32_t index, int weight, EdgeNode* next) {
    EdgeNode* node = malloc(sizeof(EdgeNode));
    if (node == NULL) {
        fprintf(stderr, "Failed to allocate edge node.\n");
        exit(EXIT_FAILURE);
    }
    node->index = index;
    node->weight = weight;
    node->next = next;
    return node;
}

static void add_edge(Graph* graph, size_t source, size_t dest, int weight) {
    graph->adj_list[source] = new_edge_node(dest, weight, graph->adj_list[source]);
    graph->n_edges++;
}

static void generate_complete_graph_undirected(Graph* graph) {
    size_t n_vertices = graph->n_nodes;
    for (size_t i = 0; i < n_vertices; i++) {
        for (size_t j = i + 1; j < n_vertices; j++) {
            add_edge(graph, i, j, 0);
            add_edge(graph, j, i, 0);
        }
    }
}

static void generate_random_graph_undirected(Graph* graph) {
    // TODO
    (void)graph;
}

static void generate_random_edges(Graph* graph, size_t num_edges, bool directed) {
    if (!directed) {
        if (is_complete(num_edges, graph->n_nodes)) {
            generate_complete_graph_undirected(graph);
        }
        else {
            generate_random_graph_undirected(graph);
        }
    } else {
        // TODO
    }
}

static void generate_random_nodes(Graph* graph, size_t num_vertices) {
    graph->n_nodes = num_vertices;
    graph->nodes = malloc(sizeof(Node) * num_vertices);
    if (graph->nodes == NULL) {
        fprintf(stderr, "Failed to allocate graph vertices.\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < num_vertices; i++) {
        init_node(&graph->nodes[i], i, BLACK, 100);
    }
}

static void init_adj_list(Graph* graph) {
    graph->adj_list = malloc(sizeof(EdgeNode*) * graph->n_nodes);
    if (graph->adj_list == NULL) {
        fprintf(stderr, "Failed to allocate graph adjacency list.\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < graph->n_nodes; i++) {
        graph->adj_list[i] = NULL;
    }
}

void init_random_graph(Graph* graph, bool directed, size_t num_vertices, size_t num_edges) {
    if (num_vertices == 0) {
        fprintf(stderr, "Cannot generate a graph with 0 vertices.\n");
        exit(EXIT_FAILURE);
    }

    graph->directed = directed;
    graph->n_edges = 0;

    generate_random_nodes(graph, num_vertices);

    // init vertices positions (for visualization)
    int radius = (int) calculate_radius(graph->n_nodes, 600);
    poisson_disk_sampling(graph->nodes, graph->n_nodes, radius, 30);

    init_adj_list(graph);
    generate_random_edges(graph, num_edges, directed);
}

void free_graph(Graph* graph) {
    for (size_t i = 0; i < graph->n_nodes; i++) {
        free_text(&graph->nodes[i].text);
    }
    free(graph->nodes);
    for (size_t i = 0; i < graph->n_nodes; i++) {
        for (EdgeNode* node = graph->adj_list[i]; node != NULL;) {
            EdgeNode* temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(graph->adj_list);
}
