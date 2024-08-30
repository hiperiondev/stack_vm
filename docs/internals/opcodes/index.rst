.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Instructions
============
   
**op format: iiOOOOOO**

* i: if # jump.
* i: if @ use indirect register.

  * 00: nothing
  * 01: indirect, not change register
  * 10: indirect, increment register
  * 11: indirect, decrement register
  
* O: opcode

*NOTE1: @arg -> support indirect*

*NOTE2: @ can change instruction behaviour*

+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|     instruction      |  id  |  arg1 |  arg2 | description                                                                                                      |
+======================+======+=======+=======+==================================================================================================================+
|**PUSH_NULL**         | 0x00 | none  | none  | push NULL value                                                                                                  |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_NULL_N**       | 0x01 |   u8  | none  | pushes N null values onto the stack                                                                              |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_TRUE**         | 0x02 | none  | none  | push boolean true value                                                                                          |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_FALSE**        | 0x03 | none  | none  | push boolean false value                                                                                         |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_INT**          | 0x04 |  i32  | none  | push integer value                                                                                               |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_UINT**         | 0x05 |  u32  | none  | push unsigned integer value                                                                                      |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_0**            | 0x06 | none  | none  | push unsigned integer 0                                                                                          |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_1**            | 0x07 | none  | none  | push unsigned integer 1                                                                                          |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_CHAR**         | 0x08 |   u8  | none  | push unsigned integer value of char                                                                              |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_FLOAT**        | 0x09 |  f32  | none  | push float value                                                                                                 |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_CONST_UINT8**  | 0x0a | @u32  | none  | push constant uint8 value from program address                                                                   |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_CONST_INT8**   | 0x0b | @u32  | none  | push constant int8 value from program address                                                                    |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_CONST_UINT16** | 0x0c | @u32  | none  | push constant uint16 value from program address                                                                  |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_CONST_INT16**  | 0x0d | @u32  | none  | push constant int16 value from program address                                                                   |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_CONST_UINT32** | 0x0e | @u32  | none  | push constant uint32 value from program address                                                                  |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_CONST_INT32**  | 0x0f | @u32  | none  | push constant int32 value from program address                                                                   |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_CONST_FLOAT**  | 0x10 | @u32  | none  | push constant float value from program address                                                                   |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_CONST_STRING** | 0x11 | @u32  | none  | push constant string value from program address                                                                  |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|** NEW_LIB_OBJ**      | 0x12 | none  | none  | create a new heap object referenced to library. @ make static (will not be released by GC)                       |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|** NEW_HEAP_OBJECT**  | 0x13 | none  | none  | make a new heap object and move first element of stack to new object. @ make static (will not be released by GC) |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|** PUSH_HEAP_OBJECT** | 0x14 | none  | none  | push a heap object                                                                                               |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|** FREE_HEAP_OBJECT** | 0x15 | none  | none  | mark as free a HEAP object                                                                                       |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**NEW_ARRAY**         | 0x16 |  u16  | none  | create new array in heap and fill with n elements from stack. Push heap index                                    |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**PUSH_ARRAY**        | 0x17 | none  | none  | push array to stack                                                                                              |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**GET_ARRAY_VALUE**   | 0x18 | @u16  | none  | get value from array                                                                                             |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**SET_ARRAY_VALUE**   | 0x19 | @u16  | none  | set value on array                                                                                               |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**ADD**               | 0x1a | none  | none  | add top and second element from stack                                                                            |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**SUB**               | 0x1b | none  | none  | subtract top and second element from stack                                                                       |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**MUL**               | 0x1c | none  | none  | multiply top and second element from stack                                                                       |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**DIV**               | 0x1d | none  | none  | divide top and second element from stack                                                                         |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**MOD**               | 0x1e | none  | none  | module from top and second element of stack                                                                      |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**OR**                | 0x1f | none  | none  | logical or from top and second element of stack                                                                  |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**AND**               | 0x20 | none  | none  | logical and from top and second element of stack                                                                 |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**LT**                | 0x21 | none  | none  | lesser. in mod jump if true                                                                                      |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**LTE**               | 0x22 | none  | none  | lesser or equal. in mod jump if true                                                                             |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**GT**                | 0x23 | none  | none  | greater. in mod jump if true                                                                                     |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**GTE**               | 0x24 | none  | none  | greater or equal. in mod jump if true                                                                            |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**INC**               | 0x25 | none  | none  | increment number (if not number do nothing). in mod jump                                                         |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**DEC**               | 0x26 | none  | none  | decrement number (if not number do nothing). in mod jump                                                         |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**EQU**               | 0x27 | none  | none  | equality. in mod jump if true                                                                                    |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**NOT**               | 0x28 | none  | none  | binary not                                                                                                       |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**SET_GLOBAL**        | 0x29 |  u32  | none  | set global variable (global 0xffffffff is indirect register)                                                     |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**GET_GLOBAL**        | 0x2a |  u32  | none  | get global variable (global 0xffffffff is indirect register)                                                     |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**GOTO**              | 0x2b | @u32  | none  | jump to pc                                                                                                       |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**GOTOZ**             | 0x2c | @u32  | none  | jump to pc if top stack is zero                                                                                  |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**CALL**              | 0x2d |   u8  |  @u32 | call function in pc with u8 stack reserved for local variables                                                   |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**RETURN**            | 0x2e | none  | none  | return from function without values                                                                              |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**RETURN_VALUE**      | 0x2f | none  | none  | return from function with value                                                                                  |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**CALL_FOREIGN**      | 0x30 |   u8  |  @u32 | call foreign function                                                                                            |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**LIB_FN**            | 0x31 |   u8  |  @u32 | call library function entry for lib_obj in stack with u8 function call type and u32 arguments                    |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**GET_LOCAL**         | 0x32 |  u32  | none  | get local variable                                                                                               |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**GET_LOCAL_FF**      | 0x33 |   u8  | none  | get local variable (first 0xff)                                                                                  |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**SET_LOCAL**         | 0x34 |  u32  | none  | set local variable                                                                                               |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**SET_LOCAL_FF**      | 0x35 |   u8  | none  | set local variable (first 0xff)                                                                                  |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**GET_RETVAL**        | 0x36 | none  | none  | push in stack value from function return                                                                         |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**TO_TYPE**           | 0x37 |   u8  | none  | convert value to new type                                                                                        |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**DROP**              | 0x38 | none  | none  | drop top of stack                                                                                                |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+
|**HALT**              | 0x39 |   u8  | none  | stop vm                                                                                                          |
+----------------------+------+-------+-------+------------------------------------------------------------------------------------------------------------------+

