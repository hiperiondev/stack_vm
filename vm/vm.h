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
 * @version 1.0
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

//////////////// VM CONFIGURATION ////////////////
///
/**
 * @def VM_THREAD_STACK_SIZE
 * @brief Maximum stack size
 *
 */
#ifndef VM_THREAD_STACK_SIZE
#define VM_THREAD_STACK_SIZE 128
#endif

/**
 * @def VM_THREAD_MAX_CALL_DEPTH
 * @brief Maximum call depth
 *
 */
#ifndef VM_THREAD_MAX_CALL_DEPTH
#define VM_THREAD_MAX_CALL_DEPTH 128
#endif

/**
 * @def VM_MAX_HEAP
 * @brief Maximum heap objects
 *
 */
#define VM_MAX_HEAP 128

/**
 * @def VM_MAX_GLOBAL_VARS
 * @brief Maximum global variables
 *
 */
#define VM_MAX_GLOBAL_VARS 128

/**
 * @def VM_HEAP_SHRINK_AFTER_GC
 * @brief Delete all upper heap objects allocated but not used
 *
 */
#define VM_HEAP_SHRINK_AFTER_GC

/**
 * @def VM_ENABLE_TOTYPES
 * @brief Enable TO_TYPES op
 *
 */
#define VM_ENABLE_TOTYPES

//////////////////////////////////////////////////
// op: iiOOOOOO
// i: use indirect register in instruction
//    00: nothing
//    01: indirect, not change register
//    10: indirect, increment register
//    11: indirect, decrement register
// O: op
#define OP_INDIRECT(op)  (op & 0xc0) /**< indirect argument */

#define STKTOP(thread)   (*thread)->stack[(*thread)->sp - 1] /**< top of stack */
//////////////////// internals ////////////////////

#define vm_new_bool(val, value) \
    vm_value_t val;             \
    val.type = VM_VAL_BOOL;     \
    val.number.boolean = value

#define vm_new_int(val, i)      \
    vm_value_t val;             \
    val.type = VM_VAL_INT;      \
    val.number.integer = i

#define vm_new_uint(val, i)     \
    vm_value_t val;             \
    val.type = VM_VAL_UINT;     \
    val.number.uinteger = i

#define vm_new_float(val, f)    \
    vm_value_t val;             \
    val.type = VM_VAL_FLOAT;    \
    val.number.real = f

////////////////////////////////////////////////////

/**
 * @enum VM_ERRORS
 * @brief VM exit errors
 *
 */
typedef enum VM_ERRORS {
    VM_ERR_OK,             /**< ok */
    VM_ERR_HALT,           /**< end with halt */
    VM_ERR_UNKNOWNOP,      /**< op unknown */
    VM_ERR_CONST_BADTYPE,  /**< bad type in constant */
    VM_ERR_PRG_END,        /**< end program or pc beyond program length */
    VM_ERR_OUTOFRANGE,     /**< value out of range */
    VM_ERR_BAD_VALUE,      /**< value type not allowed */
    VM_ERR_DIVBYZERO,      /**< division by zero */
    VM_ERR_TOOMANYTHREADS, /**< calls exceed VM_THREAD_MAX_CALL_DEPTH */
    VM_ERR_INVALIDRETURN,  /**< return not associated with call */
    VM_ERR_FOREINGFNUNKN,  /**< foreign function unknown */
    VM_ERR_OUTOFMEMORY,    /**< can't allocate heap */
    VM_ERR_LOCALNOTEXIST,  /**< local variable not exist or out of range */
    VM_ERR_HEAPNOTEXIST,   /**< referenced heap not used or not exist */
    VM_ERR_INDUNDERZERO,   /**< indirect register has been decremented under zero */
    VM_ERR_OVERFLOW,       /**< overflow */
    //[...]//
    VM_ERR_FAIL            /**< generic fail */
} vm_errors_t;

/**
 * @enum VM_VALUE_TYPE
 * @brief VM value types
 *
 */
