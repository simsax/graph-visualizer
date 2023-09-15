#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL.h"
#include "SDL_ttf.h"

#define SDL_ERROR() fprintf(stderr, "SDL_Error: %s\n", SDL_GetError())

// 0xAABBGGRR
#define R(hex) hex >> (0 * 8) & 0xFF
#define G(hex) hex >> (1 * 8) & 0xFF
#define B(hex) hex >> (2 * 8) & 0xFF
#define A(hex) hex >> (3 * 8) & 0xFF

#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF
#define RED   0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE  0xFFFF0000

#define HEX(R, G, B, A)       \
    (R & 0xFF) << (0 * 8) |   \
    (G & 0xFF) << (1 * 8) |   \
    (B & 0xFF) << (2 * 8) |   \
    (A & 0xFF) << (3 * 8) |   \

#define RGBA(hex) R(hex), G(hex), B(hex), A(hex)

void* SDL(void* ptr);

#endif // COMMON_H