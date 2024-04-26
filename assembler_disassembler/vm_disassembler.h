/*
 * @vm_disassembler.h
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

#ifndef VM_DISASSEMBLER_H_
#define VM_DISASSEMBLER_H_

#include "vm_assembler_utils.h"

enum VM_DISSASSEMBLER_ERROR {
    VM_DIS_OK   = 0x00,
    VM_DIS_FAIL = 0xff
};

uint8_t vm_disassembler(uint8_t *program, uint32_t program_len);

#endif /* VM_DISASSEMBLER_H_ */
