/*
 * @vm_heap.c
 *
 * @brief Stack VM
 * @details
 * This is based on other projects:
 *   Tiny language: https://github.com/goodpaul6/Tiny
 *   Others (see individual files)
 *
 *   please contact their authors for more information.
 *
 * @author Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * @version 0.1
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "vm_heap.h"
#include "vm.h"

#define GET_BIT(v, b)   ((v >> b) & 1)
#define SET_BIT(v, b)   ((v) | (1 << b))
#define CLR_BIT(v, b)   ((v) & (~(1 << b)))

#define HEAP_ALLOC_WORD(heap_pos)                      (heap_pos / 32)
#define HEAP_ALLOC_BIT(heap_pos)                       (heap_pos % 32)
#define HEAP_POS(heap_alloc_word, heap_alloc_bit)      ((heap_alloc_word * 32) + heap_alloc_bit)

#define vm_heap_set_used(heap_alloc_word, heap_pos)    heap_alloc_word[HEAP_ALLOC_WORD(heap_pos)] = (SET_BIT(heap_alloc_word[HEAP_ALLOC_WORD(heap_pos)], HEAP_ALLOC_BIT(heap_pos)))
#define vm_heap_set_unused(heap_alloc_word, heap_pos)  heap_alloc_word[HEAP_ALLOC_WORD(heap_pos)] = (CLR_BIT(heap_alloc_word[HEAP_ALLOC_WORD(heap_pos)], HEAP_ALLOC_BIT(heap_pos)))
#define vm_heap_isused(heap_alloc_word, heap_pos)      (GET_BIT(heap_alloc_word[HEAP_ALLOC_WORD(pos)], HEAP_ALLOC_BIT(heap_pos)) ? 1 : 0)

static uint32_t vm_heap_pos = 0;

vm_heap_t* vm_heap_create(uint32_t size) {
    if (size == 0)
        size = 1;

    static vm_heap_t *heap;

    heap = malloc(sizeof(vm_heap_t));
    heap->allocated = calloc(((size - 1) / 32) + 1, sizeof(uint32_t));
    heap->size = size;
    heap->data = calloc(size, sizeof(vm_value_t));
    return heap;
}

void vm_heap_destroy(vm_heap_t *heap) {
    free(heap->allocated);
    free(heap->data);
    free(heap);
}

uint32_t vm_heap_save(vm_heap_t *heap, vm_value_t value) {
    uint32_t allocated_word = 0xffffffff;

    // search free heap position
    while (++allocated_word <= HEAP_ALLOC_WORD(heap->size - 1)) {
        if (heap->allocated[allocated_word] == 0xffffffff) { // block is full allocated, try next
            continue;
        } else {
            for (uint8_t b = 0; b < 32; b++) {
                if (!GET_BIT(heap->allocated[allocated_word], b)) {
                    if (HEAP_POS(allocated_word, b) > heap->size - 1)
                        goto grow;
                    else {
                        vm_heap_pos = HEAP_POS(allocated_word, b);
                        goto save;
                    }
                }
            }
        }
    }

grow:
    // no more space, then grow
    if (heap->size + 1 > VM_MAX_HEAP)
        return 0xffffffff;

    heap->data = realloc(heap->data, (++heap->size + 1) * sizeof(vm_value_t));

    if (HEAP_ALLOC_WORD(heap->size) == HEAP_ALLOC_WORD(heap->size) - HEAP_ALLOC_BIT(heap->size)) { // need more allocated positions
        heap->allocated = realloc(heap->allocated, (HEAP_ALLOC_WORD(heap->size) + 1) * sizeof(uint32_t));
        heap->allocated[HEAP_ALLOC_WORD(heap->size)] = 0;
    }

    vm_heap_pos = heap->size - 1;

save:
    heap->data[vm_heap_pos] = value;
    vm_heap_set_used(heap->allocated, vm_heap_pos);

    return vm_heap_pos;
}

vm_value_t vm_heap_load(vm_heap_t *heap, uint32_t pos) {
    if (pos > heap->size - 1 || !vm_heap_isused(heap->allocated, pos))
        return vm_value_null;

    return heap->data[pos];
}

void vm_heap_free(vm_heap_t *heap, uint32_t pos) {
    if (pos <= heap->size - 1)
        vm_heap_set_unused(heap->allocated, pos);
}
