.. meta::
   :description: Generic Stack VM for Scripting Languages.
   :twitter:description: Generic Stack VM for Scripting Languages.

Disassembler
============

.. rst-class:: lead

Disassembler try to show assembler code from program data

------
 
| It is a simple disassembler that try to retrieve original code from the start of program.
| Arbitrary data inserted on code will be interpreted as real code and can break the result.
| It's rely on vm_opcodes_def.c for interpret bytecodes.

   