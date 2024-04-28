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
 * @version 1.0
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
#include "libstring.h"

//Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

//Regular underline text
#define UBLK "\e[4;30m"
#define URED "\e[4;31m"
#define UGRN "\e[4;32m"
#define UYEL "\e[4;33m"
#define UBLU "\e[4;34m"
#define UMAG "\e[4;35m"
#define UCYN "\e[4;36m"
#define UWHT "\e[4;37m"

//Regular background
#define BLKB "\e[40m"
#define REDB "\e[41m"
#define GRNB "\e[42m"
#define YELB "\e[43m"
#define BLUB "\e[44m"
#define MAGB "\e[45m"
#define CYNB "\e[46m"
#define WHTB "\e[47m"

//High intensty background
#define BLKHB "\e[0;100m"
#define REDHB "\e[0;101m"
#define GRNHB "\e[0;102m"
#define YELHB "\e[0;103m"
#define BLUHB "\e[0;104m"
#define MAGHB "\e[0;105m"
#define CYNHB "\e[0;106m"
#define WHTHB "\e[0;107m"

//High intensity text
#define HBLK "\e[0;90m"
#define HRED "\e[0;91m"
#define HGRN "\e[0;92m"
#define HYEL "\e[0;93m"
#define HBLU "\e[0;94m"
#define HMAG "\e[0;95m"
#define HCYN "\e[0;96m"
#define HWHT "\e[0;97m"

//Bold high intensity text
#define BHBLK "\e[1;90m"
#define BHRED "\e[1;91m"
#define BHGRN "\e[1;92m"
#define BHYEL "\e[1;93m"
#define BHBLU "\e[1;94m"
#define BHMAG "\e[1;95m"
#define BHCYN "\e[1;96m"
#define BHWHT "\e[1;97m"

#define COLOR_RESET "\e[0m"

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
        thread->state->program = hex;                                                                                                                   \
        thread->state->program_len = qty;                                                                                                               \
        thread->halted = false

