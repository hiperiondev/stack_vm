/*
 * @ffi_print.h
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

#ifndef FFI_PRINT_H_
#define FFI_PRINT_H_

#include <stdint.h>

#include "vm.h"

vm_value_t ffi_print(vm_thread_t **thread, uint8_t fn, uint32_t arg);

#endif /* FFI_PRINT_H_ */
