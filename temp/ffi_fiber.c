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

#include "vm.h"
#include "ffi_fiber.h"

typedef struct {
    uint32_t function; //
    uint32_t yield_pc; //
        bool in_use;   //
} ffi_fiber_t;

ffi_fiber_t scheduler_queue[FFI_MAX_FIBERS];
uint32_t scheduler_pos;

static vm_value_t ffi_fiber_yield(vm_thread_t **thread) {
    vm_value_t ret;
    ret.type = VM_VAL_BOOL;
    ret.number.boolean = false;

    uint32_t first_pos = scheduler_pos;
    do {
        if (scheduler_pos > FFI_MAX_FIBERS)
            scheduler_pos = 0;

        if(!scheduler_queue[scheduler_pos].in_use)
            continue;

        scheduler_queue[scheduler_pos].yield_pc = (*thread)->pc;

        // TODO: Implement

    } while (++scheduler_pos != first_pos);

    return ret;
}

vm_value_t ffi_fiber(vm_thread_t **thread, uint32_t fn, uint32_t arg) {
    vm_value_t ret = { VM_VAL_NULL };

    switch (fn) {
        case FFI_FIBER_NEW: {
            bool have_pos = false;
            for (uint32_t n = 0; n < FFI_MAX_FIBERS; n++) {
                if (!scheduler_queue[n].in_use) {
                    scheduler_queue[n].in_use = true;
                    scheduler_queue[n].function = arg;
                    have_pos = true;
                    break;
                }
            }
            ret.type = VM_VAL_BOOL;
            ret.number.boolean = have_pos;
        }
            break;

        case FFI_FIBER_DEQUEUE:
            scheduler_queue[scheduler_pos++].in_use = false;
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

void ffi_fiber_init(void) {
    scheduler_pos = 0;
    for (uint32_t n = 0; n < FFI_MAX_FIBERS; n++)
        scheduler_queue[n].in_use = false;
}
