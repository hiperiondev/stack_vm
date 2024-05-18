/*
 * @vm_libhashmap.h
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


#ifndef VM_LIBHASHMAP_H_
#define VM_LIBHASHMAP_H_

#include "hashtable.h"
#include "vm.h"

#define HASHTABLE_LIBRARY_IDENTIFIER 0x00000010

vm_errors_t lib_entry_hashtable(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t arg);

#endif /* VM_LIBHASHMAP_H_ */
