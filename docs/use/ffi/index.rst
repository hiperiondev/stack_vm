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

.. code-block:: C

       vm_value_t (*vm_foreign_function_t)(vm_thread_t **thread, uint8_t fn, uint32_t arg)

| Where thread is the current thread, and fn is a parameter for internal use of the function. 
| The pointer to this function must be added to the array of foreign functions (foreign_functions in the thread state).
| To call it, use: CALL_FOREIGN u8 u32 @u32 where first u8 is the fn parameter, second u32 is an arbitrary argument and third @u32 is the index in foreign_functions array.
| The function must return a value that will be assigned to ret_val