/*
 * @vm_assembler.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "vm_opcodes_def.h"
#include "vm_assembler_utils.h"
#include "vm_assembler.h"

#define OP(x) [x] = #x
const char *assembler_error[] = {
        OP(ASSMBLR_OK),            //
        OP(ASSMBLR_BADNAME),       //
        OP(ASSMBLR_INSIDEMCR),     //
        OP(ASSMBLR_DUPLICATENAME), //
        OP(ASSMBLR_BADADDR),       //
        OP(ASSMBLR_NOMACROSTART),  //
        OP(ASSBLER_UNKNOWNOP),     //
        OP(ASSBLER_BADARG),        //
        OP(ASSBLER_BADDATA),       //
        OP(ASSMBLR_DATA),          //
        OP(ASSMBLR_MACRO),         //
        OP(ASSMBLR_FAIL),          //
};

typedef enum {
    DIRECTIVE_LABEL,      //
    DIRECTIVE_ORG,        //
    DIRECTIVE_EQU,        // TODO
    DIRECTIVE_MACRO,      //
    DIRECTIVE_ENDM,       //
    DIRECTIVE_INCLUDE,    // TODO
    DIRECTIVE_COMMENT,    //
    DIRECTIVE_DATAU8,     //
    DIRECTIVE_DATAU16,    //
    DIRECTIVE_DATAU32,    //
    DIRECTIVE_DATAI8,     //
    DIRECTIVE_DATAI16,    //
    DIRECTIVE_DATAI32,    //
    DIRECTIVE_DATAF32,    //
    DIRECTIVE_STRING,     //
    // [...]
    DIRECTIVE_NONE = 0xff //
} directive_t;

enum LBL_TYPE {
    LBL_LABEL, //
    LBL_EQU,   //
    LBL_MACRO, //
    LBL_NONE   //
};

typedef struct label_macro_s label_macro_t;
typedef struct label_macro_s {
       uint8_t label_equ_macro; // 0:label, 1:equ, 2:macro
          bool label;           //
          char *name;           //
      uint32_t pc_line;         //
          char *lines;          //
       label_macro_t *next;     //
} label_macro_t;

typedef struct label_pc_s {
    uint32_t in_pc;   //
    uint32_t to_line; //
} label_pc_t;

const char *directive[15] = {
        ".LABEL",   //
        ".ORG",     //
        ".EQU",     //
        ".MACRO",   //
        ".ENDM",    //
        ".INCLUDE", //
        ".COMMENT", //
        ".DATAU8",  //
        ".DATAU16", //
        ".DATAU32", //
        ".DATAI8",  //
        ".DATAI16", //
        ".DATAI32", //
        ".DATAF32", //
        ".STRING"   //
};

static bool correct_here(char **line) {
    if (*line == NULL)
        return false;

    char *ln = strdup(*line);
    int pos = strpos(strupr(ln), "$HERE", 0);
    free(ln);
    if (pos != -1) {
        char *l = *line + pos;
        remove_char(&l, ' ');
        return true;
    }
    return false;
}

static bool here_value(char *str, uint32_t pc, uint32_t *value) {
    if (strncmp(simplify_upr(str), "$HERE", 5) != 0)
        return false;
    data_t v;

    if (!to_i32(str + 5, &v))
        return false;

    *value = pc + 5 + v.i32;
    return true;
}

static directive_t find_directive(char *str) {
    if (str == NULL)
        return DIRECTIVE_NONE;

    uint8_t ret = DIRECTIVE_NONE;
    char *_str = strdup(str);
    strupr(_str);

    for (uint8_t n = 0; n < 15; n++) {
        if (strcmp(_str, directive[n]) == 0) {
            ret = n;
            break;
        }
    }

    free(_str);
    return ret;
}

static char* find_macro(char *str, label_macro_t *label) {
    label_macro_t *lbl_tmp = label;
    while (lbl_tmp != NULL && lbl_tmp->name != NULL) {
        if (strcmp(lbl_tmp->name, str) == 0)
            return lbl_tmp->lines;

        lbl_tmp = lbl_tmp->next;
    }

    return NULL;
}

static uint8_t find_op(char *str, uint32_t *instruction, arg_type_t *arg_type, label_macro_t *label, char **macro) {
    if (str == NULL)
        return ASSBLER_UNKNOWNOP;

    uint8_t n = 0;

    while (strlen(opcodes[n].opcode) != 0) {
        if (strcmp(str, opcodes[n].opcode) == 0) {
            *instruction = n;
            arg_type[0] = opcodes[n].arg_type[0];
            arg_type[1] = opcodes[n].arg_type[1];
            arg_type[2] = opcodes[n].arg_type[2];
            arg_type[3] = opcodes[n].arg_type[3];
            arg_type[4] = opcodes[n].arg_type[4];
            arg_type[5] = opcodes[n].arg_type[5];
            arg_type[6] = opcodes[n].arg_type[6];
            arg_type[7] = opcodes[n].arg_type[7];
            return ASSMBLR_OK;
            break;
        }
        ++n;
    }

    *instruction = find_directive(str);
    if (*instruction > 6 && *instruction < 15)
        return ASSMBLR_DATA;

    *macro = find_macro(str, label);
    if(*macro != NULL)
        return ASSMBLR_MACRO;

    return ASSBLER_UNKNOWNOP;
}

static uint8_t find_label_equ(char *str, label_macro_t *label, data_t *value) {
    label_macro_t *lbl_tmp = label;
    while (lbl_tmp != NULL && lbl_tmp->name != NULL) {
        if (strcmp(lbl_tmp->name, str) == 0) {
            if(lbl_tmp->label_equ_macro == LBL_LABEL || lbl_tmp->label_equ_macro == LBL_EQU) {
                (*value).u32 = lbl_tmp->pc_line;
                return lbl_tmp->label_equ_macro;
            }
        }

        lbl_tmp = lbl_tmp->next;
    }

    return LBL_NONE;
}

static bool find_value(char *str, label_macro_t *label, arg_type_t type, uint32_t pc, uint32_t orig_pc, data_t *value, label_pc_t **label_line_to_pc,
        uint32_t *label_line_to_pc_qty) {
    uint8_t lbl_type = LBL_NONE;

    // is number
    if (to_data[type](str, value))
        return true;

    // is label/equ
    if (label != NULL) {
        lbl_type = find_label_equ(str, label, value);
        if (lbl_type == LBL_LABEL) {
            printf(" [to line %u]", value->u32);
            (*label_line_to_pc)[*label_line_to_pc_qty].in_pc = pc;
            (*label_line_to_pc)[*label_line_to_pc_qty].to_line = value->u32;
            ++(*label_line_to_pc_qty);
            (*label_line_to_pc) = realloc((*label_line_to_pc), ((*label_line_to_pc_qty) + 1) * sizeof(label_pc_t));
            return true;
        } else if (lbl_type == LBL_EQU) {
            printf(" [is value %u]", value->u32);
            return true;
        }
    }

    // is here
    uint32_t val;
    if (here_value(str, orig_pc, &val)) {
        (*value).u32 = val;
        printf(" (%u)", value->u32);
        return true;
    }

    return false;
}

static uint8_t label_equ_macro(char **program, label_macro_t **label, uint32_t *qty) {
    if (program == NULL)
        return ASSMBLR_FAIL;

    uint8_t ret = ASSMBLR_OK;
    bool is_macro = false;
    directive_t directive = DIRECTIVE_NONE;
    bool valid_name = false;
    uint32_t lem_pc = 0, progline = 0;
    data_t addr;
    size_t count = 0;
    label_macro_t *lbl = NULL;
    char *line = NULL;
    char *end = NULL;
    char *start = *program;
    char **spl;
    char *line_orig;

    *label = calloc(1, sizeof(label_macro_t));
    lbl = *label;

    while ((line = next_line(start, &end)) != NULL) {
        line_orig = strdup(line);
        correct_here(&line);
        trim(line);
        spl = strsplt(line, " ", &count);
        if (spl[0] == NULL || strlen(line) < 1)
            goto next_line;
        ++progline;
        /////////////////////////////////////////////////

        directive = find_directive(simplify_upr(spl[0]));
        valid_name = count > 1 && is_valid_name(spl[1]);

        switch (directive) {
            case DIRECTIVE_COMMENT:
                memset(start, 32, end - start);
                break;

            case DIRECTIVE_LABEL:
            case DIRECTIVE_EQU:
                if (is_macro) {
                    ret = ASSMBLR_INSIDEMCR;
                    goto error;
                }
                if (!valid_name) {
                    ret = ASSMBLR_BADNAME;
                    goto error;
                }

                label_macro_t *lbl_tmp = *label;
                while (lbl_tmp != NULL && lbl_tmp->name != NULL) {
                    if (strcmp(lbl_tmp->name, spl[1]) == 0) {
                        ret = ASSMBLR_DUPLICATENAME;
                        goto error;
                    }
                    lbl_tmp = lbl_tmp->next;
                }

                if (directive == DIRECTIVE_EQU) {
                    bool is_here = (strncmp(simplify_upr(spl[2]), "$HERE", 5) == 0);
                    if ((count < 3 || (!is_here && !to_u32(spl[2], &addr))) || (is_here && !here_value(spl[2], lem_pc, &(addr.u32)))) {
                        ret = ASSMBLR_BADADDR;
                        goto error;
                    }
                    lbl->label_equ_macro = LBL_EQU;
                } else {
                    lbl->label_equ_macro = LBL_LABEL;
                }

                memset(start, 32, end - start);
                lbl->name = strdup(spl[1]);
                lbl->pc_line = directive == DIRECTIVE_LABEL ? lem_pc + 1 : addr.u32;
                lbl->next = (label_macro_t*) calloc(1, sizeof(label_macro_t));
                lbl->next->next = NULL;
                lbl = lbl->next;

                *qty = *qty + 1;
                break;
            case DIRECTIVE_MACRO:
                if (!valid_name) {
                    ret = ASSMBLR_BADNAME;
                    goto error;
                }

                is_macro = true;
                lbl->name = strdup(spl[1]);
                lbl->lines = calloc(1, 1);
                lbl->label_equ_macro = LBL_MACRO;
                memset(start, 32, end - start);
                break;
            case DIRECTIVE_ENDM:
                if (!is_macro) {
                    ret = ASSMBLR_NOMACROSTART;
                    goto error;
                }
                memcpy(lbl->lines, lbl->lines + 1, strlen(lbl->lines));
                lbl->lines[strlen(lbl->lines) + 1] = '\n';
                lbl->next = (label_macro_t*) calloc(1, sizeof(label_macro_t) );
                lbl->next->next = NULL;
                lbl = lbl->next;
                memset(start, 32, end - start);
                is_macro = false;
                break;
            default:
                if (is_macro) {
                    uint32_t sz = strlen(lbl->lines);
                    lbl->lines[sz] = '\n';
                    lbl->lines = realloc(lbl->lines, sz + strlen(line_orig) + 2);
                    memcpy(lbl->lines + sz + 1, line_orig, strlen(line_orig) + 1);
                    memset(start, 32, end - start);
                } else {
                    ++lem_pc;
                    start = end + 1;
                }

                free(line_orig);
                line_orig = NULL;
        }

        /////////////////////////////////////////////////

next_line:
        start = end + 1;

        free(line_orig);
        free(line);
        free(spl);
    }

    remove_empty_lines(program);
    return ret;

error:
    *qty = progline;
    free(line_orig);
    free(line);
    free(spl);
    return ret;
}

static void assembler_free(label_macro_t **label) {
    if (*label == NULL)
        return;

    while (*label != NULL) {
        label_macro_t *lbl = *label;
        free((*label)->name);
        if ((*label)->lines != NULL)
            free((*label)->lines);
        if ((*label)->next == NULL) {
            free(*label);
            break;
        }
        *label = (*label)->next;
        free(lbl);
    }
}

static bool emit(uint8_t **hex, data_t data, arg_type_t type, uint32_t **pc) {
    switch (type) {
        case ARG_U08:
            *hex = realloc(*hex, (**pc + 1) * sizeof(uint8_t));
            (*hex)[**pc] = data.u8;
            **pc += 1;
            break;
        case ARG_U16:
            *hex = realloc(*hex, (**pc + 1) * sizeof(uint16_t));
            (*hex)[**pc] = data.byte[0];
            (*hex)[**pc + 1] = data.byte[1];
            **pc += 2;
            break;
        case ARG_U32:
            *hex = realloc(*hex, (**pc + 1) * sizeof(uint32_t));
            (*hex)[**pc] = data.byte[0];
            (*hex)[**pc + 1] = data.byte[1];
            (*hex)[**pc + 2] = data.byte[2];
            (*hex)[**pc + 3] = data.byte[3];
            **pc += 4;
            break;
        case ARG_I08:
            *hex = realloc(*hex, (**pc + 1) * sizeof(int8_t));
            (*hex)[**pc] = data.i8;
            **pc += 1;
            break;
        case ARG_I16:
            *hex = realloc(*hex, (**pc + 1) * sizeof(int16_t));
            (*hex)[**pc] = data.byte[0];
            (*hex)[**pc + 1] = data.byte[1];
            **pc += 2;
            break;
        case ARG_I32:
            *hex = realloc(*hex, (**pc + 1) * sizeof(int32_t));
            (*hex)[**pc] = data.byte[0];
            (*hex)[**pc + 1] = data.byte[1];
            (*hex)[**pc + 2] = data.byte[2];
            (*hex)[**pc + 3] = data.byte[3];
            **pc += 4;
            break;
        case ARG_F32:
            *hex = realloc(*hex, (**pc + 1) * sizeof(float));
            (*hex)[**pc] = data.byte[0];
            (*hex)[**pc + 1] = data.byte[1];
            (*hex)[**pc + 2] = data.byte[2];
            (*hex)[**pc + 3] = data.byte[3];
            **pc += 4;
            break;
        case ARG_STR:
            *hex = realloc(*hex, (strlen(data.str) + **pc + 1) * sizeof(uint8_t));
            memcpy((*hex + **pc), data.str, strlen(data.str) + 1);
            **pc += strlen(data.str);
            break;
        default:
            return false;
    }

    return true;
}

static uint8_t vm_emit_args(char **spl, uint8_t instruction, arg_type_t arg_type[8], uint32_t *pc, uint8_t **hex, label_macro_t *label,
        label_pc_t **label_line_to_pc, uint32_t *label_line_to_pc_qty) {
    data_t value;

    for (uint8_t cnt = 0; cnt < 8; cnt++) {
        if (arg_type[cnt] == ARG_NON)
            break;

        if (spl[cnt + 1] == NULL) {
            return ASSBLER_BADARG;
        }

        switch (arg_type[cnt]) {
            case ARG_U08:
            case ARG_U16:
            case ARG_U32:
            case ARG_I08:
            case ARG_I16:
            case ARG_I32:
            case ARG_F32:
                uint32_t _pc = *pc - 1;
                if (!find_value(spl[cnt + 1], label, arg_type[cnt] - 1, *pc, _pc, &value, label_line_to_pc, label_line_to_pc_qty))
                    return ASSBLER_BADDATA;

                emit(hex, value, arg_type[cnt], &pc);

                break;

            case ARG_STR:
                if (to_data[arg_type[cnt] - 1](spl[cnt + 1], &value) == false)
                    return ASSBLER_BADDATA;

                emit(hex, value, instruction - DIRECTIVE_DATAU8 + 1, &pc);
                free(value.str);

                break;

            case ARG_NVL:
                if (to_u32(spl[cnt + 1], &value) == false)
                    return ASSBLER_BADDATA;

                uint32_t args_len = value.u32;

                for (uint32_t n = 0; n < args_len; n++) {
                    if (to_data[arg_type[cnt + 1] - 1](spl[n + 2], &value) == false)
                        return ASSBLER_BADDATA;

                    emit(hex, value, arg_type[cnt + 1] + 1, &pc);
                }
                break;

            default:
        }
    }

    return ASSMBLR_OK;
}
////////////////////////////////////////////////////////////////////////////////

uint8_t vm_assemble_line(char *str, uint32_t *pc, uint8_t **hex) {
    if (str == NULL)
        return false;

    char **spl = NULL;
    size_t count = 0;
    uint32_t instruction = 0;
    arg_type_t arg_type[8];
    arg_type[0] = ARG_UNK;
    uint8_t n = 0;

    spl = strsplt(str, " ", &count);
    while (strlen(opcodes[n].opcode) != 0) {
        if (strcmp(spl[0], opcodes[n].opcode) == 0) {
            instruction = n;
            arg_type[0] = opcodes[n].arg_type[0];
            arg_type[1] = opcodes[n].arg_type[1];
            arg_type[2] = opcodes[n].arg_type[2];
            arg_type[3] = opcodes[n].arg_type[3];
            arg_type[4] = opcodes[n].arg_type[4];
            arg_type[5] = opcodes[n].arg_type[5];
            arg_type[6] = opcodes[n].arg_type[6];
            arg_type[7] = opcodes[n].arg_type[7];
            break;
        }
        ++n;
    }
    if (arg_type[0] == ARG_UNK) {
        free(spl);
        return false;
    }

    data_t value;
    value.u32 = instruction;
    emit(hex, value, ARG_U08, &pc);
    uint8_t res = vm_emit_args(spl, instruction, arg_type, pc, hex, NULL, NULL, NULL);
    free(spl);
    return res;
}

char* vm_assembler_load_file(char *file_name) {
    FILE *fp;
    long lSize;
    static char *program;

    fp = fopen(file_name, "rb");
    if (!fp)
        perror(file_name), exit(1);

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    program = calloc(1, lSize + 1);
    if (!program)
        fclose(fp), fputs("memory alloc fails", stderr), exit(1);

    if (1 != fread(program, lSize, 1, fp))
        fclose(fp), free(program), fputs("entire read fails", stderr), exit(1);

    fclose(fp);

    return program;
}

assembler_error_t vm_assembler(char **program, uint32_t *pc, uint8_t **hex, uint32_t *errline) {
    if (program == NULL)
        return ASSMBLR_FAIL;

    uint32_t instruction = 0;
    arg_type_t arg_type[8];
    label_macro_t *label = NULL;
    uint32_t label_qty = 0;
    uint8_t ret = ASSMBLR_OK;
    uint32_t progline = 0;
    size_t count = 0;
    data_t value;
    char *line = NULL;
    char *end = NULL;
    char *start = *program;
    char **spl;
    char *line_orig;
    char *macro;
    label_pc_t *label_to_pc = calloc(1, sizeof(label_pc_t));
    uint32_t label_to_pc_qty = 0;
    uint32_t *line_pc = calloc(1, sizeof(uint32_t));
    uint32_t line_pc_qty = 0;

    // search labels, equ and macro directives
    uint8_t res = label_equ_macro(program, &label, &label_qty);
    if (res != ASSMBLR_OK) {
        ret = res;
        assembler_free(&label);
        goto end;
    }

    if (*hex == NULL)
        *hex = calloc(1, sizeof(uint8_t));

    // loop all lines
    while ((line = next_line(start, &end)) != NULL) {
        line_orig = strdup(line);
        correct_here(&line);
        trim(line);
        trim(line_orig);
        spl = strsplt(line, " ", &count);

        if (spl[0] == NULL || strlen(line) < 1)
            goto next_line;

        ++progline;
        line_pc[line_pc_qty++] = *pc;
        line_pc = realloc(line_pc, (line_pc_qty + 1) * sizeof(uint32_t));
        line_pc[line_pc_qty] = 0;

        printf("(%04u) [%04u] %s", progline, *pc, line_orig);

        // find mnemonics
        res = find_op(spl[0], &instruction, arg_type, label, &macro);
        if (res == ASSBLER_UNKNOWNOP) {
            if ((macro = find_macro(spl[0], label)) == NULL) {
                ret = ASSBLER_UNKNOWNOP;
                goto error;
            }
        }

        // if is macro reinterpret
        if (res == ASSMBLR_MACRO) {
            printf("\n--start macro: %s --\n", spl[0]);
            uint32_t _pc = *pc;
            if ((ret = vm_assembler(&macro, &_pc, hex, errline)) != ASSMBLR_OK) {
                *errline = _pc;
                goto error;
            }
            *pc = _pc;
            printf("--end macro --\n");
            goto next_line;
        }

        // if is data directive, emit
        if (res == ASSMBLR_DATA) {
            switch (instruction) {
                case DIRECTIVE_DATAU8:
                case DIRECTIVE_DATAU16:
                case DIRECTIVE_DATAU32:
                case DIRECTIVE_DATAI8:
                case DIRECTIVE_DATAI16:
                case DIRECTIVE_DATAI32:
                case DIRECTIVE_DATAF32:
                    for (uint32_t n = 1; n < count; n++) {
                        if (to_data[instruction - DIRECTIVE_DATAU8](spl[n], &value) == false) {
                            ret = ASSBLER_BADDATA;
                            goto error;
                        }
                        emit(hex, value, instruction - DIRECTIVE_DATAU8 + 1, &pc);
                    }
                    printf("\n");
                    goto next_line;
                    break;

                case DIRECTIVE_STRING:
                    if (to_data[instruction - DIRECTIVE_DATAU8](line_orig, &value) == false) {
                        ret = ASSBLER_BADDATA;
                        goto error;
                    }
                    emit(hex, value, instruction - DIRECTIVE_DATAU8 + 1, &pc);
                    free(value.str);
                    printf("\n");
                    goto next_line;
                    break;

                default:
                    ret = ASSBLER_BADDATA;
                    printf("\n");
                    goto error;
            }
        }

        // if is instruction parse arguments and emit
        value.u32 = instruction;
        emit(hex, value, ARG_U08, &pc);

        ret = vm_emit_args(spl, instruction, arg_type, pc, hex, label, &label_to_pc, &label_to_pc_qty);
        if (ret != ASSMBLR_OK)
            goto error;


        /////////////////////////////////////////////////
        printf("\n");

next_line:
        start = end + 1;
        free(line_orig);
        free(line);
        free(spl);
    }

end:
    if (label_to_pc_qty > 0) {
        printf("-- correct labels lines to pc\n");
        for (uint32_t n = 0; n < label_to_pc_qty; n++) {
            printf("   -- in pc:%u label to line:%u is pc: %u\n", label_to_pc[n].in_pc, label_to_pc[n].to_line, line_pc[label_to_pc[n].to_line - 1]);
            (*hex)[label_to_pc[n].in_pc] = line_pc[label_to_pc[n].to_line - 1] & 0xff;
            (*hex)[label_to_pc[n].in_pc + 1] = (line_pc[label_to_pc[n].to_line - 1] >> 8) & 0xff;
            (*hex)[label_to_pc[n].in_pc + 2] = (line_pc[label_to_pc[n].to_line - 1] >> 16) & 0xff;
            (*hex)[label_to_pc[n].in_pc + 3] = line_pc[label_to_pc[n].to_line - 1] >> 24;
        }
    }
    free(line_pc);
    free(label_to_pc);
    assembler_free(&label);

    return ret;

error:
    *pc = progline;

    assembler_free(&label);
    free(line_orig);
    free(line);
    free(spl);
    free(label_to_pc);
    free(line_pc);

    if (*errline == 0)
        printf(" <<<<<< [ ERROR!! (%u) %s ]\n", ret, assembler_error[ret]);

    return ret;
}
