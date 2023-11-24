#include "ui.h"
#include "common.h"
#include "physics.h"
#include "render.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static Ui ui;

void init_ui(void) {
  ui.active_item = 0;
  ui.hot_item = 0;
  ui.mouse_position = (PointI){0, 0};
  ui.mouse_down = NO_BUTTON;
  ui.mouse_up = NO_BUTTON;
  ui.dropdown_open = false;
}

static Group *peek_group(void) {
  assert(ui.group_count > 0);
  return &ui.groups[ui.group_count - 1];
}

static Group *peek_parent_group(void) {
  assert(ui.group_count > 1);
  return &ui.groups[ui.group_count - 2];
}

void end_group(void) {
  assert(ui.group_count > 0);
  ui.group_count--;
}

void begin_ui(Layout layout, Alignment alignment, Padding padding, PointI size,
              PointI starting_pos) {
  ui.id_count = 0;
  begin_group(layout, alignment, padding, 1);
  size.x = size.x - padding.left - padding.right;
  size.y = size.y - padding.top - padding.bottom;

  Group *root = peek_group();
  root->next_item_position.x += starting_pos.x;
  root->next_item_position.y += starting_pos.y;
  root->size = size;

#if DEBUG_UI
  render_rect(root->next_item_position,
              (PointI){root->next_item_position.x + size.x,
                       root->next_item_position.y + size.y},
              YELLOW);
#endif
}

void end_ui(void) {
  ui.mouse_down = NO_BUTTON;
  ui.mouse_up = NO_BUTTON;
  end_group();
}

void begin_group(Layout layout, Alignment alignment, Padding padding,
                 float fill_perc) {
  size_t n_groups = ui.group_count;
  assert(n_groups < MAX_GROUPS);
  if ((alignment == LEFT_ALIGNMENT || alignment == RIGHT_ALIGNMENT) &&
      layout == HORIZONTAL_LAYOUT) {
    fprintf(stderr,
            "Left/Right alignments are supported only in a vertical layout\n");
    exit(EXIT_FAILURE);
  }
  if ((alignment == TOP_ALIGNMENT || alignment == BOTTOM_ALIGNMENT) &&
      layout == VERTICAL_LAYOUT) {
    fprintf(
        stderr,
        "Top/Bottom alignments are supported only in a horizontal layout\n");
    exit(EXIT_FAILURE);
  }
  Group *parent = NULL;
  PointI next_item_position = (PointI){0, 0};
  PointI size = (PointI){0, 0};
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
  ui.groups[n_groups] = (Group){.next_item_position = next_item_position,
                                .layout = layout,
                                .alignment = alignment,
                                .padding = padding,
                                .size = size};
  ui.group_count++;

#if DEBUG_UI
  if (n_groups > 0) {
    render_rect(
        next_item_position,
        (PointI){next_item_position.x + size.x, next_item_position.y + size.y},
        BLUE);
  }
#endif
}

static bool is_hot(ui_id item_id) { return item_id == ui.hot_item; }

static bool is_active(ui_id item_id) { return item_id == ui.active_item; }

static void update_layout(Group* group, PointI* start_pos, PointI* end_pos, PointI item_size, Padding padding) {
    switch (group->alignment) {
        case CENTER_ALIGNMENT:
            if (group->layout == VERTICAL_LAYOUT) {
                start_pos->x =
                    start_pos->x + (int)(group->size.x / 2) - (int)item_size.x / 2;
                start_pos->y += padding.top;
            } else {
                start_pos->y =
                    start_pos->y + (int)(group->size.y / 2) - (int)item_size.y / 2;
                start_pos->x += padding.left;
            }
            break;
        case LEFT_ALIGNMENT:
            start_pos->x += padding.left;
            start_pos->y += padding.top;
            break;
        case RIGHT_ALIGNMENT:
            start_pos->x =
                start_pos->x + group->size.x - item_size.x - padding.right;
            start_pos->y += padding.top;
            break;
        case TOP_ALIGNMENT:
            start_pos->x += padding.left;
            start_pos->y += padding.top;
            break;
        case BOTTOM_ALIGNMENT:
            start_pos->x += padding.left;
            start_pos->y =
                start_pos->y + group->size.y - item_size.y - padding.bottom;
            break;
        default:
            // unreachable
            exit(EXIT_FAILURE);
            break;
    }
    *end_pos = (PointI) {start_pos->x + item_size.x,
        start_pos->y + item_size.y};

    PointI next_position =
        (PointI){end_pos->x + padding.right, end_pos->y + padding.bottom};
    if (group->layout == HORIZONTAL_LAYOUT) {
        group->next_item_position.x = next_position.x;
    } else {
        group->next_item_position.y = next_position.y;
    }

    // this behavior will go away when I'll add scrollbars
    if (ui.group_count > 1) {
        Group *parent_group = peek_parent_group();
        if (parent_group->layout == HORIZONTAL_LAYOUT) {
            parent_group->next_item_position.x =
                fmax(parent_group->next_item_position.x, next_position.x);
        } else {
            parent_group->next_item_position.y =
                fmax(parent_group->next_item_position.y, next_position.y);
        }
    }
}

