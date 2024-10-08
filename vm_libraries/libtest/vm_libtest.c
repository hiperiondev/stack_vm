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

    switch (call_type) {
        // vm internal cases
        case VM_EDFAT_NEW: {
            NEW_HEAP_REF(obj, arg);
            obj->lib_obj.addr = (void*) malloc(sizeof(libtest_data_t));
            obj->lib_obj.identifier = TEST_LIBRARY_IDENTIFIER;
            ((libtest_data_t*) obj->lib_obj.addr)->data1 = 123;
            ((libtest_data_t*) obj->lib_obj.addr)->data2 = 1.23;
            printf("NEW OBJECT addr: %p\n", obj->lib_obj.addr);
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

            // library cases
        case LIBTEST_FN_TEST0: {
            vm_value_t obj = STK_TOP(thread);
            printf("[test0] TEST_LIBRARY_IDENTIFIER: %u (arg: %u)\n", TEST_LIBRARY_IDENTIFIER, arg);
            printf("    (value addr: %p)\n", vm_heap_load((*thread)->heap, obj.lib_obj.heap_ref)->lib_obj.addr);
            printf("    (value1: %u)\n", ((libtest_data_t*) (vm_heap_load((*thread)->heap, obj.lib_obj.heap_ref)->lib_obj.addr))->data1);
        }
            break;

        case LIBTEST_FN_TEST1: {
            vm_value_t obj = STK_TOP(thread);
            printf("[test1] IDX: %u, ARGUMENT: %u\n", lib_idx, arg);
            printf("    (value2: %f)\n", ((libtest_data_t*) (vm_heap_load((*thread)->heap, obj.lib_obj.heap_ref)->lib_obj.addr))->data2);

        }
            break;
    }

    return res;
}
