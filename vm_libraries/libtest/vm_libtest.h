/*
 * @vm_libtest.h
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
 * @version 2.0
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#ifndef VM_LIBTEST_H_
#define VM_LIBTEST_H_

#include "vm.h"

#define TEST_LIBRARY_IDENTIFIER 0x0000000e

typedef struct libtest_data_s {
    uint32_t data1;
       float data2;
} libtest_data_t;

enum LIBTEST_FN {
    LIBTEST_FN_TEST0,
    LIBTEST_FN_TEST1,
};

vm_errors_t lib_entry_test(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t arg);

#endif /* VM_LIBTEST_H_ */
