#include "spring_layout.h"
#include "render.h"

typedef PointI Vec2;

Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2){a.x + b.x, a.y + b.y};
}

Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return (Vec2){a.x - b.x, a.y - b.y};
}

Vec2 vec2_mul(Vec2 a, Vec2 b) {
    return (Vec2){a.x * b.x, a.y * b.y};
}

Vec2 vec2_div(Vec2 a, Vec2 b) {
    return (Vec2){a.x / b.x, a.y / b.y};
}

Vec2 vec2(int x, int y) {
    return (Vec2){x, y};
}

void spring_layout(Node* vertices, size_t num_vertices) {
    int screen_width = get_window_width();
    int screen_height = get_window_height();
    Vec2 origin = vec2(screen_width, screen_height);
}