typedef enum VM_VALUE_TYPE {
    VM_VAL_NULL,         /**< Value NULL */
    VM_VAL_BOOL,         /**< Value BOOL */
    VM_VAL_UINT,         /**< Value UINT */
    VM_VAL_INT,          /**< Value INT */
    VM_VAL_FLOAT,        /**< Value FLOAT */
    VM_VAL_CONST_STRING, /**< Value CONST_STRING */
    VM_VAL_NATIVE,       /**< Value NATIVE */
    VM_VAL_ARRAY,        /**< Value ARRAY */
    VM_VAL_GENERIC,      /**< Value GENERIC */
    VM_VAL_HEAP_REF,     /**< Value HEAP REFERENCE */
    VM_VAL_LIB_OBJ,      /**< Value LIB_OBJ */
} vm_value_type_t;

typedef enum VM_EXTERNAL_DATA_FUNCTION_ARG_TYPE {
    VM_EDFAT_NEW = 0xfa,  //
    VM_EDFAT_PUSH,        //
    VM_EDFAT_CMP,         //
    VM_EDFAT_GC,          //
} vm_edf_arg_type_t;

typedef struct vm_heap_object_s vm_heap_object_t;
typedef struct vm_thread_s vm_thread_t;
typedef struct vm_heap_s vm_heap_t;

/**
 * @struct vm_value_s
 * @brief VM values
 *
 */
typedef struct vm_value_s {
    uint8_t type;
    union {
        union {
                bool boolean;                                         /**< VM_VAL_BOOL */
             int32_t integer;                                         /**< VM_VAL_INT */
            uint32_t uinteger;                                        /**< VM_VAL_UINT */
               float real;                                            /**< VM_VAL_FLOAT */
             uint8_t byte[4];                                         /**< for serialize */
        } number;

        struct {
                void *addr;   /**< library obj reference */
            uint32_t lib_idx; /**< library entry function */
        } lib_obj;

        const char *cstr;    /**< VM_VAL_CONST_STRING */
          uint32_t heap_ref; /**< VM_VAL_HEAP_REF */
    };
} vm_value_t;

/**
 * @fn vm_value_t (*vm_foreign_function_t)(vm_state_thread_t *thread, const vm_value_t *args, uint32_t count)
 * @brief Foreign function prototype
 *
 * @param thread Thread
 * @param args Arguments
 * @param count Arguments quantity
 * @return Any value to ret-val
 */
typedef vm_value_t (*vm_foreign_function_t)(vm_thread_t *thread, const vm_value_t *args, uint32_t count);

/**
 * @fn vm_errors_t (*lib_entry)(vm_thread_t **thread, uint8_t call_type, void *args)
 * @brief Main entry for libraries.
 *
 * @param thread Thread.
 * @param call_type Type of call.
 * @param args stack arguments qty.
 * @return Status.
 */
typedef vm_errors_t (*lib_entry)(vm_thread_t **thread, uint8_t call_type, uint32_t args);

/**
 * @struct vm_state_s
 * @brief VM internal state
 *
 */
typedef struct vm_state_s {
                  uint8_t *program;              /**< program */
                 uint32_t program_len;           /**< program length */
                vm_heap_t *heap;                 /**< heap */
    vm_foreign_function_t *foreign_functions;    /**< pointers to foreign functions */
                 uint32_t foreign_functions_qty; /**< foreign functions quantity */
                lib_entry *lib;                  /**< library entry functions */
                 uint32_t lib_qty;               /**< library quantity */
} vm_state_t;

/**
 * @struct vm_frame_s
 * @brief VM frame structure
 *
 */
typedef struct vm_frame_s {
    uint32_t pc, fp;   /**< program counter, frame counter */
     uint8_t locals;   /**< number of arguments (for call) */
    uint32_t *gc_mark; /**< garbage collector mark allocator (used for a per frame gc) */
} vm_frame_t;

/**
 * @struct vm_thread_s
 * @brief VM main thread state
 *
 */
typedef struct vm_thread_s {
             uint8_t exit_value;                       /**< exit value from HALT */
         vm_errors_t status;                           /**< vm status */
                bool halted;                           /**< vm is halted */
          vm_state_t *state;                           /**< each thread stores a reference to its state */
            uint32_t global_vars[VM_MAX_GLOBAL_VARS];  /**< global vars (frame 0 heap indexes) */
            uint32_t global_vars_qty;                  /**< global vars quantity */
            uint32_t indirect;                         /**< indirect register */
            uint32_t pc, fp, sp;                       /**< program counter, frame pointer, stack pointer */
          vm_value_t ret_val;                          /**< return value from CALL / CALL_FOREIGN */
            uint32_t fc;                               /**< frame counter */
          vm_frame_t frames[VM_THREAD_MAX_CALL_DEPTH]; /**< frames */
          vm_value_t stack[VM_THREAD_STACK_SIZE];      /**< vm stack */
                void *userdata;                        /**< generic userdata pointer (not used in vm but useful for foreign functions) */
} vm_thread_t;

