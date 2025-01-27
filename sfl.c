// Copyright: 313 CA Degeratu Razvan-Andrei

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define max_command 500
#define data_size 1
#define buffer_size 601

typedef unsigned int un_int;
typedef unsigned long long llu;
typedef long long ll;

typedef struct dll_node_t {
	void *data;
	struct dll_node_t *prev, *next;
} dll_node_t;

typedef struct dll_t {
	dll_node_t *head;
	un_int data_size_t;
	un_int node_count_per_list;
	un_int mem_per_node;
	un_int index;
	bool tip;
} dll_t;

typedef struct heap_t {
	dll_t **seg_lists;
	ll adr_start;
	un_int list_count;
	un_int m_calls;
	un_int free_calls;
	un_int frag_calls;
	un_int bytes_per_list;

} heap_t;

typedef struct block {
	un_int bytes_count;
	un_int adress;
	void *values;
	bool written;
	bool allocated;
} block;

typedef struct alloc_t {
	dll_node_t *head;
	un_int size;
	un_int data_size_t;
} alloc_t;

un_int min(un_int a, un_int b)
{
	if (a < b)
		return a;
	return b;
}

alloc_t *allocated_dll_create(un_int data_size_t)
{
	alloc_t *alloc = malloc(sizeof(alloc_t));
	alloc->data_size_t = data_size_t;
	alloc->head = NULL;
	alloc->size = 0;
	return alloc;
}

dll_t *dll_create(un_int list_index, bool tip)
{
	dll_t *list = malloc(sizeof(dll_t));
	list->head = NULL;
	list->index = list_index;
	list->data_size_t = data_size;
	list->tip = tip;
	return list;
}

dll_node_t *dll_get_nth_node_alloc(alloc_t *list,  un_int n)
{
	if (!list)
		return NULL;

	un_int count = 0;
	dll_node_t *current = list->head;

	while (count < n && current->next) {
		current = current->next;
		count++;
	}

	return current;
}

dll_node_t *dll_get_nth_node(dll_t *list, un_int n)
{
	if (!list || !list->head)
		return NULL;

	un_int count = 0;
	dll_node_t *current = list->head;

	while (count < n && current->next) {
		current = current->next;
		count++;
	}

	return current;
}

void allocated_dll_remove_nth_node(alloc_t *list, un_int n)
{
	if (n == 0) {
		dll_node_t *removed_node = list->head;
		list->head = list->head->next;
		if (list->head)
			list->head->prev = NULL;
		free(removed_node->data);
		removed_node->data = NULL;
		free(removed_node);
		removed_node = NULL;
	} else if (n >= (list->size - 1)) {
		dll_node_t *removed_node = NULL;
		dll_node_t *current = list->head;
		while (current->next)
			current = current->next;
		removed_node = current;
		if (current->prev)
			current->prev->next = NULL;
		free(removed_node->data);
		removed_node->data = NULL;
		free(removed_node);
		removed_node = NULL;
	} else if (n < (list->size - 1) && n > 0) {
		dll_node_t *removed_node = NULL;
		dll_node_t *current = dll_get_nth_node_alloc(list, n);
		if (current) {
			removed_node = current;
			current->prev->next = current->next;
			current->next->prev = current->prev;
			free(removed_node->data);
			removed_node->data = NULL;
			free(removed_node);
			removed_node = NULL;
		}
	}
	list->size--;
}

void dll_remove_nth_node(dll_t *list, un_int n)
{
	if (n == 0) {
		list->head = list->head->next;
		if (list->head) {
			list->head->prev = NULL;
			list->node_count_per_list--;
		}
	} else if (n >= (list->node_count_per_list - 1)) {
		dll_node_t *current = list->head;
		while (current->next)
			current = current->next;
		if (current->prev)
			current->prev->next = NULL;
		list->node_count_per_list--;
	}
	if (n < (list->node_count_per_list - 1) && n > 0) {
		dll_node_t *current = dll_get_nth_node(list, n);
		if (current) {
			current->prev->next = current->next;
			current->next->prev = current->prev;
			list->node_count_per_list--;
		}
	}
}

