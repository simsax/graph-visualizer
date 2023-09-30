#include "poisson.h"
#include "render.h"
#include "node.h"
#include <assert.h>

typedef struct {
    PointI* points;
    int count;
} ActiveList;

typedef struct {
    PointI* points;
    int length;
    int height;
    int width;
    float cell_size;
} Grid;


static float rand_uniform() {
    return (float) rand() / (float) RAND_MAX;
}

static void add_point_to_grid(Grid* grid, PointI p) {
    int row = floor(p.y / grid->cell_size);
    int col = floor(p.x / grid->cell_size);
    grid->points[row * grid->width + col] = p;
}

static void init_grid(Grid* grid, float cell_size, int screen_height, int screen_width) {
    grid->height = floor(screen_height / cell_size);
    grid->width = floor(screen_width / cell_size);
    grid->length = grid->height * grid->width;
    grid->cell_size = cell_size;
    size_t grid_size = grid->length * sizeof(PointI);
    grid->points = malloc(grid_size);
    assert(grid->points != NULL);
    memset(grid->points, -1, grid_size);
}

static void free_grid(Grid* grid) {
    free(grid->points);
}

static void init_active_list(ActiveList* active_list, size_t num_points) {
    active_list->count = 0;
    size_t list_size = num_points * sizeof(PointI);
    active_list->points = malloc(list_size);
    assert(active_list->points != NULL);
    memset(active_list->points, -1, list_size);
}

static void free_active_list(ActiveList* active_list) {
    free(active_list->points);
}

static void list_insert(ActiveList* list, PointI p) {
    list->points[list->count++] = p;
}

static PointI pick_point(ActiveList* list, int index) {
    assert(index < list->count && index >= 0);
    return list->points[index];
}

static void list_remove(ActiveList* list, int index) {
    assert(index < list->count && index >= 0);
    for (int i = index; i < list->count; i++) {
        list->points[i] = list->points[i + 1];
    }
    list->count--;
}

static float square_distance(PointI a, PointI b) {
    return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

static bool valid_point_in_grid(Grid* grid, PointI p, int radius, int screen_height, int screen_width) {
    if (p.y < 0 || p.y >= screen_height || p.x < 0 || p.x >= screen_width) {
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



void poisson_disk_sampling(NodeArray* vertices, int radius, int k) {
    int screen_height = get_window_height();
    int screen_width = get_window_width();
    size_t num_points = vertices->count;
    float cell_size = (float) radius / sqrt(2);
    Grid grid;
    init_grid(&grid, cell_size, screen_height, screen_width);
    ActiveList active_list;
    init_active_list(&active_list, num_points);
    size_t point_counter = 0;
    PointI first_point = (PointI) {
        .x = floor(get_window_width() / 2),
        .y = floor(get_window_height() / 2)
    };
    add_point_to_grid(&grid, first_point);
    list_insert(&active_list, first_point);
    point_counter++;

    while (active_list.count > 0 && point_counter < num_points) {
        int i = rand() % active_list.count;
        PointI current_point = pick_point(&active_list, i);
        bool found = false;
        for (int j = 0; j < k; j++) {
            float r = (1 + sqrt(rand_uniform())) * radius;
            float theta = 2 * PI * rand_uniform();

            PointI point = (PointI) {
                .x = current_point.x + r * cos(theta),
                .y = current_point.y + r * sin(theta)
            };

            if (valid_point_in_grid(&grid, point, radius, screen_height, screen_width)) {
                found = true;
                add_point_to_grid(&grid, point);
                list_insert(&active_list, point);
                point_counter++;
                if (point_counter >= num_points)
                    break;
            }
        }
        if (!found) {
            list_remove(&active_list, i);
        }
    }

    int p = 0;
    for (int i = 0; i < grid.length; i++) {
        if (grid.points[i].x != -1) {
            vertices->nodes[p++]->position = grid.points[i];
        }
    }

    assert(p == num_points);

    free_grid(&grid);
    free_active_list(&active_list);
}
