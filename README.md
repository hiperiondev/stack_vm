<a name="readme-top"></a>
<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/hiperiondev/stack_vm">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">Stack VM</h3>

  <p align="center">
    Generic VM for scripting
    <br />   
  </p>
</div>

<!-- ABOUT THE PROJECT -->
## About The Project
Stack-vm is a virtual machine oriented towards scripting languages, especially glue ones, prioritizing low memory consumption and predictability.
The size is about 20k.

It has very useful features for use in microcontrollers such as: Fixed stack size, gc per frame with optional shrink and management of both local variables in the stack and a generic heap.

Functions that are not essential must be implemented through FFI (example: strings) but have the minimum constructs so that the VM can handle them.
Also the use of native data types has generic options for use as well as the definition of external functions to be interpreted within the VM.
<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- GETTING STARTED -->
## Features
- [x] Fully commented code in Doxygen format
- [x] 6 bits op (easily extendable)
- [x] An indirection register with automatic increment/decrement
- [x] Heap with automatic growth and optional shrink. Allocation management by bit mask (simple and fast)
- [x] Call with frame placement and stack space reservation for local variables
- [x] Heap management per frame with automatic release at the exit of the frame
- [x] Native external data management (via custom function)
- [x] Primitives for strings
- [x] Simple external functions (FFI)
- [x] Global variables and arrays allocated in heap
  - [x] Can allocate local arrays 
- [x] Constants in the program area
- [x] Complete Assembler/Disassembler with directives easily expandable

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- OPCODE -->
## Opcodes
**op format: iiOOOOOO**
  + i: use indirect register in instruction
    + 00: nothing
    + 01: indirect, not change register
    + 10: indirect, increment register
    + 11: indirect, decrement register
  + O: opcode

*NOTE: @arg -> support indirect*
|          opcode          |  id  |  arg1 |  arg2  | description
|--------------------------|------|-------|--------|------------
|****PUSH_NULL****         | 0x00 | none  | none   | push NULL value
|****PUSH_NULL_N****       | 0x01 |   u8  | none   | pushes N null values onto the stack
|****PUSH_NEW_HEAP_OBJ**** | 0x02 |  u32  | none   | create a new heap object referenced to library
|****PUSH_TRUE****         | 0x03 | none  | none   | push boolean true value
|****PUSH_FALSE****        | 0x04 | none  | none   | push boolean false value
|****PUSH_INT****          | 0x05 |  i32  | none   | push integer value
|****PUSH_UINT****         | 0x06 |  u32  | none   | push unsigned integer value
|****PUSH_0****            | 0x07 | none  | none   | push unsigned integer 0
|****PUSH_1****            | 0x08 | none  | none   | push unsigned integer 1
|****PUSH_CHAR****         | 0x09 |   u8  | none   | push unsigned integer value of char
|****PUSH_FLOAT****        | 0x0a |  f32  | none   | push float value
|****PUSH_CONST_UINT8****  | 0x0b | @u32  | none   | push constant uint8 value from program address
|****PUSH_CONST_INT8****   | 0x0c | @u32  | none   | push constant int8 value from program address
|****PUSH_CONST_UINT16**** | 0x0d | @u32  | none   | push constant uint16 value from program address
|****PUSH_CONST_INT16****  | 0x0e | @u32  | none   | push constant int16 value from program address
|****PUSH_CONST_UINT32**** | 0x0f | @u32  | none   | push constant uint32 value from program address
|****PUSH_CONST_INT32****  | 0x10 | @u32  | none   | push constant int32 value from program address
|****PUSH_CONST_FLOAT****  | 0x11 | @u32  | none   | push constant float value from program address
|****PUSH_CONST_STRING**** | 0x12 | @u32  | none   | push constant string value from program address
|****PUSH_HEAP_OBJECT****  | 0x13 |  u32  | none   | push a heap object
|****NEW_ARRAY****         | 0x14 |  u16  | none   | create new array in heap and fill with n elements from stack. Push heap index
|****PUSH_ARRAY****        | 0x15 | none  | none   | push array to stack
|****GET_ARRAY_VALUE****   | 0x16 | @u16  | none   | get value from array
|****SET_ARRAY_VALUE****   | 0x17 | @u16  | none   | set value on array
|****ADD****               | 0x18 | none  | none   | add top and second element from stack
|****SUB****               | 0x19 | none  | none   | subtract top and second element from stack
|****MUL****               | 0x1a | none  | none   | multiply top and second element from stack
|****DIV****               | 0x1b | none  | none   | divide top and second element from stack
|****MOD****               | 0x1c | none  | none   | module from top and second element of stack
|****OR****                | 0x1d | none  | none   | logical or from top and second element of stack
|****AND****               | 0x1e | none  | none   | logical and from top and second element of stack
|****LT****                | 0x1f | none  | none   | lesser
|****LTE****               | 0x20 | none  | none   | lesser or equal
|****GT****                | 0x21 | none  | none   | greater
|****GTE****               | 0x22 | none  | none   | greater or equal
|****INC****               | 0x23 | none  | none   | increment number (if not number do nothing)
|****DEC****               | 0x24 | none  | none   | decrement number (if not number do nothing)
|****EQU****               | 0x25 | none  | none   | equality
|****NOT****               | 0x26 | none  | none   | binary not
|****SET_GLOBAL****        | 0x27 |  u32  | none   | set global variable (global 0xffffffff is indirect register)
|****GET_GLOBAL****        | 0x28 |  u32  | none   | get global variable (global 0xffffffff is indirect register)
|****GOTO****              | 0x29 | @u32  | none   | jump to pc
|****GOTOZ****             | 0x2a | @u32  | none   | jump to pc if top stack is zero
|****CALL****              | 0x2b |   u8  |  @u32  | call function in pc with u8 stack reserved for local variables
|****RETURN****            | 0x2c | none  | none   | return from function without values
|****RETURN_VALUE****      | 0x2d | none  | none   | return from function with value
|****CALL_FOREIGN****      | 0x2e |   u8  |  @u32  | call foreign function
|****LIB_FN****            | 0x2f |   u8  |  @u32  | call library function entry for lib_obj in stack with u8 function call type and u32 arguments
|****GET_LOCAL****         | 0x30 |  u32  | none   | get local variable
|****GET_LOCAL_FF****      | 0x31 |   u8  | none   | get local variable (first 0xff)
|****SET_LOCAL****         | 0x32 |  u32  | none   | set local variable
|****SET_LOCAL_FF****      | 0x33 |   u8  | none   | set local variable (first 0xff)
|****GET_RETVAL****        | 0x34 | none  | none   | push in stack value from function return
|****TO_TYPE****           | 0x35 |   u8  | none   | convert value to new type
|****DROP****              | 0x36 | none  | none   | drop top of stack
|****HALT****              | 0x37 |   u8  | none   | stop vm

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- USAGE -->
## Usage
> See test.c

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- ASSEMBLER -->
## ASSEMBLER
### Directives
```
    .equ: Assigns a value to a label.
  .macro: Start of a macro. Takes the name as parameter.
          When the name of the macro is written later in the program, 
          the macro definition is expanded at the place it was used. 
   .endm: End of a macro definition.
.include: Start reading from a specified file.
  .label: Assign the address at this point to label.
.comment: Comment line.
 .datau8: Insert a list of unsigned 8 bits values.
.datau16: Insert a list of unsigned 16 bits values.
.datau32: Insert a list of unsigned 32 bits values.
 .datai8: Insert a list of signed 8 bits values.
.datai16: Insert a list of signed 16 bits values.
.datai32: Insert a list of signed 32 bits values.
.dataf32: Insert a list of float 32 bits values.
 .string: Insert string text.

NOTE 1: All content after ; is considered comment.
NOTE 2: Value $HERE represent actual address pointer, can be 
        offsetted ex: .equ label $HERE + 3
NOTE 3: Labels can be offsetted when referenced ex: GOTO label + 5 
```
### OP definition
All opcodes are defined on vm_opcodes_def.c and can be modified according to changes in the VM

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- API -->
## API
### VM
Run a single cycle of the vm
```C
void vm_step(vm_thread_t **thread);
```

