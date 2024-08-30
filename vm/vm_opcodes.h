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
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
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
// NOTE: @ support mod indirect
// NOTE: # support mod jump

//|   opcode              |  id  |  arg1 |  arg2  |  arg3  | description
    PUSH_NULL,         // | 0x00 |   -   |   -    |    -   | push NULL value
    PUSH_NULL_N,       // | 0x01 |   u8  |   -    |    -   | pushes N null values onto the stack
    PUSH_TRUE,         // | 0x02 |   -   |   -    |    -   | push boolean true value
    PUSH_FALSE,        // | 0x03 |   -   |   -    |    -   | push boolean false value
    PUSH_INT,          // | 0x04 |  i32  |   -    |    -   | push integer value
    PUSH_UINT,         // | 0x05 |  u32  |   -    |    -   | push unsigned integer value
    PUSH_0,            // | 0x06 |   -   |   -    |    -   | push integer 0
    PUSH_1,            // | 0x07 |   -   |   -    |    -   | push integer 1
    PUSH_CHAR,         // | 0x08 |   u8  |   -    |    -   | push integer value of char
    PUSH_FLOAT,        // | 0x09 |  f32  |   -    |    -   | push float value
    PUSH_CONST_UINT8,  // | 0x0a | @u32  |   -    |    -   | push constant uint8 value from program address
    PUSH_CONST_INT8,   // | 0x0b | @u32  |   -    |    -   | push constant int8 value from program address
    PUSH_CONST_UINT16, // | 0x0c | @u32  |   -    |    -   | push constant uint16 value from program address
    PUSH_CONST_INT16,  // | 0x0d | @u32  |   -    |    -   | push constant int16 value from program address
    PUSH_CONST_UINT32, // | 0x0e | @u32  |   -    |    -   | push constant uint32 value from program address
    PUSH_CONST_INT32,  // | 0x0f | @u32  |   -    |    -   | push constant int32 value from program address
    PUSH_CONST_FLOAT,  // | 0x10 | @u32  |   -    |    -   | push constant float value from program address
    PUSH_CONST_STRING, // | 0x11 | @u32  |   -    |    -   | push constant string value from program address
    NEW_LIB_OBJ,       // | 0x12 |   -   |   -    |    -   | create a new heap object referenced to library. @ make static (will not be released by GC)
    NEW_HEAP_OBJECT,   // | 0x13 |   -   |   -    |    -   | make a new heap object and move first element of stack to new object. @ make static (will not be released by GC)
    PUSH_HEAP_OBJECT,  // | 0x14 |   -   |   -    |    -   | push a heap object
    FREE_HEAP_OBJECT,  // | 0x15 |   -   |   -    |    -   | mark as free a HEAP object
    NEW_ARRAY,         // | 0x16 |  u16  |   -    |    -   | create new array in heap and fill with n elements from stack. push heap index
    PUSH_ARRAY,        // | 0x17 |   -   |   -    |    -   | push array to stack
    GET_ARRAY_VALUE,   // | 0x18 | @u16  |   -    |    -   | get value from array
    SET_ARRAY_VALUE,   // | 0x19 | @u16  |   -    |    -   | set value on array
    ADD,               // | 0x1a |   -   |   -    |    -   | add top and second element from stack
    SUB,               // | 0x1b |   -   |   -    |    -   | subtract top and second element from stack
    MUL,               // | 0x1c |   -   |   -    |    -   | multiply top and second element from stack
    DIV,               // | 0x1d |   -   |   -    |    -   | divide top and second element from stack
    MOD,               // | 0x1e |   -   |   -    |    -   | module from top and second element of stack
    OR,                // | 0x1f |   -   |   -    |    -   | logical or from top and second element of stack
    AND,               // | 0x20 |   -   |   -    |    -   | logical and from top and second element of stack
    LT,                // | 0x21 | #u32  |   -    |    -   | lesser. in mod jump if true
    LTE,               // | 0x22 | #u32  |   -    |    -   | lesser or equal. in mod jump if true
    GT,                // | 0x23 | #u32  |   -    |    -   | greater. in mod jump if true
    GTE,               // | 0x24 | #u32  |   -    |    -   | greater or equal. in mod jump if true
    INC,               // | 0x25 | #u32  |   -    |    -   | increment number (if not number do nothing). in mod jump
    DEC,               // | 0x26 | #u32  |   -    |    -   | decrement number (if not number do nothing). in mod jump
    EQU,               // | 0x27 | #u32  |   -    |    -   | equality. in mod jump if true
    NOT,               // | 0x28 |   -   |   -    |    -   | binary not
    SET_GLOBAL,        // | 0x29 |  u32  |   -    |    -   | set global variable (global 0xffffffff is indirect register)
    GET_GLOBAL,        // | 0x2a |  u32  |   -    |    -   | get global variable (global 0xffffffff is indirect register)
    GOTO,              // | 0x2b | @u32  |   -    |    -   | jump to pc
    GOTOZ,             // | 0x2c | @u32  |   -    |    -   | jump to pc if top stack is zero
    CALL,              // | 0x2d |   u8  |  @u32  |    -   | call function in pc, u8 stack reserved for local variables
    RETURN,            // | 0x2e |   -   |   -    |    -   | return from function without values
    RETURN_VALUE,      // | 0x2f |   -   |   -    |    -   | return from function with value
    CALL_FOREIGN,      // | 0x30 |   u8  |   u32  |  @u32  | call foreign function
    LIB_FN,            // | 0x31 |   u8  |   u32  |    -   | call library function entry for lib_obj in stack with u8 function call type and u32 arguments
    GET_LOCAL,         // | 0x32 |  u32  |   -    |    -   | get local variable
    GET_LOCAL_FF,      // | 0x33 |   u8  |   -    |    -   | get local variable if the local index >= 0 and <= 0xff
    SET_LOCAL,         // | 0x34 |  u32  |   -    |    -   | set local variable
    SET_LOCAL_FF,      // | 0x35 |   u8  |   -    |    -   | set local variable if the local index >= 0 and <= 0xff
    GET_RETVAL,        // | 0x36 |   -   |   -    |    -   | push in stack value from function return
    TO_TYPE,           // | 0x37 |   u8  |   -    |    -   | convert value to new type
    DROP,              // | 0x38 |   -   |   -    |    -   | drop top of stack
    SWAP,              // | 0x39 |   -   |   -    |    -   | swap top and second element on stack
    HALT,              // | 0x3a |   u8  |   -    |    -   | stop vm
};

#endif /* VM_OPCODES_H */
