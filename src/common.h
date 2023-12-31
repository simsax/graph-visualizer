#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL.h"

#define DEBUG_UI 0
#define SDL_ERROR() fprintf(stderr, "SDL_Error: %s\n", SDL_GetError())

typedef struct {
    float x;
    float y;
} PointF;

typedef struct {
    int x;
    int y;
} PointI;


void* SDL(void* ptr);
float rand_uniform(void);

#endif // COMMON_H
