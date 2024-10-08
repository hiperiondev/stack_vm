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
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vm.h"

vm_heap_object_t vm_heap_object_null = { VM_VAL_NULL };

vm_heap_t* vm_heap_create(uint32_t size) {
    if (size == 0)
        size = 1;

    static vm_heap_t *heap;

    heap = malloc(sizeof(vm_heap_t));
    heap->allocated = calloc(((size - 1) / 32) + 1, sizeof(uint32_t));
    heap->size = size;
    heap->data = calloc(size, sizeof(vm_heap_object_t));
    return heap;
}

void vm_heap_destroy(vm_heap_t *heap, vm_thread_t **thread) {
    vm_heap_gc_collect(heap, &(heap->allocated), true, thread, true);
    free(heap->data);
    free(heap);
}

uint32_t vm_heap_save(vm_heap_t *heap, vm_heap_object_t value, uint32_t **gc_mark) {
    uint32_t allocated_word = 0xffffffff;
    uint32_t vm_heap_pos = 0;

    // search free heap position
    while (++allocated_word <= ID_ALLOC_WORD(heap->size - 1)) {
        if (heap->allocated[allocated_word] == 0xffffffff) { // block is fully allocated, try next
            continue;
        } else {
            for (uint8_t b = 0; b < 32; b++) {
                if (!GET_BIT(heap->allocated[allocated_word], b)) {
                    if (ID_POS(allocated_word, b) > heap->size - 1)
                        goto grow;
                    else {
                        vm_heap_pos = ID_POS(allocated_word, b);
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

    heap->data = realloc(heap->data, (++heap->size + 1) * sizeof(vm_heap_object_t));

    if (ID_ALLOC_WORD(heap->size) == ID_ALLOC_WORD(heap->size) - ID_ALLOC_BIT(heap->size)) { // need more allocated positions
        heap->allocated = realloc(heap->allocated, (ID_ALLOC_WORD(heap->size) + 1) * sizeof(uint32_t));
        heap->allocated[ID_ALLOC_WORD(heap->size)] = 0;

        *gc_mark = realloc(*gc_mark, (ID_ALLOC_WORD(heap->size) + 1) * sizeof(uint32_t));
        *gc_mark[ID_ALLOC_WORD(heap->size)] = 0;
    }

    vm_heap_pos = heap->size - 1;

save:

    memcpy(heap->data + vm_heap_pos, &value, sizeof(vm_heap_object_t));
    vm_wordpos_set_bit(heap->allocated, vm_heap_pos);
    vm_wordpos_set_bit((*gc_mark), vm_heap_pos);

    return vm_heap_pos;
}

vm_heap_object_t* vm_heap_load(vm_heap_t *heap, uint32_t pos) {
    if (pos > heap->size - 1 || !vm_wordpos_isset_bit(heap->allocated, pos))
        return &vm_heap_object_null;

    return &(heap->data[pos]);
}

bool vm_heap_set(vm_heap_t *heap, vm_heap_object_t value, uint32_t pos) {
    if(!vm_heap_isallocated(heap, pos))
        return false;

    memcpy(heap->data + pos, &value, sizeof(vm_heap_object_t));

    return true;
}

void vm_heap_free(vm_heap_t *heap, uint32_t pos) {
    if (pos <= heap->size - 1)
        vm_wordpos_unset_bit(heap->allocated, pos);
}

bool vm_heap_isallocated(vm_heap_t *heap, uint32_t pos) {
    return vm_wordpos_isset_bit(heap->allocated, pos);
}

bool vm_heap_isgc(vm_heap_t *heap, uint32_t pos, uint32_t *gc_mark) {
    return vm_wordpos_isset_bit(gc_mark, pos);
}

bool vm_heap_isstatic(vm_heap_t *heap, uint32_t pos) {
    if (pos > heap->size - 1 || !vm_wordpos_isset_bit(heap->allocated, pos))
        return false;

    return heap->data[pos].static_obj;
}

void vm_heap_gc_collect(vm_heap_t *heap, uint32_t **gc_mark, bool free_mark, vm_thread_t **thread, bool full) {
    uint32_t allocated_word = 0xffffffff;

    while (++allocated_word < ((ID_ALLOC_WORD(heap->size)) / 32) + 1) {
        if ((*gc_mark)[allocated_word] == 0) { // block is not used
            continue;
        } else {
            for (uint8_t b = 0; b < 32; b++) {
                if (GET_BIT((*gc_mark)[allocated_word], b) && (!(heap->data[ID_POS(allocated_word, b)].static_obj) || full)) {
                    switch (heap->data[ID_POS(allocated_word, b)].type) {
                        case VM_VAL_LIB_OBJ: {
                            uint32_t idx = ID_POS(allocated_word, b);
                            (*thread)->externals->lib[heap->data[ID_POS(allocated_word, b)].lib_obj.lib_idx](thread, VM_EDFAT_GC,
                                    heap->data[ID_POS(allocated_word, b)].lib_obj.lib_idx, idx);
                        }
                            break;
                        case VM_VAL_GENERIC: {
                            if (heap->data[ID_POS(allocated_word, b)].value.type == VM_VAL_CONST_STRING
                                    && heap->data[ID_POS(allocated_word, b)].value.cstr.is_program == false)
                                free(heap->data[ID_POS(allocated_word, b)].value.cstr.addr);
                        }
                            break;
                        case VM_VAL_ARRAY:
                            free(heap->data[ID_POS(allocated_word, b)].array.fields);
                            break;
                        default:
                    }

                    vm_wordpos_unset_bit((*gc_mark), ID_POS(allocated_word, b));
                    vm_wordpos_unset_bit(heap->allocated, ID_POS(allocated_word, b));
                }
            }
        }
    }

    if (free_mark)
        free(*gc_mark);
}

uint32_t* vm_heap_new_gc_mark(vm_heap_t *heap) {
    return calloc(((ID_ALLOC_WORD(heap->size)) / 32) + 1, sizeof(uint32_t));
}

void vm_heap_shrink(vm_heap_t *heap) {
    if (heap->size == 0)
        return;

    uint32_t allocated_word = ID_ALLOC_WORD(heap->size) + 1;

    while (--allocated_word >= 0) {
        if (heap->allocated[allocated_word] == 0) { // block is empty, erase
            if (allocated_word > 0) {
                heap->size -= 32;
                heap->data = realloc(heap->data, (heap->size + 1) * sizeof(vm_heap_object_t));
                heap->allocated = realloc(heap->allocated, (ID_ALLOC_WORD(heap->size) + 1) * sizeof(uint32_t));
            } else {
                heap->size = 1;
                heap->allocated = realloc(heap->allocated, sizeof(uint32_t));
                break;
            }
        }

        if(allocated_word == 0)
            break;
    }
}
