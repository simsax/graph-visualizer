#ifndef RENDER_H
#define RENDER_H

#include "common.h"

// 0xAABBGGRR
#define R(hex) hex >> (0 * 8) & 0xFF
#define G(hex) hex >> (1 * 8) & 0xFF
#define B(hex) hex >> (2 * 8) & 0xFF
#define A(hex) hex >> (3 * 8) & 0xFF

#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF
#define RED   0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE  0xFFFF0000

#define HEX(R, G, B, A)       \
    (R & 0xFF) << (0 * 8) |   \
    (G & 0xFF) << (1 * 8) |   \
    (B & 0xFF) << (2 * 8) |   \
    (A & 0xFF) << (3 * 8) |   \

#define RGBA(hex) R(hex), G(hex), B(hex), A(hex)

#define PI 3.14159265359

// palette 1
#define COLOR1 0xFF23141A
#define COLOR2 0xFF492537
#define COLOR3 0xFF604C77
#define COLOR4 0xFF695DB7
#define COLOR5 0xFFC2CDEA

// palette 2
#define COLOR6 0xFF423614
#define COLOR7 0xFF4622D6
#define COLOR7L 0XFFA18EED
#define COLOR8 0xFFD9D9D9
#define COLOR9 0xFFAC8071
#define COLOR10 0xFFDBD0A8

typedef struct Node Node;
typedef struct Graph Graph;

typedef struct Text {
    SDL_Texture* texture;
    int height;
    int width;
    uint32_t color;
} Text;

void init_window(const char* name, int width, int height);
int get_window_height();
int get_window_width();
void free_window();
void init_renderer();
void free_renderer();
void init_text(Text* text, const char* message, uint32_t color, int size);
void free_text(Text* text);
void render_text(Text* text, PointF p);
void render_background();
void render_graph(Graph* graph, bool use_label);

extern SDL_Renderer* renderer;

#endif // RENDER_H