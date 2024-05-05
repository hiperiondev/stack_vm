.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Customize
=========

.. rst-class:: lead

   Things you can adjust on Stack VM.

Compile conditions
------------------
*Adjust global behaviour*

* VM_THREAD_STACK_SIZE: Maximum stack size 
* VM_THREAD_MAX_CALL_DEPTH: Maximum call depth (Frames)
* VM_MAX_HEAP: Maximum heap objects
* VM_MAX_GLOBAL_VARS: Maximum global variables
* VM_HEAP_SHRINK_AFTER_GC: Dealloc all upper heap objects allocated but not used after every gc
* VM_ENABLE_TOTYPES: Enable TO_TYPES instruction