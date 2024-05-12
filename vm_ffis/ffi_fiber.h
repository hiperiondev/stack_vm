/*
 * @ffi_fiber.h
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
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#ifndef FFI_FIBER_H_
#define FFI_FIBER_H_

#define FFI_MAX_FIBERS 64

typedef enum {
    FFI_FIBER_ENQUEUE, // enqueue this frame
    FFI_FIBER_DEQUEUE, // dequeue this fame
    FFI_FIBER_CURRENT, // fiber id for this frame
    FFI_FIBER_RESUME,  // resume fiber id
    FFI_FIBER_YIELD,   // return to scheduler
} ffi_fiber_fn_t;

vm_value_t ffi_fiber(vm_thread_t **thread, uint32_t fn, uint32_t arg);

#endif /* FFI_FIBER_H_ */
