#ifndef SPRING_LAYOUT_H
#define SPRING_LAYOUT_H

#include "common.h"
#include "graph.h"

void spring_layout(Node* vertices, size_t num_vertices, double delta_time);
void init_spring_layout(size_t num_vertices);
void free_spring_layout();

#endif // SPRING_LAYOUT_H