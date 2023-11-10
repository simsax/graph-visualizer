#include <assert.h>
#include "SDL_image.h"
#include "SDL2_gfxPrimitives.h"
#include "SDL_render.h"
#include "render.h"
#include "graph.h"

#define BACKGROUND_COLOR 0xFF111111
#define VERTEX_COLOR COLOR7
#define SELECTED_VERTEX_COLOR COLOR7L
#define EDGE_COLOR COLOR10
#define N_COL_FONT_ATLAS 18

typedef struct {
    SDL_Window* sdl_window;
    int width;
    int height;
} Window;

SDL_Renderer* renderer;
Window window;
// 128x64, each char is 7x7, there are 18 chars in each line
static SDL_Texture* font_atlas;

SDL_Texture* load_texture(const char* path) {
    return SDL(IMG_LoadTexture(renderer, path));
}

void free_texture(SDL_Texture* texture) {
    SDL_DestroyTexture(texture);
}

void init_window(const char* name, int width, int height) {
    window.sdl_window = SDL(SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN));
    window.width = width;
    window.height = height;
}

void init_renderer(void) {
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ); // nearest filtering
    renderer = SDL(SDL_CreateRenderer(
        window.sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    font_atlas = load_texture("../fonts/charmap-oldschool_white.png");
}

int get_window_height(void) {
    return window.height;
}

int get_window_width(void) {
    return window.width;
}

void free_renderer(void) {
    free_texture(font_atlas);
    SDL_DestroyRenderer(renderer);
}

void free_window(void) {
    SDL_DestroyWindow(window.sdl_window);
}

void render_background(void) {
    SDL_SetRenderDrawColor(renderer, RGBA(BACKGROUND_COLOR));
    SDL_RenderClear(renderer);
}

static void render_circle_filled(PointF center, int radius, uint32_t color) {
    filledCircleColor(renderer, (int) center.x, (int) center.y, radius, color);
    // aacircleColor(renderer, (int) center.x, (int) center.y, radius, color);
}

static void render_circle_outline_filled(PointF center, int radius, int thickness, uint32_t color1,
    uint32_t color2) {
    render_circle_filled(center, radius, color1);
    render_circle_filled(center, radius - thickness, color2);
}

static void render_circle_outline(PointF center, int radius, int thickness, uint32_t color) {
    render_circle_outline_filled(center, radius, thickness, color, BACKGROUND_COLOR);
}

static PointI ndc_to_screen_coords(PointF ndc) {
    PointI screen_coords = {
        .x = (ndc.x + 1) * 0.5f * window.width,
        .y = (1 - ndc.y) * 0.5f * window.height
    };
    return screen_coords;
}

static void render_node(Node* node, bool use_label, int radius, uint32_t color) {
    if (use_label) {
        // TODO
        /* render_circle_outline_filled(node->position, node->text.height, radius * 1.2, BACKGROUND_COLOR, color); */
        /* render_text(&node->text, node->position); */
    } else {
        render_circle_filled(node->position, radius, color);
    }
}

static void render_edges(Graph* graph, size_t node_index) {
    uint8_t thickness = 1;
    PointF source_position = graph->nodes[node_index].position;
    for (EdgeNode* node = graph->adj_list[node_index]; node != NULL; node = node->next) {
        PointF dest_position = graph->nodes[node->index].position;
        aalineColor(renderer, (int) source_position.x, (int) source_position.y,
            (int) dest_position.x, (int) dest_position.y, EDGE_COLOR);
        // thickLineColor(renderer, (int) source_position.x, (int) source_position.y,
        //     (int) dest_position.x, (int) dest_position.y, thickness, COLOR4);
    }
}

void render_graph(Graph* graph, bool use_label) {
    // render edges
    for (size_t i = 0; i < graph->n_nodes; i++) {
        render_edges(graph, i);
    }
    // render nodes
    for (size_t i = 0; i < graph->n_nodes; i++) {
        Node* node = &graph->nodes[i];
        uint32_t color = graph->colliding_vertex == i ? SELECTED_VERTEX_COLOR : VERTEX_COLOR;
        render_node(node, use_label, graph->node_radius, color);
    }
}

static uint32_t mult_color(uint32_t color, float factor) {
    uint32_t red = (uint32_t) ((R(color)) * factor);
    uint32_t green = (uint32_t) ((G(color)) * factor);
    uint32_t blue = (uint32_t) ((B(color)) * factor);
    red = factor < 1 ? fmax(0, red) : fmin(0xFF, red);
    green = factor < 1 ? fmax(0, green) : fmin(0xFF, green);
    blue = factor < 1 ? fmax(0, blue) : fmin(0xFF, blue);
    return HEX(red, green, blue, 0xFF);
}

void render_line(PointI a, PointI b, uint8_t thickness, uint32_t color) {
    thickLineColor(renderer, a.x, a.y, b.x, b.y, thickness, color);
}

void render_rect(PointI a, PointI b, uint32_t color) {
    rectangleColor(renderer, a.x, a.y, b.x, b.y, color);
}

static void render_char(char c, int x, int y, int width, int height) {
    int index = c - ' ';
    int x_atlas = (index % N_COL_FONT_ATLAS) * FONT_OFFSET_X;
    int y_atlas = (index / N_COL_FONT_ATLAS) * FONT_OFFSET_Y;
    SDL_Rect src = { x_atlas, y_atlas, FONT_OFFSET_X, FONT_OFFSET_Y };
    SDL_Rect dest = { x, y, width, height };
    SDL_RenderCopy(renderer, font_atlas, &src, &dest);
}

void render_text(const char* text, PointI position, float size) {
    int x = position.x;
    int y = position.y;
    float scaling_factor = size / FONT_OFFSET_Y;
    int char_width = scaling_factor * FONT_OFFSET_X;
    for (size_t i = 0; i < strlen(text); i++) {
        char c = text[i];
        render_char(c, x, y, char_width, size);
        x += char_width;
    }
}

void render_button(const char* text, bool is_hot, bool is_active, PointI start_position,
        PointI text_size, PointI button_size) {
    uint32_t color = COLOR6;
    if (is_active && is_hot) {
        // clicking inside the button
        float shrinkage = 0.95;
        color = mult_color(color, 1.2);
        PointI prev_size = button_size;
        button_size.x *= shrinkage;
        button_size.y *= shrinkage;
        text_size.x *= shrinkage;
        text_size.y *= shrinkage;
        start_position.x += (prev_size.x - button_size.x) * 0.5;
        start_position.y += (prev_size.y - button_size.y) * 0.5;
    }
    else if (is_hot)
        color = mult_color(color, 1.5);
    PointI text_position = { start_position.x + (button_size.x - text_size.x) * 0.5,
        start_position.y + (button_size.y - text_size.y) * 0.5, };
    PointI end_position = { start_position.x + button_size.x, start_position.y + button_size.y };
    roundedBoxColor(renderer, start_position.x, start_position.y, end_position.x, end_position.y,
            10, color);   
    render_text(text, text_position, text_size.y);
}