#define TEST_EXECUTE                                                                                                                                    \
	    printf("      -- start execute\n");                                                                                                             \
        while(thread->pc < thread->state->program_len && thread->halted == false)                                                                       \
            vm_step(&thread);                                                                                                                           \
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
    label_macro_t **label = NULL;
    uint32_t label_qty = 0;

    ///////////////////////////////////
    START_TEST(PUSH_NULL,
            "PUSH_NULL"
            );

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_NULL_N,
            "PUSH_NULL_N 3"
            );

    TEST_EXECUTE;
    OP_TEST_START(2, 3, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_TRUE,
            "PUSH_TRUE"
            );

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == true);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_FALSE,
            "PUSH_FALSE"
            );

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == false);
    OP_TEST_END();

    END_TEST();

    ///////////////////////////////////
    START_TEST(PUSH_INT,
            "PUSH_INT -10"
            );

    TEST_EXECUTE;
    OP_TEST_START(5, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == -10);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_UINT,
            "PUSH_UINT 98345"
            );

    TEST_EXECUTE;
    OP_TEST_START(5, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.integer == 98345);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_0,
            "PUSH_0"
            );

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_1,
            "PUSH_1"
            );

    TEST_EXECUTE;
    OP_TEST_START(1, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_CHAR,
            "PUSH_CHAR 43"
            );

    TEST_EXECUTE;
    OP_TEST_START(2, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 43);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_FLOAT,
            "PUSH_FLOAT -12.6"
            );

    TEST_EXECUTE;
    OP_TEST_START(5, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_FLOAT);
    assert(vm_value.number.real == (float)-12.600000);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(NEW_ARRAY,
            "PUSH_INT 10\n"
            "PUSH_INT 20\n"
            "NEW_ARRAY 2\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(13, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_ARRAY);
    assert(vm_value.heap_ref == 0);
    OP_TEST_END();

    END_TEST();

    START_TEST(DROP ARRAY,
            "PUSH_INT 10\n"
            "PUSH_INT 20\n"
            "NEW_ARRAY 2\n"
            "DROP\n"
            "PUSH_UINT 0\n"
            "PUSH_ARRAY\n"
            "DROP\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(21, 0, 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GET_ARRAY,
            "PUSH_INT 10\n"
            "PUSH_INT 20\n"
            "NEW_ARRAY 2\n"
            "GET_ARRAY_VALUE 0\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(16, 2, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 10);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(SET_ARRAY,
            "PUSH_INT 10\n"
            "PUSH_INT 20\n"
            "NEW_ARRAY 2\n"
            "PUSH_INT 45\n"
            "SET_ARRAY_VALUE 0\n"
            "GET_ARRAY_VALUE 0\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(24, 2, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 45);
    OP_TEST_END();

    END_TEST();
    //////////////////////////////////
    START_TEST(INDIRECT ARRAY,
            "PUSH_UINT 2\n"
            "SET_GLOBAL 4294967295\n"
            "PUSH_INT 10\n"
            "PUSH_INT 20\n"
            "PUSH_INT 30\n"
            "PUSH_INT 40\n"
            "PUSH_INT 50\n"
            "PUSH_INT 60\n"
            "NEW_ARRAY 6\n"
            "-@GET_ARRAY_VALUE 0\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(46, 2, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 30);
    assert(thread->indirect == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(ADD,
            "PUSH_INT 3\n"
            "PUSH_INT 5\n"
            "ADD"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 8);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(SUB,
            "PUSH_INT 5\n"
            "PUSH_INT 3\n"
            "SUB"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 2);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(MUL,
            "PUSH_INT 5\n"
            "PUSH_INT 3\n"
            "MUL"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 15);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(DIV,
            "PUSH_INT 6\n"
            "PUSH_INT 3\n"
            "DIV"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 2);
    OP_TEST_END();

    END_TEST();

    START_TEST(DIV,
            "PUSH_FLOAT 6\n"
            "PUSH_FLOAT 0\n"
            "DIV"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    assert(thread->status == VM_ERR_DIVBYZERO);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(MOD,
            "PUSH_INT 6\n"
            "PUSH_INT 3\n"
            "MOD"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(OR,
            "PUSH_INT 6\n"
            "PUSH_INT 3\n"
            "OR"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 7);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(AND,
            "PUSH_INT 3\n"
            "PUSH_INT 9\n"
            "AND"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(LT,
            "PUSH_INT 3\n"
            "PUSH_INT 6\n"
            "LT"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(LTE,
            "PUSH_INT 6\n"
            "PUSH_INT 3\n"
            "LTE"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GT,
            "PUSH_INT 6\n"
            "PUSH_INT 3\n"
            "GT"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GTE,
            "PUSH_INT 6\n"
            "PUSH_INT 3\n"
            "GTE"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 1);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(INC,
            "PUSH_INT 6\n"
            "INC"
            );

    TEST_EXECUTE;
    OP_TEST_START(6, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 7);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(DEC,
            "PUSH_INT 6\n"
            "DEC"
            );

    TEST_EXECUTE;
    OP_TEST_START(6, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 5);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(EQU,
            "PUSH_INT 6\n"
            "PUSH_INT 3\n"
            "EQU"
            );

    TEST_EXECUTE;
    OP_TEST_START(11, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(NOT,
            "PUSH_TRUE\n"
            "NOT"
            );

    TEST_EXECUTE;
    OP_TEST_START(2, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_BOOL);
    assert(vm_value.number.boolean == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(SET_GLOBAL / GET_GLOBAL,
            "PUSH_INT 12\n"
            "SET_GLOBAL 0\n"
            "PUSH_FLOAT 34.0\n"
            "GET_GLOBAL 0\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(20, 2, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 12);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_FLOAT);
    assert(vm_value.number.real == 34);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GOTO,
            "GOTO $HERE + 7\n"
            ".label stop\n"
            "HALT 3\n"
            "PUSH_INT 30\n"
            "PUSH_INT 10\n"
            "GOTO stop\n"
            "ADD\n"
            "HALT 1"
            );

    TEST_EXECUTE;
    OP_TEST_START(6, 2, 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(GOTOZ,
            "GOTO $HERE + 7\n"
            ".label stop\n"
            "HALT 3\n"
            "PUSH_INT 30\n"
            "PUSH_INT 50\n"
            "PUSH_FALSE\n"
            "GOTOZ stop\n"
            "ADD\n"
            "HALT 1"
            );

    TEST_EXECUTE;
    OP_TEST_START(6, 2, 0);
    assert(thread->halted == true);
    assert(thread->exit_value == 3);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(CALL / RETURN / RETURN_VALUE,
            "PUSH_INT 10\n"
            "PUSH_INT 20\n"
            "CALL 2 fun1\n"
            "GET_RETVAL\n"
            "PUSH_INT 5\n"
            "PUSH_INT 2\n"
            "CALL 2 fun2\n"
            "GET_RETVAL\n"
            "HALT 69\n"
            ".label fun1\n"
            "ADD\n"
            "RETURN_VALUE\n"
            ".label fun2\n"
            "PUSH_UINT 34\n"
            "RETURN\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(35, 2, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_NULL);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 30);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(CALL_FOREIGN,
            "PUSH_INT 34\n"
            "PUSH_INT 21\n"
            "CALL_FOREIGN 2 0\n"
            "GET_RETVAL\n"
            );

    vm_value_t foreign_function_test(vm_thread_t *thread, const vm_value_t *args, uint32_t count) {
        vm_value_t ret;
        ret.type = VM_VAL_INT;
        ret.number.integer = args[0].number.integer + args[1].number.integer;
        return ret;
    }

    thread->state->foreign_functions = malloc(sizeof(void*));
    thread->state->foreign_functions_qty = 1;
    thread->state->foreign_functions[0] = foreign_function_test;
    TEST_EXECUTE;
    OP_TEST_START(17, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 55);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(SET_LOCAL / GET_LOCAL,
            "PUSH_INT 10\n"
            "PUSH_INT 20\n"
            "CALL 2 fun1\n"
            "GET_RETVAL\n"
            "HALT 69\n"
            ".label fun1\n"
            "ADD\n"
            "SET_LOCAL 0\n"
            "PUSH_INT 5\n"
            "GET_LOCAL 0\n"
            "ADD\n"
            "RETURN_VALUE\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(18, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 35);
    OP_TEST_END();

    END_TEST();

    START_TEST(SET_LOCAL / GET_LOCAL,
            "PUSH_INT 1\n"
            "PUSH_INT 2\n"
            "PUSH_INT 3\n"
            "PUSH_INT 4\n"
            "CALL 3 fun1\n"
            "HALT 69\n"
            ".label fun1\n"
            "PUSH_INT 10\n"
            "SET_LOCAL 0\n"
            "PUSH_INT 11\n"
            "SET_LOCAL 1\n"
            "PUSH_INT 12\n"
            "SET_LOCAL 2\n"
            "RETURN\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(27, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 1);
    OP_TEST_END();

    END_TEST();

    START_TEST(SET_LOCAL_FF / GET_LOCAL_FF,
            "PUSH_INT 10\n"
            "PUSH_INT 20\n"
            "CALL 2 fun1\n"
            "GET_RETVAL\n"
            "HALT 69\n"
            ".label fun1\n"
            "ADD\n"
            "SET_LOCAL_FF 0\n"
            "PUSH_INT 5\n"
            "GET_LOCAL_FF 0\n"
            "ADD\n"
            "RETURN_VALUE\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(18, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == 35);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
    START_TEST(PUSH_CONST_UINT8 / PUSH_CONST_INT8 / PUSH_CONST_UINT16 / PUSH_CONST_INT16 / PUSH_CONST_UINT32 / PUSH_CONST_INT32 / PUSH_CONST_FLOAT / PUSH_CONST_STRING,
            "PUSH_CONST_UINT8 _data\n"
            "PUSH_CONST_INT8 _data + 1\n"
            "PUSH_CONST_UINT16 _data + 2\n"
            "PUSH_CONST_INT16 _data + 4\n"
            "PUSH_CONST_UINT32 _data + 6\n"
            "PUSH_CONST_INT32 _data + 10\n"
            "PUSH_CONST_FLOAT _data + 14\n"
            "HALT 0\n"
            ".label _data\n"
            ".datau8 3\n"
            ".datai8 -1\n"
            ".datau16 33000\n"
            ".datai16 -31000\n"
            ".datau32 66000\n"
            ".datai32 -67000\n"
            ".dataf32 -234.45\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(36, 7, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_FLOAT);
    assert(vm_value.number.real == (float)-234.45);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == -67000);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 66000);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == -31000);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 33000);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_INT);
    assert(vm_value.number.integer == -1);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 3);
    OP_TEST_END();

    END_TEST();

    START_TEST(PUSH_CONST_STRING,
            "PUSH_CONST_STRING _data\n"
            "HALT 0\n"
            ".label _data\n"
            ".string \"string test\"\n");

    TEST_EXECUTE;
    OP_TEST_START(6, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "string test") == 0);
    OP_TEST_END();

    END_TEST();
    ///////////////////////////////////
#ifdef VM_ENABLE_TOTYPES
    START_TEST(TO_TYPE,
            "PUSH_INT 23\n"
            "TO_TYPE 4"
            );

    TEST_EXECUTE;
    OP_TEST_START(7, 1, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_FLOAT);
    assert(vm_value.number.real == 23);
    OP_TEST_END();
    END_TEST();
#endif
    ///////////////////////////////////
    START_TEST(DROP,//
            "PUSH_INT 23\n"
            "DROP"
            );

    TEST_EXECUTE;
    OP_TEST_START(6, 0, 0);
    OP_TEST_END();
    END_TEST();
    ///////////////////////////////////
    START_TEST(HALT,//
            "HALT 201"
            );

    TEST_EXECUTE;
    OP_TEST_START(1, 0, 0);
    assert(thread->exit_value == 201);
    OP_TEST_END();
    END_TEST();
    ///////////////////////////////////
    START_TEST(SET/GET INDIRECT REGISTER,
            "PUSH_UINT 12\n"
            "SET_GLOBAL 0xffffffff\n"
            "GET_GLOBAL 0xffffffff\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(15, 1, 0);
    assert(thread->indirect == 12);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_UINT);
    assert(vm_value.number.uinteger == 12);
    OP_TEST_END();
    END_TEST();

    START_TEST(DECREMENT INDIRECT REGISTER,
            "PUSH_UINT 12\n"
            "SET_GLOBAL 0xffffffff\n"
            "-@PUSH_CONST_UINT8 1\n"
            );

    TEST_EXECUTE;
    OP_TEST_START(15, 1, 0);
    assert(thread->indirect == 11);
    OP_TEST_END();
    END_TEST();
    ///////////////////////////////////
    START_TEST(STRING LIBRARY: TO_CSTR,//
            "PUSH_CONST_STRING str\n"
            "PUSH_UINT 0\n"
            "PUSH_NEW_HEAP_OBJ\n"
            "SET_GLOBAL 0\n"
            "PUSH_INT 99\n"
            "GET_GLOBAL 0\n"
            "LIB_FN 9 0\n"
            "HALT 99\n"
            ".label str\n"
            ".string \"string test\"\n"
            );

    thread->state->lib = calloc(1, sizeof(lib_entry));
    thread->state->lib[0] = lib_entry_strings;
    ++thread->state->lib_qty;

    TEST_EXECUTE;
    OP_TEST_START(33, 2, 0);
    vm_value = vm_do_pop(&thread);
    assert(vm_value.type == VM_VAL_CONST_STRING);
    assert(strcmp(vm_value.cstr.addr, "string test") == 0);
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
