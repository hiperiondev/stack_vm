/*
 * @libstring.h
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

#ifndef LIBSTRING_H_
#define LIBSTRING_H_

#include "vm.h"

enum LIBSTRING_FN {
    LIBSTRING_FN_LEN,      //
    LIBSTRING_FN_LEFT,     //
    LIBSTRING_FN_RIGHT,    //
    LIBSTRING_FN_MID,      //
    LIBSTRING_FN_CONCAT,   //
    LIBSTRING_FN_INSERT,   //
    LIBSTRING_FN_DELETE,   //
    LIBSTRING_FN_REPLACE,  //
    LIBSTRING_FN_FIND,     //
};

/**
 * @fn vm_errors_t lib_entry_strings(vm_thread_t **thread, uint8_t call_type, void *args)
 * @brief Main call
 *
 * @param thread Thread
 * @param call_type Type of call
 * @param args Arguments
 * @return
 */
vm_errors_t lib_entry_strings(vm_thread_t **thread, uint8_t call_type, void *args);

#endif /* LIBSTRING_H_ */
