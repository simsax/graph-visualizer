#include "common.h"
#include "SDL2_gfxPrimitives.h"
#include "text.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main(void)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_ERROR();
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() < 0) {
        SDL_ERROR();
        exit(EXIT_FAILURE);
    }


    SDL_Window* window = SDL(SDL_CreateWindow("Graph Visualizer", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN));

    SDL_Renderer* renderer
        = SDL(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    Text text;
    init_text(&text, renderer, "Hello, World", WHITE);

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

        SDL_SetRenderDrawColor(renderer, RGBA(BLACK));
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, RGBA(RED));
        SDL_Rect rectangle = {
            .x = SCREEN_WIDTH / 4,
            .y = SCREEN_HEIGHT / 4,
            .w = SCREEN_WIDTH / 2,
            .h = SCREEN_HEIGHT / 2
        };
        SDL_RenderFillRect(renderer, &rectangle);

        thickLineColor(renderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, 4, BLUE);

        filledCircleColor(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 4,
            RED | BLUE);


        render_text(&text, renderer, 0, 0);

        SDL_RenderPresent(renderer); // swap front and back buffer
    }

    free_text(&text);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
