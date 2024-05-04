.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Assembler
=========

.. rst-class:: lead

Assembler create a real code from mnemonic representation of code

------

| Is a very complete assembler with a lot of directives (see DIRECTVES table)
| Instructions can be extended easily on vm_opcodes_def.c
| For that you can edit one of the not use instructions (Usually after the HALT instruction)
| Up to 8 arguments can be specified. The assembler stop to search after the first ARG_NON in the list
| When read the program it's ensure that the data type is correct and emit the code into the program
| The possible arguments types are:

* ARG_NON: Non arguments. All arguments defined after this are ignored.
* ARG_U08: Unsigned 8 bits number.
* ARG_U16: Unsigned 16 bits number.
* ARG_U32: Unsigned 32 bits number.
* ARG_I08: Signed 8 bits number.
* ARG_I16: Signed 16 bits number.
* ARG_I32: Signed 32 bits number.
* ARG_F32: Float 32 bits number.
* ARG_STR: String
* ARG_NVL: Have n values from previous argument, next argument is type. This type is not actually supported by the VM but exist for future expansions

| 
| *In program all numbers can be expressed as decimal or hexadecimal*
 

Directives
----------

========== =============================================================
Directive  Description
========== =============================================================
    .equ   Assigns a value to a label.
  .macro   Start of a macro. Takes the name as parameter.
   .endm   End of a macro definition.
.include   Start reading from a specified file.
  .label   Assign the address at this point to label.
.comment   Comment line.
 .datau8   Insert a list of unsigned 8 bits values.
.datau16   Insert a list of unsigned 16 bits values.
.datau32   Insert a list of unsigned 32 bits values.
 .datai8   Insert a list of signed 8 bits values.
.datai16   Insert a list of signed 16 bits values.
.datai32   Insert a list of signed 32 bits values.
.dataf32   Insert a list of float 32 bits values.
 .string   Insert string text.
========== =============================================================

*NOTE 1*: All content after ; is considered comment.

*NOTE 2*: Value $HERE represent actual address pointer, can be offsetted ex: .equ label $HERE + 3

*NOTE 3*: Labels can be offsetted when referenced ex: GOTO label + 5
 