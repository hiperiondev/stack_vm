/*
 * @vm_libhashmap.c
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
 * @version 2.0
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdlib.h>

#include "vm_libhashmap.h"
#include "vm.h"

vm_errors_t lib_entry_hashtable(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t arg) {
    if (*thread == NULL)
        return VM_ERR_FAIL;

    vm_errors_t res = VM_ERR_OK;
    switch (call_type) {
        // vm cases
        case VM_EDFAT_NEW: {
            NEW_HEAP_REF(obj, arg);
            obj->lib_obj.addr = NULL;
            obj->lib_obj.identifier = HASHTABLE_LIBRARY_IDENTIFIER;
            STKDROPSND(thread);
        }
            break;

        case VM_EDFAT_PUSH: {

        }
            break;

        case VM_EDFAT_CMP: {

        }
            break;

        case VM_EDFAT_GC: {
            free(vm_heap_load((*thread)->heap, arg)->lib_obj.addr);
        }
            break;

        case VM_EDFAT_TOTYPE: {

        }
            break;
    }

    return res;
}
