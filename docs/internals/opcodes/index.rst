.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Instructions
============
   
**op format: iiOOOOOO**

* i: use indirect register in instruction

  * 00: nothing
  * 01: indirect, not change register
  * 10: indirect, increment register
  * 11: indirect, decrement register
  
* O: opcode

*NOTE: @arg -> support indirect*

+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|     instruction      |  id  |  arg1 |  arg2  | description                                                                                   |
+======================+======+=======+========+===============================================================================================+
|**PUSH_NULL**         | 0x00 | none  | none   | push NULL value                                                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_NULL_N**       | 0x01 |   u8  | none   | pushes N null values onto the stack                                                           |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_NEW_HEAP_OBJ** | 0x02 |  u32  | none   | create a new heap object referenced to library                                                |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_TRUE**         | 0x03 | none  | none   | push boolean true value                                                                       |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_FALSE**        | 0x04 | none  | none   | push boolean false value                                                                      |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_INT**          | 0x05 |  i32  | none   | push integer value                                                                            |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_UINT**         | 0x06 |  u32  | none   | push unsigned integer value                                                                   |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_0**            | 0x07 | none  | none   | push unsigned integer 0                                                                       |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_1**            | 0x08 | none  | none   | push unsigned integer 1                                                                       |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_CHAR**         | 0x09 |   u8  | none   | push unsigned integer value of char                                                           |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_FLOAT**        | 0x0a |  f32  | none   | push float value                                                                              |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_CONST_UINT8**  | 0x0b | @u32  | none   | push constant uint8 value from program address                                                |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_CONST_INT8**   | 0x0c | @u32  | none   | push constant int8 value from program address                                                 |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_CONST_UINT16** | 0x0d | @u32  | none   | push constant uint16 value from program address                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_CONST_INT16**  | 0x0e | @u32  | none   | push constant int16 value from program address                                                |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_CONST_UINT32** | 0x0f | @u32  | none   | push constant uint32 value from program address                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_CONST_INT32**  | 0x10 | @u32  | none   | push constant int32 value from program address                                                |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_CONST_FLOAT**  | 0x11 | @u32  | none   | push constant float value from program address                                                |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_CONST_STRING** | 0x12 | @u32  | none   | push constant string value from program address                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_HEAP_OBJECT**  | 0x13 |  u32  | none   | push a heap object                                                                            |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**NEW_ARRAY**         | 0x14 |  u16  | none   | create new array in heap and fill with n elements from stack. Push heap index                 |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**PUSH_ARRAY**        | 0x15 | none  | none   | push array to stack                                                                           |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**GET_ARRAY_VALUE**   | 0x16 | @u16  | none   | get value from array                                                                          |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**SET_ARRAY_VALUE**   | 0x17 | @u16  | none   | set value on array                                                                            |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**ADD**               | 0x18 | none  | none   | add top and second element from stack                                                         |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**SUB**               | 0x19 | none  | none   | subtract top and second element from stack                                                    |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**MUL**               | 0x1a | none  | none   | multiply top and second element from stack                                                    |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**DIV**               | 0x1b | none  | none   | divide top and second element from stack                                                      |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**MOD**               | 0x1c | none  | none   | module from top and second element of stack                                                   |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**OR**                | 0x1d | none  | none   | logical or from top and second element of stack                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**AND**               | 0x1e | none  | none   | logical and from top and second element of stack                                              |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+ 
|**LT**                | 0x1f | none  | none   | lesser                                                                                        |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**LTE**               | 0x20 | none  | none   | lesser or equal                                                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**GT**                | 0x21 | none  | none   | greater                                                                                       |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**GTE**               | 0x22 | none  | none   | greater or equal                                                                              |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**INC**               | 0x23 | none  | none   | increment number (if not number do nothing)                                                   |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**DEC**               | 0x24 | none  | none   | decrement number (if not number do nothing)                                                   |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**EQU**               | 0x25 | none  | none   | equality                                                                                      |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**NOT**               | 0x26 | none  | none   | binary not                                                                                    |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**SET_GLOBAL**        | 0x27 |  u32  | none   | set global variable (global 0xffffffff is indirect register)                                  |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**GET_GLOBAL**        | 0x28 |  u32  | none   | get global variable (global 0xffffffff is indirect register)                                  |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**GOTO**              | 0x29 | @u32  | none   | jump to pc                                                                                    |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**GOTOZ**             | 0x2a | @u32  | none   | jump to pc if top stack is zero                                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**CALL**              | 0x2b |   u8  |  @u32  | call function in pc with u8 stack reserved for local variables                                |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**RETURN**            | 0x2c | none  | none   | return from function without values                                                           |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**RETURN_VALUE**      | 0x2d | none  | none   | return from function with value                                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**CALL_FOREIGN**      | 0x2e |   u8  |  @u32  | call foreign function                                                                         |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**LIB_FN**            | 0x2f |   u8  |  @u32  | call library function entry for lib_obj in stack with u8 function call type and u32 arguments |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**GET_LOCAL**         | 0x30 |  u32  | none   | get local variable                                                                            |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+ 
|**GET_LOCAL_FF**      | 0x31 |   u8  | none   | get local variable (first 0xff)                                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**SET_LOCAL**         | 0x32 |  u32  | none   | set local variable                                                                            |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**SET_LOCAL_FF**      | 0x33 |   u8  | none   | set local variable (first 0xff)                                                               |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**GET_RETVAL**        | 0x34 | none  | none   | push in stack value from function return                                                      |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**TO_TYPE**           | 0x35 |   u8  | none   | convert value to new type                                                                     |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**DROP**              | 0x36 | none  | none   | drop top of stack                                                                             |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+
|**HALT**              | 0x37 |   u8  | none   | stop vm                                                                                       |
+----------------------+------+-------+--------+-----------------------------------------------------------------------------------------------+

