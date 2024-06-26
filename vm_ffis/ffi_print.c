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
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

vm_value_t ffi_print(vm_thread_t **thread, uint8_t fn, uint32_t arg) {
    vm_value_t val = vm_pop(thread);
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
            if (!val.cstr.is_program)
                free(val.cstr.addr);
            break;

        default:
            printf("(error) non printable object [type: %u]\n", val.type);
    }

    val.type = VM_VAL_NULL;

    return val;
}
