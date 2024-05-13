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
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
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
        OP(ASSBLER_INDNOTALLOWED), //
        OP(ASSMBLR_DATA),          //
        OP(ASSMBLR_MACRO),         //
        OP(ASSMBLR_FAIL),          //
};

typedef enum {
    DIRECTIVE_LABEL,      //
    DIRECTIVE_EQU,        //
    DIRECTIVE_MACRO,      //
    DIRECTIVE_ENDM,       //
    DIRECTIVE_INCLUDE,    //
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

typedef struct label_pc_s {
    uint32_t in_pc;   //
    uint32_t to_line; //
    uint32_t plus;    //
} label_pc_t;

const char *directive[14] = {
        ".LABEL",   //
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

uint32_t assembler_deep = 0;
uint32_t label_deep = 0;

static bool correct_plus(char **line) {
    if (*line == NULL)
        return false;

    char *ln = strdup(*line);
    int pos = strpos(strupr(ln), "+", 1);
    free(ln);
    if (pos != -1) {
        char *l = *line + pos - 1;
        remove_char(&l, ' ');
        return true;
    }
    return false;
}

static bool plus_value(char *str, uint32_t pc, uint32_t *value) {
    int pos = strpos(str, "+", 0);

    if (pos == -1)
        return false;

    data_t v;

    if (!to_i32(str + pos + 1, &v))
        return false;

    *value = pc + v.i32;

    return true;
}

static directive_t find_directive(char *str) {
    if (str == NULL)
        return DIRECTIVE_NONE;

    uint8_t ret = DIRECTIVE_NONE;
    char *_str = strdup(str);
    strupr(_str);

    for (uint8_t n = 0; n < 14; n++) {
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

static uint8_t find_op_directive(char *str, uint32_t *instruction, arg_type_t *arg_type, label_macro_t *label, char **macro, uint8_t *indirect) {
    if (str == NULL)
        return ASSBLER_UNKNOWNOP;

    uint8_t n = 0;
    *indirect = 0;

    if (str[0] == '@') {
        *indirect = 0x40;
        str[0] = ' ';
    } else if (str[0] == '-' && str[1] == '@') {
        *indirect = 0x80;
        str[0] = ' ';
        str[1] = ' ';
    } else if (str[0] == '+' && str[1] == '@') {
        *indirect = 0xc0;
        str[0] = ' ';
        str[1] = ' ';
    }

    trim(str);

    while (strlen(opcodes[n].opcode) != 0) {
        if (strcmp(str, opcodes[n].opcode) == 0) {
            if(*indirect != 0 && !opcodes[n].indirect)
                return ASSBLER_INDNOTALLOWED;

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

    if (*instruction == DIRECTIVE_INCLUDE)
        return ASSMBLR_INCLUDE;

    if (*instruction > 5 && *instruction < 14)
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
            if (lbl_tmp->type == LBL_LABEL || lbl_tmp->type == LBL_EQU) {
                (*value).u32 = lbl_tmp->pc_line;
                return lbl_tmp->type;
            }
        }

        lbl_tmp = lbl_tmp->next;
    }

    return LBL_NONE;
}

static bool find_value(char *str, label_macro_t *label, arg_type_t type, uint32_t pc, uint32_t orig_pc, data_t *value, label_pc_t **label_line_to_pc,
        uint32_t *label_line_to_pc_qty) {
    uint8_t lbl_type = LBL_NONE;
    uint32_t val = 0;

    // is number
    if (to_data[type](str, value))
        return true;

    // is label/equ
    if (label != NULL) {
        char *_str = strdup(str);
        int32_t pos = strpos(_str, "+", 0);
        if (pos != -1)
            _str[pos] = '\0';
        lbl_type = find_label_equ(_str, label, value);
        free(_str);

        if (lbl_type == LBL_LABEL) {
            printf(" [to line %u]", value->u32);
            (*label_line_to_pc)[*label_line_to_pc_qty].in_pc = pc;
            (*label_line_to_pc)[*label_line_to_pc_qty].to_line = value->u32;
            (*label_line_to_pc)[*label_line_to_pc_qty].plus = plus_value(str, 0, &val) ? val : 0;
            ++(*label_line_to_pc_qty);
            (*label_line_to_pc) = realloc((*label_line_to_pc), ((*label_line_to_pc_qty) + 1) * sizeof(label_pc_t));
            return true;
        } else if (lbl_type == LBL_EQU) {
            printf(" [is value %u]", value->u32);
            return true;
        }
    }

    // is here
    if (strncmp(str, "$HERE", 5) == 0)
        if (plus_value(str, orig_pc, &val)) {
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
    uint32_t lem_pc = 0;
    data_t addr;
    size_t count = 0;
    uint32_t progline = 0;

    char *line = NULL;
    char *end = NULL;
    char *start = *program;
    char **spl;
    char *line_orig;

    *label = calloc(1, sizeof(label_macro_t));
    label_macro_t *lbl = *label;

    ++label_deep;

    while ((line = next_line(start, &end)) != NULL) {
        line_orig = strdup(line);
        correct_plus(&line);
        trim(line);
        spl = strsplt(line, " ", &count);
        if (spl[0] == NULL || strlen(line) < 1)
            goto next_line;
        ++progline;
        /////////////////////////////////////////////////

        directive = find_directive(simplify_upr(spl[0]));
        valid_name = count > 1 && is_valid_name(spl[1]);

        switch (directive) {
                break;
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
                    if ((count < 3 || (!is_here && !to_u32(spl[2], &addr))) || (is_here && !plus_value(spl[2], lem_pc, &(addr.u32)))) {
                        ret = ASSMBLR_BADADDR;
                        goto error;
                    }
                    lbl->type = LBL_EQU;
                } else {
                    lbl->type = LBL_LABEL;
                }

                memset(start, 32, end - start);
                int32_t pos = strpos(spl[1], "+", 0);
                if (pos != -1)
                    spl[1][pos] = '\0';

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
                lbl->type = LBL_MACRO;
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

static bool emit(uint8_t **hex, data_t data, arg_type_t type, uint32_t **pc, uint8_t ind) {
    switch (type) {
        case ARG_U08:
            *hex = realloc(*hex, (**pc + 1) * sizeof(uint8_t));
            (*hex)[**pc] = data.u8;
            (*hex)[**pc] |= ind;
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

                emit(hex, value, arg_type[cnt], &pc, 0);

                break;

            case ARG_STR:
                if (to_data[arg_type[cnt] - 1](spl[cnt + 1], &value) == false)
                    return ASSBLER_BADDATA;

                emit(hex, value, instruction - DIRECTIVE_DATAU8 + 1, &pc, 0);
                free(value.str);

                break;

            case ARG_NVL:
                if (to_u32(spl[cnt + 1], &value) == false)
                    return ASSBLER_BADDATA;

                uint32_t args_len = value.u32;

                for (uint32_t n = 0; n < args_len; n++) {
                    if (to_data[arg_type[cnt + 1] - 1](spl[n + 2], &value) == false)
                        return ASSBLER_BADDATA;

                    emit(hex, value, arg_type[cnt + 1] + 1, &pc, 0);
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
    uint8_t indirect = 0;

    spl = strsplt(str, " ", &count);

    if (spl[0][0] == '@') {
        indirect = 0x40;
        spl[0][0] = ' ';
    } else if (spl[0][0] == '-' && spl[0][1] == '@') {
        indirect = 0x80;
        spl[0][0] = ' ';
        spl[0][1] = ' ';
    } else if (spl[0][0] == '+' && spl[0][1] == '@') {
        indirect = 0xc0;
        spl[0][0] = ' ';
        spl[0][1] = ' ';
    }

    trim(spl[0]);

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
    emit(hex, value, ARG_U08, &pc, indirect);
    uint8_t res = vm_emit_args(spl, instruction, arg_type, pc, hex, NULL, NULL, NULL);
    free(spl);
    return res;
}

static char* vm_assembler_load_fullfile(char *file_name) {
    FILE *fp;
    long lSize;
    static char *program;

    fp = fopen(file_name, "rb");
    if (!fp)
        perror(file_name), exit(1);

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    program = calloc(1, lSize + 2);
    if (!program)
        fclose(fp), fputs("memory alloc fails", stderr), exit(1);

    if (1 != fread(program, lSize, 1, fp)) {
        fclose(fp);
        free(program);
        return NULL;
    }

    fclose(fp);
    program[lSize + 1] = '\0';
    return program;
}

char* vm_assembler_load_file(char *file_name) {
    char *prg = vm_assembler_load_fullfile(file_name);
    if (prg == NULL) {
        fputs("entire read fails", stderr);
        return NULL;
    }

    char *program = calloc(1, 1);

    size_t count = 0;
    char **spl;
    char *line = NULL;
    char *line_orig = NULL;
    char *end = NULL;
    char *start = prg;

    while ((line = next_line(start, &end)) != NULL) {
        int32_t pos = strpos(line, ";", 0);
        if (pos != -1)
            line[pos] = '\0'; // delete semicolon comment

        line_orig = strdup(line);
        trim(line);
        trim(line_orig);
        spl = strsplt(line, " ", &count);

        if (spl[0] == NULL || strlen(line) < 1)
            goto next_line;

        strupr(spl[0]);

        //////////////////////////////////////////
        if (strcmp(spl[0], ".INCLUDE") == 0) {
            if (count != 2) {
                free(line_orig);
                free(line);
                free(spl);
                free(prg);
                free(program);
                fputs("ERROR: include no file name", stderr);
                return NULL;
            }

            uint32_t pos = (uint32_t) (end - prg);
            char *ins = vm_assembler_load_fullfile(spl[1]);
            if (ins == NULL) {
                free(line_orig);
                free(line);
                free(spl);
                free(prg);
                free(program);
                fputs("include file read fails", stderr);
                return NULL;
            }

            strins(&prg, "\n", pos);
            strins(&prg, ins, pos + 1);
            free(ins);

            end = prg + pos - 1;
            goto next_line;
        }
        //////////////////////////////////////////

        program = realloc(program, strlen(program) + strlen(line_orig) + 2);
        strcat(program, "\n");
        strcat(program, line_orig);

next_line:
        free(line_orig);
        free(line);
        free(spl);

        start = end + 1;
    }

    free(prg);

    printf("---------- LOAD: %s ----------\n%s\n\n---------- END LOAD ----------\n\n", file_name, program);
    return program;
}

assembler_error_t vm_assembler(char **program, uint32_t *pc, uint8_t **hex, uint32_t *errline, uint32_t *progline, label_macro_t **lbl, uint32_t *label_qty) {
    if (program == NULL)
        return ASSMBLR_FAIL;

    label_macro_t *label = *lbl;
    uint32_t instruction = 0;
    arg_type_t arg_type[8];
    uint8_t ret = ASSMBLR_OK;
    size_t count = 0;
    data_t value;
    uint8_t indirect = 0;
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

    ++assembler_deep;

    // search labels, equ and macro directives
    uint8_t res = label_equ_macro(program, &label, label_qty);
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
        correct_plus(&line);
        trim(line);
        trim(line_orig);
        spl = strsplt(line, " ", &count);

        if (spl[0] == NULL || strlen(line) < 1)
            goto next_line;

        ++(*progline);
        line_pc[line_pc_qty++] = *pc;
        line_pc = realloc(line_pc, (line_pc_qty + 1) * sizeof(uint32_t));
        line_pc[line_pc_qty] = 0;

        printf("(%04u) [%04u] %s", (*progline), *pc, line_orig);

        // find mnemonics
        res = find_op_directive(spl[0], &instruction, arg_type, label, &macro, &indirect);

        if (res == ASSBLER_INDNOTALLOWED) {
            ret = ASSBLER_INDNOTALLOWED;
            goto error;
        }

        if (res == ASSBLER_UNKNOWNOP) {
            if ((macro = find_macro(spl[0], label)) == NULL) {
                ret = ASSBLER_UNKNOWNOP;
                goto error;
            }
        }

        if (res == ASSMBLR_INCLUDE) {
            if (count != 2) {
                ret = ASSMBLR_FAIL;
                goto error;
            }

            printf("\n--start include: %s --\n", spl[1]);
            char *include_prg = vm_assembler_load_file(spl[1]);
            --(*progline);
            if ((ret = vm_assembler(&include_prg, pc, hex, errline, progline, lbl, label_qty)) != ASSMBLR_OK) {
                *errline = *pc;
                goto error;
            }
            printf("--end include --\n");

            free(include_prg);
            goto next_line;
        }

        // if is macro reinterpret
        if (res == ASSMBLR_MACRO) {
            printf("\n--start macro: %s --\n", spl[0]);
            uint32_t _pc = *pc;
            if ((ret = vm_assembler(&macro, &_pc, hex, errline, progline, lbl, label_qty)) != ASSMBLR_OK) {
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
                        emit(hex, value, instruction - DIRECTIVE_DATAU8 + 1, &pc, 0);
                    }
                    printf("\n");
                    goto next_line;
                    break;

                case DIRECTIVE_STRING:
                    if (to_data[instruction - DIRECTIVE_DATAU8](line_orig, &value) == false) {
                        ret = ASSBLER_BADDATA;
                        goto error;
                    }
                    emit(hex, value, instruction - DIRECTIVE_DATAU8 + 1, &pc, 0);
                    free(value.str);
                    value.u8 = 0;
                    emit(hex, value, ARG_U08, &pc, 0);
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
        emit(hex, value, ARG_U08, &pc, indirect);
        indirect = 0;

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
    --assembler_deep;
    if (label_to_pc_qty > 0 && assembler_deep == 0) {
        printf("-- correct labels lines to pc\n");
        for (uint32_t n = 0; n < label_to_pc_qty; n++) {

            printf("   -- in pc:%u label to line:%u is pc: %u (+%u = %u)\n",
                    label_to_pc[n].in_pc,
                    label_to_pc[n].to_line,
                    line_pc[label_to_pc[n].to_line - 1],
                    label_to_pc[n].plus,
                    line_pc[label_to_pc[n].to_line - 1] + label_to_pc[n].plus
            );

            uint32_t pc_value = line_pc[label_to_pc[n].to_line - 1] + label_to_pc[n].plus;
            (*hex)[label_to_pc[n].in_pc] = pc_value & 0xff;
            (*hex)[label_to_pc[n].in_pc + 1] = (pc_value >> 8) & 0xff;
            (*hex)[label_to_pc[n].in_pc + 2] = (pc_value >> 16) & 0xff;
            (*hex)[label_to_pc[n].in_pc + 3] = pc_value >> 24;
        }
    }
    free(line_pc);
    free(label_to_pc);
    assembler_free(&label);

    return ret;

error:
    *pc = *progline;

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
