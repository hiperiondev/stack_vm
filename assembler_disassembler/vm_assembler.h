/*
 * @vm_assembler.h
 *
 * @brief Stack VM
 * @details
 * This is based on other projects:
 *   Tiny language: https://github.com/goodpaul6/Tiny
 *   Others (see individual files)
 *
 *   please contact their authors for more information.
 *
 * @author Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * @version 2.0
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <stdint.h>

typedef enum ASSEMBLER_ERROR{
    ASSMBLR_OK,            //
    ASSMBLR_BADNAME,       //
    ASSMBLR_INSIDEMCR,     //
    ASSMBLR_DUPLICATENAME, //
    ASSMBLR_BADADDR,       //
    ASSMBLR_NOMACROSTART,  //
    ASSBLER_UNKNOWNOP,     //
    ASSBLER_BADARG,        //
    ASSBLER_BADDATA,       //
    ASSBLER_INDNOTALLOWED, //
    // [...]
    ASSMBLR_DATA,          //
    ASSMBLR_MACRO,         //
    ASSMBLR_INCLUDE,       //
    ASSMBLR_FAIL           //
} assembler_error_t;

typedef struct label_macro_s label_macro_t;
typedef struct label_macro_s {
       uint8_t type;        // 0:label, 1:equ, 2:macro, 3: include
          bool label;       //
          char *name;       //
      uint32_t pc_line;     //
          char *lines;      //
       label_macro_t *next; //
} label_macro_t;

extern const char *assembler_error[];

          uint8_t vm_assemble_line(char *str, uint32_t *pc, uint8_t **hex);
            char* vm_assembler_load_file(char *file_name);
assembler_error_t vm_assembler(char **program, uint32_t *pc, uint8_t **hex, uint32_t *errline, uint32_t *progline, label_macro_t **lbl, uint32_t *label_qty);

#endif /* ASSEMBLER_H_ */
