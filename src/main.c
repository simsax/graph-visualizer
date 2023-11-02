#include <time.h>
#include <assert.h>
#include "common.h"
#include "render.h"
#include "ui.h"
#include "graph.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define N_VERTICES 8

// TODO: read graph from file

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
    init_graph(&graph, COMPLETE_CONFIG, false, N_VERTICES, 8);

    Ui ui;
    init_ui(&ui);

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
                    mouse_down(&ui, LEFT_BUTTON);
                    drag();
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    mouse_down(&ui, RIGHT_BUTTON);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouse_up(&ui, LEFT_BUTTON);
                    undrag();
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    mouse_up(&ui, RIGHT_BUTTON);
                }
                break;
            case SDL_MOUSEMOTION:
                update_cursor_position(e.motion.x, e.motion.y);
                mouse_moved(&ui, e.motion.x, e.motion.y);
            default:
                break;
            }
        }

        last = current;
        current = SDL_GetPerformanceCounter();

        double delta_time = (double)((current - last) / (double)SDL_GetPerformanceFrequency());

        update_graph(&graph, delta_time);

        render_background();

        switch (simulation_state) {
            case MENU_STATE: 
                {
                    if (do_button(&ui, 1, (PointI) {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, "Start")) {
                        printf("Start button pressed!\n");
                        simulation_state = RUNNING_STATE;
                    }
                    if (do_button(&ui, 2, (PointI) {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 120}, "Quit")) {
                        printf("Quit button pressed!\n");
                        simulation_state = QUIT_STATE;
                    }
                }
                break;
            case RUNNING_STATE:
            case PAUSED_STATE:
                render_graph(&graph, false);
                break;
            default:
                break;
        }

        SDL_RenderPresent(renderer);
        reset_mouse(&ui);
    }
    free_graph(&graph);
    free_renderer();
    free_window();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
