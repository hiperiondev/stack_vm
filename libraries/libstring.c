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
#include <stdio.h>

#include "vm.h"
#include "libstring.h"

#define STR_OBJ(thread, heap_ref)  vm_heap_load((*thread)->state->heap, heap_ref) /**< string object in heap */

/**
 * @def STR_NEW_OBJ
 * @brief create string object in heap
 *
 */
#define STR_NEW_OBJ(thread)                                                                                                                    \
            {                                                                                                                                  \
            vm_value_t _zzz_new_value;                                                                                                         \
            _zzz_new_value.type = VM_VAL_LIB_OBJ;                                                                                              \
            vm_heap_object_t _zzz_new_obj;                                                                                                     \
            _zzz_new_obj.type = VM_VAL_LIB_OBJ;                                                                                                \
            _zzz_new_obj.lib_obj.identifier = STRING_LIBRARY_IDENTIFIER;                                                                       \
            _zzz_new_value.lib_obj.heap_ref = vm_heap_save((*thread)->state->heap, _zzz_new_obj, &((*thread)->frames[(*thread)->fc].gc_mark)); \
            vm_do_push(thread, _zzz_new_value);                                                                                                \
            }

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

vm_errors_t lib_entry_strings(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t arg) {
    if (*thread == NULL)
        return VM_ERR_FAIL;

    vm_errors_t res = VM_ERR_OK;
    switch (call_type) {
        // vm cases
        case VM_EDFAT_NEW: {
            NEW_HEAP_REF(obj, arg);
            obj->lib_obj.addr = strdup(STKSND(thread).cstr);
            obj->lib_obj.identifier = STRING_LIBRARY_IDENTIFIER;
            obj->lib_obj.lib_idx = lib_idx;
            STKDROPSND(thread);
        }
            break;
        case VM_EDFAT_PUSH:
            break;
        case VM_EDFAT_CMP: {
            NEW_HEAP_REF(obj1, STKTOP(thread).lib_obj.heap_ref);
            NEW_HEAP_REF(obj2, STKSND(thread).lib_obj.heap_ref);
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
            NEW_HEAP_REF(obj, STKTOP(thread).lib_obj.heap_ref);
            STKTOP(thread).number.uinteger = strlen((char*)obj->lib_obj.addr);
        }
        break;
        case LIBSTRING_FN_LEFT:
        case LIBSTRING_FN_RIGHT: {
            if(STKSND(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj, STKTOP(thread).lib_obj.heap_ref);
            STR_NEW_OBJ(thread);
            uint32_t pos = STKSND(thread).number.uinteger;
            STKDROPST(thread);
            NEW_HEAP_REF(new_obj, STKTOP(thread).lib_obj.heap_ref);

            if(call_type == LIBSTRING_FN_RIGHT) {
                new_obj->lib_obj.addr = calloc(strlen(obj->lib_obj.addr) - pos + 1, sizeof(char));
                memcpy(new_obj->lib_obj.addr, obj->lib_obj.addr, strlen(obj->lib_obj.addr) - pos);
            } else {
                new_obj->lib_obj.addr = calloc(pos + 2, sizeof(char));
                memcpy(new_obj->lib_obj.addr, obj->lib_obj.addr, pos + 1);
            }
        }
        break;
        case LIBSTRING_FN_MID: {
            if(STKSND(thread).type != VM_VAL_UINT || STKTRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj, STKTOP(thread).lib_obj.heap_ref);
            uint32_t posl = STKSND(thread).number.uinteger;
            uint32_t posr = STKTRD(thread).number.uinteger;

        }
        break;
        case LIBSTRING_FN_CONCAT: {
            bool is_lib_obj = false;

            NEW_HEAP_REF(obj2, STKSND(thread).lib_obj.heap_ref);
            if(STKSND(thread).type == VM_VAL_LIB_OBJ && obj2->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER) {
                is_lib_obj = true;
            } else if(STKSND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj, STKTOP(thread).lib_obj.heap_ref);
            STR_NEW_OBJ(thread);
            STKDROPST(thread);
            NEW_HEAP_REF(new_obj, STKTOP(thread).lib_obj.heap_ref);

            if(is_lib_obj) {
                vm_heap_object_t *obj2 = vm_heap_load((*thread)->state->heap, STKSND(thread).heap_ref);
                new_obj->lib_obj.addr = calloc(strlen(obj->lib_obj.addr) + strlen(obj2->lib_obj.addr) + 1, sizeof(char));
                sprintf(new_obj->lib_obj.addr, "%s%s", (char*)obj->lib_obj.addr, (char*)obj2->lib_obj.addr);
            } else {
                new_obj->lib_obj.addr = calloc(strlen(obj->lib_obj.addr) + strlen(STKSND(thread).cstr) + 1, sizeof(char));
                sprintf(new_obj->lib_obj.addr, "%s%s", (char*)obj->lib_obj.addr, STKSND(thread).cstr);
            }
            STKDROPSND(tread);
        }
        break;
        case LIBSTRING_FN_DELETE: {
            if (STKSND(thread).type != VM_VAL_UINT || STKTRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            uint32_t posl = STKSND(thread).number.uinteger;
            uint32_t posr = STKTRD(thread).number.uinteger;
            STKDROPST(tread);
            NEW_HEAP_REF(obj, STKTOP(thread).lib_obj.heap_ref);
            STR_NEW_OBJ(thread);
            STKDROPST(thread);
            NEW_HEAP_REF(new_obj, STKTOP(thread).lib_obj.heap_ref);

            new_obj->lib_obj.addr = calloc(strlen(obj->lib_obj.addr) - (posr-posl) + 1, sizeof(char));
            memcpy(new_obj->lib_obj.addr, obj->lib_obj.addr, posl + 1);
            memcpy(new_obj->lib_obj.addr + posl + 1, obj->lib_obj.addr + posr + 1, strlen(obj->lib_obj.addr) - posr + 1);
        }
        break;
        case LIBSTRING_FN_INSERT:
        case LIBSTRING_FN_REPLACE: {
            bool is_lib_obj = false;

            if (STKTRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj2, STKSND(thread).lib_obj.heap_ref);
            if(STKSND(thread).type == VM_VAL_LIB_OBJ && obj2->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER) {
                is_lib_obj = true;
            } else if(STKSND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj, STKTOP(thread).lib_obj.heap_ref);
            uint32_t pos = STKTRD(thread).number.uinteger;

            //TODO: implement
        }
        break;
        case LIBSTRING_FN_FIND: {
            bool is_lib_obj = false;

            if (STKTRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj2, STKSND(thread).lib_obj.heap_ref);
            if(STKSND(thread).type == VM_VAL_LIB_OBJ && obj2->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER) {
                is_lib_obj = true;
            } else if(STKSND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj1, STKTOP(thread).lib_obj.heap_ref);
            uint32_t pos = STKTRD(thread).number.uinteger;

            //TODO: implement
        }
        break;
        default:
        res = VM_ERR_FAIL;
    }

    return res;
}
