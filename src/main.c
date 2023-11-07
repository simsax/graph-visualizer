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

    init_ui();
    uint64_t current = SDL_GetPerformanceCounter();
    uint64_t last = 0;
    while (simulation_state != QUIT_STATE) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                simulation_state = QUIT_STATE;
                break;

            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                case SDLK_SPACE:
                    if (simulation_state != MENU_STATE) {
                        simulation_state
                            = simulation_state == PAUSED_STATE ? RUNNING_STATE : PAUSED_STATE;
                    }
                    break;
                case SDLK_m:
                    simulation_state = MENU_STATE;
                    break;
                default:
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouse_down(LEFT_BUTTON);
                    drag();
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    mouse_down(RIGHT_BUTTON);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouse_up(LEFT_BUTTON);
                    undrag();
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    mouse_up(RIGHT_BUTTON);
                }
                break;
            case SDL_MOUSEMOTION:
                update_cursor_position(e.motion.x, e.motion.y);
                mouse_moved(e.motion.x, e.motion.y);
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
                    Padding button_padding = {
                        .top = 10,
                        .bottom = 10,
                        .left = 10,
                        .right = 10
                    };
                    begin_ui(VERTICAL_LAYOUT, LEFT_ALIGNMENT, (Padding) {10, 10, 10, 10}, 
                            (PointI) { SCREEN_WIDTH, SCREEN_HEIGHT }, (PointI) {0, 0});
                    /* begin_ui(HORIZONTAL_LAYOUT, TOP_ALIGNMENT, (Padding) {10, 10, 10, 10}, */ 
                    /*         (PointI) { 800, 300 }, (PointI) {0, 540}); */
                    begin_group(HORIZONTAL_LAYOUT, TOP_ALIGNMENT, (Padding) {10, 10, 10, 10}, 0.5);
                    begin_group(HORIZONTAL_LAYOUT, BOTTOM_ALIGNMENT, (Padding) {10, 10, 10, 10}, 0.5);
                    if (do_button("Start", button_padding)) {
                        printf("Start\n");
                        /* simulation_state = RUNNING_STATE; */
                    }
                    if (do_button("Quit", button_padding)) {
                        printf("Quit\n");
                        /* simulation_state = QUIT_STATE; */
                    }
                    if (do_button("Foo", button_padding)) {
                        printf("Foo\n");
                    }
                    end_group();
                    begin_group(HORIZONTAL_LAYOUT, TOP_ALIGNMENT, (Padding) {10, 10, 10, 10}, 0.5);
                    if (do_button("Nothing", button_padding)) {
                        printf("Nothing\n");
                    }
                    if (do_button("Miao", button_padding)) {
                        printf("Miao\n");
                    }
                    if (do_button("Bar", button_padding)) {
                        printf("Bar\n");
                    }
                    end_group();
                    end_group();
                    begin_group(HORIZONTAL_LAYOUT, BOTTOM_ALIGNMENT, (Padding) {10, 10, 10, 10}, 0.5);
                    begin_group(VERTICAL_LAYOUT, LEFT_ALIGNMENT, (Padding) {10, 10, 10, 10}, 0.5);
                    if (do_button("Start", button_padding)) {
                        printf("Start2\n");
                        /* simulation_state = RUNNING_STATE; */
                    }
                    if (do_button("Quit", button_padding)) {
                        printf("Quit2\n");
                        /* simulation_state = QUIT_STATE; */
                    }
                    if (do_button("Foo", button_padding)) {
                        printf("Foo2\n");
                    }
                    end_group();
                    begin_group(VERTICAL_LAYOUT, RIGHT_ALIGNMENT, (Padding) {10, 10, 10, 10}, 0.5);
                    if (do_button("Nothing", button_padding)) {
                        printf("Nothing2\n");
                    }
                    if (do_button("Miao", button_padding)) {
                        printf("Miao2\n");
                    }
                    if (do_button("Bar", button_padding)) {
                        printf("Bar2\n");
                    }
                    end_group();
                    end_group();
                    end_ui();
                }
                break;
            case RUNNING_STATE:
            case PAUSED_STATE:
                render_graph(&graph, false);
                break;
            default:
                break;
        }

#if DEBUG_UI
        render_line((PointI) {960, 0}, (PointI) {960, 1080}, 1, RED);
        render_line((PointI) {0, 540}, (PointI) {1920, 540}, 1, RED);
#endif

        SDL_RenderPresent(renderer);
    }
    free_graph(&graph);
    free_renderer();
    free_window();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
