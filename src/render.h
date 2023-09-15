#ifndef RENDER_H
#define RENDER_H

#include "common.h"

typedef struct Node Node;

typedef struct {
    int x;
    int y;
} Point;

typedef struct Text {
    TTF_Font* font;
    SDL_Texture* texture;
    int height;
    int width;
    uint32_t color;
} Text;

void init_renderer(SDL_Window* window);
void free_renderer();
void set_background_color(uint32_t color);
void init_text(Text* text, const char* message, uint32_t color, int size);
void free_text(Text* text);
void render_node(Node* node, Point position, bool use_label);
void render_text(Text* text, Point p);
void render_background();

extern SDL_Renderer* renderer;

#endif // RENDER_H