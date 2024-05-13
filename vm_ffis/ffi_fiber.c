/*
 * @ffi_fiber.c
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
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdint.h>
#include <stdlib.h>

#include "vm.h"
#include "ffi_fiber.h"

typedef struct {
        bool in_use;      //
    uint32_t function;    //
  vm_value_t *stack;      //
  vm_frame_t fiber_state; //
} ffi_fiber_t;

ffi_fiber_t scheduler_queue[FFI_MAX_FIBERS];
uint32_t scheduler_pos;

static vm_value_t ffi_fiber_yield(vm_thread_t **thread) {
    vm_value_t ret;
    ret.type = VM_VAL_BOOL;
    ret.number.boolean = false;

    uint32_t actual_pos = scheduler_pos;
    do {
        if (scheduler_pos > FFI_MAX_FIBERS)
            scheduler_pos = 0;

        if(!scheduler_queue[scheduler_pos].in_use)
            continue;

        // TODO: Implement
        break;

    } while (++scheduler_pos != actual_pos);

    return ret;
}

vm_value_t ffi_fiber(vm_thread_t **thread, uint32_t fn, uint32_t arg) {
    vm_value_t ret = { VM_VAL_NULL };

    switch (fn) {
        case FFI_FIBER_ENQUEUE: {
            bool have_pos = false;
            for (uint32_t n = 1; n < FFI_MAX_FIBERS; n++) {
                if (!scheduler_queue[n].in_use) {
                    scheduler_queue[n].in_use = true;
                    scheduler_queue[n].function = arg;
                    scheduler_queue[n].stack = calloc(VM_THREAD_STACK_SIZE, sizeof(vm_value_t));
                    scheduler_queue[n].fiber_state.gc_mark = vm_heap_new_gc_mark((*thread)->heap);
                    have_pos = true;
                    break;
                }
            }
            ret.type = VM_VAL_BOOL;
            ret.number.boolean = have_pos;
        }
            break;

        case FFI_FIBER_DEQUEUE: {
            vm_heap_gc_collect((*thread)->heap, &(scheduler_queue[scheduler_pos].fiber_state.gc_mark), true, thread);
            for (uint32_t n = 0; n < VM_THREAD_STACK_SIZE; ++n)
                if (scheduler_queue[scheduler_pos].stack[n].type == VM_VAL_CONST_STRING && scheduler_queue[scheduler_pos].stack[n].cstr.is_program == false)
                    free(scheduler_queue[scheduler_pos].stack[n].cstr.addr);
            free(scheduler_queue[scheduler_pos].stack);
            scheduler_queue[scheduler_pos++].in_use = false;
        }
            break;

        case FFI_FIBER_CURRENT:
            ret.type = VM_VAL_UINT;
            ret.number.uinteger = scheduler_pos;
            break;

        case FFI_FIBER_RESUME:
            ret = vm_pop(thread);
            if (ret.type == VM_VAL_UINT && ret.number.uinteger < FFI_MAX_FIBERS) {
                scheduler_pos = ret.number.uinteger;
                ret = ffi_fiber_yield(thread);
            } else {
                ret.type = VM_VAL_BOOL;
                ret.number.boolean = false;
            }
            break;

        case FFI_FIBER_YIELD:
            ret = ffi_fiber_yield(thread);
            break;
    }
    return ret;
}

void ffi_fiber_init(vm_thread_t **thread) {
    scheduler_pos = 1;
    for (uint32_t n = 1; n < FFI_MAX_FIBERS; n++)
        scheduler_queue[n].in_use = false;

    scheduler_queue[0].in_use = true;
    scheduler_queue[0].stack = (*thread)->stack;
    scheduler_queue[0].fiber_state.fp = (*thread)->fp;
    scheduler_queue[0].fiber_state.gc_mark = (*thread)->frames[0].gc_mark;
}