Instruction Description
-----------------------

.. describe:: PUSH_NULL  

| *Push NULL value*
|
| Program: no parameters
| Stack: ``( - NULL)``


.. describe:: PUSH_NULL_N

| *Pushes N null values onto the stack*
|
| Program: u8 = N
| Stack: ``( - NULL ..(n) NULL)``
 
.. describe:: PUSH_NEW_HEAP_OBJ

| *Create a new heap object referenced to library*
|
| Program: 
| Stack: ``()``
 
.. describe:: PUSH_TRUE

| *Push boolean true value*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_FALSE

| *Push boolean false value*
|
| Program: 
| Stack: ``()``
 
.. describe:: PUSH_INT

| *Push integer value*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_UINT

| *Push unsigned integer value*
|
| Program: 
| Stack: ``()``
 
.. describe:: PUSH_0

| *Push unsigned integer 0*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_1

| *Push unsigned integer 1*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_CHAR

| *Push unsigned integer value of char*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_FLOAT

| *Push float value*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_CONST_UINT8

| *Push constant uint8 value from program address*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_CONST_INT8

| *Push constant int8 value from program address*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_CONST_UINT16

| *Push constant uint16 value from program address*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_CONST_INT16

| *Push constant int16 value from program address*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_CONST_UINT32

| *Push constant uint32 value from program address*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_CONST_INT32

| *Push constant int32 value from program address*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_CONST_FLOAT

| *Push constant float value from program address*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_CONST_STRING

| *Push constant string value from program address*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_HEAP_OBJECT

| *Push a heap object*
|
| Program: 
| Stack: ``()``

.. describe:: NEW_ARRAY

| *Create new array in heap and fill with n elements from stack. Push heap index*
|
| Program: 
| Stack: ``()``

.. describe:: PUSH_ARRAY

| *Push array to stack*
|
| Program: 
| Stack: ``()``

.. describe:: GET_ARRAY_VALUE

| *Get value from array*
|
| Program: 
| Stack: ``()``

.. describe:: SET_ARRAY_VALUE

| *Set value on array*
|
| Program: 
| Stack: ``()``

.. describe:: ADD

| *Add top and second element from stack*
|
| Program: 
| Stack: ``()``

.. describe:: SUB

| *Subtract top and second element from stack*
|
| Program: 
| Stack: ``()``

.. describe:: MUL

| *Multiply top and second element from stack*
|
| Program: 
| Stack: ``()``

.. describe:: DIV

| *Divide top and second element from stack*
|
| Program: 
| Stack: ``()``

.. describe:: MOD

| *Module from top and second element of stack*
|
| Program: 
| Stack: ``()``

.. describe:: OR

| *Logical OR from top and second element of stack*
|
| Program: 
| Stack: ``()``

.. describe:: AND

| *Logical AND from top and second element of stack*
|
| Program: 
| Stack: ``()``

.. describe:: LT

| *Lesser*
|
| Program: 
| Stack: ``()``

.. describe:: LTE

| *Lesser or equal*
|
| Program: 
| Stack: ``()``

.. describe:: GT

| *Greater*
|
| Program: 
| Stack: ``()``

.. describe:: GTE

| *Greater or equal*
|
| Program: 
| Stack: ``()``

.. describe:: INC

| *Increment*
|
| Program: 
| Stack: ``()``

.. describe:: DEC

| *Decrement*
|
| Program: 
| Stack: ``()``

.. describe:: EQU

| *Equality*
|
| Program: 
| Stack: ``()``

.. describe:: NOT

| *Binary negation*
|
| Program: 
| Stack: ``()``

.. describe:: SET_GLOBAL

| *Set global variable*
|
| Program: 
| Stack: ``()``

.. describe:: GET_GLOBAL

| *Get global variable*
|
| Program: 
| Stack: ``()``

.. describe:: GOTO

| *Jump*
|
| Program: 
| Stack: ``()``

.. describe:: GOTOZ

| *Jump if top stack is zero*
|
| Program: 
| Stack: ``()``

.. describe:: CALL

| *Call function*
|
| Program: 
| Stack: ``()``

.. describe:: RETURN

| *Return from function without values*
|
| Program: 
| Stack: ``()``

.. describe:: RETURN_VALUE

| *Return from function with value*
|
| Program: 
| Stack: ``()``

.. describe:: CALL_FOREIGN

| *Call foreign function*
|
| Program: 
| Stack: ``()``

.. describe:: LIB_FN

| *Call library function*
|
| Program: 
| Stack: ``()``

.. describe:: GET_LOCAL

| *Get local variable*
|
| Program: 
| Stack: ``()``

.. describe:: GET_LOCAL_FF

| *Get local variable (first 255)*
|
| Program: 
| Stack: ``()``

.. describe:: SET_LOCAL

| *Set local variable*
|
| Program: 
| Stack: ``()``

.. describe:: SET_LOCAL_FF

| *Set local variable (first 255)*
|
| Program: 
| Stack: ``()``

.. describe:: GET_RETVAL

| *Push in stack value from function return*
|
| Program: 
| Stack: ``()``

.. describe:: TO_TYPE

| *Convert value to new type*
|
| Program: 
| Stack: ``()``

.. describe:: DROP

| *Drop top of stack*
|
| Program: 
| Stack: ``()``

.. describe:: HALT

| *Stop VM*
|
| Program: 
| Stack: ``()``