void dll_add_node_in_order(alloc_t *list, block *data)
{
	dll_node_t *new_node = malloc(sizeof(dll_node_t));
	if (!new_node) {
		printf("Error allocating memory for new node\n");
		return;
	}

	new_node->data = malloc(sizeof(block));
	if (!new_node->data) {
		printf("Error allocating memory for block structure\n");
		free(new_node);
		return;
	}

	memcpy(new_node->data, data, sizeof(block));
	new_node->prev = NULL;
	new_node->next = NULL;

	if (!list->head ||
		((block *)data)->adress < ((block *)list->head->data)->adress) {
		new_node->next = list->head;
		if (list->head)
			list->head->prev = new_node;
		list->head = new_node;
		list->size++;
		free(data);
		return;
	}

	dll_node_t *current = list->head;
	while (current->next &&
		   ((block *)current->next->data)->adress < ((block *)data)->adress)
		current = current->next;

	new_node->next = current->next;
	if (current->next)
		current->next->prev = new_node;
	current->next = new_node;
	new_node->prev = current;
	list->size++;
	free(data);
}

void dll_add_nth_node(dll_t *list, un_int n, block *block_to_add)
{
	if (!list) {
		printf("Error: List is NULL\n");
		return;
		
	}

	dll_node_t *new_node = (dll_node_t *)malloc(sizeof(dll_node_t));
	if (!new_node) {
		printf("Error allocating memory for new node\n");
		return;
	}

	new_node->data = (block *)malloc(sizeof(block));
	if (!new_node->data) {
		printf("Error allocating memory for block structure\n");
		free(new_node);
		return;
	}
	memcpy(new_node->data, block_to_add, sizeof(block));

	new_node->prev = NULL;
	new_node->next = NULL;

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
	un_int i = 0;
	while (current && i < n) {
		current = current->next;
		i++;
	}

	if (!current) {
		dll_node_t *tail = list->head;
		while (tail->next)
			tail = tail->next;
		tail->next = new_node;
		new_node->prev = tail;
		return;
	}

	new_node->prev = current->prev;
	new_node->next = current;
	if (current->prev)
		current->prev->next = new_node;
	current->prev = new_node;
}

void remove_block_from_heap(heap_t *heap, block *block_to_remove, dll_t *list)
{
	if (!heap || !block_to_remove || !list) {
		printf("Invalid input parameters for removing block from heap.\n");
		return;
	}

	dll_node_t *current = list->head;
	un_int index = 0;
	while (current) {
		block *block_in_list = current->data;
		if (block_in_list == block_to_remove) {
			dll_remove_nth_node(list, index);
			return;
		}
		current = current->next;
		index++;
	}
}

int find_pos_heap(block *block, heap_t *heap)
{
	for (un_int i = 0; i < heap->list_count; i++) {
		if (heap->seg_lists[i]->mem_per_node == block->bytes_count)
			return i;
		if (heap->seg_lists[i]->mem_per_node > block->bytes_count) {
			dll_t *new_list = dll_create(i, heap->seg_lists[i]->tip);
			new_list->data_size_t = data_size;
			new_list->mem_per_node = block->bytes_count;
			new_list->node_count_per_list = 0;

			heap->seg_lists = realloc(heap->seg_lists,
									  (heap->list_count + 1) * sizeof(dll_t *));
			if (!heap->seg_lists) {
				printf("Memory reallocation failed\n");
				return -1;
			}

			for (un_int j = heap->list_count; j > i; j--)
				heap->seg_lists[j] = heap->seg_lists[j - 1];
			heap->seg_lists[i] = new_list;
			heap->list_count++;

			return i;
		}
	}
	dll_t *new_list = dll_create(heap->list_count, heap->seg_lists[0]->tip);
	new_list->data_size_t = data_size;
	new_list->mem_per_node = block->bytes_count;
	new_list->node_count_per_list = 0;

	heap->seg_lists = realloc(heap->seg_lists,
							  (heap->list_count + 1) * sizeof(dll_t *));
	if (!heap->seg_lists) {
		printf("Memory reallocation failed\n");
		return -1;
	}

	heap->seg_lists[heap->list_count] = new_list;
	heap->list_count++;

	return heap->list_count - 1;
}

