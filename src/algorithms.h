#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"

#define FOREACH_ALG(ACTION) \
    ACTION(BFS_ALG) \
    ACTION(DFS_ALG) \
    ACTION(A_STAR_ALG) \
    ACTION(NUM_ALG) \

typedef enum {
    FOREACH_ALG(GENERATE_ENUM)
} Algorithm;

typedef struct {
    Algorithm algorithm;
} Alg;

void init_alg(Alg* alg);

#endif // ALGORITHMS_H
