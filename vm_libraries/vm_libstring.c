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
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
 * @version 2.0
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "vm.h"
#include "vm_libstring.h"

/**
 * @def STR_NEW_OBJ
 * @brief create string object in heap
 *
 */
#define STR_NEW_OBJ(thread, lidx)                                                                                                                  \
            {                                                                                                                                      \
                vm_value_t _zzz_new_value;                                                                                                         \
                _zzz_new_value.type = VM_VAL_LIB_OBJ;                                                                                              \
                _zzz_new_value.lib_obj.lib_idx = lidx;                                                                                             \
                vm_heap_object_t _zzz_new_obj;                                                                                                     \
                _zzz_new_obj.type = VM_VAL_LIB_OBJ;                                                                                                \
                _zzz_new_obj.lib_obj.identifier = STRING_LIBRARY_IDENTIFIER;                                                                       \
                _zzz_new_obj.lib_obj.lib_idx = lidx;                                                                                               \
                _zzz_new_obj.lib_obj.addr = NULL;                                                                                                  \
                _zzz_new_value.lib_obj.heap_ref = vm_heap_save((*thread)->heap, _zzz_new_obj, &((*thread)->frames[(*thread)->fc].gc_mark)); \
                vm_push(thread, _zzz_new_value);                                                                                                \
            }

///// utils /////
static uint32_t libstring_strpos(char *str, char *substr, uint32_t offset) {
    if (str == NULL || substr == NULL)
        return 0;

    if (str == NULL || substr == NULL)
        return 0xffffffff;
    char *found = NULL;

    if ((found = strstr(str + offset, substr)) == NULL)
        return 0xffffffff;

    return found - str;
}

static void libstring_strins(char **str_to, char *str_ins, size_t pos) {
    *str_to = realloc(*str_to, (strlen(*str_to) + strlen(str_ins) + 1) * sizeof(char));
    memcpy(*str_to + pos + strlen(str_ins), *str_to + pos, strlen(*str_to) - pos + 1);
    memcpy(*str_to + pos, str_ins, strlen(str_ins));
}