un_int find_pos_list(dll_t *list, ll address)
{
	dll_node_t *curr = list->head;
	un_int pos = 0;

	while (curr) {
		block *blockk = curr->data;

		if (blockk->adress > address)
			return pos;

		pos++;
		curr = curr->next;
	}
	return pos;
}

void fragment_block(heap_t *heap, block *block_to_frag,
					un_int bytes, dll_t *list, alloc_t *allocated_mem)
{
	heap->frag_calls++;

	block new_empty_block;
	new_empty_block.adress = block_to_frag->adress + bytes;
	new_empty_block.bytes_count = block_to_frag->bytes_count - bytes;
	new_empty_block.allocated = false;
	new_empty_block.written = false;
	new_empty_block.values = NULL;

	un_int pos = find_pos_heap(&new_empty_block, heap);
	un_int pos_in_list = find_pos_list(heap->seg_lists[pos],
	new_empty_block.adress);

	dll_add_nth_node(heap->seg_lists[pos], pos_in_list, &new_empty_block);

	heap->seg_lists[pos]->node_count_per_list++;
	block_to_frag->allocated = true;
	block_to_frag->bytes_count = bytes;
	block_to_frag->written = false;
	dll_add_node_in_order(allocated_mem, block_to_frag);
	dll_remove_nth_node(list, 0);
}

void my_malloc(heap_t *heap, un_int bytes_needed, alloc_t *allocated_mem)
{
	bool solved = false;
	for (un_int i = 0; i < heap->list_count && !solved; i++) {
		dll_t *curr_list = heap->seg_lists[i];
		if (!curr_list->head)
			continue;
		dll_node_t *curr = curr_list->head;
		while (curr && !solved) {
			block *blockk = curr->data;
			dll_node_t *next = curr->next;
			un_int bytes_count = blockk->bytes_count;
			if (bytes_count == bytes_needed && !blockk->allocated) {
				blockk->allocated = true;
				remove_block_from_heap(heap, blockk, curr_list);
				dll_add_node_in_order(allocated_mem, blockk);

				solved = true;
				heap->m_calls++;
				free(curr);
			} else if (bytes_count > bytes_needed && !blockk->allocated) {
				un_int needed = bytes_needed;
				fragment_block(heap, blockk, needed, curr_list, allocated_mem);
				solved = true;
				heap->m_calls++;
				free(curr);
			}
			curr = next;
		}
	}

	if (!solved)
		printf("Out of memory\n");

	for (un_int i = 0; i < heap->list_count; i++) {
		if (!heap->seg_lists[i]->head) {
			free(heap->seg_lists[i]);
			for (un_int j = i; j < heap->list_count - 1; j++)
				heap->seg_lists[j] = heap->seg_lists[j + 1];
			heap->list_count--;
		}
	}
}

heap_t *init_heap(ll adr_start, un_int list_count,
				  un_int bytes_per_list, bool tip)
{
	heap_t *heap = malloc(sizeof(heap_t));
	if (!heap) {
		printf("Error allocating memory for heap\n");
		exit(0);
	}
	heap->adr_start = adr_start;
	ll adress_copy = adr_start;
	heap->bytes_per_list = bytes_per_list;
	heap->frag_calls = 0;
	heap->free_calls = 0;
	heap->list_count = list_count;
	heap->m_calls = 0;
	heap->seg_lists = malloc(sizeof(dll_t *) * heap->list_count);

	for (un_int i = 0; i < heap->list_count; i++) {
		ll size_per_node = 1 << (i + 3);
		heap->seg_lists[i] = dll_create(i, tip);
		heap->seg_lists[i]->mem_per_node = size_per_node;
		un_int aux = 1 << (i + 3);
		heap->seg_lists[i]->node_count_per_list = bytes_per_list / aux;

		for (un_int j = 0; j < heap->seg_lists[i]->node_count_per_list; j++) {
			block block_to_add;
			block_to_add.adress = adress_copy;
			block_to_add.bytes_count = size_per_node;
			block_to_add.allocated = false;
			block_to_add.written = false;
			block_to_add.values = NULL;
			dll_add_nth_node(heap->seg_lists[i], j, &block_to_add);
			adress_copy += 1 << (i + 3);
		}
	}
	return heap;
}