/**
 * @struct vm_heap_object_s
 * @brief VM heap objects
 *
 */
typedef struct vm_heap_object_s {
    vm_value_type_t type;       /**< value type */

    union {
        vm_value_t value;       /**< generic value */

        struct {
               uint8_t qty;     /**< quantity */
            vm_value_t *fields; /**< data */
        } array;

        struct {
            void *addr;       /**< library obj reference */
            uint32_t lib_idx; /**< library entry function */
        } lib_obj;
    };
} vm_heap_object_t;

/**
 * @struct vm_heap_s
 * @brief VM heap structure
 *
 */
typedef struct vm_heap_s {
            uint32_t *allocated; /**< mark allocated data */
            uint32_t size;       /**< size of data heap */
    vm_heap_object_t *data;      /**< heap data */
} vm_heap_t;

/////////////////// API ///////////////////

/**
 * @fn void vm_step(vm_state_thread_t **thread)
 * @brief Run a single cycle of the vm
 *
 * @param thread Thread
 */
void vm_step(vm_thread_t **thread);

/**
 * @fn void vm_create_thread(vm_state_thread_t **thread)
 * @brief Create new thread
 *
 * @param thread Thread
 */
void vm_create_thread(vm_thread_t **thread);

/**
 * @fn void vm_destroy_thread(vm_state_thread_t **thread))
 * @brief Destroy thread
 *
 * @param thread Thread
 */
void vm_destroy_thread(vm_thread_t **thread);

/**
 * @fn void vm_do_push(vm_thread_t **thread, vm_value_t value)
 * @brief Push value
 *
 * @param thread Thread
 * @param value Value
 */
void vm_do_push(vm_thread_t **thread, vm_value_t value);

/**
 * @fn vm_value_t vm_do_pop(vm_thread_t **thread)
 * @brief Pop value
 *
 * @param thread Thread
 * @return Value Value
 */
vm_value_t vm_do_pop(vm_thread_t **thread);

/**
 * @fn void vm_push_frame(vm_thread_t **thread, uint8_t nargs)
 * @brief Push frame (for call)
 *
 * @param thread Thread
 * @param nargs Number of values in stack passed as arguments in call
 */
void vm_push_frame(vm_thread_t **thread, uint8_t nargs);

/**
 * @fn void vm_pop_frame(vm_thread_t **thread)
 * @brief Pop frame (for return from call)
 *
 * @param thread Thread
 */
void vm_pop_frame(vm_thread_t **thread);

/**
 * @fn uint8_t vm_read_byte(vm_thread_t **thread, uint32_t *pc)
 * @brief Read byte from program
 *
 * @param thread Thread
 * @param pc Program counter
 * @return a uint8 value from program
 */
uint8_t vm_read_byte(vm_thread_t **thread, uint32_t *pc);

/**
 * @fn int16_t vm_read_i16(vm_thread_t **thread, uint32_t *pc)
 * @brief Read 16 bit integer from program
 *
 * @param thread Thread
 * @param pc Program counter
 * @return a int16 value from program
 */
int16_t vm_read_i16(vm_thread_t **thread, uint32_t *pc);

/**
 * @fn uint16_t vm_read_u16(vm_thread_t **thread, uint32_t *pc)
 * @brief Read 16 bit unsigned integer from program
 *
 * @param thread Thread
 * @param pc Program counter
 * @return a uint16 value from program
 */
uint16_t vm_read_u16(vm_thread_t **thread, uint32_t *pc);

/**
 * @fn int32_t vm_read_i32(vm_thread_t **thread, uint32_t *pc)
 * @brief Read 32 bit integer from program
 *
 * @param thread Thread
 * @param pc Program counter
 * @return a int32 value from program
 */
int32_t vm_read_i32(vm_thread_t **thread, uint32_t *pc);

