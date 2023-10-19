#ifndef PHYSICS_H
#define PHYSICS_H

#include "common.h"

typedef struct {
    float min_x;
    float max_x;
    float min_y;
    float max_y;
} Aabb;

typedef struct Graph Graph;

bool intersect(Aabb* a, Aabb* b);
bool intersect_point(Aabb* a, PointI point);
void spring_layout_live(Graph* graph, double delta_time);
void spring_layout(Graph* graph);
void init_spring_layout(size_t num_vertices);
void free_spring_layout();

#endif // PHYSICS_H