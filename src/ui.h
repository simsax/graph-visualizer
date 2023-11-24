#ifndef UI_H
#define UI_H

#include "common.h"

#define MAX_BUTTONS 5
#define MAX_GROUPS 16

#define CHAR_SIZE(size) ((PointI) { (size) * FONT_OFFSET_X / FONT_OFFSET_Y, (size) })

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
    PointI size;
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
    bool dropdown_open;
    size_t group_count;
    Group groups[MAX_GROUPS];
} Ui;


void init_ui(void);
void begin_ui(Layout layout, Alignment alignment, Padding padding, PointI size, PointI starting_pos);
void end_ui(void);
void begin_group(Layout layout, Alignment alignment, Padding padding, float fill_perc);
void end_group(void);
void mouse_moved(int x, int y);
void mouse_down(MouseButton button);
void mouse_up(MouseButton button);
bool do_button(const char* text, Padding padding, int size, PointI button_pos, PointI button_size,
        int roundness, uint32_t color);
bool do_textbox(const char *text, Padding padding, int text_padding, Alignment alignment,
        PointI char_size, uint32_t color, PointI box_pos, PointI box_size, PointI* final_pos,
        PointI* final_size, bool clickable);
void do_input_uint(uint64_t* input_number, Padding padding, int text_size, PointI box_size);
void do_dropdown(int* cur_val, const char* enum_string[], int num_options, Padding padding,
        int text_size);

#endif // UI_H
