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

#define STRING_LIBRARY_IDENTIFIER 0x00000001

enum LIBSTRING_FN {
    LIBSTRING_FN_LEN,     //
    LIBSTRING_FN_LEFT,    //
    LIBSTRING_FN_RIGHT,   //
    LIBSTRING_FN_MID,     //
    LIBSTRING_FN_CONCAT,  //
    LIBSTRING_FN_INSERT,  //
    LIBSTRING_FN_DELETE,  //
    LIBSTRING_FN_REPLACE, //
    LIBSTRING_FN_FIND,    //
};

/**
 * @fn vm_errors_t lib_entry_strings(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t arg)
 * @brief Function entry for strings library
 * This functions support functions:
 *       LEN: Return the length of a string.
 *      LEFT: Return the left part of a string.
 *     RIGHT: Return the right part of a string.
 *       MID: Return the middle part of a string.
 *    CONCAT: Add together (concatenate) two or more strings.
 *    INSERT: Insert one string into another string.
 *    DELETE: Delete part of a string.
 *   REPLACE: Replaces part of one string with another string.
 *      FIND: Finds the location of one string within another.
 *
 * @param thread Thread
 * @param call_type Call type
 * @param lib_idx Index of called lib
 * @param args Arguments
 * @return
 */
vm_errors_t lib_entry_strings(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t arg);

#endif /* LIBSTRING_H_ */
