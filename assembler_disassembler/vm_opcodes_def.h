/*
 * @vm_opcodes_def.h
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

#ifndef VM_OPCODES_DEF_H_
#define VM_OPCODES_DEF_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ARG_NON = 0x0, // non arguments
    ARG_U08,       // unsigned 8 bits
    ARG_U16,       // unsigned 16 bits
    ARG_U32,       // unsigned 32 bits
    ARG_I08,       // signed 8 bits
    ARG_I16,       // signed 16 bits
    ARG_I32,       // signed 32 bits
    ARG_F32,       // float 32 bits
    ARG_STR,       // string
    ARG_NVL,       // have n values from previous argument, next argument is type
    //[...]//
    ARG_UNK        // error: unknown type - ONLY FOR INTERNAL USE!
} arg_type_t;

typedef struct op_s {
    const char opcode[64];
    bool indirect;
    arg_type_t arg_type[8];
} op_t;

extern const op_t opcodes[];

#endif /* VM_OPCODES_DEF_H_ */
