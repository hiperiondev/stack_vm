/*
 * @vm.h
 *
 * @brief virtual machine
 * @details
 * This is based on other projects:
 *   Others (see individual files)
 *
 *   please contact their authors for more information.
 *
 * @author Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * @version 0.1
 * @date 2023
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#ifndef VM_H_
#define VM_H_

/*
 * OPCODES
 *
 * opcode: ooooommm, o: opcode, m: modifier
 * NEXT_BYTE_DATA: xXbbbaaa: aaa=a data type, bbb=b data type, x, X: 1=a/b from prog, 0= a/b from stack
 * NEXT_BYTE_LEN: uint8, if 0: from temp
 *
 * | MNEMONIC           | OP   | MODIFIER                | NOTES                          | FUNCTION
 * | VM_NOP             | 0x00 | NONE                    |                                | do nothing
 * | VM_POP             | 0x01 | NONE                    | to temp                        |
 * | VM_PUSH            | 0x02 | MODIFIER_DATA_TYPE      | if VM_MOD_REFERENCE: from temp |
 * | VM_STACK           | 0x03 | MODIFIER_STACK          |                                |
 * | VM_GOTO            | 0x04 | MODIFIER_LEN            | if 0: indirect from temp       |
 * | VM_IF              | 0x05 | MODIFIER_IF             | NEXT_BYTE_DATA                 |
 * | VM_CALL            | 0x06 | MODIFIER_CALL           |                                |
 * | VM_RETURN          | 0x07 | MODIFIER_DATA_TYPE      |                                |
 * | VM_LOAD            | 0x08 | MODIFIER_LEN            |                                |
 * | VM_STORE           | 0x09 | MODIFIER_LEN            |                                |
 * | VM_CONST           | 0x0a | MODIFIER_DATA_TYPE      |                                |
 * | VM_NEWARRAY        | 0x0b | MODIFIER_LEN            | dimension - if 0 NEXT_BYTE_LEN |
 * | VM_ALOAD           | 0x0c | MODIFIER_LEN            | elements - if 0 NEXT_BYTE_LEN  |
 * | VM_ASTORE          | 0x0d | MODIFIER_LEN            | elements - if 0 NEXT_BYTE_LEN  |
 * | VM_ALU_ARITHMETIC  | 0x0e | MODIFIER_ALU_ARITHMETIC | NEXT_BYTE_DATA                 |
 * | VM_ALU_BITWISE     | 0x0f | MODIFIER_ALU_BITWISE    | NEXT_BYTE_DATA                 |
 * | VM_CONVERT         | 0x10 | MODIFIER_DATA_TYPE      | NEXT_BYTE_DATA                 |
 * | VM_COMPARE         | 0x11 | MODIFIER_DATA_TYPE      | NEXT_BYTE_DATA                 |
 * | VM_OP_12           | 0x12 | not implemented         |                                |
 * | VM_OP_13           | 0x13 | not implemented         |                                |
 * | VM_OP_14           | 0x14 | not implemented         |                                |
 * | VM_OP_15           | 0x15 | not implemented         |                                |
 * | VM_OP_16           | 0x16 | not implemented         |                                |
 * | VM_OP_17           | 0x17 | not implemented         |                                |
 * | VM_OP_18           | 0x18 | not implemented         |                                |
 * | VM_OP_19           | 0x19 | not implemented         |                                |
 * | VM_OP_1A           | 0x1A | not implemented         |                                |
 * | VM_OP_1B           | 0x1B | not implemented         |                                |
 * | VM_OP_1C           | 0x1C | not implemented         |                                |
 * | VM_OP_1D           | 0x1D | not implemented         |                                |
 * | VM_OP_1E           | 0x1E | not implemented         |                                |
 * | VM_OP_1F           | 0x1F | not implemented         |                                |
 */

/////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define VM_MEMORY_SIZE      (640 << 10)
#define VM_INITIAL_OBJ_REF  42
#define VM_INITIAL_PC       1