int check_allocated_mem(alloc_t *allocated_mem)
{
	block *block_to_check;
	dll_node_t *curr = allocated_mem->head;
	int alloc = 0;
	while (curr) {
		block_to_check = curr->data;
		if (block_to_check->allocated)
			alloc += block_to_check->bytes_count;
		curr = curr->next;
	}
	return alloc;
}

un_int check_free_blocks(heap_t *heap)
{
	un_int free = 0;

	for (un_int i = 0; i < heap->list_count; i++)
		free += heap->seg_lists[i]->node_count_per_list;
	return free;
}

void dump_memory(heap_t *heap, alloc_t *allocated_mem, un_int init_list_count)
{
	un_int free = check_free_blocks(heap);
	un_int alloc_bytes = check_allocated_mem(allocated_mem);
	printf("+++++DUMP+++++\n");
	printf("Total memory: %u bytes\n", init_list_count * heap->bytes_per_list);
	printf("Total allocated memory: %u bytes\n", alloc_bytes);
	un_int total_alloc = init_list_count * heap->bytes_per_list - alloc_bytes;
	printf("Total free memory: %u bytes\n", total_alloc);
	printf("Free blocks: %d\n", free);
	printf("Number of allocated blocks: %d\n", allocated_mem->size);
	printf("Number of malloc calls: %d\n", heap->m_calls);
	printf("Number of fragmentations: %d\n", heap->frag_calls);
	printf("Number of free calls: %d\n", heap->free_calls);

	for (un_int i = 0; i < heap->list_count; i++) {
		dll_node_t *curr = NULL;
		if (heap->seg_lists[i])
			curr = heap->seg_lists[i]->head;

		block *block_t;

		if (!curr)
			continue;
		block_t = curr->data;
		un_int node_per_list = heap->seg_lists[i]->node_count_per_list;
		printf("Blocks with %u bytes", block_t->bytes_count);
		printf(" - %u free block(s) : ", node_per_list);
		dll_node_t *current = heap->seg_lists[i]->head;
		while (current) {
			block *block = current->data;
			if (!block->allocated) {
				if (current->next)
					printf("0x%x ", block->adress);
				else
					printf("0x%x", block->adress);
			}
			current = current->next;
		}
		printf("\n");
	}

	printf("Allocated blocks :");
	dll_node_t *current = allocated_mem->head;
	while (current) {
		block *block = current->data;
		if (current->next)
			printf(" (0x%x - %u)", block->adress, block->bytes_count);
		else
			printf(" (0x%x - %u)", block->adress, block->bytes_count);
		current = current->next;
	}
	printf("\n-----DUMP-----\n");
}

void destroy_heap(heap_t *heap, alloc_t *allocated_mem)
{
	for (un_int i = 0; i < heap->list_count; i++) {
		dll_node_t *current = heap->seg_lists[i]->head;
		while (current) {
			dll_node_t *temp = current;
			current = current->next;
			free(temp->data);
			free(temp);
		}
		free(heap->seg_lists[i]);
		heap->seg_lists[i] = NULL;
	}
	free(heap->seg_lists);
	heap->seg_lists = NULL;
	free(heap);
	heap = NULL;
	dll_node_t *current = allocated_mem->head;
	for (un_int i = 0; i < allocated_mem->size; i++) {
		dll_node_t *temp = current;
		block *blockk = current->data;
		if (blockk) {
			if (blockk->values)
				free(blockk->values);
		}
		current = current->next;
		free(temp->data);
		temp->data = NULL;
		free(temp);
		temp = NULL;
	}
	free(allocated_mem);
	allocated_mem = NULL;
}

int find_pos_allocated(alloc_t *allocated_mem, block *block_to_find)
{
	if (!allocated_mem || !allocated_mem->head)
		return -1;

	int pos = 0;
	dll_node_t *current = allocated_mem->head;
	while (current) {
		block *curr_block = (block *)current->data;
		if (curr_block->adress == block_to_find->adress)
			return pos;
		current = current->next;
		pos++;
	}

	return -1;
}

