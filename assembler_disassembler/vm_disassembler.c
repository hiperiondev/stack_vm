/*
 * @vm_disassembler.c
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
 * @version 0.1
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdint.h>
#include <stdio.h>

#include "vm.h"
#include "vm_assembler_utils.h"
#include "vm_disassembler.h"
#include "vm_opcodes_def.h"

uint8_t vm_disassembler(uint8_t *program, uint32_t program_len) {
    vm_state_thread_t *thread = NULL;
    vm_create_thread(&thread);
    thread->state->program = program;
    uint32_t pc = 0;
    uint8_t op = thread->state->program[pc];

    while (pc < program_len) {
        if(op > 0x3f) {
            vm_destroy_thread(&thread);
            return VM_DIS_FAIL;
        }

        printf("[%04d] %s ", pc, opcodes[op].opcode);
        ++pc;
        for (uint8_t n = 0; n < 8; n++) {
            if (opcodes[op].arg_type[n] == ARG_NON)
                break;

            switch (opcodes[op].arg_type[n]) {
                case ARG_U08:
                    printf("%u ", vm_api_read_byte(&thread, &pc));
                    break;
                case ARG_U16:
                    printf("%u ", vm_api_read_u16(&thread, &pc));
                    break;
                case ARG_U32:
                    printf("%u ", vm_api_read_u32(&thread, &pc));
                    break;
                case ARG_I08:
                    printf("%d ", (int8_t) vm_api_read_byte(&thread, &pc));
                    break;
                case ARG_I16:
                    printf("%d ", vm_api_read_i16(&thread, &pc));
                    break;
                case ARG_I32:
                    printf("%d ", vm_api_read_i32(&thread, &pc));
                    break;
                case ARG_F32:
                    printf("%f ", vm_api_read_f32(&thread, &pc));
                    break;
                case ARG_STR:

                    break;
                case ARG_NVL:

                    break;
                default:
                    vm_destroy_thread(&thread);
                    return VM_DIS_FAIL;
            }
        }

        printf("\n");

        if (pc > program_len - 1)
            break;

        op = program[pc];
    }

    vm_destroy_thread(&thread);
    return 0;
}
