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
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
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

#define COMPILER_VERSION_MAYOR 4 // indicate a really big change that can cause a lot of incompatibilities with previous versions
#define COMPILER_VERSION_MINOR 0 // indicate some change on API or opcode
#define COMPILER_VERSION_PATCH 0 // indicate some minor change or correction

//////////////// VM CONFIGURATION ////////////////

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
 * @brief Dealloc all upper heap objects allocated but not used after every gc
 *
 */
#define VM_HEAP_SHRINK_AFTER_GC

/**
 * @def VM_ENABLE_TOTYPES
 * @brief Enable TO_TYPES instruction
 *
 */
#define VM_ENABLE_TOTYPES

/**
 * @def VM_ENABLE_FRAMES_ALIVE
 * @brief Enable frame alive tracking
 *
 */
#define VM_ENABLE_FRAMES_ALIVE

////////////// END VM CONFIGURATION //////////////

////////////////// word id ///////////////////////

#define GET_BIT(v, b)  ((v >> b) & 1)
#define SET_BIT(v, b)  ((v) | (1 << b))
#define CLR_BIT(v, b)  ((v) & (~(1 << b)))

#define ID_ALLOC_WORD(id)        (id / 32)
#define ID_ALLOC_BIT(id)         (id % 32)
#define ID_POS(id_word, id_bit)  ((id_word * 32) + id_bit)

#define vm_wordpos_set_bit(alloc_word, id)    alloc_word[ID_ALLOC_WORD(id)] = (SET_BIT(alloc_word[ID_ALLOC_WORD(id)], ID_ALLOC_BIT(id)))
#define vm_wordpos_unset_bit(alloc_word, id)  alloc_word[ID_ALLOC_WORD(id)] = (CLR_BIT(alloc_word[ID_ALLOC_WORD(id)], ID_ALLOC_BIT(id)))
#define vm_wordpos_isset_bit(alloc_word, id)  (GET_BIT(alloc_word[ID_ALLOC_WORD(id)], ID_ALLOC_BIT(id)) ? 1 : 0)

//////////////////////////////////////////////////

#define OP_MODIFIER(op)       (op & 0xc0)                         /**< indirect argument */

#define STK_OBJ(thread, pos)  (*thread)->stack[pos]               /**< generic access to stack */
#define STK_NEW(thread)       (*thread)->stack[(*thread)->sp]     /**< new stack stack object (over top) */
#define STK_TOP(thread)       (*thread)->stack[(*thread)->sp - 1] /**< top of stack */
#define STK_SND(thread)       (*thread)->stack[(*thread)->sp - 2] /**< second element of stack */
#define STK_TRD(thread)       (*thread)->stack[(*thread)->sp - 3] /**< third element of stack */
#define STK_FTH(thread)       (*thread)->stack[(*thread)->sp - 4] /**< fourth element of stack */

/**
 * @def STK_SWAP
 * @brief Swap top/second elements of stack
 *
 */
#define STK_SWAP(thread)                           \
        vm_value_t __tmp_swap__ = STK_SND(thread); \
        STK_SND(thread) = STK_TOP(thread);         \
        STK_TOP(thread) = __tmp_swap__;

 /**< (internal util) free cstr on stack */
#define STK_FREECSTR(thread, pos)                                            \
        if (pos.type == VM_VAL_CONST_STRING && pos.cstr.is_program == false) \
            free(pos.cstr.addr)

 /**< drop top of stack */
#define STK_DROP(thread)                       \
        STK_FREECSTR(thread, STK_TOP(thread)); \
        --((*thread)->sp)

/**< drop 2 top elements of stack */
#define STK_DROP2(thread)                      \
        STK_FREECSTR(thread, STK_TOP(thread)); \
        STK_FREECSTR(thread, STK_SND(thread)); \
        (*thread)->sp -= 2

/**< drop 3 top elements of stack */
#define STK_DROP3(thread)                      \
        STK_FREECSTR(thread, STK_TOP(thread)); \
        STK_FREECSTR(thread, STK_SND(thread)); \
        STK_FREECSTR(thread, STK_TRD(thread)); \
        (*thread)->sp -= 3

