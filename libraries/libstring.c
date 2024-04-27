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

#define STR_OBJ(thread, stk)  vm_heap_load((*thread)->state->heap, (stk).heap_ref) /**< string object in heap */

/**
 * @fn vm_errors_t lib_entry_strings(vm_thread_t **thread, uint8_t call_type, void *args)
 * @brief Function entry for strings library
 * This functions support functions:
 *       LEN: Return the length of a string.
 *      LEFT: Return the left part of a string.
 *     RIGHT: Return the right part of a string.
 *       MID: Return the middle part of a string.
 *    CONCAT: Add together (concatenate) two or more strings.
 *    INSERT: Insert one string into another string.
 *    DELETE: Delete part of a string.
 *   REPLACE: Replaces part of one string with another string.
 *      FIND: Finds the location of one string within another.
 *
 * @param thread Thread
 * @param call_type Call type
 * @param args Arguments
 * @return
 */

vm_errors_t lib_entry_strings(vm_thread_t **thread, uint8_t call_type, uint32_t arg) {
    if (*thread == NULL)
        return VM_ERR_FAIL;

    vm_errors_t res = VM_ERR_OK;
    switch (call_type) {
        // vm cases
        case VM_EDFAT_NEW: {
            vm_heap_object_t *obj = STR_OBJ(thread, STKTOP(thread));
            obj->lib_obj.addr = strdup(STKSND(thread).cstr);
            STKDROP2(thread);
        }
            break;
        case VM_EDFAT_PUSH:
            break;
        case VM_EDFAT_CMP: {
            if (strcmp(STKTOP(thread).lib_obj.addr, STKSND(thread).lib_obj.addr) != 0)
            res = VM_ERR_FAIL;
        }
        break;
        case VM_EDFAT_GC: {
            free(vm_heap_load((*thread)->state->heap, arg)->lib_obj.addr);
        }
        break;

        // internal cases
        case LIBSTRING_FN_LEN: {
            STKTOP(thread).type = VM_VAL_UINT;
            vm_heap_object_t* obj = vm_heap_load((*thread)->state->heap, STKTOP(thread).heap_ref);
            STKTOP(thread).number.uinteger = strlen((char*)obj->lib_obj.addr);
        }
        break;
        case LIBSTRING_FN_LEFT: {

        }
        break;
        case LIBSTRING_FN_RIGHT: {

        }
        break;
        case LIBSTRING_FN_MID: {

        }
        break;
        case LIBSTRING_FN_CONCAT: {

        }
        break;
        case LIBSTRING_FN_INSERT: {

        }
        break;
        case LIBSTRING_FN_DELETE: {

        }
        break;
        case LIBSTRING_FN_REPLACE: {

        }
        break;
        case LIBSTRING_FN_FIND: {

        }
        break;
        default:
        res = VM_ERR_FAIL;
    }

    return res;
}