bool check_free(ll adress, alloc_t *allocated_mem)
{
	bool found = false;
	block *block_in_list = NULL;
	dll_node_t *current = allocated_mem->head;
	while (current) {
		block_in_list = (block *)current->data;
		if (block_in_list->adress == adress) {
			found = true;
			break;
		}

		current = current->next;
	}
	return found;
}

void free_memory_block(heap_t *heap, ll adress,
					   alloc_t *alloc_mem)
{
	bool allocated_index = check_free(adress, alloc_mem);
	if (allocated_index) {
		block block_in_list;
		block_in_list.allocated = false;
		block_in_list.adress = adress;
		block_in_list.values = NULL;
		int pos_allocated_mem = find_pos_allocated(alloc_mem, &block_in_list);
		dll_node_t *curr = alloc_mem->head;
		int i = 0;
		while (i < pos_allocated_mem) {
			curr = curr->next;
			i++;
		}
		block *curr_block = curr->data;
		if (curr_block->values)
			free(curr_block->values);
		block_in_list.bytes_count = curr_block->bytes_count;
		int index = find_pos_heap(&block_in_list, heap);

		un_int pos_in_list = find_pos_list(heap->seg_lists[index],
											block_in_list.adress);
		dll_add_nth_node(heap->seg_lists[index], pos_in_list, &block_in_list);
		heap->seg_lists[index]->node_count_per_list++;
		allocated_dll_remove_nth_node(alloc_mem, pos_allocated_mem);
		heap->free_calls++;

		dll_node_t *current = heap->seg_lists[index]->head;
		while (current && current->next) {
			block *current_block = current->data;
			block *next_block = current->next->data;
			if (current_block->adress > next_block->adress) {
				block temp = *current_block;
				*current_block = *next_block;
				*next_block = temp;
			}
			current = current->next;
		}
	} else {
		printf("Invalid free\n");
	}
}

dll_node_t *find_node_in_list(alloc_t *allocated_mem, ll address)
{
	dll_node_t *curr = allocated_mem->head;

	while (curr) {
		block *curr_block = curr->data;
		if (curr_block->adress >= address)
			return curr;
		curr = curr->next;
	}

	return NULL;
}

bool check_write_in_list(alloc_t *allocated_mem, ll address,
						 char *bytes_to_write, un_int bytes_count)
{
	dll_node_t *node = find_node_in_list(allocated_mem, address);

	if (!node)
		return false;

	block *curr_block = node->data;
	ll curr_address = curr_block->adress;
	un_int bytes_found = 0;

	un_int string_length = strlen(bytes_to_write);

	bytes_count = (bytes_count < string_length) ? bytes_count : string_length;

	while (node) {
		curr_block = node->data;

		if (curr_block->adress != curr_address)
			return false;

		bytes_found += curr_block->bytes_count;

		if (bytes_found >= bytes_count)
			return true;

		curr_address += curr_block->bytes_count;
		node = node->next;
	}

	return false;
}

dll_node_t *find_node(ll adress, alloc_t *allocated_mem)
{
	dll_node_t *curr = allocated_mem->head;
	while (curr) {
		block *blockk = curr->data;
		if (blockk->adress >= adress)
			return curr;
		curr = curr->next;
	}
	return NULL;
}

bool check_read_in_list(alloc_t *allocated_mem, ll address, un_int bytes)
{
	dll_node_t *node = find_node_in_list(allocated_mem, address);

	if (!node)
		return false;

	block *curr_block = node->data;
	ll curr_address = curr_block->adress;
	un_int bytes_found = 0;

	while (node) {
		curr_block = node->data;

		if (curr_block->adress != curr_address)
			return false;

		bytes_found += curr_block->bytes_count;

		if (bytes_found >= bytes)
			return true;

		curr_address += curr_block->bytes_count;
		node = node->next;
	}

	return false;
}

