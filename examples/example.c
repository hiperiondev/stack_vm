/*
 * @example.c
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
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "vm.h"
#include "vm_assembler.h"
#include "vm_assembler_utils.h"
#include "vm_disassembler.h"
#include "vm_libstring.h"
#include "ffi_print.h"

#define EP(x) [x] = #x
const char *vm_err[] = {
        EP(VM_ERR_OK),
        EP(VM_ERR_HALT),
        EP(VM_ERR_UNKNOWNOP),
        EP(VM_ERR_CONST_BADTYPE),
        EP(VM_ERR_PRG_END),
        EP(VM_ERR_OUTOFRANGE),
        EP(VM_ERR_BAD_VALUE),
        EP(VM_ERR_DIVBYZERO),
        EP(VM_ERR_TOOMANYTHREADS),
        EP(VM_ERR_INVALIDRETURN),
        EP(VM_ERR_FOREINGFNUNKN),
        EP(VM_ERR_OUTOFMEMORY),
        EP(VM_ERR_LOCALNOTEXIST),
        EP(VM_ERR_HEAPNOTEXIST),
        EP(VM_ERR_INDUNDERZERO),
        EP(VM_ERR_OVERFLOW),
        EP(VM_ERR_FAIL),
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("MISSING FILENAME!!!! \n");
        exit(1);
    }

    uint32_t progline = 0;
    uint32_t qty = 0;
    uint8_t res;
    uint32_t errline;
    uint8_t *hex = NULL;
    char *program = NULL;
    vm_thread_t *thread = NULL;
    label_macro_t **label = NULL;
    uint32_t label_qty = 0;

    printf("start file: %s\n\n", argv[1]);

    // create new thread
    vm_create_thread(&thread);

    // load FFI print (foreign function 0)
    thread->state->foreign_functions = malloc(sizeof(void*));
    thread->state->foreign_functions[0] = ffi_print;
    ++thread->state->foreign_functions_qty;

    // load LIBRARY string (library 0)
    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    // empty code space
    hex = malloc(sizeof(uint8_t));

    // load program
    program = vm_assembler_load_file(argv[1]);

    // assemble file
    printf("---------- start assembler\n\n");
    res = vm_assembler(&program, &qty, &hex, &errline, &progline, label, &label_qty);
    printf("\n---------- assembler result %s [(%u) %s]\n\n", res == ASSMBLR_OK ? "ok" : "fail", res, assembler_error[res]);

    //print hex of code
    print_hex(hex, qty, 0);

    // load code on VM
    thread->state->program = hex;
    thread->state->program_len = qty;
    thread->halted = false;

    // execute code
    printf("\n---------- execute code\n");
    while (thread->pc < thread->state->program_len && thread->halted == false)
        vm_step(&thread);

    // print internal end result
    printf("---------- execute result: %s [(%u) %s] (exit value: %u)\n", thread->status == VM_ERR_OK ? "ok" : "fail", thread->status, vm_err[thread->status],
            thread->exit_value);
    printf("-------------- pc: %u, sp: %u, fp: %u\n", thread->pc, thread->sp, thread->fp);

    // destroy VM thread
    vm_destroy_thread(&thread);

    // free all used
    free(program);
    free(hex);

    printf("\n");
    return 0;
}
