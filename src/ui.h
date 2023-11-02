#ifndef UI_H
#define UI_H

#include "common.h"

#define MAX_BUTTONS 5

typedef size_t ui_id;

typedef enum {
    NO_BUTTON,
    LEFT_BUTTON,
    RIGHT_BUTTON
} MouseButton;

typedef struct {
    ui_id active_item;
    ui_id hot_item;
    PointI mouse_position;
    MouseButton mouse_down;
    MouseButton mouse_up; 
} Ui;

void init_ui(Ui* ui);
ui_id new_id();
bool do_button(Ui* ui, ui_id button_id, PointI position, const char* text);
void mouse_moved(Ui* ui, int x, int y);
void mouse_down(Ui* ui, MouseButton button);
void mouse_up(Ui* ui, MouseButton button);
void reset_mouse(Ui* ui);

#endif // UI_H