/**
 * @fn uint32_t vm_read_u32(vm_thread_t **thread, uint32_t *pc)
 * @brief Read 32 bit unsigned integer from program
 *
 * @param thread Thread
 * @param pc Program counter
 * @return a uint32 value from program
 */
uint32_t vm_read_u32(vm_thread_t **thread, uint32_t *pc);

/**
 * @fn float vm_read_f32(vm_thread_t **thread, uint32_t *pc)
 * @brief Read 32 bit float from program
 *
 * @param thread Thread
 * @param pc Program counter
 * @return a float value from program
 */
float vm_read_f32(vm_thread_t **thread, uint32_t *pc);

/////////// heap ////////

/**
 * @fn vm_heap_t* vm_heap_create(uint32_t size)
 * @brief Create heap
 *
 * @param size Initial preallocated data size
 * @return Heap
 */
vm_heap_t* vm_heap_create(uint32_t size);

/**
 * @fn void vm_heap_destroy(vm_heap_t *heap, vm_thread_t **thread)
 * @brief Destroy heap
 *
 * @param heap Heap
 * @param thread Thread
 */
void vm_heap_destroy(vm_heap_t *heap, vm_thread_t **thread);

/**
 * @fn uint32_t vm_heap_save(vm_heap_t *heap, vm_heap_object_t value, uint32_t **gc_mark)
 * @brief Save new value in an empty space on heap
 *
 * @param heap Heap
 * @param value Value
 * @param gc_mark Local gc allocated mark
 * @return
 */
uint32_t vm_heap_save(vm_heap_t *heap, vm_heap_object_t value, uint32_t **gc_mark);

/**
 * @fn vm_heap_object_t* vm_heap_load(vm_heap_t *heap, uint32_t pos)
 * @brief Retrieve heap object
 *
 * @param heap Heap
 * @param pos Heap position
 * @return Value
 */
vm_heap_object_t* vm_heap_load(vm_heap_t *heap, uint32_t pos);

/**
 * @fn bool vm_heap_set(vm_heap_t*, vm_heap_object_t, uint32_t)
 * @brief Set value on occupied heap object
 *
 * @param heap Heap
 * @param value Value
 * @param pos Heap position
 * @return Status
 */
bool vm_heap_set(vm_heap_t *heap, vm_heap_object_t value, uint32_t pos);

/**
 * @fn void vm_heap_free(vm_heap_t *heap, uint32_t pos)
 * @brief Mark as free a heap object
 *
 * @param heap Heap
 * @param pos Heap position
 */
void vm_heap_free(vm_heap_t *heap, uint32_t pos);

/**
 * @fn bool vm_heap_isgc(vm_heap_t *heap, uint32_t pos, uint32_t *gc_mark)
 * @brief Check free mark status of gc heap object
 *
 * @param heap Heap
 * @param pos Position
 * @param gc_mark Local gc allocated mark
 * @return
 */
bool vm_heap_isgc(vm_heap_t *heap, uint32_t pos, uint32_t *gc_mark);

/**
 * @fn bool vm_heap_isallocated(vm_heap_t *heap, uint32_t pos)
 * @brief Check allocated mark of heap object
 *
 * @param heap Heap
 * @param pos Heap position
 * @return Status
 */
bool vm_heap_isallocated(vm_heap_t *heap, uint32_t pos);

/**
 * @fn void vm_heap_gc_collect(vm_heap_t *heap, uint32_t **gc_mark, bool free_mark, vm_thread_t **thread)
 * @brief Mark as free all gc mark objects
 *
 * @param heap Heap
 * @param gc_mark Local gc allocated mark
 * @param free_mark If true erase Local gc allocated mark (free)
 * @param thread Thread
 */
void vm_heap_gc_collect(vm_heap_t *heap, uint32_t **gc_mark, bool free_mark, vm_thread_t **thread);

/**
 * @fn uint32_t* vm_heap_new_gc_mark(vm_heap_t *heap)
 * @brief Create new gc mark
 *
 * @param heap Heap
 */
uint32_t* vm_heap_new_gc_mark(vm_heap_t *heap);

/**
 * @fn void vm_heap_shrink(vm_heap_t *heap)
 * @brief Liberate all memory allocated in heap from not used upper objects
 *
 * @param heap Heap
 */
void vm_heap_shrink(vm_heap_t *heap);

///////////////////////////////////////////

#endif /* VM_H */