/////////////////////////////////////////////////////////////////////////////////

#define TYPE_POS(x)  (x % 3)
#define TYPE_MASK(x) (0x07 << TYPE_POS(x))
#define TYPE_WORD(x) ((x) / 3)
#define TYPE_TYPE(x) ((TYPE_WORD(x) && TYPE_MASK) >> TYPE_POS(x))

#define VALUE_GET(type, data, vm_i)                                                   \
            (type == VM_MOD_UINT8)     ? ((uint8_t)(data))                          : \
            (type == VM_MOD_INT8)      ? ((int8_t)(data))                           : \
            (type == VM_MOD_UINT16)    ? ((uint16_t)(data))                         : \
            (type == VM_MOD_INT16)     ? ((int16_t)(data))                          : \
            (type == VM_MOD_FLOAT)     ? ((float)(data))                            : \
            (type == VM_MOD_UINT32)    ? ((uint32_t)(data))                         : \
            (type == VM_MOD_INT32)     ? ((int32_t)(data))                          : \
            (type == VM_MOD_REFERENCE) ? (vm_i->stack[vm_i->lv + (uint32_t)(data)]) : \
            0

#define VALUE_GET_INT(type, data, vm_i)                                               \
            (type == VM_MOD_UINT8)     ? ((uint8_t)(data))                          : \
            (type == VM_MOD_INT8)      ? ((int8_t)(data))                           : \
            (type == VM_MOD_UINT16)    ? ((uint16_t)(data))                         : \
            (type == VM_MOD_INT16)     ? ((int16_t)(data))                          : \
            (type == VM_MOD_UINT32)    ? ((uint32_t)(data))                         : \
            (type == VM_MOD_INT32)     ? ((int32_t)(data))                          : \
            (type == VM_MOD_REFERENCE) ? (vm_i->stack[vm_i->lv + (uint32_t)(data)]) : \
            0

#define VALUE_FETCH(type, vm_i)                                    \
            (type == VM_MOD_UINT8)     ? (vm_fetch_uint8(vm_i))  : \
            (type == VM_MOD_INT8)      ? (vm_fetch_int8(vm_i))   : \
            (type == VM_MOD_UINT16)    ? (vm_fetch_uint16(vm_i)) : \
            (type == VM_MOD_INT16)     ? (vm_fetch_uint16(vm_i)) : \
            (type == VM_MOD_FLOAT)     ? (vm_fetch_float(vm_i))  : \
            (type == VM_MOD_UINT32)    ? (vm_fetch_uint32(vm_i)) : \
            (type == VM_MOD_INT32)     ? (vm_fetch_uint32(vm_i)) : \
            (type == VM_MOD_REFERENCE) ? (vm_fetch_uint32(vm_i)) : \
            0

#define OP(x)        ((x && 0xf8) >> 3)
#define MODIFIER(x)  (x && 0x07)
#define A_TYPE(x)    (x && 0x07)
#define B_TYPE(x)    ((x && 0x38) >> 3)
#define A_FROM(x)    (x && 0x80)
#define B_FROM(x)    (x && 0x40)

typedef enum VM_ERRORS {
    VM_ERR_OK     = 0x00, // ok
    VM_ERR_DIVBYZ = 0x01, // division by zero
    VM_ERR_BADLEN = 0x02, // bad operator length
    VM_ERR_NOINT  = 0x03, // operation only allow integer
    VM_ERR_NOFFI  = 0x04, // FFI function not exist
    VM_ERR_NOOP   = 0x05, // unknown opcode
    VM_ERR_NOCALL = 0x06, // unknown call type
    //...//
    VM_ERR_FAIL   = 0xff  // generic fail

} vm_error_t;

