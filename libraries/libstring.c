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

#define STR_OBJ(thread, heap_ref)  vm_heap_load((*thread)->state->heap, heap_ref) /**< string object in heap */

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
            vm_heap_object_t *obj = STR_OBJ(thread, arg);
            obj->lib_obj.addr = strdup(STKSND(thread).cstr);
            obj->lib_obj.identifier = STRING_LIBRARY_IDENTIFIER;
            STKDROP2(thread);
        }
            break;
        case VM_EDFAT_PUSH:
            break;
        case VM_EDFAT_CMP: {
            vm_heap_object_t *obj1 = STR_OBJ(thread, STKTOP(thread).lib_obj.heap_ref);
            vm_heap_object_t *obj2 = STR_OBJ(thread, STKSND(thread).lib_obj.heap_ref);
            if (strcmp(obj1->lib_obj.addr, obj2->lib_obj.addr) != 0)
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
            if(STKSND(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj = vm_heap_load((*thread)->state->heap, STKTOP(thread).heap_ref);
            uint32_t pos = STKSND(thread).number.uinteger;
        }
        break;
        case LIBSTRING_FN_RIGHT: {
            if(STKSND(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj = vm_heap_load((*thread)->state->heap, STKTOP(thread).heap_ref);
            uint32_t pos = STKSND(thread).number.uinteger;
        }
        break;
        case LIBSTRING_FN_MID: {
            if(STKSND(thread).type != VM_VAL_UINT || STKTRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj = vm_heap_load((*thread)->state->heap, STKTOP(thread).heap_ref);
            uint32_t posl = STKSND(thread).number.uinteger;
            uint32_t posr = STKTRD(thread).number.uinteger;
        }
        break;
        case LIBSTRING_FN_CONCAT: {
            bool is_lib_obj = false;

            vm_heap_object_t* obj2 = vm_heap_load((*thread)->state->heap, STKSND(thread).heap_ref);
            if(STKSND(thread).type == VM_VAL_LIB_OBJ && obj2->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER) {
                is_lib_obj = true;
            } else if(STKSND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj1 = vm_heap_load((*thread)->state->heap, STKTOP(thread).heap_ref);
        }
        break;
        case LIBSTRING_FN_INSERT: {
            bool is_lib_obj = false;

            if (STKTRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj2 = vm_heap_load((*thread)->state->heap, STKSND(thread).heap_ref);
            if(STKSND(thread).type == VM_VAL_LIB_OBJ && obj2->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER) {
                is_lib_obj = true;
            } else if(STKSND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj1 = vm_heap_load((*thread)->state->heap, STKTOP(thread).heap_ref);
            uint32_t pos = STKTRD(thread).number.uinteger;
        }
        break;
        case LIBSTRING_FN_DELETE: {
            if (STKSND(thread).type != VM_VAL_UINT || STKTRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            uint32_t posl = STKSND(thread).number.uinteger;
            uint32_t posr = STKTRD(thread).number.uinteger;
        }
        break;
        case LIBSTRING_FN_REPLACE: {
            bool is_lib_obj = false;

            if (STKTRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj2 = vm_heap_load((*thread)->state->heap, STKSND(thread).heap_ref);
            if(STKSND(thread).type == VM_VAL_LIB_OBJ && obj2->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER) {
                is_lib_obj = true;
            } else if(STKSND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj1 = vm_heap_load((*thread)->state->heap, STKTOP(thread).heap_ref);
            uint32_t pos = STKTRD(thread).number.uinteger;
        }
        break;
        case LIBSTRING_FN_FIND: {
            bool is_lib_obj = false;

            if (STKTRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj2 = vm_heap_load((*thread)->state->heap, STKSND(thread).heap_ref);
            if(STKSND(thread).type == VM_VAL_LIB_OBJ && obj2->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER) {
                is_lib_obj = true;
            } else if(STKSND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            vm_heap_object_t* obj1 = vm_heap_load((*thread)->state->heap, STKTOP(thread).heap_ref);
            uint32_t pos = STKTRD(thread).number.uinteger;
        }
        break;
        default:
        res = VM_ERR_FAIL;
    }

    return res;
}
