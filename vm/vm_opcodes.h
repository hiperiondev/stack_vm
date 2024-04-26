/*
 * @vm_opcodes.h
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

#ifndef VM_OPCODES_H
#define VM_OPCODES_H

/**
 * @enum VM_OPCODE
 * @brief
 *
 */
enum VM_OPCODE {
// NOTE: @ support indirect
//|   opcode              |  id  |  arg1 |  arg2  | description
    PUSH_NULL,         // | 0x00 |   -   |   -    | push NULL value
    PUSH_NULL_N,       // | 0x01 |   u8  |   -    | pushes N null values onto the stack
    PUSH_TRUE,         // | 0x02 |   -   |   -    | push boolean true value
    PUSH_FALSE,        // | 0x03 |   -   |   -    | push boolean false value
    PUSH_INT,          // | 0x04 |  i32  |   -    | push integer value
    PUSH_UINT,         // | 0x05 |  u32  |   -    | push unsigned integer value
    PUSH_0,            // | 0x06 |   -   |   -    | push integer 0
    PUSH_1,            // | 0x07 |   -   |   -    | push integer 1
    PUSH_CHAR,         // | 0x08 |   u8  |   -    | push integer value of char
    PUSH_FLOAT,        // | 0x09 |  f32  |   -    | push float value
    PUSH_CONST_UINT8,  // | 0x0a | @u32  |   -    | push constant uint8 value from program address
    PUSH_CONST_INT8,   // | 0x0b | @u32  |   -    | push constant int8 value from program address
    PUSH_CONST_UINT16, // | 0x0c | @u32  |   -    | push constant uint16 value from program address
    PUSH_CONST_INT16,  // | 0x0d | @u32  |   -    | push constant int16 value from program address
    PUSH_CONST_UINT32, // | 0x0e | @u32  |   -    | push constant uint32 value from program address
    PUSH_CONST_INT32,  // | 0x0f | @u32  |   -    | push constant int32 value from program address
    PUSH_CONST_FLOAT,  // | 0x10 | @u32  |   -    | push constant float value from program address
    PUSH_CONST_STRING, // | 0x11 | @u32  |   -    | push constant string value from program address
    PUSH_HEAP_OBJECT,  // | 0x12 |  u32  |   -    | push a heap object
    NEW_ARRAY,         // | 0x13 |  u16  |   -    | create new array in heap and fill with n elements from stack. Push heap index
    PUSH_ARRAY,        // | 0x14 |   -   |   -    | push array to stack
    GET_ARRAY_VALUE,   // | 0x15 | @u16  |   -    | get value from array
    SET_ARRAY_VALUE,   // | 0x16 | @u16  |   -    | set value on array
    ADD,               // | 0x17 |   -   |   -    | add top and second element from stack
    SUB,               // | 0x18 |   -   |   -    | subtract top and second element from stack
    MUL,               // | 0x19 |   -   |   -    | multiply top and second element from stack
    DIV,               // | 0x1a |   -   |   -    | divide top and second element from stack
    MOD,               // | 0x1b |   -   |   -    | module from top and second element of stack
    OR,                // | 0x1c |   -   |   -    | logical or from top and second element of stack
    AND,               // | 0x1d |   -   |   -    | logical and from top and second element of stack
    LT,                // | 0x1e |   -   |   -    | lesser
    LTE,               // | 0x1f |   -   |   -    | lesser or equal
    GT,                // | 0x20 |   -   |   -    | greater
    GTE,               // | 0x21 |   -   |   -    | greater or equal
    INC,               // | 0x22 |   -   |   -    | increment number (if not number do nothing)
    DEC,               // | 0x23 |   -   |   -    | decrement number (if not number do nothing)
    EQU,               // | 0x24 |   -   |   -    | equality
    NOT,               // | 0x25 |   -   |   -    | binary not
    SET_GLOBAL,        // | 0x26 |  u32  |   -    | set global variable (global 0xffffffff is indirect register)
    GET_GLOBAL,        // | 0x27 |  u32  |   -    | get global variable (global 0xffffffff is indirect register)
    GOTO,              // | 0x28 | @u32  |   -    | jump to pc
    GOTOZ,             // | 0x29 | @u32  |   -    | jump to pc if top stack is zero
    CALL,              // | 0x2a |   u8  |  @u32  | call function in pc, u8 stack reserved for local variables
    RETURN,            // | 0x2b |   -   |   -    | return from function without values
    RETURN_VALUE,      // | 0x2c |   -   |   -    | return from function with value
    CALL_FOREIGN,      // | 0x2d |   u8  |  @u32  | call foreign function
    GET_LOCAL,         // | 0x2e |  u32  |   -    | get local variable
    GET_LOCAL_FF,      // | 0x2f |   u8  |   -    | get local variable if the local index >= 0 and <= 0xff
    SET_LOCAL,         // | 0x30 |  u32  |   -    | set local variable
    SET_LOCAL_FF,      // | 0x31 |   u8  |   -    | set local variable if the local index >= 0 and <= 0xff
    GET_RETVAL,        // | 0x32 |   -   |   -    | push in stack value from function return
    TO_TYPE,           // | 0x33 |   u8  |   -    | convert value to new type
    DROP,              // | 0x34 |   -   |   -    | drop top of stack
    HALT,              // | 0x35 |   u8  |   -    | stop vm
};

#endif /* VM_OPCODES_H */
