#ifndef DLL_H
#define DLL_H

#include <stdbool.h>

// Structura nodului listei dublu înlănțuite
typedef struct dll_node_t {
    void *data;
    struct dll_node_t *prev, *next;
} dll_node_t;

// Structura listei dublu înlănțuite
typedef struct dll_t {
    dll_node_t *head;
    unsigned int data_size_t;
    unsigned int node_count_per_list;
    unsigned int mem_per_node;
    unsigned int index;
    bool tip;
} dll_t;

// Funcții pentru gestionarea listei dublu înlănțuite
dll_t *dll_create(unsigned int list_index, bool tip);
dll_node_t *dll_get_nth_node(dll_t *list, unsigned int n);
void dll_add_nth_node(dll_t *list, unsigned int n, void *data);
void dll_remove_nth_node(dll_t *list, unsigned int n);

#endif // DLL_H
