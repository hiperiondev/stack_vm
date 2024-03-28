/*
 * @vm.h
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

#ifndef VM_H
#define VM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

#include "vm_heap.h"

/**
 * @enum VM_ERRORS
 * @brief
 *
 */
typedef enum VM_ERRORS {
    VM_ERR_OK,             // ok
    VM_ERR_HALT,           //
    VM_ERR_UNKNOWNOP,      //
    VM_ERR_CONST_BADTYPE,  // bad type in constant
    VM_ERR_PRG_END,        // end program or pc beyond program length
    VM_ERR_OUTOFRANGE,     //
    VM_ERR_BAD_VALUE,      //
    VM_ERR_DIVBYZERO,      //
    VM_ERR_TOOMANYTHREADS, //
    VM_ERR_INVALIDRETURN,  //
    VM_ERR_FOREINGFNUNKN,  //
    //[...]//
    VM_ERR_FAIL            // generic fail
} vm_errors_t;

/**
 * @enum VM_VALUE_TYPE
 * @brief
 *
 */
typedef enum VM_VALUE_TYPE {
    VM_VAL_NULL,         /**< Value NULL */
    VM_VAL_BOOL,         /**< Value BOOL */
    VM_VAL_UINT,         /**< Value UINT */
    VM_VAL_INT,          /**< Value INT */
    VM_VAL_FLOAT,        /**< Value FLOAT */
    VM_VAL_STRING,       /**< Value STRING */
    VM_VAL_CONST_STRING, /**< Value CONST_STRING */
    VM_VAL_NATIVE,       /**< Value NATIVE */
    VM_VAL_LIGHT_NATIVE, /**< Value LIGHT_NATIVE */
    VM_VAL_ARRAY,        /**< Value ARRAY */
} vm_value_type_t;

/**
 * @def VM_THREAD_STACK_SIZE
 * @brief
 *
 */
#ifndef VM_THREAD_STACK_SIZE
#define VM_THREAD_STACK_SIZE 128
#endif

/**
 * @def VM_THREAD_MAX_CALL_DEPTH
 * @brief
 *
 */
#ifndef VM_THREAD_MAX_CALL_DEPTH
#define VM_THREAD_MAX_CALL_DEPTH 64
#endif

/**
 * @def VM_MAX_STRINGS
 * @brief
 *
 */
#define VM_MAX_STRINGS 1024

/**
 * @def VM_MAX_HEAP
 * @brief
 *
 */
#define VM_MAX_HEAP 128

typedef struct vm_object_s vm_object_t;
typedef struct vm_state_thread_s vm_state_thread_t;
typedef struct vm_heap_s vm_heap_t;

/**
 * @fn void* (*vm_alloc_function_t)(void *ptr, size_t size, void *userdata)
 * @brief This function should be able to handle all of malloc, realloc, and free
 *
 * malloc => ptr is NULL, size is provided
 * realloc => ptr is not NULL, size is provided
 * free => ptr is not NULL, size is 0
 *
 * @param ptr
 * @param size
 * @param userdata
 */
typedef void* (*vm_alloc_function_t)(size_t size, void *userdata);

/**
 * @struct vm_context_s
 * @brief You can provide this when you create a vm_state to override how memory is allocated within the vm library.
 *
 * Note that the userdata pointer here is non-const. That means if you have context that is shared in a vm_state
 * used across multiple threads, you have to ensure it is synchronized appropriately
 *
 */
typedef struct vm_context_s {
    vm_alloc_function_t alloc;
                   void *userdata;
} vm_context_t;

/**
 * @struct vm_value_s
 * @brief
 *
 */
typedef struct vm_value_s {
    uint8_t type;
    union {
        union {
               bool boolean;
            int32_t integer;
           uint32_t uinteger;
              float real;
            uint8_t byte[4];
        } number;
        const char *cstr;  // for VM_VAL_CONST_STRING
              void *addr;  // for VM_VAL_LIGHT_NATIVE
       vm_object_t *obj;
    };
} vm_value_t;

/**
 * @fn vm_value_t (*vm_foreign_function_t)(vm_state_thread_t *thread, const vm_value_t *args, uint32_t count)
 * @brief
 *
 * @param thread
 * @param args
 * @param count
 * @return
 */
typedef vm_value_t (*vm_foreign_function_t)(vm_state_thread_t *thread, const vm_value_t *args, uint32_t count);

/**
 * @struct vm_state_s
 * @brief
 *
 */
typedef struct vm_state_s {
             vm_context_t ctx;
                  uint8_t *program;
                 uint32_t program_len;
                vm_heap_t *heap;
                 uint32_t strings_qty;
                    char *strings[VM_MAX_STRINGS];
                 uint32_t global_vars_qty;
                 uint32_t functions_qty;
                 uint32_t *functions;
                 uint32_t foreign_functions_qty;
    vm_foreign_function_t *foreign_functions;
} vm_state_t;

extern const vm_value_t vm_value_null;
typedef struct vm_context_s vm_context_t;

/**
 * @struct vm_native_prop_s
 * @brief Stores properties about a native object.
 * This should be statically allocated and only one should exist for each type of Native value.
 *
 */
typedef struct vm_native_prop_s {
    const char *name;
          void (*finalize)(vm_context_t*, void*);
} vm_native_prop_t;

/**
 * @struct vm_object_s
 * @brief
 *
 */
typedef struct vm_object_s {
                  bool marked;             // mark to gc
       vm_value_type_t type;               //

    union {
        struct {
            size_t len;                    //
              char *ptr;                   // if calls "vm_string_len" this is just a pointer to the same allocation as this object
        } string;                          //

        struct {
                              void *addr;  //
            const vm_native_prop_t *prop;  // can be used to check type of native (ex. obj->native.prop == &ArrayProp // this is an Array)
        } native;                          //

        struct {
               uint8_t qty;                // quantity
            vm_value_t fields[];           // data
        } array;                           //
    };
} vm_object_t;

