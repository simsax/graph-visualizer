#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "extern/sdl2_gfx/SDL2_gfxPrimitives.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

void sdl_error(const char* message)
{
    fprintf(stderr, "%s\nSDL_Error: %s\n", message, SDL_GetError());
}

int main(void)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        sdl_error("SDL initialization failed.");
        exit(EXIT_FAILURE);
    }

    SDL_Window* window = SDL_CreateWindow("Graph Visualizer", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        sdl_error("Window creation failed.");
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer
        = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        sdl_error("Renderer creation failed.");
        SDL_DestroyWindow(window);
        exit(EXIT_FAILURE);
    }

    bool quit = false;
    while (quit == false) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            default:
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
        SDL_Rect rectangle = {
            .x = SCREEN_WIDTH / 4,
            .y = SCREEN_HEIGHT / 4,
            .w = SCREEN_WIDTH / 2,
            .h = SCREEN_HEIGHT / 2
        };
        SDL_RenderFillRect(renderer, &rectangle);

        //Render green outlined quad
        SDL_SetRenderDrawColor( renderer, 0x00, 0xFF, 0x00, 0xFF );
        SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6,
            SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
        SDL_RenderDrawRect( renderer, &outlineRect );

        thickLineRGBA(renderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, 4,
            0x00, 0x00, 0xFF, 0xFF);

        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0x00, 0xFF );

        for (int y = 0; y < SCREEN_HEIGHT; y += 1) {
            SDL_RenderDrawPoint(renderer, SCREEN_WIDTH / 2, y);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