enum MODIFIER_DATA_TYPE {
    VM_MOD_UINT8,     // 0x00 *
    VM_MOD_INT8,      // 0x01 *
    VM_MOD_UINT16,    // 0x02 *
    VM_MOD_INT16,     // 0x03 *
    VM_MOD_FLOAT,     // 0x04 *
    VM_MOD_UINT32,    // 0x05 *
    VM_MOD_INT32,     // 0x06 *
    VM_MOD_REFERENCE, // 0x07 *
};

enum MODIFIER_ALU_ARITHMETIC {
    VM_MOD_ADD, // 0x00 *
    VM_MOD_SUB, // 0x01 *
    VM_MOD_MUL, // 0x02 *
    VM_MOD_DIV, // 0x03 *
    VM_MOD_MOD, // 0x04 *
    VM_MOD_NEG, // 0x05 *
    VM_MOD_INC, // 0x06 *
    VM_MOD_DEC, // 0x07 *
};

enum MODIFIER_ALU_BITWISE {
    VM_MOD_AND, // 0x00 *
    VM_MOD_OR,  // 0x01 *
    VM_MOD_XOR, // 0x02 *
    VM_MOD_SHL, // 0x03 *
    VM_MOD_SHR, // 0x04 *
    VM_MOD_ROL, // 0x05 *
    VM_MOD_ROR, // 0x06 *
    VM_MOD_NOT, // 0x07 *
};

enum MODIFIER_IF {
    VM_MOD_EQZ,  // 0x00 * A == 0
    VM_MOD_GTZ,  // 0x01 * A > 0
    VM_MOD_LEZ,  // 0x02 * A < 0
    VM_MOD_EQ,   // 0x03 * A == B
    VM_MOD_GT,   // 0x04 * A > B
    VM_MOD_GEQ,  // 0x05 * A >= B
    VM_MOD_LEQ,  // 0x06 * A <= B
    VM_MOD_NEQ   // 0x07 * A != B
};

enum MODIFIER_LEN {
    VM_MOD_LEN0, // 0x00 *
    VM_MOD_LEN1, // 0x01 *
    VM_MOD_LEN2, // 0x02 *
    VM_MOD_LEN3, // 0x03 *
    VM_MOD_LEN4, // 0x04 *
    VM_MOD_LEN5, // 0x05 *
    VM_MOD_LEN6, // 0x06 *
    VM_MOD_LEN7, // 0x07 *
};

enum MODIFIER_CALL {
    VM_MOD_PROG,  // 0x00 *
    VM_MOD_STACK, // 0x01 *
    VM_MOD_TEMP,  // 0x02 *
    VM_MOD_AXXX,  // 0x03 *
    VM_MOD_BXXX,  // 0x04 *
    VM_MOD_CXXX,  // 0x05 *
    VM_MOD_DXXX,  // 0x06 *
    VM_MOD_EXXX,  // 0x07 *
};

enum MODIFIER_STACK {
    VM_MOD_DUP,  // 0x00 * ( a -- a a )
    VM_MOD_SWAP, // 0x01 * ( a b -- b a )
    VM_MOD_OVER, // 0x02 * ( a b -- a b a )
    VM_MOD_ROT,  // 0x03 * ( a b c -- b c a )
    VM_MOD_DROP, // 0x04 * ( a -- )
    VM_MOD_NIP,  // 0x05 * ( a b -- b )
    VM_MOD_LIT0, // 0x06 * ( -- 0 )
    VM_MOD_LI1,  // 0x07 * ( -- 1 )
};