Create new thread
```C
void vm_create_thread(vm_thread_t **thread);
```

Destroy thread
```C
void vm_destroy_thread(vm_thread_t **thread);
```
Push value
```C
void vm_do_push(vm_thread_t **thread, vm_value_t value);
```

Pop value
```C
vm_value_t vm_do_pop(vm_thread_t **thread);
```

Drop elements n elements from stack
```C
vm_errors_t vm_do_drop_n(vm_thread_t **thread, uint32_t qty);
```

Push frame (for call)
```C
void vm_push_frame(vm_thread_t **thread, uint8_t nargs);
```

Pop frame (for return from call)
```C
void vm_pop_frame(vm_thread_t **thread);
```

Read byte from program
```C
uint8_t vm_read_byte(vm_thread_t **thread, uint32_t *pc);
```

Read 16 bit integer from program
```C
int16_t vm_read_i16(vm_thread_t **thread, uint32_t *pc);
```

Read 16 bit unsigned integer from program
```C
uint16_t vm_read_u16(vm_thread_t **thread, uint32_t *pc);
```

Read 32 bit integer from program
```C
int32_t vm_read_i32(vm_thread_t **thread, uint32_t *pc);
```

Read 32 bit unsigned integer from program
```C
uint32_t vm_read_u32(vm_thread_t **thread, uint32_t *pc);
```

Read 32 bit float from program
```C
float vm_read_f32(vm_thread_t **thread, uint32_t *pc);
```

### HEAP
Create heap
```C
vm_heap_t* vm_heap_create(uint32_t size);
```

Destroy heap
```C
void vm_heap_destroy(vm_heap_t *heap, vm_thread_t **thread);
```

Save new value in an empty space on heap
```C
uint32_t vm_heap_save(vm_heap_t *heap, vm_heap_object_t value, uint32_t **gc_mark);
```

Retrieve heap object
```C
vm_heap_object_t* vm_heap_load(vm_heap_t *heap, uint32_t pos);
```

Set value on occupied heap object
```C
bool vm_heap_set(vm_heap_t *heap, vm_heap_object_t value, uint32_t pos);
```

Mark as free a heap object
```C
void vm_heap_free(vm_heap_t *heap, uint32_t pos);
```

Check free mark status of gc heap object
```C
bool vm_heap_isgc(vm_heap_t *heap, uint32_t pos, uint32_t *gc_mark);
```

Check allocated mark of heap object
```C
bool vm_heap_isallocated(vm_heap_t *heap, uint32_t pos);
```

Mark as free all gc mark objects
```C
void vm_heap_gc_collect(vm_heap_t *heap, uint32_t **gc_mark, bool free_mark, vm_thread_t **thread);
```

Create new gc mark
```C
uint32_t* vm_heap_new_gc_mark(vm_heap_t *heap);
```

Liberate all memory allocated in heap from not used upper objects
```C
void vm_heap_shrink(vm_heap_t *heap);
```

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/hiperiondev/stack_vm/issues) for a full list of proposed features (and known issues).

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork it (<https://github.com/hiperiondev/stack_vm/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- CONTACT -->
## Contact

*Emiliano Augusto Gonzalez - egonzalez.hiperion@gmail.com*

Project Link: [https://github.com/hiperiondev/stack_vm](https://github.com/hiperiondev/stack_vm)

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>
