#include "common.h"

void* SDL(void* ptr) {
    if (ptr == NULL) {
        SDL_ERROR();
        exit(EXIT_FAILURE);
    }
    return ptr;
}