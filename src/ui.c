#include "ui.h"
#include "render.h"
#include "physics.h"

static ui_id id_count = 0;

void init_ui(Ui *ui) {
    ui->active_item = 0;
    ui->hot_item = 0;
    ui->mouse_position = (PointI) {0, 0};
    ui->mouse_down = NO_BUTTON;
    ui->mouse_up = NO_BUTTON;
}

ui_id new_id() {
    return ++id_count;
}

static bool is_hot(Ui* ui, ui_id item_id) {
    return item_id == ui->hot_item;
}

static bool is_active(Ui* ui, ui_id item_id) {
    return item_id == ui->active_item;
}

bool do_button(Ui* ui, ui_id button_id, PointI position, const char* text) {
    bool result = false;
    bool hot = is_hot(ui, button_id);
    PointI position2 = {position.x + 200, position.y + 100};
    if (is_active(ui, button_id) && ui->mouse_up == LEFT_BUTTON) {
        if (hot) {
            result = true;
        }
        ui->active_item = 0;
    }
    else if (hot && ui->mouse_down == LEFT_BUTTON) {
        ui->active_item = button_id;
    }

    Aabb button_aabb = {position.x, position2.x, position.y, position2.y};
    if (intersect_point(&button_aabb, ui->mouse_position)) {
        ui->hot_item = button_id;
    } else if (hot) {
        ui->hot_item = 0;
    }
    render_button(position, position2, text, hot);
    return result;
}

void mouse_moved(Ui* ui, int x, int y) {
    ui->mouse_position = (PointI) {x, y};
}

void mouse_down(Ui* ui, MouseButton button) {
    ui->mouse_down = button;
}

void mouse_up(Ui* ui, MouseButton button) {
    ui->mouse_up = button;
}

void reset_mouse(Ui* ui) {
    ui->mouse_up = NO_BUTTON;
    ui->mouse_down = NO_BUTTON;
}
