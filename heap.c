#include "heap.h"
#include <stdlib.h>
#include <stdio.h>

heap_t *create_heap(unsigned int heap_size) {
    heap_t *heap = malloc(sizeof(heap_t));
    if (!heap) {
        printf("Error: Could not allocate heap\n");
        return NULL;
    }

    heap->lists = malloc(sizeof(dll_t*) * heap_size);
    if (!heap->lists) {
        printf("Error: Could not allocate lists\n");
        free(heap);
        return NULL;
    }

    for (unsigned int i = 0; i < heap_size; i++) {
        heap->lists[i] = dll_create(i, true);
    }

    heap->heap_size = heap_size;
    return heap;
}

void alloc(heap_t *heap, unsigned int bytes) {
    memory_block_t *block = malloc(sizeof(memory_block_t));
    if (!block) {
        printf("Error: Could not allocate memory block\n");
        return;
    }

    block->size = bytes;
    block->start_address = rand() % 100000; // Exemplu de adresă
    block->is_free = false;

    dll_add_nth_node(heap->lists[0], 0, block); // Adaugă în prima listă, de exemplu
    printf("Allocated %u bytes at address %lu\n", bytes, block->start_address);
}

void free_memory(heap_t *heap, unsigned long start_address) {
    for (unsigned int i = 0; i < heap->heap_size; i++) {
        dll_node_t *current = heap->lists[i]->head;
        while (current) {
            memory_block_t *block = (memory_block_t *)current->data;
            if (block->start_address == start_address) {
                block->is_free = true;
                printf("Freed memory at address %lu\n", start_address);
                return;
            }
            current = current->next;
        }
    }
    printf("Error: Memory address not found\n");
}
