#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL.h"
#include "SDL_ttf.h"

#define DEBUG_UI 1
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
float rand_uniform();

#endif // COMMON_H
