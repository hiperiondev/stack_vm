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
 * @version 0.1
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include "vm_opcodes.h"
#include "vm_opcodes_def.h"

#define OP(x) #x

const op_t opcodes[] = { //
   // |   opcode             |  arg1 |  arg2  |  arg3  |  arg4  |  arg5  |  arg6  |  arg7  |  arg8  |
     {OP(PUSH_NULL)        , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x00
     {OP(PUSH_NULL_N)      , {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x01
     {OP(PUSH_TRUE)        , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x02
     {OP(PUSH_FALSE)       , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x03
     {OP(PUSH_INT)         , {ARG_I32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x04
     {OP(PUSH_UINT)        , {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x05
     {OP(PUSH_0)           , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x06
     {OP(PUSH_1)           , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x07
     {OP(PUSH_CHAR)        , {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x08
     {OP(PUSH_FLOAT)       , {ARG_F32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x09
     {OP(PUSH_STRING)      , {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0a
     {OP(PUSH_STRING_FF)   , {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0b
     {OP(NEW_ARRAY)        , {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0c
     {OP(PUSH_ARRAY)       , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0d
     {OP(GET_ARRAY_VALUE)  , {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0e
     {OP(SET_ARRAY_VALUE)  , {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x0f
     {OP(ADD)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x10
     {OP(SUB)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x11
     {OP(MUL)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x12
     {OP(DIV)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x13
     {OP(MOD)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x14
     {OP(OR)               , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x15
     {OP(AND)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x16
     {OP(LT)               , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x17
     {OP(LTE)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x18
     {OP(GT)               , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x19
     {OP(GTE)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1a
     {OP(INC)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1b
     {OP(DEC)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1c
     {OP(EQU)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1d
     {OP(NOT)              , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1e
     {OP(SET_GLOBAL)       , {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x1f
     {OP(GET_GLOBAL)       , {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x20
     {OP(GOTO)             , {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x21
     {OP(GOTOZ)            , {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x22
     {OP(CALL)             , {ARG_U08, ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x23
     {OP(RETURN)           , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x24
     {OP(RETURN_VALUE)     , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x25
     {OP(CALL_FOREIGN)     , {ARG_U08, ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x26
     {OP(GET_LOCAL)        , {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x27
     {OP(GET_LOCAL_W)      , {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x28
     {OP(SET_LOCAL)        , {ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x29
     {OP(GET_RETVAL)       , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2a
     {OP(GET_CONST)        , {ARG_U08, ARG_U16, ARG_U32, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2b
     {OP(TO_TYPE)          , {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2c
     {OP(DROP)             , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2d
     {OP(HALT)             , {ARG_U08, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2e
     {"2F_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x2f
     {"30_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x30
     {"31_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x31
     {"32_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x32
     {"33_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x33
     {"34_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x34
     {"35_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x35
     {"36_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x36
     {"37_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x37
     {"38_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x38
     {"39_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x39
     {"3A_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3a
     {"3B_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3b
     {"3C_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3c
     {"3D_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3d
     {"3E_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3e
     {"3F_INSTRUCTION_"    , {ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON, ARG_NON}}, // 0x3f
     { "", { 0, 0, 0, 0, 0, 0, 0, 0 }}                                                               // end
}; //