enum OPCODES {
    VM_NOP,            // 0x00
    VM_POP,            // 0x01
    VM_PUSH,           // 0x02
    VM_STACK,          // 0x03
    VM_GOTO,           // 0x04
    VM_IF,             // 0x05
    VM_CALL,           // 0x06
    VM_RETURN,         // 0x07
    VM_LOAD,           // 0x08
    VM_STORE,          // 0x09
    VM_CONST,          // 0x0a
    VM_NEWARRAY,       // 0x0b
    VM_ALOAD,          // 0x0c
    VM_ASTORE,         // 0x0d
    VM_ALU_ARITHMETIC, // 0x0e
    VM_ALU_BITWISE,    // 0x0f
    VM_CONVERT,        // 0x10
    VM_COMPARE,        // 0x11
    VM_OP_12,          // 0x12
    VM_OP_13,          // 0x13
    VM_OP_14,          // 0x14
    VM_OP_15,          // 0x15
    VM_OP_16,          // 0x16
    VM_OP_17,          // 0x17
    VM_OP_18,          // 0x18
    VM_OP_19,          // 0x19
    VM_OP_1A,          // 0x1A
    VM_OP_1B,          // 0x1B
    VM_OP_1C,          // 0x1C
    VM_OP_1D,          // 0x1D
    VM_OP_1E,          // 0x1E
    VM_OP_1F,          // 0x1F
};

struct vm_s;
typedef vm_error_t (*func_builtin)(struct vm_s *vm);

typedef struct vm_s {
    uint32_t sp;         //
    uint32_t lv;         //
    uint32_t pc;         //
    uint32_t temp;       //
    uint32_t *stack;     //
    uint32_t *cpp;       //
    uint8_t *method;    //
    uint32_t initial_sp; //
    func_builtin *ffi;       //
    uint32_t ffi_qty;    //
} vm_t;

typedef struct vm_image_s {
    uint16_t main_index;       //
    uint8_t *method_area;     //
    uint32_t method_area_size; //
    uint32_t *cpool;           //
    uint32_t cpool_size;       //
} vm_image_t;

/////////////////////////////////////////////////////////////////////////////////

inline int8_t vm_fetch_int8(vm_t *i) {
    int8_t byte = 0;

    byte = i->method[i->pc];
    i->pc = i->pc + 1;

    return byte;
}

inline uint8_t vm_fetch_uint8(vm_t *i) {
    uint8_t byte = 0;

    byte = i->method[i->pc];
    i->pc = i->pc + 1;

    return byte;
}

inline int16_t vm_fetch_int16(vm_t *i) {
    int16_t word = 0;

    word |= i->method[i->pc++] << 8;
    word |= i->method[i->pc++];

    return word;
}

inline uint16_t vm_fetch_uint16(vm_t *i) {
    uint16_t word = 0;

    word |= i->method[i->pc++] << 8;
    word |= i->method[i->pc++];

    return word;
}

inline int32_t vm_fetch_int32(vm_t *i) {
    int32_t word = 0;

    word |= i->method[i->pc++] << 24;
    word |= i->method[i->pc++] << 16;
    word |= i->method[i->pc++] << 8;
    word |= i->method[i->pc++];

    return word;
}

inline uint32_t vm_fetch_uint32(vm_t *i) {
    uint32_t word = 0;

    word |= i->method[i->pc++] << 24;
    word |= i->method[i->pc++] << 16;
    word |= i->method[i->pc++] << 8;
    word |= i->method[i->pc++];

    return word;
}

inline float vm_fetch_float(vm_t *i) {
    uint32_t word = 0;

    word |= i->method[i->pc++] << 24;
    word |= i->method[i->pc++] << 16;
    word |= i->method[i->pc++] << 8;
    word |= i->method[i->pc++];

    return (float) word;
}

inline void vm_push(vm_t *i, uint32_t word) {
    i->stack[++i->sp] = word;
}

inline uint32_t vm_pop(vm_t *i) {
    return i->stack[i->sp--];
}

