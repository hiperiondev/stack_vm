/*
 * @vm_assembler_utils.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#include "vm_assembler_utils.h"

bool to_u8(char *val, data_t *res) {
    if (val == NULL)
        return NULL;

    if (val[0] == '-')
        return false;

    char *end;
    errno = 0;

    uint32_t sl;
    if (val[0] == '0' && val[1] == 'x')
        sl = strtoul(val, &end, 16);
    else
        sl = strtoul(val, &end, 10);

    if (end == val || '\0' != *end || ((0 == sl || UINT_MAX == sl) && ERANGE == errno) || sl > UINT_MAX || sl > 0xff)
        return false;

    (*res).u8 = sl;
    return true;
}

bool to_u16(char *val, data_t *res) {
    if (val == NULL)
        return NULL;

    if (val[0] == '-')
        return false;

    char *end;
    errno = 0;

    uint32_t sl;
    if (val[0] == '0' && val[1] == 'x')
        sl = strtoul(val, &end, 16);
    else
        sl = strtoul(val, &end, 10);

    if (end == val || '\0' != *end || ((0 == sl || UINT_MAX == sl) && ERANGE == errno) || sl > UINT_MAX || sl > 0xffff)
        return false;

    (*res).u16 = sl;
    return true;
}

bool to_u32(char *val, data_t *res) {
    if (val == NULL)
        return NULL;

    if (val[0] == '-')
        return false;

    char *end;
    errno = 0;

    uint32_t sl;
    if (val[0] == '0' && val[1] == 'x')
        sl = strtoul(val, &end, 16);
    else
        sl = strtoul(val, &end, 10);

    if (end == val || '\0' != *end || ((0 == sl || UINT_MAX == sl) && ERANGE == errno) || sl > UINT_MAX)
        return false;

    (*res).u32 = sl;
    return true;
}

bool to_i8(char *val, data_t *res) {
    if (val == NULL)
        return NULL;

    char *end;
    errno = 0;

    int32_t sl;
    if (val[0] == '0' && val[1] == 'x')
        sl = strtol(val, &end, 16);
    else
        sl = strtol(val, &end, 10);

    if (end == val || '\0' != *end || sl < -128 || sl > 127)
        return false;

    (*res).i8 = sl;
    return true;
}

bool to_i16(char *val, data_t *res) {
    if (val == NULL)
        return NULL;

    char *end;
    errno = 0;

    int32_t sl;
    if (val[0] == '0' && val[1] == 'x')
        sl = strtol(val, &end, 16);
    else
        sl = strtol(val, &end, 10);

    if (end == val || '\0' != *end || sl < -32768 || sl > 32767)
        return false;

    (*res).i16 = sl;
    return true;
}

bool to_i32(char *val, data_t *res) {
    if (val == NULL)
        return NULL;

    char *end;
    errno = 0;

    int32_t sl;
    if (val[0] == '0' && val[1] == 'x')
        sl = strtol(val, &end, 16);
    else
        sl = strtol(val, &end, 10);

    if (end == val || '\0' != *end || ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno) || sl > INT_MAX || sl < INT_MIN)
        return false;

    (*res).i32 = sl;
    return true;
}

bool to_f32(char *val, data_t *res) {
    if (val == NULL)
        return NULL;

    char *end;
    errno = 0;

    const float sl = strtof(val, &end);

    if (end == val || '\0' != *end || ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno) || sl > INT_MAX || sl < INT_MIN)
        return false;

    (*res).f32 = sl;
    return true;
}

bool to_string(char *val, data_t *res) {
    size_t count = 0;
    char **spl = NULL;

    spl = strsplt(val, "\042", &count);
    if (count < 2)
        return false;

    res->str = strdup(spl[1]);
    free(spl);
    return true;
}

bool (*to_data[8])(char*, data_t*) = {to_u8, to_u16, to_u32, to_i8, to_i16, to_i32, to_f32, to_string};

////////////////////////////////////////////////////////////////////////////////

char** strsplt(char *str, const char *delim, size_t *count) {
    if (str == NULL || delim == NULL)
        return NULL;

    size_t arrSz = 2;
    size_t arrIndex = 0;
    char **spltArr;
    char **reallocBuf;
    char *sPtr;

    if (!(spltArr = malloc(arrSz * sizeof(*spltArr))))
        return NULL;

    spltArr[arrIndex] = strtok_r(str, delim, &sPtr);

    while (spltArr[arrIndex] != NULL) {
        arrIndex++;
        if (arrIndex >= arrSz) {
            arrSz = arrSz << 1;
            if (!(reallocBuf = realloc(spltArr, (arrSz * sizeof(*spltArr))))) {
                free(spltArr);
                return NULL;
            }
            spltArr = reallocBuf;
        }
        spltArr[arrIndex] = strtok_r(NULL, delim, &sPtr);
    }
    *count = arrIndex;

    return spltArr;
}

void trim(char *str) {
    if (str == NULL)
        return;

    char *p = str;
    int l = strlen(p);
    if (l == 0)
        return;

    while (l > 0 && isspace(p[l - 1]))
        p[--l] = 0;
    while (*p && isspace(*p))
        ++p, --l;

    memmove(str, p, l + 1);
}

void simplify_white_space(char *str) {
    if (str == NULL)
        return;

    char *dst = str;

    for (; *str; ++dst, ++str) {
        *dst = *str;
        if (isspace(*str))
            while (isspace(*(str + 1)))
                ++str;
    }
    *dst = '\0';
}

char* strlwr(char *str) {
    if (str == NULL)
        return NULL;

    unsigned char *p = (unsigned char*) str;
    while (*p) {
        *p = tolower((unsigned char) *p);
        p++;
    }
    return str;
}

char* strupr(char *str) {
    if (str == NULL)
        return NULL;

    unsigned char *p = (unsigned char*) str;
    while (*p) {
        *p = toupper((unsigned char) *p);
        p++;
    }
    return str;
}

char* simplify_lwr(char *str) {
    if (str == NULL)
        return NULL;

    trim(str);
    simplify_white_space(str);
    return strlwr(str);
}

char* simplify_upr(char *str) {
    if (str == NULL)
        return NULL;

    trim(str);
    simplify_white_space(str);
    return strupr(str);
}

void remove_char(char **str, char c) {
    if (str == NULL)
        return;

    int i, j;
    int len = strlen(*str);

    for (i = 0; i < len; i++) {
        if ((*str)[i] == c) {
            for (j = i; j < len; j++) {
                (*str)[j] = (*str)[j + 1];
            }

            len--;
            i--;
        }
    }
}

int32_t strpos(char *str, char *substr, uint32_t offset) {
    if (str == NULL || substr == NULL)
        return 0;

    if (str == NULL || substr == NULL)
        return -1;
    char *found = NULL;

    if ((found = strstr(str + offset, substr)) == NULL)
        return -1;

    return found - str;
}

void strins(char **str_to, char *str_ins, size_t pos) {
    *str_to = realloc(*str_to, (strlen(*str_to) + strlen(str_ins) + 1) * sizeof(char));
    memcpy(*str_to + pos + strlen(str_ins), *str_to + pos, strlen(*str_to) - pos + 1);
    memcpy(*str_to + pos, str_ins, strlen(str_ins));
}

void remove_empty_lines(char **str) {
    if (*str == NULL)
        return;

    char *start, *end;
    start = *str;

    while((end = strchr(start, '\n')) != NULL) {
        if (end - start == 0)
            *end = ' ';
        start = end + 1;
        ++end;
    }
}

char* next_line(char *start, char **end) {
    *end = NULL;

    if (start == NULL)
        return false;

    if (*start == 0)
        return false;

    static char *line = NULL;

    *end = strchr(start, '\n');

    if (*end != NULL) {
        bool comment = false;
        char *st = start;

        while ((*end - st) > 0) {
            if (*st == ';')
                comment = true;

            if (comment) {
                *st = ' ';
            }

            st++;
        }
    }

    if (*end != NULL && (*end - start) == 0) {
        line = calloc(2, sizeof(char));
        line[0] = '\n';
    } else if (*end != NULL && (*end - start) > 0) {
        line = calloc(*end - start + 1, sizeof(char));
        memcpy(line, start, *end - start);
    } else if (strlen(start) > 0) {
        line = calloc(strlen(start) + 1, sizeof(char));
        line = memcpy(line, start, strlen(start));
        *end = start + strlen(line) - 1;
    } else
        return NULL;

    return line;
}

bool is_valid_name(char *str) {
    if (str == NULL)
        return NULL;

    if (!(str[0] == '_' || (str[0] > 96 && str[0] < 123) || (str[0] > 40 && str[0] < 91)))
        return false;

    if (!(str[0] == '_' || (str[0] > 96 && str[0] < 123) || (str[0] > 64 && str[0] < 91)))
        return false;

    for (uint32_t n = 1; n < strlen(str); n++) {
        if (!(str[n] == '_' || str[n] == '+' || (str[n] > 96 && str[n] < 123) || (str[n] > 64 && str[n] < 91) || (str[n] > 47 && str[n] < 58)))
            return false;
    }

    return true;
}

void print_hex(uint8_t *hex, uint32_t qty, uint8_t spaces) {
    uint32_t n;
    char hexstr[32];
    memset(hexstr, 0, 32);
    memset(hexstr, 32, 20);

    printf("%*s--- START HEX (length: %u) ---\n", spaces, " ", qty);
    printf("%*s                                      ------- ASCII -------\n", spaces, " ");
    printf("%*s   PC  00 01 02 03 04 05 06 07 08 09   0 1 2 3 4 5 6 7 8 9 \n", spaces, " ");
    printf("%*s  0000 ", spaces, " ");
    uint32_t str_pos = 0;
    for (n = 0; n < qty; n++) {
        printf("%02x ", hex[n]);

        hexstr[str_pos++] = isprint(hex[n]) ? hex[n] : '.';
        hexstr[str_pos++] = ' ';

        if (((n + 1) % 10) == 0) {
            printf("[ %s]\n", hexstr);
            str_pos = 0;
            if ((n + 2) <= qty)
                printf("%*s  %04d ", spaces, " ", n + 1);
            memset(hexstr, 32, 20);
        }
    }

    if (n % 10 != 0)
        printf("%*s[ %s]\n", ((10 - (qty % 10)) * 3), " ", hexstr);

    printf("%*s--- END HEX ---\n", spaces, " ");
}
