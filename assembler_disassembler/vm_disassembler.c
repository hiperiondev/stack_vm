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
 * @version 1.0
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "vm_assembler_utils.h"
#include "vm_disassembler.h"
#include "vm_opcodes_def.h"

#define OP_INDIRECT(op)  (op & 0xc0)

uint8_t vm_disassembler(uint8_t *program, uint32_t program_len) {
    vm_thread_t *thread = calloc(1, sizeof(vm_thread_t));
    thread->state = calloc(1, sizeof(vm_state_t));
    thread->state->program = program;
    uint32_t pc = 0;
    uint8_t op = thread->state->program[pc];
    char prefix[3] = { '\0' };

    while (pc < program_len) {
        switch(OP_INDIRECT(op)) {
            case 0:
                sprintf(prefix, "  ");
                break;
            case 0x40:
                sprintf(prefix, " @");
                break;
            case 0x80:
                sprintf(prefix, "-@");
                break;
            case 0xc0:
                sprintf(prefix, "+@");
                break;
        }
        op &= 0x3f;

        printf("[%04d] %s%s ", pc, prefix, opcodes[op].opcode);
        ++pc;

        uint32_t pc_remainder = program_len - pc;

        for (uint8_t n = 0; n < 8; n++) {
            if (opcodes[op].arg_type[n] == ARG_NON)
                break;

            switch (opcodes[op].arg_type[n]) {
                case ARG_U08:
                    if (pc_remainder >= 1)
                        printf("%u ", vm_read_byte(&thread, &pc));
                    break;
                case ARG_U16:
                    if (pc_remainder >= 2)
                        printf("%u ", vm_read_u16(&thread, &pc));
                    break;
                case ARG_U32:
                    if (pc_remainder >= 4)
                        printf("%u ", vm_read_u32(&thread, &pc));
                    break;
                case ARG_I08:
                    if (pc_remainder >= 1)
                        printf("%d ", (int8_t) vm_read_byte(&thread, &pc));
                    break;
                case ARG_I16:
                    if (pc_remainder >= 1)
                        printf("%d ", vm_read_i16(&thread, &pc));
                    break;
                case ARG_I32:
                    if (pc_remainder >= 4)
                        printf("%d ", vm_read_i32(&thread, &pc));
                    break;
                case ARG_F32:
                    if (pc_remainder >= 4)
                        printf("%f ", vm_read_f32(&thread, &pc));
                    break;
                case ARG_STR:
                    printf("\"%s\" ", thread->state->program + pc);
                    break;
                case ARG_NVL:
                    // TODO: implement
                    break;
                default:
                    free(thread->state);
                    free(thread);
                    return VM_DIS_FAIL;
            }
        }

        printf("\n");

        if (pc > program_len - 1)
            break;

        op = program[pc];
    }

    free(thread->state);
    free(thread);
    return 0;
}
