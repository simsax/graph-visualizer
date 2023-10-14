#ifndef SPRING_LAYOUT_H
#define SPRING_LAYOUT_H

#include "common.h"
#include "graph.h"

void spring_layout(Graph* graph, double delta_time);
void init_spring_layout(size_t num_vertices);
void free_spring_layout();

#endif // SPRING_LAYOUT_H