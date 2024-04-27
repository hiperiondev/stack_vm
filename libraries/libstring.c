/*
 * @libstring.c
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

#include <string.h>
#include <wchar.h>

#include "vm.h"
#include "libstring.h"

/**
 * @fn vm_errors_t libstring_len(uint32_t result, char *str)
 * @brief Return the length of a string.
 *
 * @param result
 * @param str
 * @return
 */
static vm_errors_t libstring_len(uint32_t result, char *str) {

    return VM_ERR_OK;
}

/**
 * @fn vm_errors_t libstring_left(char *result, char *str, uint32_t pos)
 * @brief Return the left part of a string.
 *
 * @param result
 * @param str
 * @param pos
 * @return
 */
static vm_errors_t libstring_left(char *result, char *str, uint32_t pos) {

    return VM_ERR_OK;
}

/**
 * @fn vm_errors_t libstring_right(char *result, char *str, uint32_t pos)
 * @brief Return the right part of a string.
 *
 * @param result
 * @param str
 * @param pos
 * @return
 */
static vm_errors_t libstring_right(char *result, char *str, uint32_t pos) {

    return VM_ERR_OK;
}

/**
 * @fn vm_errors_t libstring_mid(char *result, char *str, uint32_t posl, uint32_t posr)
 * @brief Return the middle part of a string.
 *
 * @param result
 * @param str
 * @param posl
 * @param posr
 * @return
 */
static vm_errors_t libstring_mid(char *result, char *str, uint32_t posl, uint32_t posr) {

    return VM_ERR_OK;
}

/**
 * @fn vm_errors_t libstring_concat(char *result, char *str1, char *str2)
 * @brief Add together (concatenate) two or more strings.
 *
 * @param result
 * @param str1
 * @param str2
 * @return
 */
static vm_errors_t libstring_concat(char *result, char *str1, char *str2) {

    return VM_ERR_OK;

}

/**
 * @fn vm_errors_t libstring_insert(char *result, char *str1, char *str2, uint32_t pos)
 * @brief Insert one string into another string.
 *
 * @param result
 * @param str1
 * @param str2
 * @param pos
 * @return
 */
static vm_errors_t libstring_insert(char *result, char *str1, char *str2, uint32_t pos) {

    return VM_ERR_OK;
}

/**
 * @fn vm_errors_t libstring_delete(char *result, char *str, uint32_t posl, uint32_t posr)
 * @brief Delete part of a string.
 *
 * @param result
 * @param str
 * @param posl
 * @param posr
 * @return
 */
static vm_errors_t libstring_delete(char *result, char *str, uint32_t posl, uint32_t posr) {

    return VM_ERR_OK;
}

/**
 * @fn vm_errors_t libstring_replace(char *result, char *str1, char *str2, uint32_t pos)
 * @brief Replaces part of one string with another string.
 *
 * @param result
 * @param str1
 * @param str2
 * @param pos
 * @return
 */
static vm_errors_t libstring_replace(char *result, char *str1, char *str2, uint32_t pos) {

    return VM_ERR_OK;
}

/**
 * @fn vm_errors_t libstring_find(char *result, char *str1, char *str2)
 * @brief Finds the location of one string within another.
 *
 * @param result
 * @param str1
 * @param str2
 * @return
 */
static vm_errors_t libstring_find(char *result, char *str1, char *str2) {

    return VM_ERR_OK;
}


// vm cases
static vm_errors_t libstring_edfat_push(char *result, char *str1, char *str2) {

    return VM_ERR_OK;
}

static vm_errors_t libstring_edfat_cmp(char *result, char *str1, char *str2) {

    return VM_ERR_OK;
}

static vm_errors_t libstring_edfat_gc(char *result, char *str1, char *str2) {

    return VM_ERR_OK;
}

vm_errors_t libstring(vm_thread_t **thread, uint8_t call_type, vm_string_args_s *str_args) {
    if (*thread == NULL)
        return VM_ERR_FAIL;

    vm_errors_t res = VM_ERR_OK;

    switch (call_type) {
        // vm cases
        case VM_EDFAT_PUSH:

            break;
        case VM_EDFAT_CMP:

            break;
        case VM_EDFAT_GC:

            break;

        // libstring cases
        case LIBSTRING_FN_LEN:

            break;
        case LIBSTRING_FN_LEFT:

            break;
        case LIBSTRING_FN_RIGHT:

            break;
        case LIBSTRING_FN_MID:

            break;
        case LIBSTRING_FN_CONCAT:

            break;
        case LIBSTRING_FN_INSERT:

            break;
        case LIBSTRING_FN_DELETE:

            break;
        case LIBSTRING_FN_REPLACE:

            break;
        case LIBSTRING_FN_FIND:

            break;
        default:
            res = VM_ERR_FAIL;
    }

    return res;
}
