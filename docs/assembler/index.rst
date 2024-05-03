.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Stack VM Assembler
==================

.. rst-class:: lead

   Stack VM Assembler

========== =============================================================
Directives             Description
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
 