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
 * @version 0.1
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
    PUSH_NULL,             // 0x00 push NULL value
    PUSH_NULL_N,           // 0x01 pushes N null values onto the stack (where n <= 0xff)
    PUSH_TRUE,             // 0x02 push boolean true value
    PUSH_FALSE,            // 0x03 push boolnea false value
    PUSH_INT,              // 0x04 push integer value
    PUSH_UINT,             // 0x05 push unsigned integer value
    PUSH_0,                // 0x06 fast integer op
    PUSH_1,                // 0x07 fast integer op
    PUSH_CHAR,             // 0x08 push integer value of char
    PUSH_FLOAT,            // 0x09 push float value
    PUSH_STRING,           // 0x0a push const string value from strings pool
    PUSH_STRING_FF,        // 0x0b if the string is in the first 0xff constants, use this opcode
    NEW_ARRAY,             // 0x0c create new array and fill with n elements from stack
    PUSH_ARRAY,            // 0x0d push array to stack
    GET_ARRAY_VALUE,       // 0x0e get value from array
    SET_ARRAY_VALUE,       // 0x0f set value on array
    ADD,                   // 0x10 add top and second element from stack
    SUB,                   // 0x11 subtract top and second element from stack
    MUL,                   // 0x12 multiply top and second element from stack
    DIV,                   // 0x13 divide top and second element from stack
    MOD,                   // 0x14 module from top and second element of stack
    OR,                    // 0x15 logical or from top and second element of stack
    AND,                   // 0x16 logical and from top and second element of stack
    LT,                    // 0x17 lesser
    LTE,                   // 0x18 lesser or equal
    GT,                    // 0x19 greater
    GTE,                   // 0x1a greater or equal
    INC,                   // 0x1b increment number (if not number do nothing)
    DEC,                   // 0x1c decrement number (if not number do nothing)
    EQU,                   // 0x1d equality
    NOT,                   // 0x1e binary not
    SET_GLOBAL,            // 0x1f
    GET_GLOBAL,            // 0x20
    GOTO,                  // 0x21 jump to pc
    GOTOZ,                 // 0x22 jump to pc if top stack is zero
    CALL,                  // 0x23
    RETURN,                // 0x24
    RETURN_VALUE,          // 0x25
    CALL_FOREIGN,          // 0x26
    GET_LOCAL,             // 0x27
    GET_LOCAL_W,           // 0x28 if the local index >= 0 and <= 0xff
    SET_LOCAL,             // 0x29
    GET_RETVAL,            // 0x2a
    GET_CONST,             // 0x2b get value (type: 0=uint8, 1=int8, 2=uint16, 3=int16, 4=uint32, 5=int32, 6=float, 7=string) from program at index + pc
    TO_TYPE,               // 0x2c convert value to new type
    DROP,                  // 0x2d drop top of stack
    HALT,                  // 0x2e stop vm
};

#endif /* VM_OPCODES_H */
