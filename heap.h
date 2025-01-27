#ifndef HEAP_H
#define HEAP_H

#include "dll.h"

// Structura pentru un bloc de memorie din heap
typedef struct {
    unsigned long start_address;
    unsigned long size;
    bool is_free;
} memory_block_t;

// Structura pentru heap
typedef struct {
    dll_t **lists;
    unsigned int heap_size;
} heap_t;

// Funcții pentru gestionarea heap-ului
heap_t *create_heap(unsigned int heap_size);
void alloc(heap_t *heap, unsigned int bytes);
void free_memory(heap_t *heap, unsigned long start_address);

#endif // HEAP_H
