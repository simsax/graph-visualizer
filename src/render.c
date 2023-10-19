#include <assert.h>
#include "SDL2_gfxPrimitives.h"
#include "render.h"
#include "graph.h"

#define BACKGROUND_COLOR 0xFF111111
#define VERTEX_COLOR COLOR7
#define SELECTED_VERTEX_COLOR COLOR7L
#define EDGE_COLOR COLOR10

typedef struct {
    SDL_Window* sdl_window;
    int width;
    int height;
} Window;

SDL_Renderer* renderer;
TTF_Font* font;
Window window;

void init_window(const char* name, int width, int height) {
    window.sdl_window = SDL(SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN));
    window.width = width;
    window.height = height;
}

void init_renderer() {
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ); // linear filtering
    renderer = SDL(SDL_CreateRenderer(
        window.sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    font = SDL(TTF_OpenFont("../fonts/UbuntuMono-R.ttf", 300));
}

int get_window_height() {
    return window.height;
}

int get_window_width() {
    return window.width;
}

void free_renderer() {
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
}

void free_window() {
    SDL_DestroyWindow(window.sdl_window);
}

void init_text(Text* text, const char* message, uint32_t color, int size) {
    // consider using a global font and passing size as parameter
    SDL_Surface* surface = SDL(TTF_RenderText_Solid(font, message, (SDL_Color){RGBA(color)}));
    SDL_Texture* texture = SDL(SDL_CreateTextureFromSurface(renderer, surface));
    text->texture = texture;
    text->height = surface->h;
    text->width = surface->w;
    text->color = color;
    SDL_FreeSurface(surface);
}

void free_text(Text* text) {
    SDL_DestroyTexture(text->texture);
}

void render_text(Text* text, PointF p) {
    int x_centered = (int) (p.x - text->width / 2);
    int y_centered = (int) (p.y - text->height / 2);
    SDL_Rect message_rect = {.x = x_centered, .y = y_centered, .w = text->width, .h = text->height};
    SDL_RenderCopy(renderer, text->texture, NULL, &message_rect);
}

void render_background() {
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
        render_circle_outline_filled(node->position, node->text.height, radius * 1.2, BACKGROUND_COLOR, color);
        render_text(&node->text, node->position);
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