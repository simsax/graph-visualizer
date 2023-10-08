#ifndef POISSON_H
#define POISSON_H

#include "common.h"
#include "graph.h"

void poisson_disk_sampling(Node* vertices, size_t num_vertices, int radius, int k);

#endif // POISSON_H