/**
 * @def STKDROPSND
 * @brief Drop second element of stack
 *
 */
#define STKDROPSND(thread)                     \
        STK_FREECSTR(thread, STK_SND(thread)); \
        STK_SND(thread) = STK_TOP(thread);     \
        --((*thread)->sp)


/**
 * @def STKDROPST
 * @brief Drop second and third element of stack
 *
 */
#define STKDROPST(thread)                      \
        STK_FREECSTR(thread, STK_SND(thread)); \
        STK_FREECSTR(thread, STK_TRD(thread)); \
        STK_TRD(thread) = STK_TOP(thread);     \
        (*thread)->sp -= 2


/**
 * @def STKDROPSTF
 * @brief Drop second, third and fourth element of stack
 *
 */
#define STKDROPSTF(thread)                     \
        STK_FREECSTR(thread, STK_SND(thread)); \
        STK_FREECSTR(thread, STK_TRD(thread)); \
        STK_FREECSTR(thread, STK_FTH(thread)); \
        STK_FTH(thread) = STK_TOP(thread);     \
        (*thread)->sp -= 3

/**
 * @def HEAP_OBJ
 * @brief Heap object by reference
 *
 */
#define HEAP_OBJ(ref)  vm_heap_load((*thread)->heap, ref)

/**
 * @def NEW_HEAP_REF
 * @brief Create a new variable pointer referenced heap object
 *
 */
#define NEW_HEAP_REF(_refobj_, ref)  vm_heap_object_t *_refobj_ = vm_heap_load((*thread)->heap, ref)

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

//////////////////////////////////////////////////

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
    VM_VAL_ARRAY,        /**< Value ARRAY */
    VM_VAL_GENERIC,      /**< Value GENERIC */
    VM_VAL_HEAP_REF,     /**< Value HEAP REFERENCE */
    VM_VAL_LIB_OBJ,      /**< Value LIB_OBJ */
} vm_value_type_t;

typedef enum VM_EXTERNAL_DATA_FUNCTION_ARG_TYPE {
    VM_EDFAT_NEW = 0xfa,  /**< VM_EDFAT_NEW */
    VM_EDFAT_PUSH,        /**< VM_EDFAT_PUSH */
    VM_EDFAT_CMP,         /**< VM_EDFAT_CMP */
    VM_EDFAT_GC,          /**< VM_EDFAT_GC */
    VM_EDFAT_TOTYPE,      /**< VM_EDFAT_TOTYPE */
} vm_edf_arg_type_t;

typedef struct vm_thread_s vm_thread_t;

/**
 * @struct vm_value_s
 * @brief VM values
 *
 */
typedef struct vm_value_s {
    vm_value_type_t type;      /**< value type */
    union {
        union {
                bool boolean;  /**< VM_VAL_BOOL */
             int32_t integer;  /**< VM_VAL_INT */
            uint32_t uinteger; /**< VM_VAL_UINT */
               float real;     /**< VM_VAL_FLOAT */
             uint8_t byte[4];  /**< for serialize */
        } number;

        struct {
            uint32_t heap_ref; /**< library object reference */
            uint32_t lib_idx;  /**< library entry function */
        } lib_obj;

        struct {
              bool is_program; /**< is a reference to constant in program area */
              char *addr;      /**< VM_VAL_CONST_STRING */
        } cstr;
          uint32_t heap_ref;   /**< VM_VAL_HEAP_REF */
    };
} vm_value_t;

/**
 * @fn vm_value_t (*vm_foreign_function_t)(vm_state_thread_t **thread, uint8_t fn, uint32_t arg)
 * @brief Foreign function prototype
 *
 * @param thread Thread
 * @param fn Internal function
 * @param arg Generic argument for function
 * @return Any value to ret-val
 */
typedef vm_value_t (*vm_foreign_function_t)(vm_thread_t **thread, uint8_t fn, uint32_t arg);

/**
 * @fn vm_errors_t (*lib_entry)(vm_thread_t **thread, uint8_t call_type, void *args)
 * @brief Main entry for libraries.
 *
 * @param thread Thread.
 * @param call_type Type of call.
 * @param lib_idx Index of called lib
 * @param args stack arguments qty.
 * @return Status.
 */
