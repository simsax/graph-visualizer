#include <time.h>
#include <assert.h>
#include "common.h"
#include "render.h"
#include "node.h"
#include "graph.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define NUM_POINTS 10
#define PI 3.14159265359

typedef struct nodeL {
    PointI p;
    struct nodeL* next;
} NodeL;

typedef struct {
    NodeL* head;
    int count;
} ActiveList;

typedef struct {
    PointI* points;
    int length;
    int height;
    int width;
    float cell_size;
} Grid;


float rand_uniform() {
    return (float) rand() / (float) RAND_MAX;
}

void add_point_to_grid(Grid* grid, PointI p) {
    int row = floor(p.y / grid->cell_size);
    int col = floor(p.x / grid->cell_size);
    grid->points[row * grid->width + col] = p;
}

void remove_point_from_grid(Grid* grid, PointI p) {
    int row = floor(p.y / grid->cell_size);
    int col = floor(p.x / grid->cell_size);
    grid->points[row * grid->width + col] = (PointI) {.x = -1, .y = -1};
}

void init_grid(Grid* grid, float cell_size) {
    grid->height = floor(SCREEN_HEIGHT / cell_size);
    grid->width = floor(SCREEN_WIDTH / cell_size);
    grid->length = grid->height * grid->width;
    grid->cell_size = cell_size;
    grid->points = malloc(sizeof(PointI) * grid->length);
    assert(grid->points != NULL); // print a proper error message
    for (int i = 0; i < grid->length; i++) {
        grid->points[i].x = -1;
        grid->points[i].y = -1;
    }
}


NodeL* new_NodeL(PointI p, NodeL* next) {
    NodeL* node = malloc(sizeof(NodeL));
    assert(node != NULL);
    node->p = p;
    node->next = next;
    return node;
}

void list_insert(ActiveList* list, PointI p) {
    list->head = new_NodeL(p, list->head);
    list->count++;
}

NodeL* pick_i_NodeL(ActiveList* list, int index) {
    assert(index < list->count);
    NodeL* node = list->head;
    for (int i = 0; i < index; i++) {
        node = node->next;
    }
    return node;
}

bool is_list_empty(ActiveList* list) {
    return list->count == 0;
}

void remove_i_NodeL(ActiveList* list, int index) {
    assert(index < list->count);
    NodeL* node = list->head;
    if (index == 0) {
        list->head = node->next;
        free(node);
        list->count--;
        return;
    }
    NodeL* prev = NULL;
    for (int i = 0; i < index; i++) {
        prev = node;
        node = node->next;
    }
    assert(node != NULL);
    prev->next = node->next;
    free(node);
    list->count--;
}


void init_active_list(ActiveList* active_list) {
    active_list->head = NULL;
    active_list->count = 0;
}

void free_active_list(ActiveList* active_list) {
    for (NodeL* node = active_list->head; node != NULL;) {
        NodeL* temp = node;
        node = node->next;
        free(temp);
    }
}

float square_distance(PointI a, PointI b) {
    return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

bool valid_point_in_grid(Grid* grid, PointI p, int radius) {
    if (p.y < 0 || p.y >= SCREEN_HEIGHT || p.x < 0 || p.x >= SCREEN_WIDTH) {
        // point is outside of window
        return false;
    }

    int row = floor(p.y / grid->cell_size);
    int col = floor(p.x / grid->cell_size);

    PointI grid_point = grid->points[row * grid->width + col];

    if (grid_point.x != -1) {
        // already occupied
        return false;
    }

    int left_cell = fmax(0, col - 2);
    int right_cell = fmin(col + 2, grid->width);
    int top_cell = fmax(0, row - 2);
    int bottom_cell = fmin(row + 2, grid->height);

    int square_radius = radius * radius;

    for (int i = top_cell; i <= bottom_cell; i++) {
        for (int j = left_cell; j <= right_cell; j++) {
            PointI point = grid->points[i * grid->width + j];
            if (point.x != -1 && square_distance(point, p) <= square_radius)
                return false;
        }
    }

    return true;
}


PointI* poisson_disk_sampling(const int num_points, int radius, int k, Grid* grid) {
    PointI* final_points = malloc(sizeof(PointI) * num_points);
    for (int i = 0; i < num_points; i++) {
        final_points[i].x = -1;
        final_points[i].y = -1;
    }
    float cell_size = (float) radius / sqrt(2);
    init_grid(grid, cell_size);
    ActiveList active_list;
    init_active_list(&active_list);
    int point_counter = 0;
    PointI first_point = (PointI) {
        .x = floor(SCREEN_WIDTH / 2),
        .y = floor(SCREEN_HEIGHT / 2)
    };
    add_point_to_grid(grid, first_point);
    list_insert(&active_list, first_point);
    point_counter++;


    while (active_list.count > 0 && point_counter < num_points) {
        int i = rand() % active_list.count;
        NodeL* node = pick_i_NodeL(&active_list, i);
        assert(node != NULL);
        PointI current_point = node->p;
        bool found = false;
        for (int j = 0; j < k; j++) {
            float r = (1 + sqrt(rand_uniform())) * radius;
            float theta = 2 * PI * rand_uniform();

            PointI point = (PointI) {
                .x = current_point.x + r * cos(theta),
                .y = current_point.y + r * sin(theta)
            };

            if (valid_point_in_grid(grid, point, radius)) {
                found = true;
                add_point_to_grid(grid, point);
                list_insert(&active_list, point);
                point_counter++;
                if (point_counter >= num_points)
                    break;
            }
        }
        if (!found) {
            remove_i_NodeL(&active_list, i);
        }
        // printf("Active list count: %d; Point counter: %d\n", active_list.count, point_counter);

    }


    int p = 0;
    for (int i = 0; i < grid->length; i++) {
        if (grid->points[i].x != -1) {
            final_points[p++] = grid->points[i];
        }
    }


    free(grid->points);
    free_active_list(&active_list);
    return final_points;
}

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
    set_background_color(WHITE);

    Graph graph;
    init_graph(&graph, NUM_POINTS);

    // PointI points2[NUM_POINTS];
    // float radius = 10.0f;
    // for (int i = 0; i < NUM_POINTS; i++) {
    //     float r = sqrt(rand_uniform()) * radius;
    //     float theta = 2 * PI * rand_uniform();
    //     points2[i] = (PointI) {.x = r * cos(theta), .y = r * sin(theta)};
    // }
    Grid grid;
    PointI* poisson_points = poisson_disk_sampling(NUM_POINTS, 300, 30, &grid);

    // printf("Grid length: %d\n", grid.length);
    // for (int i = 0; i < NUM_POINTS; i++) {
    //     printf("PointI {%d} (%d, %d)\n", i + 1, poisson_points[i].x, poisson_points[i].y);
    // }

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
        // PointI* poisson_points = poisson_disk_sampling(NUM_POINTS, 100, 30, &grid);

        render_graph(&graph, poisson_points, NUM_POINTS, false);

        SDL_RenderPresent(renderer); // swap front and back buffer
    }
    // free(poisson_points);
    free_graph(&graph);
    free_renderer();
    free_window();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

// if I assume the number of vertices to be generated is the input,
// then I can use that number as the size for the active list

// if NUM_POINTS is higher than final grid, then initialize the graph later (actually I will
// never generate that many points in real life)

// consider the radius and some margin as limits inside valid_point_in_grid
// custom mem allocator or use a static array, hardcoding a maximum node_size for simplicity