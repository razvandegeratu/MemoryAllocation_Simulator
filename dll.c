#include "dll.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

dll_t *dll_create(unsigned int list_index, bool tip) {
    dll_t *list = malloc(sizeof(dll_t));
    list->head = NULL;
    list->index = list_index;
    list->data_size_t = sizeof(void *);
    list->tip = tip;
    return list;
}

dll_node_t *dll_get_nth_node(dll_t *list, unsigned int n) {
    if (!list || !list->head) return NULL;
    unsigned int count = 0;
    dll_node_t *current = list->head;
    while (count < n && current->next) {
        current = current->next;
        count++;
    }
    return current;
}

void dll_add_nth_node(dll_t *list, unsigned int n, void *data) {
    if (!list) {
        printf("Error: List is NULL\n");
        return;
    }

    dll_node_t *new_node = (dll_node_t *)malloc(sizeof(dll_node_t));
    if (!new_node) {
        printf("Error allocating memory for new node\n");
        return;
    }
    new_node->data = malloc(list->data_size_t);
    memcpy(new_node->data, data, list->data_size_t);

    if (!list->head) {
        list->head = new_node;
        return;
    }

    if (n == 0) {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
        return;
    }

    dll_node_t *current = list->head;
    unsigned int i = 0;
    while (current && i < n) {
        current = current->next;
        i++;
    }

    if (!current) {
        dll_node_t *tail = list->head;
        while (tail->next) tail = tail->next;
        tail->next = new_node;
        tail->next = new_node;
        new_node->prev = tail;
    } else {
        new_node->next = current;
        new_node->prev = current->prev;
        if (current->prev) {
            current->prev->next = new_node;
        }
        current->prev = new_node;
    }
}

void dll_remove_nth_node(dll_t *list, unsigned int n) {
    if (!list || !list->head) {
        printf("Error: List is NULL or empty\n");
        return;
    }

    dll_node_t *to_remove = dll_get_nth_node(list, n);
    if (!to_remove) {
        printf("Error: Node does not exist\n");
        return;
    }

    if (to_remove->prev) {
        to_remove->prev->next = to_remove->next;
    } else {
        // Removing the head node
        list->head = to_remove->next;
    }

    if (to_remove->next) {
        to_remove->next->prev = to_remove->prev;
    }

    free(to_remove->data);
    free(to_remove);
}
