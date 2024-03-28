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
 * @version 0.1
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
    // [...]
    ASSMBLR_DATA,          //
    ASSMBLR_MACRO,         //
    ASSMBLR_FAIL           //
} assembler_error_t;

extern const char *assembler_error[];

          uint8_t vm_assemble_line(char *str, uint32_t *pc, uint8_t **hex);
            char* vm_assembler_load_file(char *file_name);
assembler_error_t vm_assembler(char **program, uint32_t *pc, uint8_t **hex, uint32_t *errline);

#endif /* ASSEMBLER_H_ */
