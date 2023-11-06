#include "graph.h"

State simulation_state = MENU_STATE;
static bool dragging = false;
static PointI cursor_position;

static void init_node(Node* node, uint32_t label, uint32_t label_color, int label_size)
{
    char label_str[5];
    node->label = label;
    sprintf(&label_str[0], "%d", node->label);
    init_text(&node->text, &label_str[0], label_color, label_size);
    node->position = (PointF) { -1, -1 };
}

static float calculate_radius(int num_points, int k) { return k / sqrt(num_points); }

static bool is_complete(size_t num_edges, size_t num_vertices)
{
    return num_edges * (num_edges - 1) / 2 >= num_vertices;
}

// bool is_complete(Graph* graph) {
//     return graph->n_edges * (graph->n_edges - 1) / 2 == graph->vertices.count;
// }

static EdgeNode* new_edge_node(uint32_t index, EdgeNode* next, int weight)
{
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

static void add_edge(Graph* graph, size_t source, size_t dest, int weight)
{
    graph->adj_list[source] = new_edge_node(dest, graph->adj_list[source], weight);

    if (!graph->directed)
        graph->adj_list[dest] = new_edge_node(source, graph->adj_list[dest], weight);

    graph->n_edges++;
}

static void generate_complete_graph_undirected(Graph* graph)
{
    size_t n_vertices = graph->n_nodes;
    for (size_t i = 0; i < n_vertices; i++) {
        for (size_t j = i + 1; j < n_vertices; j++) {
            add_edge(graph, i, j, 1);
        }
    }
}

static void generate_random_undirected(Graph* graph)
{
    size_t n_vertices = graph->n_nodes;
    int max_vertex_degree = 3;
    float edge_probability = 0.4f;
    // array of nodes counter, stop iterating when counter is over a given threshold
    int* vertex_degree = calloc(n_vertices, sizeof(int));
    if (vertex_degree == NULL) {
        fprintf(stderr, "Calloc failed.\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < n_vertices; i++) {
        for (size_t j = i + 1; j < n_vertices; j++) {
            if (vertex_degree[i] >= max_vertex_degree)
                break;
            if (vertex_degree[j] >= max_vertex_degree)
                continue;
            if (rand_uniform() < edge_probability) {
                vertex_degree[i]++;
                vertex_degree[j]++;
                add_edge(graph, i, j, 1);
            }
        }
    }

    free(vertex_degree);
}

static void print_adj_lsit(Graph* graph)
{
    for (size_t i = 0; i < graph->n_nodes; i++) {
        printf("Node %zu: ", i);
        for (EdgeNode* edge_node = graph->adj_list[i]; edge_node != NULL;
             edge_node = edge_node->next) {
            printf("%d -> ", edge_node->index);
        }
        printf(" NULL\n");
    }
    printf("\n");
}

static void generate_complete_graph(Graph* graph)
{
    if (!graph->directed) {
        generate_complete_graph_undirected(graph);
    } else {
        // TODO
    }
}


static void generate_random_edges(Graph* graph)
{
    if (!graph->directed) {
        generate_random_undirected(graph);
    } else {
        // TODO
    }
}

static void generate_nodes(Graph* graph)
{
    graph->nodes = malloc(sizeof(Node) * graph->n_nodes);
    if (graph->nodes == NULL) {
        fprintf(stderr, "Failed to allocate graph vertices.\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < graph->n_nodes; i++) {
        init_node(&graph->nodes[i], i, BLACK, 100);
    }
}

static void init_adj_list(Graph* graph)
{
    graph->adj_list = malloc(sizeof(EdgeNode*) * graph->n_nodes);
    if (graph->adj_list == NULL) {
        fprintf(stderr, "Failed to allocate graph adjacency list.\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < graph->n_nodes; i++) {
        graph->adj_list[i] = NULL;
    }
}

/* static void generate_nodes_positions_poisson(Graph* graph) */
/* { */
/*     int radius = (int)calculate_radius(graph->n_nodes, 600); */
/*     poisson_disk_sampling(graph->nodes, graph->n_nodes, radius, 30); */
/* } */

static void generate_nodes_positions_random(Graph* graph)
{
    int width = get_window_width();
    int height = get_window_height();
    for (size_t i = 0; i < graph->n_nodes; i++) {
        graph->nodes[i].position
            = (PointF) { .x = rand_uniform() * width, .y = rand_uniform() * height };
    }
}

// static void generate_nodes_positions_spring(Graph* graph) {
//     spring_layout(graph->nodes, graph->n_nodes);
// }

void init_graph(
    Graph* graph, GraphConfig config, bool directed, size_t num_vertices, int node_radius)
{
    if (num_vertices == 0) {
        fprintf(stderr, "Cannot generate a graph with 0 vertices.\n");
        exit(EXIT_FAILURE);
    }

    graph->directed = directed;
    graph->n_edges = 0;
    graph->n_nodes = num_vertices;
    graph->node_radius = node_radius;
    graph->colliding_vertex = -1;

    init_adj_list(graph);
    generate_nodes(graph);

    switch (config) {
    case RANDOM_CONFIG:
        generate_random_edges(graph);
        break;
    case COMPLETE_CONFIG:
        generate_complete_graph(graph);
        break;
    default:
        break;
    }

    generate_nodes_positions_random(graph);
    // generate_nodes_positions_spring(graph);

    init_spring_layout(graph->n_nodes);
    // spring_layout(graph);
}

void free_graph(Graph* graph)
{
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

static void check_vertex_cursor_collision(Graph* graph)
{
    if (dragging) {
        // move dragged vertex to cursor position
        if (graph->colliding_vertex != -1) {
            Node* vertex = &graph->nodes[graph->colliding_vertex];
            vertex->position = (PointF) { cursor_position.x, cursor_position.y };
        }
    } else {
        Node* vertices = graph->nodes;
        int radius = graph->node_radius;
        bool found = false;
        for (size_t i = 0; i < graph->n_nodes && !found; i++) {
            Aabb vertex_aabb = { vertices[i].position.x - radius, vertices[i].position.x + radius,
                vertices[i].position.y - radius, vertices[i].position.y + radius };
            if (intersect_point(&vertex_aabb, cursor_position)) {
                graph->colliding_vertex = i;
                found = true;
            }
        }
        if (!found) {
            graph->colliding_vertex = -1;
        }
    }
}

void update_graph(Graph* graph, double delta_time)
{
    if (simulation_state == RUNNING_STATE) {
        spring_layout_live(graph, delta_time);
        check_vertex_cursor_collision(graph);
    }
}

bool exists_edge(Graph* graph, size_t v1, size_t v2)
{
    for (EdgeNode* edge_node = graph->adj_list[v1]; edge_node != NULL;
         edge_node = edge_node->next) {
        if (edge_node->index == v2)
            return true;
    }
    return false;
}

void drag() { dragging = true; }

void undrag() { dragging = false; }

void update_cursor_position(int x, int y)
{
    // called only when mouse is moved
    cursor_position.x = x;
    cursor_position.y = y;
}