bool do_button(const char *text, Padding padding, int size, PointI button_pos, PointI button_size,
        int roundness, uint32_t color) {
    // size is the pixel height of the text
    ui.id_count++;
    ui_id button_id = ui.id_count;
    bool result = false;
    bool hot = is_hot(button_id);
    bool active = is_active(button_id);
    Group *group = peek_group();
    PointI start_position = group->next_item_position;

    float scaling_factor = (float)size / FONT_OFFSET_Y;
    int char_width = scaling_factor * FONT_OFFSET_X;
    size_t text_len = strlen(text);
    int text_width = text_len * char_width;
    int text_padding = 4;
    if (button_size.x == 0)
        button_size.x = text_padding * 2 + text_width;
    if (button_size.y == 0)
        button_size.y = text_padding * 2 + size;

    PointI end_position;
    
    if (button_pos.x != -1) {
        start_position = button_pos;
        end_position = (PointI) {button_pos.x + button_size.x,
            button_pos.y + button_size.y};
    } else {
        update_layout(group, &start_position, &end_position, button_size, padding);
    }

    if (active && ui.mouse_up == LEFT_BUTTON) {
        if (hot) {
            result = true;
        }
        ui.active_item = 0;
    } else if (hot && ui.mouse_down == LEFT_BUTTON) {
        ui.active_item = button_id;
    }

    Aabb button_aabb = {start_position.x, end_position.x, start_position.y,
        end_position.y};
    if (intersect_point(&button_aabb, ui.mouse_position)) {
        ui.hot_item = button_id;
    } else if (hot) {
        ui.hot_item = 0;
    }

    // TODO: make it graphics API agnostic
    render_button(text, hot, active, start_position, (PointI){text_width, size}, button_size, 
            roundness, color);
    return result;
}

void mouse_moved(int x, int y) { ui.mouse_position = (PointI){x, y}; }

void mouse_down(MouseButton button) { ui.mouse_down = button; }

void mouse_up(MouseButton button) { ui.mouse_up = button; }

bool do_textbox(const char *text, Padding padding, int text_padding, Alignment alignment,
        PointI char_size, uint32_t color, PointI box_pos, PointI box_size,
        PointI* final_pos, PointI* final_size, bool clickable) {
    Group *group = peek_group();
    PointI start_position = group->next_item_position;

    size_t text_len = strlen(text);
    int text_width = text_len * char_size.x;
    if (box_size.x == 0)
        box_size.x = text_padding * 2 + text_width;
    if (box_size.y == 0)
        box_size.y = text_padding * 2 + char_size.y;

    PointI end_position;

    if (box_pos.x != -1) {
        start_position = box_pos;
        end_position = (PointI) {
            box_pos.x + box_size.x,
                box_pos.y + box_size.y
        };
    } else {
        update_layout(group, &start_position, &end_position, box_size, padding);
    }

    if (final_size != NULL) {
        *final_size = box_size;
    }
    if (final_pos != NULL) {
        *final_pos = start_position;
    }

    bool result = false;
    bool hot = false;
    bool active = false;
    if (clickable) {
        ui.id_count++;
        ui_id item_id = ui.id_count;
        hot = is_hot(item_id);
        active = is_active(item_id);

        if (active && ui.mouse_up == LEFT_BUTTON) {
            if (hot) {
                result = true;
            }
            ui.active_item = 0;
        } else if (hot && ui.mouse_down == LEFT_BUTTON) {
            ui.active_item = item_id;
        }

        Aabb item_aabb = {start_position.x, end_position.x, start_position.y,
            end_position.y};
        if (intersect_point(&item_aabb, ui.mouse_position)) {
            ui.hot_item = item_id;
        } else if (hot) {
            ui.hot_item = 0;
        }
    }

    render_textbox(text, hot, active, start_position, box_size, (PointI) {text_width, char_size.y}, color,
            alignment, text_padding);
    return result;
}

