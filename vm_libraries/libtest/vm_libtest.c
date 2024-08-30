/*
 * @vm_libtest.c
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

#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "vm_libtest.h"

vm_errors_t lib_entry_test(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t arg) {
    if (*thread == NULL)
        return VM_ERR_FAIL;

    vm_errors_t res = VM_ERR_OK;
    static libtest_data_t *data_test;

    data_test = malloc(sizeof(libtest_data_t));

    switch (call_type) {
        // vm cases
        case VM_EDFAT_NEW: {
            NEW_HEAP_REF(obj, arg);
            obj->lib_obj.addr = data_test;
            obj->lib_obj.identifier = TEST_LIBRARY_IDENTIFIER;
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
            if (!vm_heap_load((*thread)->heap, arg)->static_obj) {

            }

        }
            break;

        case VM_EDFAT_TOTYPE: {

        }
            break;

        case LIBTEST_FN_TEST0: {
            printf("TEST_LIBRARY_IDENTIFIER: %u\n", TEST_LIBRARY_IDENTIFIER);
        }
            break;

        case LIBTEST_FN_TEST1: {
            printf("IDX: %u, ARGUMENT: %u\n", lib_idx, arg);
        }
            break;
    }

    return res;
}
