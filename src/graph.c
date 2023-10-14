#include "graph.h"
#include "poisson.h"
#include "spring_layout.h"


static void init_node(Node* node, uint32_t label, uint32_t label_color, int label_size) {
    char label_str[5];
    node->label = label;
    sprintf(&label_str[0], "%d", node->label);
    init_text(&node->text, &label_str[0], label_color, label_size);
    node->position = (PointF){-1, -1};
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

static EdgeNode* new_edge_node(uint32_t index, EdgeNode* next, int weight) {
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
    graph->adj_list[source] = new_edge_node(dest, graph->adj_list[source], weight);

    if (!graph->directed)
        graph->adj_list[dest] = new_edge_node(source, graph->adj_list[dest], weight);

    graph->n_edges++;
}

static void generate_complete_graph_undirected(Graph* graph, size_t num_edges) {
    // first dumb approach: try to generate complete graph
    // until there are `num_edges` connected
    size_t n_vertices = graph->n_nodes;
    for (size_t i = 0; i < n_vertices && graph->n_edges < num_edges; i++) {
        for (size_t j = i + 1; j < n_vertices && graph->n_edges < num_edges; j++) {
            add_edge(graph, i, j, 1);
        }
    }
}

static void generate_random_undirected(Graph* graph) {
    size_t n_vertices = graph->n_nodes;
    float edge_probability = 0.03f;
    for (size_t i = 0; i < n_vertices; i++) {
        for (size_t j = i + 1; j < n_vertices; j++) {
            if (rand_uniform() < edge_probability)
                add_edge(graph, i, j, 1);
        }
    }
}

static void print_adj_lsit(Graph* graph) {
    for (size_t i = 0; i < graph->n_nodes; i++) {
        printf("Node %d: ", i);
        for (EdgeNode* edge_node = graph->adj_list[i]; edge_node != NULL; edge_node = edge_node->next) {
            printf("%d -> ", edge_node->index);
        }
        printf(" NULL\n");
    }
    printf("\n");
}

static void generate_random_edges(Graph* graph, size_t num_edges) {
    if (!graph->directed) {
        // generate_complete_graph_undirected(graph, num_edges);
        generate_random_undirected(graph);
    } else {
        // TODO
    }

}

static void generate_nodes(Graph* graph) {
    graph->nodes = malloc(sizeof(Node) * graph->n_nodes);
    if (graph->nodes == NULL) {
        fprintf(stderr, "Failed to allocate graph vertices.\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < graph->n_nodes; i++) {
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

static void generate_nodes_positions_poisson(Graph* graph) {
    int radius = (int) calculate_radius(graph->n_nodes, 600);
    poisson_disk_sampling(graph->nodes, graph->n_nodes, radius, 30);
}

static void generate_nodes_positions_random(Graph* graph) {
    int width = get_window_width();
    int height = get_window_height();
    for (size_t i = 0; i < graph->n_nodes; i++) {
        graph->nodes[i].position = (PointF) {
            .x = rand_uniform() * width,
            .y = rand_uniform() * height
        };
    }
}

// static void generate_nodes_positions_spring(Graph* graph) {
//     spring_layout(graph->nodes, graph->n_nodes);
// }

void init_random_graph(Graph* graph, bool directed, size_t num_vertices, size_t num_edges) {
    if (num_vertices == 0) {
        fprintf(stderr, "Cannot generate a graph with 0 vertices.\n");
        exit(EXIT_FAILURE);
    }

    graph->directed = directed;
    graph->n_edges = 0;
    graph->n_nodes = num_vertices;

    init_adj_list(graph);
    generate_nodes(graph);
    generate_random_edges(graph, num_edges);
    generate_nodes_positions_random(graph);
    // generate_nodes_positions_spring(graph);

    init_spring_layout(graph->n_nodes);
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

    free_spring_layout();
}

void update_graph(Graph* graph, double delta_time) {
    spring_layout(graph, delta_time);
}

bool exists_edge(Graph* graph, size_t v1, size_t v2) {
    for (EdgeNode* edge_node = graph->adj_list[v1]; edge_node != NULL; edge_node = edge_node->next) {
        if (edge_node->index == v2)
            return true;
    }
    return false;
}