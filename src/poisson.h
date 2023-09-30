#ifndef POISSON_H
#define POISSON_H

typedef struct NodeArray NodeArray;

void poisson_disk_sampling(NodeArray* vertices, int radius, int k);

#endif // POISSON_H