typedef vm_errors_t (*lib_entry)(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t args);

typedef struct vm_program_s {
    uint8_t *prog;              /**< program */
   uint32_t prog_len;           /**< program length */
} vm_program_t;

/**
 * @struct vm_ffilib_s
 * @brief External functions
 *
 */
typedef struct vm_ffilib_s {
    vm_foreign_function_t *foreign_functions;    /**< pointers to foreign functions */
                 uint32_t foreign_functions_qty; /**< foreign functions quantity */
                lib_entry *lib;                  /**< library entry functions */
                 uint32_t lib_qty;               /**< library quantity */
} vm_ffilib_t;

/**
 * @struct vm_frame_s
 * @brief VM frame structure
 *
 */
typedef struct vm_frame_s {
    uint32_t pc, fp;   /**< program counter, frame counter */
     uint8_t locals;   /**< number of locals */
    uint32_t *gc_mark; /**< garbage collector mark allocator (used for a per frame gc) */
} vm_frame_t;


typedef struct vm_globals_s {
    uint32_t global_vars[VM_MAX_GLOBAL_VARS]; /**< global vars (frame 0 heap indexes) */
    uint32_t global_vars_qty;                 /**< global vars quantity */
} vm_globals_t;

/**
 * @struct vm_heap_object_s
 * @brief VM heap objects
 *
 */
