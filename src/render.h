#ifndef RENDER_H
#define RENDER_H

#include "common.h"
#include "ui.h"

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
#define YELLOW RED | GREEN

#define HEX(R, G, B, A)       \
    (R & 0xFF) << (0 * 8) |   \
    (G & 0xFF) << (1 * 8) |   \
    (B & 0xFF) << (2 * 8) |   \
    (A & 0xFF) << (3 * 8)     \

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

#define BACKGROUND_COLOR 0xFF111111

#define FONT_OFFSET_X 7
#define FONT_OFFSET_Y 9

typedef struct Node Node;
typedef struct Graph Graph;

void init_window(const char* name, int width, int height);
int get_window_height(void);
int get_window_width(void);
void free_window(void);
void init_renderer(void);
void free_renderer(void);
SDL_Texture* load_texture(const char* path);
void free_texture(SDL_Texture* texture);
void render_background(void);
void render_graph(Graph* graph, bool use_label);
void render_line(PointI a, PointI b, uint8_t thickness, uint32_t color);
void render_rect(PointI a, PointI b, uint32_t color);
void render_text(const char* text, PointI position, float size);
void render_textbox(const char* text, bool is_hot, bool is_active, PointI position, PointI box_size, PointI text_size,
        uint32_t color, Alignment alignment, int text_padding);
void render_button(const char* text, bool is_hot, bool is_active, PointI position,
        PointI text_size, PointI button_size, int roundness, uint32_t color);

extern SDL_Renderer* renderer;

#endif // RENDER_H