Instruction Description
-----------------------

.. describe:: PUSH_NULL

| *Push NULL value*
|
| ``Program: none``
| ``Stack: ( - NULL)``


.. describe:: PUSH_NULL_N

| *Pushes N null values onto the stack*
|
| ``Program: u8 = N``
| ``Stack: ( - NULL ..(n) NULL)``
 
.. describe:: PUSH_TRUE

| *Push boolean true value*
|
| ``Program: none``
| ``Stack: ( - true )``

.. describe:: PUSH_FALSE

| *Push boolean false value*
|
| ``Program: none``
| ``Stack: ( - false )``
 
.. describe:: PUSH_INT 

| *Push integer value*
|
| ``Program: i32``
| ``Stack: ( - i32 )``

.. describe:: PUSH_UINT

| *Push unsigned integer value*
|
| ``Program: none``
| ``Stack: ( - u32 )``
 
.. describe:: PUSH_0

| *Push unsigned integer 0*
|
| ``Program: none``
| ``Stack: ( - 0 )``

.. describe:: PUSH_1

| *Push unsigned integer 1*
|
| ``Program: none``
| ``Stack: ( - 1 )``

.. describe:: PUSH_CHAR

| *Push unsigned integer value of char*
|
| ``Program: u8``
| ``Stack: ( - u8 )``

