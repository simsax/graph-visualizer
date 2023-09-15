#include "SDL2_gfxPrimitives.h"
#include "render.h"
#include "node.h"

SDL_Renderer* renderer;
uint32_t background_color;

void init_renderer(SDL_Window* window) {
    renderer = SDL(SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    background_color = WHITE;
}

void free_renderer() {
    SDL_DestroyRenderer(renderer);
}

void set_background_color(uint32_t color) {
    background_color = color;
}

void init_text(Text* text, const char* message, uint32_t color, int size) {
    // consider using a global font and passing size as parameter
    TTF_Font* font = SDL(TTF_OpenFont("../fonts/UbuntuMono-R.ttf", size));
    SDL_Surface* surface = SDL(TTF_RenderText_Solid(font, message, (SDL_Color){RGBA(color)}));
    SDL_Texture* texture = SDL(SDL_CreateTextureFromSurface(renderer, surface));
    text->font = font;
    text->texture = texture;
    text->height = surface->h;
    text->width = surface->w;
    text->color = color;
    SDL_FreeSurface(surface);
}

void free_text(Text* text) {
    TTF_CloseFont(text->font);
    SDL_DestroyTexture(text->texture);
}

void render_text(Text* text, Point p) {
    int x_centered = p.x - text->width / 2;
    int y_centered = p.y - text->height / 2;
    SDL_Rect message_rect = {.x = x_centered, .y = y_centered, .w = text->width, .h = text->height};
    SDL_RenderCopy(renderer, text->texture, NULL, &message_rect);
}

static void render_circle_filled(Point center, int radius, int thickness, uint32_t color1,
    uint32_t color2) {
    filledCircleColor(renderer, center.x, center.y, radius, color1);
    filledCircleColor(renderer, center.x, center.y, radius - thickness, color2);
}

static void render_circle_outline(Point center, int radius, int thickness, uint32_t color) {
    render_circle_filled(center, radius, thickness, color, background_color);
}

void render_node(Node* node, Point position, bool use_label) {
    if (use_label) {
        render_circle_filled(position, node->text.height , 10, BLACK, RED | GREEN);
        render_text(&node->text, position);
    }
}

void render_background() {
    SDL_SetRenderDrawColor(renderer, RGBA(background_color));
    SDL_RenderClear(renderer);
}