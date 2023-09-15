#include "common.h"
#include "render.h"
#include "node.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int test_node(void) {
    NodeArray nodes;
    init_node_array(&nodes);
    for (size_t i = 0; i < 10; i++) {
        Node* node = new_node(i, -1, BLACK, 100);
        add_node(&nodes, node);
        printf("Iteration %zu, count: %zu, capacity: %zu\n", i, nodes.count, nodes.capacity);
    }

    printf("== Nodes ==\n");
    for (size_t i = 0; i < nodes.count; i++) {
        printf("%d -> \n", nodes.nodes[i]->label);
    }

    free_node_array(&nodes);
    printf("\nFreed, count: %zu, capacity: %zu\n", nodes.capacity, nodes.count);

    return 0;
}

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

    init_renderer(window);
    set_background_color(WHITE);

    NodeArray node_array;
    init_node_array(&node_array);
    for (size_t i = 0; i < 3; i++) {
        Node* node = new_node(i, -1, BLACK, 100);
        add_node(&node_array, node);
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

        //update();
        // render();
        render_background();

        for (size_t i = 0; i < 3; i++) {
            Node* node = node_array.nodes[i];
            int height = node->text.height;
            int width = node->text.width;

            int x = i * SCREEN_WIDTH / 3 + SCREEN_WIDTH / 6;
            int y = SCREEN_HEIGHT / 2 - height / 2;

            render_node(node, (Point){x, y}, true);
        }


        SDL_RenderPresent(renderer); // swap front and back buffer
    }

    free_node_array(&node_array);
    free_renderer();
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
