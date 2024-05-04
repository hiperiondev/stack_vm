.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Data Types
==========

.. rst-class:: lead
Data types are the objects managed internally by the VM and are the only way to represent information internally.


NULL
----

| *Represents "non-data", an empty object or a result that cannot be operated on.*

BOOL
----

| *A boolean value (true or false)*

UINT
----

| *A 32 bit unsigned integer number*

INT
---

| *A 32 bits signed integer number*

FLOAT
-----

| *A 32 bits float number*

CONST_STRING
------------

| *A constant string.*
| This type can contain two types of strings:
* A referenced constant string on program area (referenced by instruction PUSH_CONST_STRING) and have is_program attribute true
* An arbitrary pointer to an external dynamically allocated string and have is_program attribute false. In this case when object is dropped the string pointer is automatically released.


ARRAY
-----
| *An array of any type of objects*
| This type ever reside in Heap.
| Can be created by instruction NEW_ARRAY and referenced by PUSH_ARRAY, GET_ARRAY_VALUE, SET_ARRAY_VALUE.
| Objects contained in an array can also be arrays 

GENERIC
-------
| *This object only can exist in Heap*
| Represent an standard data value in Heap (mostly used in FFI or libraries)

HEAP_REF
--------
| *This object represent a direct reference to an object in Heap*
| Is use internally by others object and can be used by FFI and libraries for internal uses 


LIB_OBJ
-------
| *A generic library object that resides in Heap*
| Is the main library object (see LIBRARIES)

