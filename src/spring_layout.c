#include "spring_layout.h"
#include "render.h"

#define GRAVITY 200
#define REPULSION 100000
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

void init_spring_layout(size_t num_vertices) {
    forces = malloc(sizeof(Vec2) * num_vertices);
    if (forces == NULL) {
        fprintf(stderr, "Failed to allocate array of forces.\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < num_vertices; i++) {
        forces[i] = (Vec2) {.x = 0, .y = 0};
    }

    int screen_width = get_window_width();
    int screen_height = get_window_height();
    origin = vec2(screen_width / 2, screen_height / 2);

}

void free_spring_layout() {
    free(forces);
}

void spring_layout(Node* vertices, size_t num_vertices, double delta_time) {
    for (size_t i = 0; i < num_vertices; i++) {
        // force towards center of canvas
        forces[i] = vec2_mul_scalar(vec2_normalize(vec2_sub(origin, vertices[i].position)), GRAVITY);

        // repulsive force between nodes
        for (size_t j = 0; j < num_vertices; j++) {
            if (i != j) {
                Vec2 direction = vec2_sub(vertices[i].position, vertices[j].position);
                float sqr_radius = sqr_distance(direction);
                direction = vec2_normalize(direction);
                forces[i] = vec2_add(forces[i], vec2_mul_scalar(direction, REPULSION / sqr_radius));
            }
        }
    }

    for (size_t i = 0; i < num_vertices; i++) {
        vertices[i].position = vec2_add(vertices[i].position, vec2_mul_scalar(forces[i], delta_time));
    }
}