.. describe:: PUSH_FLOAT

| *Push float value*
|
| ``Program: f32``
| ``Stack: ( - f32 )``

.. describe:: PUSH_CONST_UINT8

| *Push constant uint8 value from program address*
|
| ``Create a copy of a value in program area``
| ``Program: @u32``
| ``Stack: ( - u8 )``

.. describe:: PUSH_CONST_INT8

| *Push constant int8 value from program address*
|
| ``Create a copy of a value in program area``
| ``Program: @u32``
| ``Stack: ( - i8 )``

.. describe:: PUSH_CONST_UINT16

| *Push constant uint16 value from program address*
|
| ``Create a copy of a value in program area``
| ``Program: @u32``
| ``Stack: ( - u16 )``

.. describe:: PUSH_CONST_INT16

| *Push constant int16 value from program address*
|
| ``Create a copy of a value in program area``
| ``Program: @u32``
| ``Stack: ( - i16 )``

.. describe:: PUSH_CONST_UINT32

| *Push constant uint32 value from program address*
|
| ``Create a copy of a value in program area``
| ``Program: @u32``
| ``Stack: ( - u32 )``

.. describe:: PUSH_CONST_INT32

| *Push constant int32 value from program address*
|
| ``Create a copy of a value in program area``
| ``Program: @u32``
| ``Stack: ( - i32 )``

.. describe:: PUSH_CONST_FLOAT

| *Push constant float value from program address*
|
| ``Create a copy of a value in program area``
| ``Program: @u32``
| ``Stack: ( - f32 )``

.. describe:: PUSH_CONST_STRING

| *Push constant string value from program address*
|
| ``Create a static reference of a string in program area``
| ``Program: @u32``
| ``Stack: ( - cstr )``

.. describe:: NEW_LIB_OBJ

| *Create a new heap object library*
|
| ``Create a new heap object referenced to library. @ make static (will not be released by GC)``
| ``Program: none``
| ``Stack: ( lib id - )``

.. describe:: NEW_HEAP_OBJECT

| *Make a new generic heap object*
|
| ``Make a new heap object and move first element of stack to new object. @ make static (will not be released by GC)``
| ``Program: none``
| ``Stack: ( value - )``

.. describe:: PUSH_HEAP_OBJECT

| *Push a heap object*
|
| ``Program: none``
| ``Stack: ( id - )``

.. describe:: FREE_HEAP_OBJECT

| *Mark as free a heap object*
|
| ``Program: none``
| ``Stack: ( id - )``

.. describe:: NEW_ARRAY

| *Create new array in heap and fill with n elements from stack. Push heap index*
|
| ``Create a new array in Heap, fill with stack elements and return an array object``
| ``Program: u16``
| ``Stack: ( value ...(n) value - array )``

.. describe:: PUSH_ARRAY

| *Push array to stack*
|
| ``Return an array object of Heap idx index``
| ``Program: none``
| ``Stack: ( idx - array )``

.. describe:: GET_ARRAY_VALUE

| *Get value from array*
|
| ``Program: @u16``
| ``Stack: ( - value )``

.. describe:: SET_ARRAY_VALUE

| *Set value on array*
|
| ``Program: @u16``
| ``Stack: ( value - )``

.. describe:: ADD

| *Add top and second element from stack*
|
| ``Program: none``
| ``Stack: ( value value - value )``

.. describe:: SUB

| *Subtract top and second element from stack*
|
| ``Program: none``
| ``Stack: ( value value - value )``

.. describe:: MUL

| *Multiply top and second element from stack*
|
| ``Program: none``
| ``Stack: ( value value - value )``

.. describe:: DIV

