#include <time.h>
#include <assert.h>
#include "common.h"
#include "render.h"
#include "graph.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define N_VERTICES 60

// TODO: read graph from file, mouse input on the vertices (drag them and update the forces
// real-time)

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

    Graph graph;
    init_random_graph(&graph, false, N_VERTICES, 8);

    uint64_t current = SDL_GetPerformanceCounter();
    uint64_t last = 0;
    while (simulation_state != QUIT_STATE) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                simulation_state = QUIT_STATE;
                break;

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                case SDLK_SPACE:
                    simulation_state
                        = simulation_state == PAUSED_STATE ? RUNNING_STATE : PAUSED_STATE;
                    break;
                default:
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    drag();
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    undrag();
                }
                break;
            case SDL_MOUSEMOTION:
                update_cursor_position(&graph, e.motion.x, e.motion.y);
            default:
                break;
            }
        }

        last = current;
        current = SDL_GetPerformanceCounter();

        double delta_time = (double)((current - last) / (double)SDL_GetPerformanceFrequency());

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
