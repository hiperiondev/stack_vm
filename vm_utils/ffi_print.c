/*
 * @ffi_print.c
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
#include <stdio.h>

#include "vm.h"

vm_value_t ffi_print(vm_thread_t **thread, uint32_t fn) {
    vm_value_t val = vm_do_pop(thread);
    switch (val.type) {
        case VM_VAL_NULL:
            printf("(null)\n");
            break;
        case VM_VAL_BOOL:
            printf("%s\n", val.number.boolean ? "true" : "false");
            break;
        case VM_VAL_UINT:
            printf("%u\n", val.number.uinteger);
            break;
        case VM_VAL_INT:
            printf("%d\n", val.number.integer);
            break;
        case VM_VAL_FLOAT:
            printf("%f\n", val.number.real);
            break;
        case VM_VAL_CONST_STRING:
            printf("%s\n", val.cstr.addr);
            break;
        default:
            printf("__ error: non printable object (type=%u)\n", val.type);
    }

    return val;
}
