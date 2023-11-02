#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"
#include "render.h"
#include "physics.h"

typedef enum {
    QUIT_STATE,
    RUNNING_STATE,
    PAUSED_STATE,
    MENU_STATE
} State;

// maybe I need different scenes

typedef enum {
    RANDOM_CONFIG,
    COMPLETE_CONFIG
} GraphConfig;

typedef struct Node {
    uint32_t label;
    Text text;
    PointF position;
} Node;

typedef struct EdgeNode {
    uint32_t index;
    int weight;
    struct EdgeNode* next;
} EdgeNode;

typedef struct Graph {
    int colliding_vertex;
    int node_radius;
    bool directed;
    size_t n_edges;
    size_t n_nodes;
    Node* nodes;
    EdgeNode** adj_list;
} Graph;

extern State simulation_state;

void init_graph(Graph* graph, GraphConfig config, bool directed, size_t num_vertices, int node_radius);
void free_graph(Graph* graph);
void update_graph(Graph* graph, double delta_time);
bool exists_edge(Graph* graph, size_t v1, size_t v2);
void drag();
void undrag();
void update_cursor_position(int x, int y);

#endif // GRAPH_H
