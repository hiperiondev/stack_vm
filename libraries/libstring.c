/*
 * @libstring.c
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
 * @version 1.0
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>

#include "vm.h"
#include "libstring.h"

vm_errors_t lib_entry_strings(vm_thread_t **thread, uint8_t call_type, void *args) {
    if (*thread == NULL)
        return VM_ERR_FAIL;

    vm_errors_t res = VM_ERR_OK;

    switch (call_type) {
        // vm cases
        case VM_EDFAT_NEW:
            break;
        case VM_EDFAT_PUSH:
            break;
        case VM_EDFAT_CMP:
            break;
        case VM_EDFAT_GC:
            break;

        // internal cases
        case LIBSTRING_FN_LEN: {// Return the length of a string.
            STKTOP(thread).type = VM_VAL_UINT;
            vm_heap_object_t* obj = vm_heap_load((*thread)->state->heap, STKTOP(thread).heap_ref);
            STKTOP(thread).number.uinteger = strlen((char*)obj->lib_obj.addr);
        }
            break;
        case LIBSTRING_FN_LEFT: {    // Return the left part of a string.

        }
            break;
        case LIBSTRING_FN_RIGHT: {   // Return the right part of a string.

        }
            break;
        case LIBSTRING_FN_MID: {     // Return the middle part of a string.

        }
            break;
        case LIBSTRING_FN_CONCAT: {  // Add together (concatenate) two or more strings.

        }
            break;
        case LIBSTRING_FN_INSERT: {  // Insert one string into another string.

        }
            break;
        case LIBSTRING_FN_DELETE: {  // Delete part of a string.

        }
            break;
        case LIBSTRING_FN_REPLACE: { // Replaces part of one string with another string.

        }
            break;
        case LIBSTRING_FN_FIND: {    // Finds the location of one string within another.

        }
            break;
        default:
            res = VM_ERR_FAIL;
    }

    return res;
}
