#include "spring_layout.h"
#include "render.h"

#define GRAVITY 100
#define SIMULATION_VELOCITY 2
#define SPRING_CONSTANT 1
// #define REPULSION 200

typedef PointF Vec2;
Vec2* forces;
Vec2 origin;

static Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2){a.x + b.x, a.y + b.y};
}

static Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return (Vec2){a.x - b.x, a.y - b.y};
}

static Vec2 vec2_mul(Vec2 a, Vec2 b) {
    return (Vec2){a.x * b.x, a.y * b.y};
}

static Vec2 vec2_div(Vec2 a, Vec2 b) {
    return (Vec2){a.x / b.x, a.y / b.y};
}

static Vec2 vec2(int x, int y) {
    return (Vec2){x, y};
}

static float sqr_distance(Vec2 a) {
    return a.x * a.x + a.y * a.y;
}

static float length(Vec2 a) {
    return sqrt(sqr_distance(a));
}

static Vec2 vec2_normalize(Vec2 a) {
    float m = length(a);
    return (Vec2) {a.x / m, a.y / m};
}

static Vec2 vec2_mul_scalar(Vec2 a, float b) {
    return (Vec2) {a.x * b, a.y * b};
}

static Vec2 vec2_div_scalar(Vec2 a, float b) {
    return (Vec2) {a.x / b, a.y / b};
}

void reset_forces(size_t length) {
    memset(forces, 0, sizeof(Vec2) * length);
}

void init_spring_layout(size_t num_vertices) {
    forces = malloc(sizeof(Vec2) * num_vertices);
    if (forces == NULL) {
        fprintf(stderr, "Failed to allocate array of forces.\n");
        exit(EXIT_FAILURE);
    }
    reset_forces(num_vertices);

    int screen_width = get_window_width();
    int screen_height = get_window_height();
    origin = vec2(screen_width / 2, screen_height / 2);

}

void free_spring_layout() {
    free(forces);
}

void spring_layout(Graph* graph,  double delta_time) {
    reset_forces(graph->n_nodes);
    int width = get_window_width();
    int height = get_window_height();

    float l = fmin(width, height) * 0.5 / sqrt(graph->n_nodes);
    // printf("l: %f\n", l);
    // float l = 200;
    Node* vertices = graph->nodes;
    double velocity = delta_time * SIMULATION_VELOCITY;
    size_t max_iterations = 1000;
    static float t = 0.99;
    float dt = 0.99 / (max_iterations + 1);
    for (size_t i = 0; i < graph->n_nodes; i++) {
        // force towards center of canvas
        forces[i] = vec2_mul_scalar(vec2_normalize(vec2_sub(origin, vertices[i].position)), GRAVITY);

        // repulsive force between nodes
        for (size_t j = 0; j < graph->n_nodes; j++) {
            if (i != j) {
                Vec2 direction = vec2_sub(vertices[i].position, vertices[j].position);
                float distance = length(direction);
                Vec2 direction_norm = vec2_div_scalar(direction, distance);
                forces[i] = vec2_add(forces[i], vec2_mul_scalar(direction_norm, l * l / distance));
            }
        }

        // attractive force between nodes
        for (EdgeNode* edge_node = graph->adj_list[i]; edge_node != NULL; edge_node = edge_node->next) {
            uint32_t dest = edge_node->index;
            Vec2 direction = vec2_sub(vertices[dest].position, vertices[i].position);
            float distance2 = sqr_distance(direction);
            Vec2 direction_norm = vec2_div_scalar(direction, sqrt(distance2));
            forces[i] = vec2_add(forces[i], vec2_mul_scalar(direction_norm, distance2 / l));
        }
    }

    for (size_t i = 0; i < graph->n_nodes; i++) {
        // prevent from going outside of frame
        vertices[i].position = vec2_add(vertices[i].position, vec2_mul_scalar(forces[i], t * velocity));
        vertices[i].position.x = fmin(width, fmax(0, vertices[i].position.x));
        vertices[i].position.y = fmin(height, fmax(0, vertices[i].position.y));
    }
    t -= dt;
    t = fmax(0, t);
}

// current problems: how to choose l with respect to the number of nodes
// how to balance gravity
// find a more aesthetic way to generate random graphs (forest fire propagation)