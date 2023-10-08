#include "common.h"

void* SDL(void* ptr) {
    if (ptr == NULL) {
        SDL_ERROR();
        exit(EXIT_FAILURE);
    }
    return ptr;
}

float rand_uniform() {
    return (float) rand() / (float) RAND_MAX;
}