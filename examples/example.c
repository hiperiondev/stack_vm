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
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
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

#define EP(x) #x

typedef struct vm_err_str_s {
    char *tag;
    char *description;
} vm_err_str_t;

// VM errors
const vm_err_str_t vm_err[] = {
      //|         error            | description                                         |
        { EP(VM_ERR_OK)            , "ok"                                                },
        { EP(VM_ERR_HALT)          , "end with halt"                                     },
        { EP(VM_ERR_UNKNOWNOP)     , "op unknown"                                        },
        { EP(VM_ERR_CONST_BADTYPE) , "bad type in constant"                              },
        { EP(VM_ERR_PRG_END)       , "end program or pc beyond program length"           },
        { EP(VM_ERR_OUTOFRANGE)    , "value out of range"                                },
        { EP(VM_ERR_BAD_VALUE)     , "value type not allowed"                            },
        { EP(VM_ERR_DIVBYZERO)     , "division by zero"                                  },
        { EP(VM_ERR_TOOMANYTHREADS), "calls exceed VM_THREAD_MAX_CALL_DEPTH"             },
        { EP(VM_ERR_INVALIDRETURN) , "return not associated with call"                   },
        { EP(VM_ERR_FOREINGFNUNKN) , "foreign function unknown"                          },
        { EP(VM_ERR_OUTOFMEMORY)   , "can't allocate heap"                               },
        { EP(VM_ERR_LOCALNOTEXIST) , "local variable not exist or out of range"          },
        { EP(VM_ERR_HEAPNOTEXIST)  , "referenced heap not used or not exist"             },
        { EP(VM_ERR_INDUNDERZERO)  , "indirect register has been decremented under zero" },
        { EP(VM_ERR_OVERFLOW)      , "overflow"                                          },
        { EP(VM_ERR_FAIL)          , "generic fail"                                      },
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
    vm_program_t prg;
    vm_ffilib_t externals;
    uint32_t label_qty = 0;

    printf("start file: %s\n\n", argv[1]);

    // create new thread
    vm_create_thread(&thread);

    // load FFI print (foreign function 0)
    externals.foreign_functions = malloc(sizeof(void*));
    externals.foreign_functions[0] = ffi_print;
    ++externals.foreign_functions_qty;

    // load LIBRARY string (library 0)
    externals.lib = malloc(sizeof(void*));
    externals.lib[0] = lib_entry_strings;
    ++externals.lib_qty;

    thread->externals = &externals;

    // create empty code space
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
    prg.prog = hex;
    prg.prog_len = qty;
    thread->halted = false;

    // execute code
    printf("\n---------- execute code\n");
    while (thread->pc < prg.prog_len && thread->halted == false)
        vm_step(&thread, &prg);

    // print internal end result
    printf("---------- execute result: %s [(%u) %s: %s] (exit value: %u)\n", thread->status == VM_ERR_OK ? "ok" : "fail", thread->status,
            vm_err[thread->status].tag, vm_err[thread->status].description, thread->exit_value);
    printf("-------------- pc: %u, sp: %u, fp: %u\n", thread->pc, thread->sp, thread->fp);

    // destroy VM thread
    vm_destroy_thread(&thread);

    // free all used
    free(program);
    free(hex);

    printf("\n");
    return 0;
}
