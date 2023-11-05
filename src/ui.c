#include "ui.h"
#include "render.h"
#include "physics.h"
#include <assert.h>

#define MENU_BUTTON_WIDTH 300
#define MENU_BUTTON_HEIGHT 150

static Ui ui;

void init_ui(void) {
    ui.active_item = 0;
    ui.hot_item = 0;
    ui.mouse_position = (PointI) {0, 0};
    ui.mouse_down = NO_BUTTON;
    ui.mouse_up = NO_BUTTON;
}

static Group* peek_group(void) {
    assert(ui.group_count > 0);
    return &ui.groups[ui.group_count - 1];
}

static Group* peek_parent_group(void) {
    assert(ui.group_count > 1);
    return &ui.groups[ui.group_count - 2];
}

void end_group(void) {
    assert(ui.group_count > 0);
    if (ui.group_count > 1) {
        Group* current_group = peek_group();
        Group* parent_group = peek_parent_group();
        if (parent_group->layout == HORIZONTAL_LAYOUT) {
            parent_group->next_group_position.x += current_group->padding.right;
        } else {
            parent_group->next_group_position.y += current_group->padding.bottom;
        }
    }
    ui.group_count--;
}

void begin_ui(Layout layout, Alignment alignment, Padding padding) {
    ui.id_count = 0;
    begin_group(layout, alignment, padding);
}

void end_ui(void) {
    ui.mouse_down = NO_BUTTON;
    ui.mouse_up = NO_BUTTON;
    end_group();
}

void begin_group(Layout layout, Alignment alignment, Padding padding) {
    assert(ui.group_count < MAX_GROUPS);
    PointI next_group_position = ui.group_count > 0 ? peek_group()->next_group_position :
        (PointI) { 0, 0 };
    next_group_position.x += padding.left;
    next_group_position.y += padding.top;
    size_t n_groups = ui.group_count;
    ui.groups[n_groups] = (Group) {
        .next_item_position = next_group_position,
        .next_group_position = next_group_position,
        .layout = layout,
        .alignment = alignment,
        .padding = padding
    };
    ui.group_count++;
}

static bool is_hot(ui_id item_id) {
    return item_id == ui.hot_item;
}

static bool is_active(ui_id item_id) {
    return item_id == ui.active_item;
}

bool do_button(const char* text, Padding padding) {
    ui.id_count++;
    ui_id button_id = ui.id_count;
    bool result = false;
    bool hot = is_hot(button_id);
    Group* group = peek_group();
    PointI start_position = group->next_item_position;
    start_position.x += padding.left;
    start_position.y += padding.top;
    PointI end_position = {
        start_position.x + MENU_BUTTON_WIDTH,
        start_position.y + MENU_BUTTON_HEIGHT
    };
    
    if (is_active(button_id) && ui.mouse_up == LEFT_BUTTON) {
        if (hot) {
            result = true;
        }
        ui.active_item = 0;
    }
    else if (hot && ui.mouse_down == LEFT_BUTTON) {
        ui.active_item = button_id;
    }

    Aabb button_aabb = {start_position.x, end_position.x, start_position.y, end_position.y};
    if (intersect_point(&button_aabb, ui.mouse_position)) {
        ui.hot_item = button_id;
    } else if (hot) {
        ui.hot_item = 0;
    }

    PointI next_position = (PointI) { end_position.x + padding.right, end_position.y + padding.bottom };
    if (group->layout == HORIZONTAL_LAYOUT) {
        group->next_item_position.x = next_position.x;
    } else {
        group->next_item_position.y = next_position.y;
    }

    if (ui.group_count > 1) {
        Group* parent_group = peek_parent_group();
        if (parent_group->layout == HORIZONTAL_LAYOUT) {
            parent_group->next_group_position.x = fmax(parent_group->next_group_position.x, next_position.x);
        } else {
            parent_group->next_group_position.y = fmax(parent_group->next_group_position.y, next_position.y);
        }
    }

    // TODO: make it graphics API agnostic
    render_button(start_position, end_position, text, hot);
    return result;
}

void mouse_moved(int x, int y) {
    ui.mouse_position = (PointI) {x, y};
}

void mouse_down(MouseButton button) {
    ui.mouse_down = button;
}

void mouse_up(MouseButton button) {
    ui.mouse_up = button;
}
