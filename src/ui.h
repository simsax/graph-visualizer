#ifndef UI_H
#define UI_H

#include "common.h"

#define MAX_BUTTONS 5
#define MAX_GROUPS 16

typedef size_t ui_id;

typedef enum {
    NO_BUTTON,
    LEFT_BUTTON,
    RIGHT_BUTTON
} MouseButton;

typedef enum {
    HORIZONTAL_LAYOUT,
    VERTICAL_LAYOUT
} Layout;

typedef enum {
    CENTER_ALIGNMENT,
    LEFT_ALIGNMENT,
    RIGHT_ALIGNMENT,
    TOP_ALIGNMENT,
    BOTTOM_ALIGNMENT
} Alignment;

typedef struct {
    uint32_t top;
    uint32_t bottom;
    uint32_t left;
    uint32_t right;
} Padding;

typedef struct Group {
    PointI next_item_position;
    PointI next_group_position;
    Alignment alignment;
    Padding padding;
    Layout layout;
} Group;

typedef struct {
    ui_id active_item;
    ui_id hot_item;
    PointI mouse_position;
    MouseButton mouse_down;
    MouseButton mouse_up; 
    ui_id id_count;
    size_t group_count;
    Group groups[MAX_GROUPS];
} Ui;


void init_ui(void);
void begin_ui(Layout layout, Alignment alignment, Padding padding);
void end_ui(void);
void begin_group(Layout layout, Alignment alignment, Padding padding);
void end_group(void);
bool do_button(const char* text, Padding padding);
void mouse_moved(int x, int y);
void mouse_down(MouseButton button);
void mouse_up(MouseButton button);

#endif // UI_H