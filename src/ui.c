#include "ui.h"
#include "common.h"
#include "render.h"
#include "physics.h"
#include <assert.h>
#include <stdlib.h>

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
    ui.group_count--;
}

void begin_ui(Layout layout, Alignment alignment, Padding padding, PointI size) {
    ui.id_count = 0;
    begin_group(layout, alignment, padding, 1);
    size.x = size.x - padding.left - padding.right;
    size.y = size.y - padding.top - padding.bottom;
    peek_group()->size = size;

#if DEBUG_UI
    Group* root = peek_group();
    render_rect(root->next_item_position, (PointI) {root->next_item_position.x + size.x,
            root->next_item_position.y + size.y}, YELLOW);
#endif
}

void end_ui(void) {
    ui.mouse_down = NO_BUTTON;
    ui.mouse_up = NO_BUTTON;
    end_group();
}

void begin_group(Layout layout, Alignment alignment, Padding padding, float fill_perc) {
    size_t n_groups = ui.group_count;
    assert(n_groups < MAX_GROUPS);
    if ((alignment == LEFT_ALIGNMENT || alignment == RIGHT_ALIGNMENT) && layout == HORIZONTAL_LAYOUT) {
        fprintf(stderr, "Left/Right alignments are supported only in a vertical layout\n");
        exit(EXIT_FAILURE);
    }
    if ((alignment == TOP_ALIGNMENT || alignment == BOTTOM_ALIGNMENT) && layout == VERTICAL_LAYOUT) {
        fprintf(stderr, "Top/Bottom alignments are supported only in a horizontal layout\n");
        exit(EXIT_FAILURE);
    }
    Group* parent = NULL;
    PointI next_item_position = (PointI) { 0, 0 };
    PointI size = (PointI) {0, 0};
    if (n_groups > 0) {
        parent = peek_group();
        next_item_position = parent->next_item_position;
        size = parent->size;
        if (parent->layout == HORIZONTAL_LAYOUT) {
            size.x *= fill_perc;
            // parent doesn't care about children groups internal padding
            parent->next_item_position.x += size.x;
        } else {
            size.y *= fill_perc;
            parent->next_item_position.y += size.y;
        }
    }
    size.x = size.x - padding.left - padding.right;
    size.y = size.y - padding.top - padding.bottom;
    next_item_position.x += padding.left;
    next_item_position.y += padding.top;
    ui.groups[n_groups] = (Group) {
        .next_item_position = next_item_position,
        .layout = layout,
        .alignment = alignment,
        .padding = padding,
        .size = size
    };
    ui.group_count++;

#if DEBUG_UI
    if (n_groups > 0) {
        render_rect(next_item_position, (PointI) {next_item_position.x + size.x,
                next_item_position.y + size.y}, BLUE);
    }
#endif
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
    switch (group->alignment) {
        case CENTER_ALIGNMENT:
            if (group->layout == VERTICAL_LAYOUT) {
                start_position.x =
                    start_position.x + (int) (group->size.x / 2) - (int) MENU_BUTTON_WIDTH / 2;
            } else {
                start_position.y =
                    start_position.y + (int) (group->size.y / 2) - (int) MENU_BUTTON_HEIGHT / 2;
            }
            break;
        case LEFT_ALIGNMENT:
            start_position.x += padding.left;
            start_position.y += padding.top;
            break;
        case RIGHT_ALIGNMENT:
            start_position.x = start_position.x + group->size.x - MENU_BUTTON_WIDTH - padding.right;
            start_position.y += padding.top;
            break;
        case TOP_ALIGNMENT:
            start_position.x += padding.left;
            start_position.y += padding.top;
            break;
        case BOTTOM_ALIGNMENT:
            start_position.x += padding.left;
            start_position.y = start_position.y + group->size.y - MENU_BUTTON_HEIGHT - padding.bottom;
            break;
        default:
            // unreachable
            exit(EXIT_FAILURE);
            break;
    }
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

    // this behavior will go away when I'll add scrollbars
    if (ui.group_count > 1) {
        Group* parent_group = peek_parent_group();
        if (parent_group->layout == HORIZONTAL_LAYOUT) {
            parent_group->next_item_position.x = fmax(parent_group->next_item_position.x, next_position.x);
        } else {
            parent_group->next_item_position.y = fmax(parent_group->next_item_position.y, next_position.y);
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
