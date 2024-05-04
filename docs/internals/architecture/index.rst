.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Architecture
============

Introduction
------------

.. rst-class:: lead

Stack VM is a classic single stack machine, with frame management. 
It uses space reservation on the stack for local variables and a separate heap for global variables, arrays and external objects (e.g., FFI and libraries). 
Generic objects of external functions can also reside in local variables.

Global Variables
----------------

| *Global variables always reside in the Heap.*
| They can be accessed by the operands: SET_GLOBAL and GET_GLOBAL.
| Internally, a tracking of its reference is kept in the state of the Thread (see struct vm_thread_s).
| When the variable is loaded, a local copy is made on the stack.

Local Variables
---------------

| *Local variables always reside on the Stack.*
| They can be accessed by the operands: GET_LOCAL, GET_LOCAL_FF, SET_LOCAL and SET_LOCAL_FF.
| Functions GET_LOCAL_FF and SET_LOCAL_FF can only access the first 255 local variables.
| Physically, they are references to the positions in the stack starting from the actual stack pointer, in reverse order, and they overwrite these positions, which is why must be reserved before making a subroutine call (see FRAMES).
 
Frames
------
 
| *A Frame is a virtual space where a subroutine call is made.* 
| Each time a jump to a subroutine (CALL) is made, the current state information for the return is added to the Frame Stack.
| Additionally, a local heap usage index is created (see HEAP). Upon returning from the call, the previous state is restored and the objects that have been created in the Heap are marked as free.
| It is also indicated how many positions of the Stack, starting from the call point, will be used as local variables.
| These will be accessed only within the call and will be deleted when you return from it (drop of all reserved ones).

Heap
----

| *The heap is a memory space where objects are dynamically created.*
| Global variables and arrays always reside in the Heap.
| Generic objects can be created and manipulated with the instructions: PUSH_NEW_HEAP_OBJ and PUSH_HEAP_OBJECT.
| This puts objects of type reference to the Heap on the Stack, not its copy.
| Internally, memory is dynamically located according to its use. Each block of 32 objects has an associated 32-bit usage index (attribute allocated) which indicates for each bit whether the place is in use or free.
| This index also grows as needed.
| Each Frame maintains its own usage index, when VM returns from a CALL the GC marks all elements created locally in the subroutine as free.
| *The objects in the heap are not deleted, only their availability is marked.*
| When a GC is performed the string constants that were dynamically allocated are also freed.
| The heap does not automatically release the real reserved space, for this the *vm_heap_shrink* function must be invoked (see API) which releases all the space prior to the highest used index (no index relocation is performed).
| You can also configure this function to be automatically invoked at each output of a Frame (See Configuration).

Stack
-----

| *The Stack is the main data structure in the VM*
| Is a statically assigned space for arbitrary data.
| The stack pointer (SP) ever point to the next free space, not to TOP of the stack, then the top of stack is SP - 1.

Indirect register
-----------------

| Some VM instruction argument can be offsetted by an internal indirect register. This behavior is indicated with @ in the assembler program.
| This register can be accessed as the global variable 0xffffffff and can be manipulated with SET_GLOBAL and GET_GLOBAL.
| When +@ or -@ is specified in the statement, this indicates that the indirect register will be automatically incremented or decremented after use. 

