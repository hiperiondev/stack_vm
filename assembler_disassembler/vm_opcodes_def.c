/*
 * @vm_opcodes_def.c
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

#include "vm_opcodes.h"
#include "vm_opcodes_def.h"

#define OP(x) #x

const op_t opcodes[] = { //
   // |   opcode              | ind |  arg1 |  arg2  |  arg3  |  arg4  |  arg5  |  arg6  |  arg7  |  arg8  |
     {OP(PUSH_NULL)            , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x00
     {OP(PUSH_NULL_N)          , 0, {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x01
     {OP(PUSH_NEW_HEAP_OBJ)    , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x02
     {OP(PUSH_TRUE)            , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x03
     {OP(PUSH_FALSE)           , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x04
     {OP(PUSH_INT)             , 0, {ARG_I32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x05
     {OP(PUSH_UINT)            , 0, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x06
     {OP(PUSH_0)               , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x07
     {OP(PUSH_1)               , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x08
     {OP(PUSH_CHAR)            , 0, {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x09
     {OP(PUSH_FLOAT)           , 0, {ARG_F32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0a
     {OP(PUSH_CONST_UINT8)     , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0b
     {OP(PUSH_CONST_INT8)      , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0c
     {OP(PUSH_CONST_UINT16)    , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0d
     {OP(PUSH_CONST_INT16)     , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0e
     {OP(PUSH_CONST_UINT32)    , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0f
     {OP(PUSH_CONST_INT32)     , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x10
     {OP(PUSH_CONST_FLOAT)     , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x11
     {OP(PUSH_CONST_STRING)    , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x12
     {OP(PUSH_HEAP_OBJECT)     , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x13
     {OP(NEW_ARRAY)            , 0, {ARG_U16, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x14
     {OP(PUSH_ARRAY)           , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x15
     {OP(GET_ARRAY_VALUE)      , 1, {ARG_U16, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x16
     {OP(SET_ARRAY_VALUE)      , 1, {ARG_U16, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x17
     {OP(ADD)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x18
     {OP(SUB)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x19
     {OP(MUL)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1a
     {OP(DIV)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1b
     {OP(MOD)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1c
     {OP(OR)                   , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1d
     {OP(AND)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1e
     {OP(LT)                   , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1f
     {OP(LTE)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x20
     {OP(GT)                   , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x21
     {OP(GTE)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x22
     {OP(INC)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x23
     {OP(DEC)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x24
     {OP(EQU)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x25
     {OP(NOT)                  , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x26
     {OP(SET_GLOBAL)           , 0, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x27
     {OP(GET_GLOBAL)           , 0, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x28
     {OP(GOTO)                 , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x29
     {OP(GOTOZ)                , 1, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2a
     {OP(CALL)                 , 1, {ARG_U08, ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2b
     {OP(RETURN)               , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2c
     {OP(RETURN_VALUE)         , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2d
     {OP(CALL_FOREIGN)         , 1, {ARG_U08, ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2e
     {OP(LIB_FN)               , 0, {ARG_U08, ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2f
     {OP(GET_LOCAL)            , 0, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x30
     {OP(GET_LOCAL_FF)         , 0, {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x31
     {OP(SET_LOCAL)            , 0, {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x32
     {OP(SET_LOCAL_FF)         , 0, {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x33
     {OP(GET_RETVAL)           , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x34
     {OP(TO_TYPE)              , 0, {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x35
     {OP(DROP)                 , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x36
     {OP(HALT)                 , 0, {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x37
     {"38_INSTRUCTION_"        , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x38
     {"39_INSTRUCTION_"        , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x39
     {"3A_INSTRUCTION_"        , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3a
     {"3B_INSTRUCTION_"        , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3b
     {"3C_INSTRUCTION_"        , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3c
     {"3D_INSTRUCTION_"        , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3d
     {"3E_INSTRUCTION_"        , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3e
     {"3F_INSTRUCTION_"        , 0, {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3f
     { "",0 , { 0, 0, 0, 0, 0, 0, 0, 0 }}                                                               // end
}; //
