#include <time.h>
#include <assert.h>
#include "common.h"
#include "render.h"
#include "graph.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define N_VERTICES 50
#define N_EDGES 100

int main(void)
{
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_ERROR();
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() < 0) {
        SDL_ERROR();
        exit(EXIT_FAILURE);
    }

    init_window("Graph Visualizer", SCREEN_WIDTH, SCREEN_HEIGHT);
    init_renderer();
    set_background_color(BLACK);

    Graph graph;
    init_random_graph(&graph, false, N_VERTICES, N_EDGES);

    bool quit = false;
    uint64_t current = SDL_GetPerformanceCounter();
    uint64_t last = 0;
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

        last = current;
        current = SDL_GetPerformanceCounter();

        double delta_time = (double)((current - last) / (double) SDL_GetPerformanceFrequency());

        update_graph(&graph, delta_time);

        render_background();
        render_graph(&graph, false);

        SDL_RenderPresent(renderer);
    }
    free_graph(&graph);
    free_renderer();
    free_window();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
