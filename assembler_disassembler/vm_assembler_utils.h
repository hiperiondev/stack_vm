/*
 * @vm_assembler_utils.h
 *
 * @brief Stack VM
 * @details
 * This is based on other projects:
 *   Tiny language: https://github.com/goodpaul6/Tiny
 *   Others (see individual files)
 *
 *   please contact their authors for more information.
 *
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#ifndef ASSEMBLER_UTILS_H_
#define ASSEMBLER_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

typedef union {
     uint8_t u8;      //
    uint16_t u16;     //
    uint32_t u32;     //
      int8_t i8;      //
     int16_t i16;     //
     int32_t i32;     //
       float f32;     //
        char *str;    //
     uint8_t byte[4]; //
} data_t;

extern bool (*to_data[8])(char*, data_t*);

     bool to_u8(char *val, data_t *res);
     bool to_u16(char *val, data_t *res);
     bool to_u32(char *val, data_t *res);
     bool to_i8(char *val, data_t *res);
     bool to_i16(char *val, data_t *res);
     bool to_i32(char *val, data_t *res);
     bool to_f32(char *val, data_t *res);
   char** strsplt(char *str, const char *delim, size_t *count);
     void trim(char *s);
     void simplify_white_space(char *src);
    char* strlwr(char *str);
    char* strupr(char *str);
    char* simplify_lwr(char *str);
    char* simplify_upr(char *str);
     void remove_char(char **str, char c);
     void remove_empty_lines(char **str);
  int32_t strpos(char *str, char *substr, uint32_t offset);
     void strins(char **str_to, char *str_ins, size_t pos);
    char* next_line(char *start, char **end);
     bool is_valid_name(char *str);

     void print_hex(uint8_t *hex, uint32_t qty, uint8_t spaces);

#endif /* ASSEMBLER_UTILS_H_ */
