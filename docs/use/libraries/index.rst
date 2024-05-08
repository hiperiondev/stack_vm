.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Libraries
=========

.. rst-class:: lead

Libraries are a more sophisticated way to create objects and functions that do not exist in the VM.

-------

| *Libraries create generic objects in the heap, which are referenced in their use.*
| Each object contains a reference to the library's entry function, so they can invoke their own methods directly.
| A library function entry must be implemented according to the prototype:

.. code-block:: C

       vm_errors_t (*lib_entry)(vm_thread_t **thread, uint8_t call_type, uint32_t lib_idx, uint32_t args)

| Where thread is the current thread, call_type is the internal call function, lib_idx is the internal identificator and args is an arbitrary number for internal use.
| The library entry function must be added to  lib array in state.

Call type
-------------

| *The call type argument is a reference of a method in an library object.*
| Exist fourth internal methods that must be implemented in the entry function and are called by the VM:

* VM_EDFAT_NEW: Called when a new library object is created.
* VM_EDFAT_PUSH: Called when a library object is pushed to the stack.
* VM_EDFAT_CMP: Called when a comparison is performed.
* VM_EDFAT_GC: Called in the garbage collector unit.
* VM_EDFAT_TOTYPE: Called when TO_TYPE is performed.

| 
| Any other value can be used by the library for its internal methods

Create a new library object
---------------------------

| *All library objects reside in the Heap*
| To create a new object, use the instruction: PUSH_NEW_HEAP_OBJ. It takes the library ID as an argument on the stack.
| At the end, it leaves the new created object on the stack.
| If the any internal methods (including the VM reserved ones) needs more arguments from the stack, it can manipulate them internally.

Use library object
------------------

| *Each library object has a reference to the library that created it, so it is not necessary to identify it for its use.*
| To call an internal method of a library object in the stack, you must use the instruction: LIB_FN u8 u32
| Where u8 is the call type and u32 is a number to be used internally in the function.
| The error returned by the method will be replicated directly in the output of the VM execution.

Example
-------
| *To see a library implementation please check libstring*