| *Divide top and second element from stack*
|
| ``Program: none``
| ``Stack: ( value value - value )``

.. describe:: MOD

| *Module from top and second element of stack*
|
| ``Program: none``
| ``Stack: ( value value - value )``

.. describe:: OR

| *Logical OR from top and second element of stack*
|
| ``Program: none``
| ``Stack: ( value value - value )``

.. describe:: AND

| *Logical AND from top and second element of stack*
|
| ``Program: none``
| ``Stack: ( value value - true/false )``

.. describe:: LT

| *Lesser*
|
| ``Program: #u32``
| ``Stack: ( value value - true/false )``

.. describe:: LTE

| *Lesser or equal*
|
| ``Program: #u32``
| ``Stack: ( value value - true/false )``

.. describe:: GT

| *Greater*
|
| ``Program: #u32``
| ``Stack: ( value value - true/false )``

.. describe:: GTE

| *Greater or equal*
|
| ``Program: #u32``
| ``Stack: ( value value - true/false )``

.. describe:: INC

| *Increment*
|
| ``Program: #u32``
| ``Stack: ( value - value )``

.. describe:: DEC

| *Decrement*
|
| ``Program: #u32``
| ``Stack: ( value - value )``

.. describe:: EQU

| *Equality*
|
| ``Program: #u32``
| ``Stack: ( value value - true/false )``

.. describe:: NOT

| *Binary negation*
|
| ``Program: none``
| ``Stack: ( bool - bool )``

.. describe:: SET_GLOBAL

| *Set global variable*
|
| ``Program: u32``
| ``Stack: ( value - )``

.. describe:: GET_GLOBAL

| *Get global variable*
|
| ``Program: u32``
| ``Stack: ( - value )``

.. describe:: GOTO

| *Jump to program position*
|
| ``Program: @u32``
| ``Stack: ( - )``

.. describe:: GOTOZ

| *Jump to program position if top stack is zero*
|
| ``Program: @u32``
| ``Stack: ( value - )``

.. describe:: CALL

| *Call function in program position*
|
| ``Program: u8: reserved space for local variables, @u32: program position``
| ``Stack: ( - )``

.. describe:: RETURN

| *Return from function without values*
|
| ``Program: none``
| ``Stack: ( - )``

.. describe:: RETURN_VALUE

| *Return from function with value*
| ``Value returned is saved on ret_val register``
| ``Program: none``
| ``Stack: ( - )``

.. describe:: CALL_FOREIGN

| *Call foreign function*
|
| ``Program: u8: internal function (see FFI), @u32: Foreign function id``
| ``Stack: ( - )``

.. describe:: LIB_FN

| *Call library function*
|
| ``Program: u8: internal function (see LIBRARIES), @u32: Library id``
| ``Stack: ( - )``

.. describe:: GET_LOCAL

| *Get local variable*
|
| ``Program: u32: local index``
| ``Stack: ( - value )``

.. describe:: GET_LOCAL_FF

| *Get local variable (first 255)*
|
| ``Program: u8: local index``
| ``Stack: ( - value )``

.. describe:: SET_LOCAL

| *Set local variable*
|
| ``Program: u32: local index``
| ``Stack: ( value - )``

.. describe:: SET_LOCAL_FF

| *Set local variable (first 255)*
|
| ``Program: u8: local index``
| ``Stack: ( value - )``

.. describe:: GET_RETVAL

| *Push in stack value from function return*
|
| ``Retrieve value for return register``
| ``Program: none``
| ``Stack: ( - value )``

.. describe:: TO_TYPE

| *Convert value to new type*
|
| ``Program: u8: new type``
| ``Stack: ( value - value )``

.. describe:: DROP

| *Drop top of stack*
|
| ``Program: none``
| ``Stack: ( value - )``

.. describe:: HALT

| *Stop VM*
|
| ``Returns a value for internal uses``
| ``Program: u8``
| ``Stack: ( - )``
