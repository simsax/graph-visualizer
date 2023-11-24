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

    init_window("Graph Visualizer", SCREEN_WIDTH, SCREEN_HEIGHT);
    init_renderer();

    Graph graph;
    init_graph(&graph, COMPLETE_CONFIG, false, N_VERTICES, 8);

    init_ui();
    uint64_t current = SDL_GetPerformanceCounter();
    uint64_t last = 0;
    const char* config_string[] = {
        FOREACH_CONFIG(GENERATE_STRING)
    };

    GraphConfig config = COMPLETE_CONFIG;
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
                        .top = 20,
                        .bottom = 20,
                        .left = 0,
                        .right = 0
                    };

                    Padding group_padding = { 100, 0, 20, 20 };
                    Padding zero_padding = { 0, 0, 0, 0 };
                    Padding text_padding = zero_padding;

                    int text_size = 60;
                    int button_width = 300;
                    int button_height = 90;
                    int n_buttons = 2;
                    int layout_padding_top = 
                        n_buttons * (button_height + button_padding.top + button_padding.bottom);


                    begin_ui(HORIZONTAL_LAYOUT, CENTER_ALIGNMENT, 
                            /* (Padding) { (SCREEN_HEIGHT - layout_padding_top) * 0.5, 0, 0, 0 }, */
                            zero_padding,
                            (PointI) { SCREEN_WIDTH, SCREEN_HEIGHT }, (PointI) {0, 0});

                    begin_group(VERTICAL_LAYOUT, RIGHT_ALIGNMENT, group_padding, 0.5);
                    do_textbox("Num vertices", button_padding, 4, CENTER_ALIGNMENT,
                            CHAR_SIZE(text_size), BACKGROUND_COLOR, (PointI){-1,-1},
                            (PointI){0,0}, NULL, NULL, false);
                    do_textbox("Config", button_padding, 4, CENTER_ALIGNMENT,
                            CHAR_SIZE(text_size), BACKGROUND_COLOR, (PointI){-1,-1},
                            (PointI){0,0}, NULL, NULL, false);
                    end_group();
                    begin_group(VERTICAL_LAYOUT, LEFT_ALIGNMENT, group_padding, 0.5);
                    do_input_uint(&graph.n_nodes, button_padding, text_size, (PointI) { 0, 0 });
                    do_dropdown(&config, config_string, BAZ + 1, button_padding, text_size);
                    end_group();
                    // update_graph (destroy old one, make new? Or better, don't create
                    // new shit until user presses start. When going back to main menu, destroy
                    // old graph(?))
                    
                    /* if (do_button("Start", button_padding, text_size, button_width, button_height)) { */
                    /*     simulation_state = RUNNING_STATE; */
                    /* } */
                    /* if (do_button("Quit", button_padding, text_size, button_width, button_height)) { */
                    /*     simulation_state = QUIT_STATE; */
                    /* } */
                    end_ui();
                    /* render_text("Hello World!", (PointI) { 0, 0 }, 18); */
                    break;
                }
            case RUNNING_STATE:
            case PAUSED_STATE:
                {
                    render_graph(&graph, false);
                    break;
                }
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
    SDL_Quit();

    return 0;
}
