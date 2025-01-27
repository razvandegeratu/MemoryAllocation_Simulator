
#include "heap.h"
#include <stdio.h>
#include <stdlib.h>


int main() {
    unsigned int heap_size = 10;
    heap_t *heap = create_heap(heap_size);

    if (!heap) {
        printf("Error: Heap could not be created\n");
        return 1;
    }

    // Alocăm memorie
    alloc(heap, 1024);
    alloc(heap, 2048);

    // Eliberăm memorie
    free_memory(heap, 12345);  // Adresa poate fi schimbată la o adresă validă

    // Curățare și eliberare resurse
    for (unsigned int i = 0; i < heap_size; i++) {
        dll_node_t *current = heap->lists[i]->head;
        while (current) {
            dll_node_t *next = current->next;
            free(current->data);
            free(current);
            current = next;
        }
        free(heap->lists[i]);
    }
    free(heap->lists);
    free(heap);

    return 0;
}
