#ifndef TEXT_H
#define TEXT_H

#include "common.h"

typedef struct {
    TTF_Font* font;
    SDL_Texture* texture;
    int height;
    int width;
    uint32_t color;
} Text;

void init_text(Text* text, SDL_Renderer* renderer, const char* message, uint32_t color);
void free_text(Text* text);
void render_text(Text* text, SDL_Renderer* renderer, int x, int y);

#endif // TEXT_H