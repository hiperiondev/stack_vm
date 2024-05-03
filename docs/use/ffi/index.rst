.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

FFI
===

.. rst-class:: lead

A foreign function interface (FFI) is a mechanism for call external routines.

------

| FFI calls are the simplest way to implement external functions in the VM.
| The function must be implemented according to the prototype:
| **vm_value_t (*vm_foreign_function_t)(vm_thread_t *thread, const vm_value_t *args, uint32_t count);**
| Where thread is the current thread, args is an array of values, and count is the number of values. 
| The arguments are optional and will be passed without verification to the foreign function.
| The pointer to this function must be added to the array of foreign functions (vm_foreign_function_t *foreign_functions in the thread state).
| To call it, use: CALL_FOREIGN u8 @u32 where u8 is an indicator of the number of arguments on the stack and u32 is the index in foreign_functions.
| The function must return a value that will be assigned to ret_val