typedef struct vm_heap_object_s {
    vm_value_type_t type;       /**< value type */
    bool static_obj;            /**< static data. Not GC */

    union {
        vm_value_t value;       /**< generic value */

        struct {
               uint8_t qty;     /**< quantity */
            vm_value_t *fields; /**< data */
        } array;

        struct {
            uint32_t identifier; /**< mark for identify library (only for use on library, not VM) */
                void *addr;      /**< library obj reference */
            uint32_t lib_idx;    /**< library entry function */
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

/**
 * @struct vm_thread_s
 * @brief VM main thread state
 *
 */
typedef struct vm_thread_s {
             uint8_t exit_value;                                             /**< exit value from HALT */
         vm_errors_t status;                                                 /**< vm status */
                bool halted;                                                 /**< vm is halted */
            uint32_t indirect;                                               /**< indirect register */
            uint32_t pc, fp, sp;                                             /**< program counter, frame pointer, stack pointer */
          vm_value_t ret_val;                                                /**< return value from CALL / CALL_FOREIGN */
            uint32_t fc;                                                     /**< frame counter */
          vm_frame_t frames[VM_THREAD_MAX_CALL_DEPTH];                       /**< frames */
#ifdef VM_ENABLE_FRAMES_ALIVE
            uint32_t frame_exist[((VM_THREAD_MAX_CALL_DEPTH - 1) / 32) + 1]; /**< frame exist (for fiber implementation) */
#endif
          vm_value_t *stack;                                                 /**< vm stack */
        vm_globals_t *globals;                                               /**< globals vars */
           vm_heap_t *heap;                                                  /**< heap */
         vm_ffilib_t *externals;                                             /**< external functions and libraries */
                void *userdata;                                              /**< generic userdata pointer (not used in vm but useful for foreign functions) */
} vm_thread_t;

/////////////////// API ///////////////////

/**
 * @fn void vm_step(vm_state_thread_t **thread, vm_program_t program)
 * @brief Run a single cycle of the vm
 *
 * @param thread Thread
 * @param program Program
 */
void vm_step(vm_thread_t **thread, vm_program_t *program);

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
 * @fn void vm_push(vm_thread_t **thread, vm_value_t value)
 * @brief Push value
 *
 * @param thread Thread
 * @param value Value
 */
inline void vm_push(vm_thread_t **thread, vm_value_t value) {
    STK_OBJ(thread, (*thread)->sp++) = value;
}

/**
 * @fn vm_value_t vm_pop(vm_thread_t **thread)
 * @brief Pop value
 *
 * @param thread Thread
 * @return Value Value
 */
inline vm_value_t vm_pop(vm_thread_t **thread) {
    return STK_OBJ(thread, --(*thread)->sp);
}

/**
 * @fn vm_errors_t vm_drop_n(vm_thread_t **thread, uint32_t qty)
 * @brief Drop elements n elements from stack
 *
 * @param thread Thread
 * @param qty Quantity

vm_errors_t vm_drop_n(vm_thread_t **thread, uint32_t qty);
 */

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
 * @fn uint8_t vm_read_byte(vm_thread_t **thread, vm_program_t program, uint32_t *pc)
 * @brief Read byte from program
 *
 * @param thread Thread
 * @param program Program
 * @param pc Program counter
 * @return a uint8 value from program
 */
#define vm_read_byte(thread, program, pc) (program)->prog[(*pc)++]

/**
 * @fn int16_t vm_read_i16(vm_thread_t **thread, vm_program_t program, uint32_t *pc)
 * @brief Read 16 bit integer from program
 *
 * @param thread Thread
 * @param program Program
 * @param pc Program counter
 * @return a int16 value from program
 */
inline int16_t vm_read_i16(vm_thread_t **thread, vm_program_t *program, uint32_t *pc) {
    *pc += 2;
    return (int16_t)*((uint32_t*)(program->prog + *pc - 2));
}

/**
 * @fn uint16_t vm_read_u16(vm_thread_t **thread, vm_program_t program, uint32_t *pc)
 * @brief Read 16 bit unsigned integer from program
 *
 * @param thread Thread
 * @param program Program
 * @param pc Program counter
 * @return a uint16 value from program
 */
inline uint16_t vm_read_u16(vm_thread_t **thread, vm_program_t *program, uint32_t *pc) {
    *pc += 2;
    return (uint16_t)*((uint32_t*)(program->prog + *pc - 2));
}

/**
 * @fn int32_t vm_read_i32(vm_thread_t **thread, vm_program_t program, uint32_t *pc)
 * @brief Read 32 bit integer from program
 *
 * @param thread Thread
 * @param program Program
 * @param pc Program counter
 * @return a int32 value from program
 */
inline int32_t vm_read_i32(vm_thread_t **thread, vm_program_t *program, uint32_t *pc) {
    *pc += 4;
    return (int32_t)*((uint32_t*)(program->prog + *pc - 4));
}

/**
 * @fn uint32_t vm_read_u32(vm_thread_t **thread, vm_program_t program, uint32_t *pc)
 * @brief Read 32 bit unsigned integer from program
 *
 * @param thread Thread
 * @param program Program
 * @param pc Program counter
 * @return a uint32 value from program
 */
inline uint32_t vm_read_u32(vm_thread_t **thread, vm_program_t *program, uint32_t *pc) {
    *pc += 4;
    return (uint32_t) *((uint32_t*) (program->prog + *pc - 4));
}

/**
 * @fn float vm_read_f32(vm_thread_t **thread, vm_program_t program, uint32_t *pc)
 * @brief Read 32 bit float from program
 *
 * @param thread Thread
 * @param program Program
 * @param pc Program counter
 * @return a float value from program
 */
inline float vm_read_f32(vm_thread_t **thread, vm_program_t *program, uint32_t *pc) {
    *pc += 4;
    return *((float*) (program->prog + *pc - 4));
}

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
 * @fn bool vm_heap_isstatic(vm_heap_t *heap, uint32_t pos)
 * @brief Check if heap object is static
 *
 * @param heap Heap
 * @param pos Heap position
 * @return Status
 */
bool vm_heap_isstatic(vm_heap_t *heap, uint32_t pos);

/**
 * @fn void vm_heap_gc_collect(vm_heap_t *heap, uint32_t **gc_mark, bool free_mark, vm_thread_t **thread, bool full)
 * @brief Mark as free all gc mark objects
 *
 * @param heap Heap
 * @param gc_mark Local gc allocated mark
 * @param free_mark If true erase Local gc allocated mark (free)
 * @param full If true ignore static and release
 * @param thread Thread
 */
void vm_heap_gc_collect(vm_heap_t *heap, uint32_t **gc_mark, bool free_mark, vm_thread_t **thread, bool full);

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