void read(heap_t *heap, alloc_t *allocated_mem, ll address,
		  size_t bytes, un_int init_list_count)
{
	bool allocated = check_read_in_list(allocated_mem, address, bytes);
	if (allocated) {
		dll_node_t *node = find_node_in_list(allocated_mem, address);
		if (node) {
			size_t bytes_read = 0;
			while (bytes_read < bytes) {
				block *block_data = node->data;
				size_t remaining_space = block_data->bytes_count -
										(address - block_data->adress);
				size_t bytes_to_read = bytes - bytes_read;
				if (bytes_to_read >= remaining_space)
					bytes_to_read = remaining_space;
				for (size_t i = 0; i < bytes_to_read; ++i) {
					un_int index = address - block_data->adress + i;
					printf("%c", ((char *)block_data->values)[index]);
				}
				bytes_read += bytes_to_read;
				address += bytes_to_read;
				if (bytes_read < bytes && node->next)
					node = node->next;
				else
					break;
			}
			printf("\n");
		}
	} else {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(heap, allocated_mem, init_list_count);
		destroy_heap(heap, allocated_mem);
		exit(0);
	}
}

void write(heap_t *heap, alloc_t *allocated_mem, ll address,
		   char *bytes_to_write, un_int bytes_count, un_int init_list_count)
{
	bool allocated = check_write_in_list(allocated_mem,
					address, bytes_to_write, bytes_count);

	if (allocated) {
		dll_node_t *node = find_node_in_list(allocated_mem, address);
		bytes_count = min(bytes_count, strlen(bytes_to_write));
		if (node) {
			block *block_data = node->data;

			un_int block_size = block_data->bytes_count;

			if (!block_data->values) {
				block_data->values = (char *)malloc(block_size * sizeof(char));
				if (!block_data->values) {
					printf("Error: Memory allocation failed\n");
					return;
				}
			}

			un_int bytes_written = 0;
			while (bytes_written < bytes_count) {
				un_int remaining_space = block_size;
				un_int bytes_left = bytes_count - bytes_written;
				un_int bytes_to_copy = (bytes_left < remaining_space) ?
				(bytes_count - bytes_written) : remaining_space;

				memcpy(block_data->values, bytes_to_write + bytes_written,
					   bytes_to_copy);
				bytes_written += bytes_to_copy;

				if (bytes_written < bytes_count) {
					node = node->next;

					block_data = node->data;
					block_size = block_data->bytes_count;

					if (!block_data->values)
						block_data->values = malloc(block_size * sizeof(char));
				}
			}
		}
	} else {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(heap, allocated_mem, init_list_count);
		destroy_heap(heap, allocated_mem);
		exit(0);
		return;
	}
}

int main(void)

{
	char command[max_command];
	alloc_t *allocated_mem = allocated_dll_create(data_size);
	ll init_list_count = 0;
	heap_t *HEAP = NULL;

	while (1) {
		scanf("%255s", command);

		if (strcmp(command, "INIT_HEAP") == 0) {
			ll adr_start;
			un_int bytes_per_list, list_count, tip;
			scanf("%llx", &adr_start);
			scanf("%u%u%u", &list_count, &bytes_per_list, &tip);
			init_list_count = list_count;
			bool tip_r = tip;
			HEAP = init_heap(adr_start, list_count, bytes_per_list, tip_r);
		} else if (strcmp(command, "MALLOC") == 0) {
			un_int bytes_needed;
			scanf("%u", &bytes_needed);
			my_malloc(HEAP, bytes_needed, allocated_mem);
		} else if (strcmp(command, "DUMP_MEMORY") == 0) {
			dump_memory(HEAP, allocated_mem, init_list_count);
		} else if (strcmp(command, "DESTROY_HEAP") == 0) {
			destroy_heap(HEAP, allocated_mem);
			return 0;
		} else if (strcmp(command, "FREE") == 0) {
			ll address;
			scanf("%llx", &address);
			if (address == 0) {
				free(NULL);
				continue;
			}
			free_memory_block(HEAP, address, allocated_mem);
		} else if (strcmp(command, "WRITE") == 0) {
			char data[buffer_size];
			ll addr;
			int n_bytes;
			int offset;
			scanf("%llx%*[^\"]\"%601[^\"]\"%n%d",
				  &addr, data, &offset, &n_bytes);
			write(HEAP, allocated_mem, addr, data, n_bytes, init_list_count);
		} else if (strcmp(command, "READ") == 0) {
			ll addr;
			un_int bytes;
			scanf("%llx%u", &addr, &bytes);
			read(HEAP, allocated_mem, addr, bytes, init_list_count);
		}
	}
	return 0;
}