/**
 * @struct vm_frame_s
 * @brief
 *
 */
typedef struct vm_frame_s {
    uint32_t pc, fp;     //
    uint8_t nargs;       //
} vm_frame_t;

/**
 * @struct vm_state_thread_s
 * @brief
 *
 */
typedef struct vm_state_thread_s {
             uint8_t exit_value;                       // exit value from HALT
         vm_errors_t status;                           // vm status
                bool halted;                           // vm is halted
        vm_context_t ctx;                              // each thread can maintain its own context so that you can e.g. override allocation on a per-thread basis.
          vm_state_t *state;                           // each thread stores a reference to its state
          vm_value_t *global_vars;                     // global vars are owned by each thread
            uint32_t global_vars_qty;                  // global vars quantity
            uint32_t pc, fp, sp;                       // program counter, frame pointer, stack pointer
          vm_value_t ret_val;                          // return value from CALL / CALL_FOREIGN
          vm_value_t stack[VM_THREAD_STACK_SIZE];      // vm stack
            uint32_t fc;                               // frame counter
          vm_frame_t frames[VM_THREAD_MAX_CALL_DEPTH]; // frames
                void *userdata;                        // userdata pointer
} vm_state_thread_t;

typedef struct vm_heap_s {
      uint32_t *allocated;
      uint32_t size;
    vm_value_t *data;
} vm_heap_t;

vm_heap_t* vm_heap_create(uint32_t size);
      void vm_heap_destroy(vm_heap_t *heap);
  uint32_t vm_heap_save(vm_heap_t *heap, vm_value_t value);
vm_value_t vm_heap_load(vm_heap_t *heap, uint32_t pos);
      void vm_heap_free(vm_heap_t *heap, uint32_t pos);

/**
 * @fn void vm_step(vm_state_thread_t *thread)
 * @brief Run a single cycle of the thread.
 * Could potentially trigger garbage collection at the end of the cycle.
 *
 * @param thread
 */
void vm_step(vm_state_thread_t **thread);

/**
 * @fn void vm_create_thread(vm_state_thread_t**)
 * @brief
 *
 * @param thread
 */
void vm_create_thread(vm_state_thread_t **thread);

/**
 * @fn void vm_destroy_thread(vm_state_thread_t**)
 * @brief
 *
 * @param thread
 */
void vm_destroy_thread(vm_state_thread_t **thread);

/////////////////// API ///////////////////

/**
 * @fn void vm_api_push(vm_state_thread_t **thread, vm_value_t value)
 * @brief
 *
 * @param thread
 * @param value
 */
void vm_api_push(vm_state_thread_t **thread, vm_value_t value);

/**
 * @fn void vm_api_push_indir(vm_state_thread_t **thread, uint8_t nargs)
 * @brief
 *
 * @param thread
 * @param nargs
 */
void vm_api_push_indir(vm_state_thread_t **thread, uint8_t nargs);

/**
 * @fn vm_value_t vm_api_pop(vm_state_thread_t **thread)
 * @brief
 *
 * @param thread
 * @return
 */
vm_value_t vm_api_pop(vm_state_thread_t **thread);

/**
 * @fn void vm_api_pop_indir(vm_state_thread_t **thread)
 * @brief
 *
 * @param thread
 */
void vm_api_pop_indir(vm_state_thread_t **thread);

/**
 * @fn uint8_t vm_api_read_byte(vm_state_thread_t **thread, uint32_t *pc)
 * @brief
 *
 * @param thread
 * @param pc
 * @return
 */
uint8_t vm_api_read_byte(vm_state_thread_t **thread, uint32_t *pc);

/**
 * @fn int16_t vm_api_read_i16(vm_state_thread_t **thread, uint32_t *pc)
 * @brief
 *
 * @param thread
 * @param pc
 * @return
 */
int16_t vm_api_read_i16(vm_state_thread_t **thread, uint32_t *pc);

/**
 * @fn uint16_t vm_api_read_u16(vm_state_thread_t **thread, uint32_t *pc)
 * @brief
 *
 * @param thread
 * @param pc
 * @return
 */
uint16_t vm_api_read_u16(vm_state_thread_t **thread, uint32_t *pc);

/**
 * @fn int32_t vm_api_read_i32(vm_state_thread_t **thread, uint32_t *pc)
 * @brief
 *
 * @param thread
 * @param pc
 * @return
 */
int32_t vm_api_read_i32(vm_state_thread_t **thread, uint32_t *pc);

/**
 * @fn uint32_t vm_api_read_u32(vm_state_thread_t **thread, uint32_t *pc)
 * @brief
 *
 * @param thread
 * @param pc
 * @return
 */
uint32_t vm_api_read_u32(vm_state_thread_t **thread, uint32_t *pc);

/**
 * @fn float vm_api_read_f32(vm_state_thread_t **thread, uint32_t *pc)
 * @brief
 *
 * @param thread
 * @param pc
 * @return
 */
float vm_api_read_f32(vm_state_thread_t **thread, uint32_t *pc);

/**
 * @fn vm_errors_t vm_api_create_string(vm_state_thread_t **thread, uint32_t index, const char *str)
 * @brief
 *
 * @param thread
 * @param index
 * @param str
 * @return
 */
vm_errors_t vm_api_create_string(vm_state_thread_t **thread, uint32_t index, const char *str);

///////////////////////////////////////////

#endif /* VM_H */
