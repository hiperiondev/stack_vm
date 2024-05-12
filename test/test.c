/*
 * @test.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "vm.h"
#include "vm_assembler.h"
#include "vm_assembler_utils.h"
#include "vm_disassembler.h"
#include "vm_libstring.h"
#include "termcolors.h"

#define EP(x) [x] = #x
const char *vm_errors[] = {
        EP(VM_ERR_OK),
        EP(VM_ERR_HALT),
        EP(VM_ERR_UNKNOWNOP),
        EP(VM_ERR_CONST_BADTYPE),
        EP(VM_ERR_PRG_END),
        EP(VM_ERR_OUTOFRANGE),
        EP(VM_ERR_BAD_VALUE),
        EP(VM_ERR_DIVBYZERO),
        EP(VM_ERR_TOOMANYTHREADS),
        EP(VM_ERR_INVALIDRETURN),
        EP(VM_ERR_FOREINGFNUNKN),
        EP(VM_ERR_OUTOFMEMORY),
        EP(VM_ERR_LOCALNOTEXIST),
        EP(VM_ERR_HEAPNOTEXIST),
        EP(VM_ERR_INDUNDERZERO),
        EP(VM_ERR_OVERFLOW),
        EP(VM_ERR_FAIL),
};

/////////////////////////////////////////////////////////////////////////////////////

void test_opcodes(void) {
#define START_TEST(opcode, prg)                                                                                                                         \
        ++tests_qty;                                                                                                                                    \
        progline = 0;                                                                                                                                   \
        printf(BWHT"  --" BLUB " start test: " #opcode COLOR_RESET BWHT "\n");                                                                          \
        qty = 0;                                                                                                                                        \
        vm_create_thread(&thread);                                                                                                                      \
        hex = malloc(sizeof(uint8_t));                                                                                                                  \
        str = strdup( prg );                                                                                                                            \
        printf("      -- start assembler: \n"BCYN);                                                                                                     \
        res = vm_assembler(&str, &qty, &hex, &errline, &progline, label, &label_qty);                                                                   \
        printf(BWHT);                                                                                                                                   \
        printf("      -- end assembler %s [(%u) %s]\n", res == ASSMBLR_OK ? BGRN"ok"BWHT : BRED"fail"BWHT, res, assembler_error[res]);                  \
        print_hex(hex, qty, 6);                                                                                                                         \
        printf(BWHT);                                                                                                                                   \
        printf("      -- start disassembler\n" BYEL);                                                                                                   \
        vm_disassembler(hex, qty);                                                                                                                      \
        printf(BWHT"      -- end disassembler\n");                                                                                                      \
        program.prog = hex;                                                                                                                             \
        program.prog_len = qty   ;                                                                                                                      \
        thread->halted = false

#define TEST_EXECUTE                                                                                                                                    \
	    printf("      -- start execute\n");                                                                                                             \
        while(thread->pc < program.prog_len && thread->halted == false)                                                                                 \
            vm_step(&thread, &program);                                                                                                                 \
        printf("      -- execute result: %s [(%u) %s] (exit value: %u)\n", thread->status == VM_ERR_OK ? BGRN"ok"BWHT : BRED"fail"BWHT, thread->status, \
            vm_errors[thread->status], thread->exit_value);                                                                                             \
        printf("      -- pc: %u, sp: %u, fp: %u\n", thread->pc, thread->sp, thread->fp)

#define END_TEST()                  \
        vm_destroy_thread(&thread); \
        free(str);                  \
        free(hex);                  \
        printf("\n")

#define OP_TEST_START(_pc, _sp, _fp)  \
        if (res == ASSMBLR_OK) {      \
          assert(thread->pc == _pc);  \
          assert(thread->sp == _sp);  \
          assert(thread->fp == _fp)

#define OP_TEST_END()                                                              \
          printf("  -- " BLUB "end test:" COLOR_RESET " " BGRN "pass" BWHT "\n");  \
        } else {                                                                   \
          printf("  -- " BLUB "end test:" COLOR_RESET " " BRED "error" BWHT "\n"); \
          ++tests_fails;                                                           \
        }

    printf("\n---[ START TEST OPCODES ]---\n\n");

    uint32_t tests_qty = 0, tests_fails = 0;
    uint32_t progline = 0;
    uint32_t qty = 0;
    uint8_t res;
    uint32_t errline;
    uint8_t *hex = NULL;
    char *str = NULL;
    vm_thread_t *thread = NULL;
    vm_value_t vm_value;
    vm_program_t program;
    label_macro_t **label = NULL;
    uint32_t label_qty = 0;

    ///////////////////////////////////
    START_TEST(PUSH_NULL,
            "PUSH_NULL"
            );

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_NULL_N, //
            "PUSH_NULL_N 3" //
            );              //

    TEST_EXECUTE;
    OP_TEST_START(2, 3, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_TRUE, //
            "PUSH_TRUE"   //
            );            //

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == true);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_FALSE, //
            "PUSH_FALSE"   //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == false);
    OP_TEST_END();

    END_TEST();

    ///////////////////////////////////
    START_TEST(PUSH_INT,   //
            "PUSH_INT -10" //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(5, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == -10);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_UINT,     //
            "PUSH_UINT 98345" //
            );                //

    TEST_EXECUTE;
    OP_TEST_START(5, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.integer == 98345);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_0, //
            "PUSH_0"   //
            );         //

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.integer == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_1, //
            "PUSH_1"   //
            );         //

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.integer == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_CHAR,  //
            "PUSH_CHAR 43" //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(2, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.integer == 43);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_FLOAT,     //
            "PUSH_FLOAT -12.6" //
            );                 //

    TEST_EXECUTE;
    OP_TEST_START(5, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_FLOAT);
    assert(vm_value.number.real == (float)-12.600000);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(NEW_ARRAY,   //
            "PUSH_INT 10\n" //
            "PUSH_INT 20\n" //
            "NEW_ARRAY 2\n" //
            );              //

    TEST_EXECUTE;
    OP_TEST_START(13, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_ARRAY);
    assert(vm_value.heap_ref == 0);
    OP_TEST_END();

    END_TEST();

    START_TEST(DROP ARRAY,  //
            "PUSH_INT 10\n" //
            "PUSH_INT 20\n" //
            "NEW_ARRAY 2\n" //
            "DROP\n"        //
            "PUSH_UINT 0\n" //
            "PUSH_ARRAY\n"  //
            "DROP\n"        //
            );              //

    TEST_EXECUTE;
    OP_TEST_START(21, 0, 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GET_ARRAY,         //
            "PUSH_INT 10\n"       //
            "PUSH_INT 20\n"       //
            "NEW_ARRAY 2\n"       //
            "GET_ARRAY_VALUE 0\n" //
            );                    //

    TEST_EXECUTE;
    OP_TEST_START(16, 2, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 10);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(SET_ARRAY,         //
            "PUSH_INT 10\n"       //
            "PUSH_INT 20\n"       //
            "NEW_ARRAY 2\n"       //
            "PUSH_INT 45\n"       //
            "SET_ARRAY_VALUE 0\n" //
            "GET_ARRAY_VALUE 0\n" //
            );                    //

    TEST_EXECUTE;
    OP_TEST_START(24, 2, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 45);
    OP_TEST_END();

    END_TEST();
    //////////////////////////////////
    START_TEST(INDIRECT ARRAY,        //
            "PUSH_UINT 2\n"           //
            "SET_GLOBAL 4294967295\n" //
            "PUSH_INT 10\n"           //
            "PUSH_INT 20\n"           //
            "PUSH_INT 30\n"           //
            "PUSH_INT 40\n"           //
            "PUSH_INT 50\n"           //
            "PUSH_INT 60\n"           //
            "NEW_ARRAY 6\n"           //
            "-@GET_ARRAY_VALUE 0\n"   //
            );

    TEST_EXECUTE;
    OP_TEST_START(46, 2, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 30);
    assert(thread->indirect == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(ADD,        //
            "PUSH_INT 3\n" //
            "PUSH_INT 5\n" //
            "ADD"          //
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 8);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(SUB,        //
            "PUSH_INT 5\n" //
            "PUSH_INT 3\n" //
            "SUB"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 2);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(MUL,        //
            "PUSH_INT 5\n" //
            "PUSH_INT 3\n" //
            "MUL"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 15);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(DIV,        //
            "PUSH_INT 6\n" //
            "PUSH_INT 3\n" //
            "DIV"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 2);
    OP_TEST_END();

    END_TEST();

    START_TEST(DIV,          //
            "PUSH_FLOAT 6\n" //
            "PUSH_FLOAT 0\n" //
            "DIV"            //
            );               //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    assert(thread->status == VM_ERR_DIVBYZERO);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(MOD,        //
            "PUSH_INT 6\n" //
            "PUSH_INT 3\n" //
            "MOD"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(OR,         //
            "PUSH_INT 6\n" //
            "PUSH_INT 3\n" //
            "OR"           //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 7);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(AND,        //
            "PUSH_INT 3\n" //
            "PUSH_INT 9\n" //
            "AND"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(LT,         //
            "PUSH_INT 3\n" //
            "PUSH_INT 6\n" //
            "LT"           //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(LTE,        //
            "PUSH_INT 6\n" //
            "PUSH_INT 3\n" //
            "LTE"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GT,         //
            "PUSH_INT 6\n" //
            "PUSH_INT 3\n" //
            "GT"           //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GTE,        //
            "PUSH_INT 6\n" //
            "PUSH_INT 3\n" //
            "GTE"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(INC,        //
            "PUSH_INT 6\n" //
            "INC"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(6, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 7);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(DEC,        //
            "PUSH_INT 6\n" //
            "DEC"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(6, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 5);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(EQU,        //
            "PUSH_INT 6\n" //
            "PUSH_INT 3\n" //
            "EQU"          //
            );             //

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(NOT,       //
            "PUSH_TRUE\n" //
            "NOT"         //
            );            //

    TEST_EXECUTE;
    OP_TEST_START(2, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(SET_GLOBAL / GET_GLOBAL, //
            "PUSH_INT 12\n"             //
            "SET_GLOBAL 0\n"            //
            "PUSH_FLOAT 34.0\n"         //
            "GET_GLOBAL 0\n"            //
            );                          //

    TEST_EXECUTE;
    OP_TEST_START(20, 2, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 12);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_FLOAT);
    assert(vm_value.number.real == 34);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GOTO,           //
            "GOTO $HERE + 7\n" //
            ".label stop\n"    //
            "HALT 3\n"         //
            "PUSH_INT 30\n"    //
            "PUSH_INT 10\n"    //
            "GOTO stop\n"      //
            "ADD\n"            //
            "HALT 1"           //
            );                 //

    TEST_EXECUTE;
    OP_TEST_START(6, 2, 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GOTOZ,          //
            "GOTO $HERE + 7\n" //
            ".label stop\n"    //
            "HALT 3\n"         //
            "PUSH_INT 30\n"    //
            "PUSH_INT 50\n"    //
            "PUSH_FALSE\n"     //
            "GOTOZ stop\n"     //
            "ADD\n"            //
            "HALT 1"           //
            );

    TEST_EXECUTE;
    OP_TEST_START(6, 2, 0);
    assert(thread->halted == true);
    assert(thread->exit_value == 3);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(CALL / RETURN / RETURN_VALUE, //
            "PUSH_INT 10\n"                  //
            "PUSH_INT 20\n"                  //
            "CALL 2 fun1\n"                  //
            "GET_RETVAL\n"                   //
            "PUSH_INT 5\n"                   //
            "PUSH_INT 2\n"                   //
            "CALL 2 fun2\n"                  //
            "GET_RETVAL\n"                   //
            "HALT 69\n"                      //
            ".label fun1\n"                  //
            "ADD\n"                          //
            "RETURN_VALUE\n"                 //
            ".label fun2\n"                  //
            "PUSH_UINT 34\n"                 //
            "RETURN\n"                       //
            );                               //

    TEST_EXECUTE;
    OP_TEST_START(35, 2, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 30);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(CALL_FOREIGN,       //
            "PUSH_INT 34\n"        //
            "PUSH_INT 21\n"        //
            "CALL_FOREIGN 2 0 0\n" //
            "GET_RETVAL\n"         //
            );                     //

    vm_value_t foreign_function_test(vm_thread_t **thread, uint8_t fn, uint32_t arg) {
        vm_value_t ret;
        ret.type = VM_VAL_INT;
        ret.number.integer = STK_TOP(thread).number.integer + STK_SND(thread).number.integer;
        return ret;
    }

    thread->state->foreign_functions = malloc(sizeof(void*));
    thread->state->foreign_functions_qty = 1;
    thread->state->foreign_functions[0] = foreign_function_test;
    TEST_EXECUTE;
    OP_TEST_START(21, 3, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 55);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(SET_LOCAL / GET_LOCAL, //
            "PUSH_INT 10\n"           //
            "PUSH_INT 20\n"           //
            "CALL 2 fun1\n"           //
            "GET_RETVAL\n"            //
            "HALT 69\n"               //
            ".label fun1\n"           //
            "ADD\n"                   //
            "SET_LOCAL 0\n"           //
            "PUSH_INT 5\n"            //
            "GET_LOCAL 0\n"           //
            "ADD\n"                   //
            "RETURN_VALUE\n"          //
            );                        //

    TEST_EXECUTE;
    OP_TEST_START(18, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 35);
    OP_TEST_END();

    END_TEST();

    START_TEST(SET_LOCAL / GET_LOCAL, //
            "PUSH_INT 1\n"            //
            "PUSH_INT 2\n"            //
            "PUSH_INT 3\n"            //
            "PUSH_INT 4\n"            //
            "CALL 3 fun1\n"           //
            "HALT 69\n"               //
            ".label fun1\n"           //
            "PUSH_INT 10\n"           //
            "SET_LOCAL 0\n"           //
            "PUSH_INT 11\n"           //
            "SET_LOCAL 1\n"           //
            "PUSH_INT 12\n"           //
            "SET_LOCAL 2\n"           //
            "RETURN\n"                //
            );                        //

    TEST_EXECUTE;
    OP_TEST_START(27, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 1);
    OP_TEST_END();

    END_TEST();

    START_TEST(SET_LOCAL_FF / GET_LOCAL_FF, //
            "PUSH_INT 10\n"                 //
            "PUSH_INT 20\n"                 //
            "CALL 2 fun1\n"                 //
            "GET_RETVAL\n"                  //
            "HALT 69\n"                     //
            ".label fun1\n"                 //
            "ADD\n"                         //
            "SET_LOCAL_FF 0\n"              //
            "PUSH_INT 5\n"                  //
            "GET_LOCAL_FF 0\n"              //
            "ADD\n"                         //
            "RETURN_VALUE\n"                //
            );

    TEST_EXECUTE;
    OP_TEST_START(18, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 35);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_CONST_UINT8 / PUSH_CONST_INT8 / PUSH_CONST_UINT16 / PUSH_CONST_INT16 / PUSH_CONST_UINT32 / PUSH_CONST_INT32 / PUSH_CONST_FLOAT / PUSH_CONST_STRING, //
            "PUSH_CONST_UINT8 _data\n"      //
            "PUSH_CONST_INT8 _data + 1\n"   //
            "PUSH_CONST_UINT16 _data + 2\n" //
            "PUSH_CONST_INT16 _data + 4\n"  //
            "PUSH_CONST_UINT32 _data + 6\n" //
            "PUSH_CONST_INT32 _data + 10\n" //
            "PUSH_CONST_FLOAT _data + 14\n" //
            "HALT 0\n"                      //
            ".label _data\n"                //
            ".datau8 3\n"                   //
            ".datai8 -1\n"                  //
            ".datau16 33000\n"              //
            ".datai16 -31000\n"             //
            ".datau32 66000\n"              //
            ".datai32 -67000\n"             //
            ".dataf32 -234.45\n"            //
            );

    TEST_EXECUTE;
    OP_TEST_START(36, 7, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_FLOAT);
    assert(vm_value.number.real == (float)-234.45);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == -67000);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 66000);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == -31000);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 33000);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == -1);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 3);
    OP_TEST_END();

    END_TEST();

    START_TEST(PUSH_CONST_STRING,       //
            "PUSH_CONST_STRING _data\n" //
            "HALT 0\n"                  //
            ".label _data\n"            //
            ".string \"string test\"\n" //
            );                          //

    TEST_EXECUTE;
    OP_TEST_START(6, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "string test") == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
#ifdef VM_ENABLE_TOTYPES
    START_TEST(TO_TYPE,     //
            "PUSH_INT 23\n" //
            "TO_TYPE 4"     //
            );              //

    TEST_EXECUTE;
    OP_TEST_START(7, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_FLOAT);
    assert(vm_value.number.real == 23);
    OP_TEST_END();
    END_TEST();
#endif
    ///////////////////////////////////
    START_TEST(DROP,        //
            "PUSH_INT 23\n" //
            "DROP"          //
            );              //

    TEST_EXECUTE;
    OP_TEST_START(6, 0, 0);
    OP_TEST_END();
    END_TEST();
    ///////////////////////////////////
    START_TEST(HALT,   //
            "HALT 201" //
            );         //

    TEST_EXECUTE;
    OP_TEST_START(1, 0, 0);
    assert(thread->exit_value == 201);
    OP_TEST_END();
    END_TEST();
    ///////////////////////////////////
    START_TEST(SET/GET INDIRECT REGISTER, //
            "PUSH_UINT 12\n"              //
            "SET_GLOBAL 0xffffffff\n"     //
            "GET_GLOBAL 0xffffffff\n"     //
            );                            //

    TEST_EXECUTE;
    OP_TEST_START(15, 1, 0);
    assert(thread->indirect == 12);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 12);
    OP_TEST_END();
    END_TEST();

    START_TEST(DECREMENT INDIRECT REGISTER, //
            "PUSH_UINT 12\n"                //
            "SET_GLOBAL 0xffffffff\n"       //
            "-@PUSH_CONST_UINT8 1\n"        //
            );                              //

    TEST_EXECUTE;
    OP_TEST_START(15, 1, 0);
    assert(thread->indirect == 11);
    OP_TEST_END();
    END_TEST();
    ///////////////////////////////////
    START_TEST(STRING LIBRARY: TO_CSTR, //
            "PUSH_CONST_STRING str\n"   //
            "PUSH_UINT 0\n"             //
            "PUSH_NEW_HEAP_OBJ\n"       //
            "SET_GLOBAL 0\n"            //
            "PUSH_INT 99\n"             //
            "GET_GLOBAL 0\n"            //
            "LIB_FN 9 0\n"              //
            "HALT 99\n"                 //
            ".label str\n"              //
            ".string \"string test\"\n" //
            );                          //

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(33, 2, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "string test") == 0);
    OP_TEST_END();
    END_TEST();

    START_TEST(STRING LIBRARY: LEN,     //
            "PUSH_CONST_STRING str\n"   //
            "PUSH_UINT 0\n"             //
            "PUSH_NEW_HEAP_OBJ\n"       //
            "SET_GLOBAL 0\n"            //
            "PUSH_INT 99\n"             //
            "GET_GLOBAL 0\n"            //
            "LIB_FN 0 0\n"              //
            "HALT 99\n"                 //
            ".label str\n"              //
            ".string \"string test\"\n" //
            );                          //

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(33, 2, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 11);
    OP_TEST_END();
    END_TEST();

    START_TEST(STRING LIBRARY: LEFT,//
            "PUSH_UINT 5\n"              // pos for LEFT
            "PUSH_CONST_STRING str\n"    // push constant string
            "PUSH_UINT 0\n"              // LIBSTRING
            "PUSH_NEW_HEAP_OBJ\n"        // push new LIBSTRING object
            "LIB_FN 1 0\n"               // LIBSTRING_FN_LEFT
            "LIB_FN 9 0\n"               // LIBSTRING_FN_TO_CSTR
            "HALT 99\n"                  // end
            ".label str\n"               //
            ".string \"string test\"\n"  //
            ".string \"other string\"\n" //
            );                           //

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(29, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "string") == 0);
    OP_TEST_END();
    END_TEST();

    START_TEST(STRING LIBRARY: RIGHT,//
            "PUSH_UINT 7\n"              // pos for RIGHT
            "PUSH_CONST_STRING str\n"    // push constant string
            "PUSH_UINT 0\n"              // LIBSTRING
            "PUSH_NEW_HEAP_OBJ\n"        // push new LIBSTRING object
            "LIB_FN 2 0\n"               // LIBSTRING_FN_RIGHT
            "LIB_FN 9 0\n"               // LIBSTRING_FN_TO_CSTR
            "HALT 99\n"                  // end
            ".label str\n"               //
            ".string \"string test\"\n"  //
            ".string \"other string\"\n" //
            );                           //

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(29, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "test") == 0);
    OP_TEST_END();
    END_TEST();

    START_TEST(STRING LIBRARY: MID,      //
            "PUSH_UINT 3\n"              // posl for MID
            "PUSH_UINT 8\n"              // posr for MID
            "PUSH_CONST_STRING str\n"    // push constant string
            "PUSH_UINT 0\n"              // LIBSTRING
            "PUSH_NEW_HEAP_OBJ\n"        // push new LIBSTRING object
            "LIB_FN 3 0\n"               // LIBSTRING_FN_MID
            "LIB_FN 9 0\n"               // LIBSTRING_FN_TO_CSTR
            "HALT 99\n"                  // end
            ".label str\n"               //
            ".string \"string test\"\n"  //
            ".string \"other string\"\n" //
            );                           //

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(34, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "ing te") == 0);
    OP_TEST_END();
    END_TEST();

    START_TEST(STRING LIBRARY: DELETE,    //
            "PUSH_UINT 2\n"               // posl for DELETE
            "PUSH_UINT 8\n"               // posr for DELETE
            "PUSH_CONST_STRING str\n"     // push constant string
            "PUSH_UINT 0\n"               // LIBSTRING
            "PUSH_NEW_HEAP_OBJ\n"         // push new LIBSTRING object
            "LIB_FN 6 0\n"                // LIBSTRING_FN_DELETE
            "LIB_FN 9 0\n"                // LIBSTRING_FN_TO_CSTR
            "HALT 99\n"                   // end
            ".label str\n"                //
            ".string \"string test\"\n"   //
            ".label str2\n"               //
            ".string \" other string\"\n" //
            );                            //

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(34, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "strest") == 0);
    OP_TEST_END();
    END_TEST();

    START_TEST(STRING LIBRARY: INSERT,    //
            "PUSH_UINT 3\n"               // pos for INSERT
            "PUSH_CONST_STRING str2\n"    // push constant string
            "PUSH_CONST_STRING str\n"     // push constant string
            "PUSH_UINT 0\n"               // LIBSTRING
            "PUSH_NEW_HEAP_OBJ\n"         // push new LIBSTRING object
            "LIB_FN 5 0\n"                // LIBSTRING_FN_INSERT
            "LIB_FN 9 0\n"                // LIBSTRING_FN_TO_CSTR
            "HALT 99\n"                   // end
            ".label str\n"                //
            ".string \"string test\"\n"   //
            ".label str2\n"               //
            ".string \" other string\"\n" //
            );                            //

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(34, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "str other stringing test") == 0);
    OP_TEST_END();
    END_TEST();

    START_TEST(STRING LIBRARY: REPLACE,   //
            "PUSH_UINT 3\n"               // pos for REPLACE
            "PUSH_CONST_STRING str2\n"    // push constant string
            "PUSH_CONST_STRING str\n"     // push constant string
            "PUSH_UINT 0\n"               // LIBSTRING
            "PUSH_NEW_HEAP_OBJ\n"         // push new LIBSTRING object
            "LIB_FN 7 0\n"                // LIBSTRING_FN_REPLACE
            "LIB_FN 9 0\n"                // LIBSTRING_FN_TO_CSTR
            "HALT 99\n"                   // end
            ".label str\n"                //
            ".string \"string test\"\n"   //
            ".label str2\n"               //
            ".string \" other string\"\n" //
            );                            //

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(34, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "str other s") == 0);
    OP_TEST_END();
    END_TEST();

    START_TEST(STRING LIBRARY: FIND,    //
            "PUSH_UINT 3\n"             // pos for FIND
            "PUSH_CONST_STRING str2\n"  // push constant string
            "PUSH_CONST_STRING str\n"   // push constant string
            "PUSH_UINT 0\n"             // LIBSTRING
            "PUSH_NEW_HEAP_OBJ\n"       // push new LIBSTRING object
            "LIB_FN 8 0\n"              // LIBSTRING_FN_FIND
            "LIB_FN 9 0\n"              // LIBSTRING_FN_TO_CSTR
            "HALT 99\n"                 // end
            ".label str\n"              //
            ".string \"string test\"\n" //
            ".label str2\n"             //
            ".string \"ing\"\n"         //
            );                          //

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(28, 1, 0);
    vm_value = vm_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 3);
    OP_TEST_END();
    END_TEST();
    ///////////////////////////////////
    printf("---( tests: %u / fails: %u )---\n", tests_qty, tests_fails);
    printf("---[ END TEST OPCODES ]---\n");

#undef START_TEST
#undef TEST_EXECUTE
#undef OP_TEST_START
#undef OP_TEST_END
}

void test_assemble_program(void) {
    printf("\n---[ START TEST ASSEMBLE PROGRAM ]---\n\n");
    uint32_t qty = 0, errline = 0, progline = 0;
    uint8_t res;
    uint8_t *hex = NULL;
    label_macro_t **label = NULL;
    uint32_t label_qty = 0;

    char *program = vm_assembler_load_file("test1.code");
    printf("  -- start assemble program\n");
    printf("(line) [ pc ] - opcode -\n");
    res = vm_assembler(&program, &qty, &hex, &errline, &progline, label, &label_qty);
    printf("  -- end assemble program\n");
    printf("  -- assembler result: %s\n", res == ASSMBLR_OK ? "ok" : "fail");
    print_hex(hex, qty, 2);

    printf("  -- start disassembler\n");
    vm_disassembler(hex, qty);
    printf("  -- end disassembler\n");

    free(hex);
    free(program);

    printf("\n---[ END TEST ASSEMBLE PROGRAM ]---\n");
}

/////////////////////////////////////////////////////////////////////////////////////

int main(void) {
    printf(BWHT"--------------- START TEST ---------------\n");

    test_opcodes();
    printf("\n>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<\n");
    test_assemble_program();

    printf("\n--------------- END TEST ---------------\n");

    return EXIT_SUCCESS;
}