inline vm_error_t vm_call(vm_t *i, uint32_t index) {
    if (index >= 0x8000) {
        if (index - 0x8000 > i->ffi_qty - 1)
            return VM_ERR_NOFFI;

        return i->ffi[index - 0x8000](i);
    }

    uint32_t address;
    uint16_t nargs, nlocals;

    address = i->cpp[index];
    nargs = i->method[address] * 256 + i->method[address + 1];
    nlocals = i->method[address + 2] * 256 + i->method[address + 3];

    i->sp += nlocals;
    vm_push(i, i->pc);
    vm_push(i, i->lv);
    i->lv = i->sp - nargs - nlocals - 1;
    i->stack[i->lv] = i->sp - 1;
    i->pc = address + 4;

    return VM_ERR_OK;
}

vm_error_t vm_step(vm_t *i) {
    uint8_t opcode, modifier, nextbyte, a_type, b_type;
    uint32_t index, varnum, opc, result;
    int16_t offset;
    uint32_t a, b;

    opc = i->pc;
    opcode = vm_fetch_uint8(i);
    modifier = MODIFIER(opcode);
    opcode = OP(opcode);

    switch (opcode) {
        case VM_NOP:
            break;

        case VM_POP:
            i->temp = vm_pop(i);
            break;

        case VM_PUSH:
            if (modifier == VM_MOD_REFERENCE)
                vm_push(i, i->temp);
            else
                vm_push(i, VALUE_FETCH(modifier, i));
            break;

        case VM_STACK:
            switch (modifier) {
                case VM_MOD_DUP:
                    vm_push(i, i->stack[i->sp]);
                    break;

                case VM_MOD_SWAP:
                    result = i->stack[i->sp];
                    i->stack[i->sp] = i->stack[i->sp - 1];
                    i->stack[i->sp - 1] = result;
                    break;

                case VM_MOD_OVER:
                    vm_push(i, i->stack[i->sp - 1]);
                    break;

                case VM_MOD_ROT:
                    result = i->stack[i->sp - 2];
                    i->stack[i->sp - 2] = i->stack[i->sp - 1];
                    i->stack[i->sp - 1] = i->stack[i->sp - 0];
                    i->stack[i->sp] = result;
                    break;

                case VM_MOD_DROP:
                    vm_pop(i);
                    break;

                case VM_MOD_NIP:
                    result = i->stack[i->sp];
                    vm_pop(i);
                    i->stack[i->sp] = result;
                    break;

                case VM_MOD_LIT0:
                    vm_push(i, 0);
                    break;

                case VM_MOD_LI1:
                    vm_push(i, 1);
                    break;
            }
            break;

        case VM_GOTO:
            if (modifier == 0)
                offset = i->temp;
            else
                offset = vm_fetch_int32(i);

            i->pc = opc + offset;
            break;

        case VM_IF:
            result = 0;
            offset = vm_fetch_int32(i);
            nextbyte = vm_fetch_uint8(i);
            a_type = A_TYPE(nextbyte);
            b_type = B_TYPE(nextbyte);

            if (A_FROM(nextbyte))
                a = VALUE_FETCH(a_type, i);
            else
                a = VALUE_GET(a_type, vm_pop(i), i);

            switch (modifier) {
                case VM_MOD_EQZ:
                    if (a == 0)
                        result = 1;
                    break;
                case VM_MOD_EQ:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);
                    if (a == b)
                        result = 1;
                    break;
                case VM_MOD_GT:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);
                    if (a > b)
                        result = 1;
                    break;
                case VM_MOD_GEQ:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);
                    if (a >= b)
                        result = 1;
                    break;
                case VM_MOD_LEQ:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);
                    if (a <= b)
                        result = 1;
                    break;
                case VM_MOD_GTZ:
                    if (a > 0)
                        result = 1;
                    break;
                case VM_MOD_LEZ:
                    if (a < 0)
                        result = 1;
                    break;
                case VM_MOD_NEQ:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);
                    if (a != b)
                        result = 1;
                    break;
            }
            if (result)
                i->pc = opc + offset;
            break;

        case VM_CALL:
            switch (modifier) {
                case VM_MOD_PROG:
                    index = vm_fetch_uint32(i);
                    break;
                case VM_MOD_STACK:
                    index = vm_pop(i);
                    break;
                case VM_MOD_TEMP:
                    index = i->temp;
                    break;
                default:
                    return VM_ERR_NOCALL;
            }

            vm_call(i, index);
            break;

        case VM_RETURN:
            int linkptr = i->stack[i->lv];
            i->stack[i->lv] = i->stack[i->sp];
            i->sp = i->lv;
            i->pc = i->stack[linkptr];
            i->lv = i->stack[linkptr + 1];
            break;

        case VM_LOAD:
            switch (modifier) {
                case VM_MOD_LEN1:
                    varnum = vm_fetch_uint8(i);
                    break;
                case VM_MOD_LEN2:
                    varnum = vm_fetch_uint16(i);
                    break;
                case VM_MOD_LEN4:
                    varnum = vm_fetch_uint32(i);
                    break;
                default:
                    return VM_ERR_BADLEN;
            }

            vm_push(i, i->stack[i->lv + varnum]);
            break;

        case VM_STORE:
            switch (modifier) {
                case VM_MOD_LEN1:
                    varnum = vm_fetch_uint8(i);
                    break;
                case VM_MOD_LEN2:
                    varnum = vm_fetch_uint16(i);
                    break;
                case VM_MOD_LEN4:
                    varnum = vm_fetch_uint32(i);
                    break;
                default:
                    return VM_ERR_BADLEN;
            }

            i->stack[i->lv + varnum] = vm_pop(i);
            break;

        case VM_CONST:
            index = vm_fetch_uint16(i);
            vm_push(i, i->cpp[index]);
            break;

        case VM_NEWARRAY:
            // TODO
            break;

        case VM_ALOAD:
            // TODO
            break;

        case VM_ASTORE:
            // TODO
            break;

        case VM_ALU_ARITHMETIC:
            nextbyte = vm_fetch_uint8(i);
            a_type = A_TYPE(nextbyte);
            b_type = B_TYPE(nextbyte);

            if (A_FROM(nextbyte))
                a = VALUE_FETCH(a_type, i);
            else
                a = VALUE_GET(a_type, vm_pop(i), i);

            switch (modifier) {
                case VM_MOD_ADD:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);
                    result = (uint32_t) ((VALUE_GET(a_type, a, i)) + (VALUE_GET(b_type, b, i)));
                    break;

                case VM_MOD_SUB:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);
                    result = (uint32_t) ((VALUE_GET(a_type, a, i)) - (VALUE_GET(b_type, b, i)));
                    break;

                case VM_MOD_MUL:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);
                    result = (uint32_t) ((VALUE_GET(a_type, a, i)) * (VALUE_GET(b_type, b, i)));
                    break;

                case VM_MOD_DIV:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);
                    if (b == 0) {
                        return VM_ERR_DIVBYZ;
                    }

                    result = (uint32_t) ((VALUE_GET(a_type, a, i)) / (VALUE_GET(b_type, b, i)));
                    break;
                case VM_MOD_MOD:
                    if (b_type == VM_MOD_FLOAT)
                        return VM_ERR_NOINT;

                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET(b_type, vm_pop(i), i);

                    result = (uint32_t) ((VALUE_GET_INT(a_type, a, i)) % (VALUE_GET_INT(b_type, b, i)));
                    break;

                case VM_MOD_NEG:
                    result = (uint32_t) (-(VALUE_GET(a_type, a, i)));
                    break;

                case VM_MOD_INC:
                    result = (uint32_t) ((VALUE_GET(a_type, a, i)) + 1);
                    break;

                case VM_MOD_DEC:
                    result = (uint32_t) ((VALUE_GET(a_type, a, i)) - 1);
                    break;
            }
            break;

        case VM_ALU_BITWISE:
            nextbyte = vm_fetch_uint8(i);
            a_type = A_TYPE(nextbyte);
            b_type = B_TYPE(nextbyte);

            if (a_type == VM_MOD_FLOAT || b_type == VM_MOD_FLOAT)
                return VM_ERR_NOINT;

            if (A_FROM(nextbyte))
                a = VALUE_FETCH(a_type, i);
            else
                a = VALUE_GET(a_type, vm_pop(i), i);
            switch (modifier) {
                case VM_MOD_AND:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET_INT(b_type, vm_pop(i), i);
                    result = a && b;
                    break;

                case VM_MOD_OR:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET_INT(b_type, vm_pop(i), i);
                    result = a || b;
                    break;

                case VM_MOD_XOR:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET_INT(b_type, vm_pop(i), i);
                    result = a ^ b;
                    break;

                case VM_MOD_SHL:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET_INT(b_type, vm_pop(i), i);
                    result = a << b;
                    break;

                case VM_MOD_SHR:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET_INT(b_type, vm_pop(i), i);
                    result = a >> b;
                    break;

                case VM_MOD_ROL:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET_INT(b_type, vm_pop(i), i);

                    result = (a << b) | (a >> (32 - b));
                    break;

                case VM_MOD_ROR:
                    if (B_FROM(nextbyte))
                        b = VALUE_FETCH(b_type, i);
                    else
                        b = VALUE_GET_INT(b_type, vm_pop(i), i);

                    result = (a >> b) | (a << (32 - b));
                    break;

                case VM_MOD_NOT:
                    result = ~((uint32_t) (VALUE_GET_INT(a_type, a, i)));
                    break;
            }
            break;

        case VM_CONVERT:
            nextbyte = vm_fetch_uint8(i);
            a_type = A_TYPE(nextbyte);
            b_type = B_TYPE(nextbyte);

            if (A_FROM(nextbyte))
                a = VALUE_FETCH(a_type, i);
            else
                a = VALUE_GET(a_type, vm_pop(i), i);

            vm_push(i, (VALUE_GET(b_type, a, i)));
            break;

        case VM_COMPARE:
            break;

        default:
            return VM_ERR_NOOP;
    }

    return VM_ERR_OK;
}