/////////////////
vm_errors_t lib_entry_strings(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t arg) {
    if (*thread == NULL)
        return VM_ERR_FAIL;

    vm_errors_t res = VM_ERR_OK;
    switch (call_type) {
        // vm cases
        case VM_EDFAT_NEW: {
            NEW_HEAP_REF(obj, arg);
            obj->lib_obj.addr = strdup(STK_SND(thread).cstr.addr);
            obj->lib_obj.identifier = STRING_LIBRARY_IDENTIFIER;
            STKDROPSND(thread);
        }
            break;

        case VM_EDFAT_PUSH:
            break;

        case VM_EDFAT_CMP: {
            NEW_HEAP_REF(obj1, STK_TOP(thread).lib_obj.heap_ref);
            NEW_HEAP_REF(obj2, STK_SND(thread).lib_obj.heap_ref);
            if (strcmp(obj1->lib_obj.addr, obj2->lib_obj.addr) != 0)
                res = VM_ERR_FAIL;
        }
            break;

        case VM_EDFAT_GC: {
            free(vm_heap_load((*thread)->heap, arg)->lib_obj.addr);
        }
            break;

        case VM_EDFAT_TOTYPE:
            break;

            // internal cases
        case LIBSTRING_FN_LEN: {
            NEW_HEAP_REF(obj, STK_TOP(thread).lib_obj.heap_ref);
            STK_TOP(thread).type = VM_VAL_UINT;
            STK_TOP(thread).number.uinteger = strlen((char*)obj->lib_obj.addr);
        }
        break;

        case LIBSTRING_FN_LEFT:
        case LIBSTRING_FN_RIGHT: {
            if(STK_SND(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            uint32_t size;
            uint32_t pos;

            NEW_HEAP_REF(obj, STK_TOP(thread).lib_obj.heap_ref);
            pos = STK_SND(thread).number.uinteger;
            STK_DROP2(thread);
            char *string = (char*)obj->lib_obj.addr;
            STR_NEW_OBJ(thread, lib_idx);
            NEW_HEAP_REF(new_obj, STK_TOP(thread).lib_obj.heap_ref);

            if(call_type == LIBSTRING_FN_RIGHT) {
                if(pos + 1 > strlen(string)) {
                    size = strlen(string);
                    pos = 0;
                } else {
                    size = strlen(string) - pos + 1;
                }
                new_obj->lib_obj.addr = calloc(size + 1, sizeof(char));
                memcpy(new_obj->lib_obj.addr, string + pos, size);
            } else {
                size = pos + 1 > strlen(string) ? strlen(string) : pos + 1;
                new_obj->lib_obj.addr = calloc(size + 1, sizeof(char));
                memcpy(new_obj->lib_obj.addr, string, size);
            }
        }
        break;

        case LIBSTRING_FN_MID: {
            if(STK_SND(thread).type != VM_VAL_UINT || STK_TRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj, STK_TOP(thread).lib_obj.heap_ref);
            uint32_t posr = STK_SND(thread).number.uinteger;
            uint32_t posl = STK_TRD(thread).number.uinteger;
            STK_DROP3(thread);
            char *string = (char*)obj->lib_obj.addr;

            if(posl > posr || posl > strlen(string) - 1 || posl > strlen(string) - 1) {
                posl = 0;
                posr = strlen(string) - 1;
            }

            STR_NEW_OBJ(thread, lib_idx);
            NEW_HEAP_REF(new_obj, STK_TOP(thread).lib_obj.heap_ref);

            uint32_t size = posr - posl + 1;
            new_obj->lib_obj.addr = calloc(size + 1, sizeof(char));
            memcpy(new_obj->lib_obj.addr, string + posl, size);
        }
        break;

        case LIBSTRING_FN_CONCAT: {
            if((STK_SND(thread).type != VM_VAL_LIB_OBJ
                    && (vm_heap_load((*thread)->heap, STK_SND(thread).lib_obj.heap_ref))->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER)
                    || STK_SND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            char *string2 = NULL;
            NEW_HEAP_REF(obj, STK_TOP(thread).lib_obj.heap_ref);
            if(STK_SND(thread).type == VM_VAL_LIB_OBJ) {
                NEW_HEAP_REF(obj2, STK_SND(thread).lib_obj.heap_ref);
                string2 = (char*)obj2->lib_obj.addr;
            } else {
                string2 = STK_SND(thread).cstr.addr;
            }

            char *string1 = (char*)obj->lib_obj.addr;
            STR_NEW_OBJ(thread, lib_idx);
            NEW_HEAP_REF(new_obj, STK_TOP(thread).lib_obj.heap_ref);

            uint32_t size = strlen(string1) + strlen(string2);
            new_obj->lib_obj.addr = calloc(size + 1, sizeof(char));
            sprintf(new_obj->lib_obj.addr, "%s%s", string1, string2);
            STKDROPST(thread);
        }
        break;

        case LIBSTRING_FN_DELETE: {
            if (STK_SND(thread).type != VM_VAL_UINT || STK_TRD(thread).type != VM_VAL_UINT) {
                return VM_ERR_BAD_VALUE;
            }

            uint32_t posr = STK_SND(thread).number.uinteger;
            uint32_t posl = STK_TRD(thread).number.uinteger;
            NEW_HEAP_REF(obj, STK_TOP(thread).lib_obj.heap_ref);
            char *string = obj->lib_obj.addr;
            STK_DROP3(thread);
            if(posl > posr || posl > strlen(string) - 1 || posl > strlen(string) - 1) {
                posl = 0;
                posr = 0;
            }

            STR_NEW_OBJ(thread, lib_idx);
            NEW_HEAP_REF(new_obj, STK_TOP(thread).lib_obj.heap_ref);

            uint32_t size = strlen(string) - (posr - posl) + 1;
            new_obj->lib_obj.addr = calloc(size + 1, sizeof(char));
            memcpy(new_obj->lib_obj.addr, string, posl + 1);
            memcpy(new_obj->lib_obj.addr + posl + 1, string + posr, strlen(string) - posr);
        }
        break;

        case LIBSTRING_FN_INSERT:
        case LIBSTRING_FN_REPLACE: {
            if(STK_TRD(thread).type != VM_VAL_UINT
                    || (STK_SND(thread).type != VM_VAL_LIB_OBJ && (HEAP_OBJ(STK_SND(thread).lib_obj.heap_ref))->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER)
                    || STK_SND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj, STK_TOP(thread).lib_obj.heap_ref);
            char *string1 = (char*)obj->lib_obj.addr;
            char *string2 = NULL;
            if(STK_SND(thread).type == VM_VAL_LIB_OBJ) {
                NEW_HEAP_REF(obj2, STK_SND(thread).lib_obj.heap_ref);
                string2 = (char*)obj2->lib_obj.addr;
            } else {
                string2 = STK_SND(thread).cstr.addr;
            }

            uint32_t pos = STK_TRD(thread).number.uinteger;
            STR_NEW_OBJ(thread, lib_idx);
            NEW_HEAP_REF(new_obj, STK_TOP(thread).lib_obj.heap_ref);
            new_obj->lib_obj.addr = strdup(string1);

            if(call_type == LIBSTRING_FN_REPLACE) {
            uint32_t len = strlen(string2) > strlen(string1) - pos ? strlen(string1) - pos : strlen(string2);
            memcpy(new_obj->lib_obj.addr + pos, string2, len);
            } else {
                libstring_strins((char**)&(new_obj->lib_obj.addr), string2, pos);
            }
            STKDROPSTF(thread);
        }
        break;

        case LIBSTRING_FN_FIND: {
            if(STK_TRD(thread).type != VM_VAL_UINT
                    || (STK_SND(thread).type != VM_VAL_LIB_OBJ && (HEAP_OBJ(STK_SND(thread).lib_obj.heap_ref))->lib_obj.identifier == STRING_LIBRARY_IDENTIFIER)
                    || STK_SND(thread).type != VM_VAL_CONST_STRING) {
                return VM_ERR_BAD_VALUE;
            }

            NEW_HEAP_REF(obj, STK_TOP(thread).lib_obj.heap_ref);
            char *string1 = (char*)obj->lib_obj.addr;
            char *string2 = NULL;
            if(STK_SND(thread).type == VM_VAL_LIB_OBJ) {
                NEW_HEAP_REF(obj2, STK_SND(thread).lib_obj.heap_ref);
                string2 = (char*)obj2->lib_obj.addr;
            } else {
                string2 = STK_SND(thread).cstr.addr;
            }

            vm_value_t find_pos = {VM_VAL_UINT};
            find_pos.number.uinteger = libstring_strpos(string1, string2, STK_TRD(thread).number.uinteger);
            STK_DROP3(thread);
            vm_push(thread, find_pos);
        }
        break;

        case LIBSTRING_FN_TO_CSTR: {
            NEW_HEAP_REF(obj, STK_TOP(thread).lib_obj.heap_ref);
            STK_TOP(thread).type = VM_VAL_CONST_STRING;
            STK_TOP(thread).cstr.addr = strdup(obj->lib_obj.addr);
            STK_TOP(thread).cstr.is_program = false;
        }
        break;
        default:
        res = VM_ERR_FAIL;
    }

    return res;
}
