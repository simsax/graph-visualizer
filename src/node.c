#include "node.h"

static inline int grow_capacity(int old_capacity) {
    return old_capacity < 8 ? 8 : old_capacity * 2;
}

Node* new_node(uint32_t label, int weight, uint32_t label_color, int label_size) {
    Node* node = malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Failed to allocate memory for Node.\n");
        exit(EXIT_FAILURE);
    }
    char label_str[5];
    node->label = label;
    sprintf(&label_str[0], "%d", node->label);
    init_text(&node->text, &label_str[0], label_color, label_size);
    node->weight = weight;
    node->next = NULL;
    return node;
}


void init_node_array(NodeArray* node_array) {
    node_array->nodes = NULL;
    node_array->capacity = 0;
    node_array->count = 0;
}

void add_node(NodeArray* node_array, Node* node) {
    if (node_array->count == node_array->capacity) {
        node_array->capacity = grow_capacity(node_array->capacity);
        node_array->nodes = realloc(node_array->nodes, node_array->capacity * sizeof(Node*));
        if (node_array->nodes == NULL) {
            fprintf(stderr, "Failed to realloc NodeArray.\n");
            exit(EXIT_FAILURE);
        }
    }
    node_array->nodes[node_array->count++] = node;
}

void free_node_array(NodeArray* node_array) {
    for (size_t i = 0; i < node_array->count; i++) {
        free_text(&node_array->nodes[i]->text);
        free(node_array->nodes[i]);
    }
    init_node_array(node_array);
}