bool vm_active(vm_t *i) {
    return i->pc != VM_INITIAL_PC;
}

vm_t* vm_new(vm_image_t *image, int argc, char *argv[]) {
    vm_t *i;
    int main_offset, nargs, j;
    char *end_ptr;

    i = malloc(sizeof(vm_t));
    i->method = malloc(VM_MEMORY_SIZE);
    i->cpp = (uint32_t*) i->method + (image->method_area_size + 3) / 4;
    i->stack = (uint32_t*) i->method;
    memset(i->method, 0, VM_MEMORY_SIZE);

    i->sp = i->cpp + image->cpool_size - i->stack - 1;
    i->initial_sp = i->sp;
    i->lv = 0;
    i->pc = VM_INITIAL_PC;

    memcpy(i->method, image->method_area, image->method_area_size);
    memcpy(i->cpp, image->cpool, image->cpool_size * sizeof(int32_t));

    main_offset = i->cpp[image->main_index];
    // Number of arguments to main
    nargs = i->method[main_offset] * 256 + i->method[main_offset + 1];

    // Don't count argv[0], argv[1] or obj. ref.
    if (argc - 2 != nargs - 1) {
        printf("Incorrect number of arguments\n");
        exit(-1);
    }

    vm_push(i, VM_INITIAL_OBJ_REF);
    for (j = 0; j < nargs - 1; j++) {
        vm_push(i, strtol(argv[j + 2], &end_ptr, 0));
        if (argv[j + 2] == end_ptr) {
            printf("Invalid argument to main method: `%s'\n", argv[j + 2]);
            exit(-1);
        }
    }

    // Initialize the VM by simulating a call to main
    vm_call(i, image->main_index);

    return i;
}

#endif /* VM_H_ */
