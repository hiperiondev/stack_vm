.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

API
===

.. rst-class:: lead

MACRO UTILS
^^^^^^^^^^^

.. code-block:: C
   :caption: Generic access to stack
   
      STK_OBJ(thread, pos)

.. code-block:: C
   :caption: New stack stack object (over top)
   
      STK_NEW(thread)

.. code-block:: C
   :caption: Top of stack
   
      STK_TOP(thread)

.. code-block:: C
   :caption: Second element of stack
   
      STK_SND(thread)

.. code-block:: C
   :caption: Third element of stack
   
      STK_TRD(thread)

.. code-block:: C
   :caption: Fourth element of stack
   
      STK_FTH(thread)

.. code-block:: C
   :caption: Swap top/second elements of stack
   
      STK_SWAP(thread)

.. code-block:: C
   :caption: Drop top of stack
   
      STK_DROP(thread)

.. code-block:: C
   :caption: Drop 2 top elements of stack
   
      STK_DROP2(thread) 

.. code-block:: C
   :caption: Drop 3 top elements of stack
   
      STK_DROP3(thread)

.. code-block:: C
   :caption: Drop second element of stack
   
      STKDROPSND(thread)

.. code-block:: C
   :caption: Drop second and third element of stack
    
      STKDROPST(thread) 

.. code-block:: C
   :caption: Drop second, third and fourth element of stack
   
      STKDROPSTF(thread) 

.. code-block:: C
   :caption: Heap object by reference
   
      HEAP_OBJ(ref)

.. code-block:: C
   :caption: Create a new variable pointer referenced heap object
   
      NEW_HEAP_REF(_refobj_, ref)

.. rst-class:: lead

VM INTERNALS
^^^^^^^^^^^^

.. code-block:: C
   :caption: Run a single cycle of the vm
   
      void vm_step(vm_thread_t **thread);

.. code-block:: C
   :caption: Create new thread
   
      void vm_create_thread(vm_thread_t **thread);

.. code-block:: C
   :caption: Destroy thread
   
      void vm_destroy_thread(vm_thread_t **thread);

.. code-block:: C
   :caption: Push value
   
      void vm_push(vm_thread_t **thread, vm_value_t value);

.. code-block:: C
   :caption: Pop value
   
      vm_value_t vm_pop(vm_thread_t **thread);

.. code-block:: C
   :caption: Drop elements n elements from stack
   
      vm_errors_t vm_drop_n(vm_thread_t **thread, uint32_t qty);

.. code-block:: C
   :caption: Push frame (for call)
   
      void vm_push_frame(vm_thread_t **thread, uint8_t nargs);

.. code-block:: C
   :caption: Pop frame (for return from call)
    
      void vm_pop_frame(vm_thread_t **thread);

.. code-block:: C
   :caption: Read byte from program
   
      uint8_t vm_read_byte(vm_thread_t **thread, uint32_t *pc);

.. code-block:: C
   :caption: Read 16 bit integer from program
   
      int16_t vm_read_i16(vm_thread_t **thread, uint32_t *pc);

.. code-block:: C
   :caption: Read 16 bit unsigned integer from program
   
      uint16_t vm_read_u16(vm_thread_t **thread, uint32_t *pc);

.. code-block:: C
   :caption: Read 32 bit integer from program
   
      int32_t vm_read_i32(vm_thread_t **thread, uint32_t *pc);

.. code-block:: C
   :caption: Read 32 bit unsigned integer from program
   
      uint32_t vm_read_u32(vm_thread_t **thread, uint32_t *pc);

.. code-block:: C
   :caption: Read 32 bit float from program
   
      float vm_read_f32(vm_thread_t **thread, uint32_t *pc);

.. rst-class:: lead

HEAP
^^^^

.. code-block:: C
   :caption: Create heap
   
      vm_heap_t* vm_heap_create(uint32_t size);

.. code-block:: C
   :caption: Destroy heap
   
      void vm_heap_destroy(vm_heap_t *heap, vm_thread_t **thread);

.. code-block:: C
   :caption: Save new value in an empty space on heap
   
      uint32_t vm_heap_save(vm_heap_t *heap, vm_heap_object_t value, uint32_t **gc_mark);

.. code-block:: C
   :caption: Retrieve heap object
   
      vm_heap_object_t* vm_heap_load(vm_heap_t *heap, uint32_t pos);

.. code-block:: C
   :caption: Set value on occupied heap object
   
      bool vm_heap_set(vm_heap_t *heap, vm_heap_object_t value, uint32_t pos);

.. code-block:: C
   :caption: Mark as free a heap object
   
      void vm_heap_free(vm_heap_t *heap, uint32_t pos);

.. code-block:: C
   :caption: Check free mark status of gc heap object
   
      bool vm_heap_isgc(vm_heap_t *heap, uint32_t pos, uint32_t *gc_mark);

.. code-block:: C
   :caption: Check allocated mark of heap object
   
      bool vm_heap_isallocated(vm_heap_t *heap, uint32_t pos);

.. code-block:: C
   :caption: Mark as free all gc mark objects
   
      void vm_heap_gc_collect(vm_heap_t *heap, uint32_t **gc_mark, bool free_mark, vm_thread_t **thread);

.. code-block:: C
   :caption: Create new gc mark
   
      uint32_t* vm_heap_new_gc_mark(vm_heap_t *heap);

.. code-block:: C
   :caption: Liberate all memory allocated in heap from not used upper objects
   
      void vm_heap_shrink(vm_heap_t *heap);
