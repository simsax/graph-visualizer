#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"
#include "render.h"
#include "physics.h"

// trick from https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c
#define FOREACH_CONFIG(ACTION) \
    ACTION(RANDOM_CONFIG) \
    ACTION(COMPLETE_CONFIG) \
    ACTION(NUM_CONFIG) \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(ENUM) #ENUM,

typedef enum {
    QUIT_STATE,
    RUNNING_STATE,
    PAUSED_STATE,
    MENU_STATE
} State;

typedef enum {
    FOREACH_CONFIG(GENERATE_ENUM)
} GraphConfig;

// now that node is slimmer this may be useless
typedef struct Node {
    uint32_t label;
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
    int max_vertex_degree;
    bool directed;
    size_t n_edges;
    size_t n_nodes;
    GraphConfig config;
    Node* nodes;
    EdgeNode** adj_list;
} Graph;

extern State simulation_state;

void init_graph(Graph* graph, bool directed, size_t num_vertices, int node_radius);
void generate_graph(Graph* graph);
void free_graph(Graph* graph);
void update_graph(Graph* graph, double delta_time);
bool exists_edge(Graph* graph, size_t v1, size_t v2);
void increment_vertices(Graph* graph);
void decrement_vertices(Graph* graph);
void drag(void);
void undrag(void);
void update_cursor_position(int x, int y);

#endif // GRAPH_H
