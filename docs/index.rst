:orphan: true

.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Stack VM
========================================

.. rst-class:: lead

   Generic Stack VM for Scripting Languages.

----

About The Project
=================

.. rst-class:: lead

Stack VM is a virtual machine oriented towards scripting languages, especially glue ones, prioritizing low memory consumption and predictability.

It has very useful features for use in microcontrollers such as: Fixed stack size, gc per frame with optional shrink and management of both local variables in the stack and a generic heap.

Functions that are not essential must be implemented through FFI (example: strings) but have the minimum constructs so that the VM can handle them.
Also the use of native data types has generic options for use as well as the definition of external functions to be interpreted within the VM.

Features
--------
* Fully commented code in Doxygen format
* 6 bits op (easily extendable)
* An indirection register with automatic increment/decrement
* Heap with automatic growth and optional shrink. Allocation management by bit mask (simple and fast)
* Call with frame placement and stack space reservation for local variables
* Heap management per frame with automatic release at the exit of the frame
* Generic data objects in HEAP and static attribute (survive to frame GC)
* Native external data management (via custom function)
* Libraries for custom data types
* Simple external functions (FFI)
* Global variables and arrays allocated in heap

  * Can allocate local arrays
  * Arrays can contain any type of value, including arrays
  
* Constants in the program area
* Complete Assembler/Disassembler with directives easily expandable

Explore
-------

.. tab-set::

 .. tab-item:: Use

      .. toctree::
         :caption: Use
         :titlesonly:

         use/customize/index
         use/embed/index
         use/data_types/index
         use/ffi/index 
         use/libraries/index

 .. tab-item:: Assembler / Disassembler

      .. toctree::
         :caption: Assembler / Disassembler
         :titlesonly:

         assembler/index
         disassembler/index
         
 .. tab-item:: Internals

      .. toctree::
         :caption: Internals
         :titlesonly:

         internals/architecture/index
         internals/opcodes/index
         internals/api/index
      
 .. tab-item:: Utilities

      .. toctree::
         :caption: Utilities
         :titlesonly:

         utilities/ffi/index
         utilities/library/index
         