void do_input_uint(uint64_t* input_number, Padding padding, int text_size, PointI box_size) {
    char number_string[8];
    // add 0 padding or just make the textbox fixed width
    snprintf(number_string, sizeof number_string, "%02zu", *input_number);

    // TODO: find prettier solution; change button size
    PointI box_pos;
    do_textbox(number_string, padding, 4, CENTER_ALIGNMENT, CHAR_SIZE(text_size), 0xFF222222, (PointI) {-1, -1},
            (PointI) {0, 0}, &box_pos, &box_size, false);
    PointI button_size = {
        box_size.y / 2,
        box_size.y / 2,
    };
    PointI pos_plus = (PointI){box_pos.x + box_size.x, box_pos.y};
    PointI pos_minus = {pos_plus.x, pos_plus.y + button_size.y};
    bool plus_pressed = do_button("+", padding, text_size * 0.5, pos_plus, button_size, 0, COLOR6);
    bool minus_pressed = do_button("-", padding, text_size * 0.5, pos_minus, button_size, 0, COLOR6);
    if (plus_pressed) {
        (*input_number)++;
    }
    if (minus_pressed && *input_number > 0) {
        (*input_number)--;
    }
}

void do_dropdown(int* cur_val, const char* enum_string[], int num_options, Padding padding,
        int text_size) {
    // find a way to pass generic enum without getting that warning
    bool user_interacted = false;
    PointI prev_pos;
    PointI prev_size;
    PointI char_size = CHAR_SIZE(text_size);
    size_t longest_string = 0;
    int text_padding = 4;
    uint8_t line_thickness = 2;
    uint32_t box_color = 0xFF222222;
    for (int i = 0; i < num_options; i++) {
        size_t len_i = strlen(enum_string[i]);
        if (len_i > longest_string)
            longest_string = len_i;
    }
    longest_string += 2; // counts for the "> "
    int text_width = longest_string * char_size.x;
    PointI box_size = { text_padding * 2 + text_width, text_padding * 2 + char_size.y };

    char default_string[256];
    if (!ui.dropdown_open) {
        // close case
        snprintf(default_string, sizeof default_string, "> %s", enum_string[*cur_val]);
        if (do_textbox(default_string, padding, text_padding, LEFT_ALIGNMENT, char_size, COLOR6,
                    (PointI) {-1, -1}, box_size, NULL, NULL, true)) {
            ui.dropdown_open = !ui.dropdown_open;
            user_interacted = true;
        }
    } else {
        // open case
        bool arrow_pressed = do_textbox("v", padding, text_padding, LEFT_ALIGNMENT, char_size,
                box_color, (PointI) {-1, -1}, box_size, &prev_pos, &prev_size, true);
        int next_y = prev_pos.y + prev_size.y + 1;
        render_line(
                (PointI) {prev_pos.x, next_y },
                (PointI) {prev_pos.x + box_size.x, next_y },
                line_thickness, COLOR7);
        prev_pos.y += line_thickness;
        if (arrow_pressed) {
            ui.dropdown_open = !ui.dropdown_open;
            user_interacted = true;
        } else {
            for (int i = 0; i < num_options; i++) {
                uint32_t textbox_color = i == *cur_val ? COLOR6 : 0xFF222222;
                bool item_selected = do_textbox(enum_string[i], padding, text_padding,
                        LEFT_ALIGNMENT, char_size, textbox_color,
                        (PointI) {prev_pos.x, prev_pos.y + prev_size.y}, box_size,
                        &prev_pos, &prev_size, i != *cur_val);
                int next_y = prev_pos.y + prev_size.y + 1;
                if (i != num_options - 1) {
                    render_line(
                            (PointI) {prev_pos.x, next_y},
                            (PointI) {prev_pos.x + box_size.x, next_y},
                            line_thickness, COLOR7);
                    prev_pos.y += line_thickness;
                }
                if (item_selected) {
                    *cur_val = i;
                    ui.dropdown_open = !ui.dropdown_open;
                    user_interacted = true;
                }
            }
        }
    }

    // user has clicked outside the widget, so it should close
    if (!user_interacted && ui.mouse_up == LEFT_BUTTON) {
        ui.dropdown_open = false;
    }
}
