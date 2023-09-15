#include <stdlib.h>
#include <stdio.h>
#include "text.h"

void init_text(Text* text, SDL_Renderer* renderer, const char* message, uint32_t color) {
    // consider using a global font and passing size as parameter
    TTF_Font* font = SDL(TTF_OpenFont("../fonts/UbuntuMono-R.ttf", 200));
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

void render_text(Text* text, SDL_Renderer* renderer, int x, int y) {
    SDL_Rect message_rect = {.x = x, .y = y, .w = text->width, .h = text->height};
    SDL_RenderCopy(renderer, text->texture, NULL, &message_rect);
}