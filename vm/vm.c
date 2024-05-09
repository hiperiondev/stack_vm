/*
 * @vm.c
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
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "vm.h"
#include "vm_opcodes.h"

// values
static inline bool vm_are_values_equal(vm_thread_t **thread, vm_value_t a, vm_value_t b) {
    bool both_strings = (a.type == VM_VAL_CONST_STRING && b.type == VM_VAL_CONST_STRING);

    if (a.type != b.type && !both_strings) {
        return false;
    }

    if (a.type == VM_VAL_NULL) {
        return true;
    }

    if (a.type == VM_VAL_BOOL) {
        return a.number.boolean == b.number.boolean;
    }

    if (a.type == VM_VAL_INT) {
        return a.number.integer == b.number.integer;
    }

    if (a.type == VM_VAL_UINT) {
        return a.number.uinteger == b.number.uinteger;
    }

    if (a.type == VM_VAL_FLOAT) {
        return a.number.real == b.number.real;
    }

    if (a.type == VM_VAL_LIB_OBJ) {
        if ((*thread)->state->lib[a.lib_obj.lib_idx](thread, VM_EDFAT_CMP, a.lib_obj.lib_idx, 2) != VM_ERR_OK)
            return false;
        return true;
    }

    if (a.type == VM_VAL_ARRAY) {
        vm_heap_object_t *a1 = vm_heap_load((*thread)->state->heap, a.heap_ref);
        vm_heap_object_t *a2 = vm_heap_load((*thread)->state->heap, b.heap_ref);
        return a1->array.fields == a2->array.fields;
    }

    return false;
}

// data program
uint8_t vm_read_byte(vm_thread_t **thread, uint32_t *pc) {
    return (*thread)->state->program[(*pc)++];
}

int16_t vm_read_i16(vm_thread_t **thread, uint32_t *pc) {
    vm_value_t val;
    val.number.byte[0] = (*thread)->state->program[*pc];
    val.number.byte[1] = (*thread)->state->program[*pc + 1];
    *pc += 2;

    return val.number.integer;
}

uint16_t vm_read_u16(vm_thread_t **thread, uint32_t *pc) {
    vm_value_t val;
    val.number.byte[0] = (*thread)->state->program[*pc];
    val.number.byte[1] = (*thread)->state->program[*pc + 1];
    *pc += 2;

    return val.number.uinteger;
}

int32_t vm_read_i32(vm_thread_t **thread, uint32_t *pc) {
    vm_value_t val;
    val.number.byte[0] = (*thread)->state->program[*pc];
    val.number.byte[1] = (*thread)->state->program[*pc + 1];
    val.number.byte[2] = (*thread)->state->program[*pc + 2];
    val.number.byte[3] = (*thread)->state->program[*pc + 3];
    *pc += 4;

    return val.number.integer;
}

uint32_t vm_read_u32(vm_thread_t **thread, uint32_t *pc) {
    vm_value_t val;
    val.number.byte[0] = (uint32_t) (*thread)->state->program[*pc];
    val.number.byte[1] = (uint32_t) (*thread)->state->program[*pc + 1];
    val.number.byte[2] = (uint32_t) (*thread)->state->program[*pc + 2];
    val.number.byte[3] = (uint32_t) (*thread)->state->program[*pc + 3];
    *pc += 4;

    return val.number.integer;
}

float vm_read_f32(vm_thread_t **thread, uint32_t *pc) {
    vm_value_t val;
    val.number.byte[0] = (*thread)->state->program[*pc];
    val.number.byte[1] = (*thread)->state->program[*pc + 1];
    val.number.byte[2] = (*thread)->state->program[*pc + 2];
    val.number.byte[3] = (*thread)->state->program[*pc + 3];
    *pc += 4;

    return val.number.real;
}

// stack
void vm_do_push(vm_thread_t **thread, vm_value_t value) {
    STK_OBJ(thread, (*thread)->sp++)= value;
}

vm_value_t vm_do_pop(vm_thread_t **thread) {
    return STK_OBJ(thread, --(*thread)->sp);
}

vm_errors_t vm_do_drop_n(vm_thread_t **thread, uint32_t qty) {
    if (qty < 1 || qty - 1 > (*thread)->sp) {
        return VM_ERR_BAD_VALUE;
    }

    for (uint32_t n = 0; n < qty; n++) {
        if ((*thread)->stack[n].type == VM_VAL_CONST_STRING && (*thread)->stack[n].cstr.is_program == false)
            free((*thread)->stack[n].cstr.addr);
    }

    (*thread)->sp -= qty;

    return VM_ERR_OK;
}

void vm_do_push_frame(vm_thread_t **thread, uint8_t locals) {
    (*thread)->frames[(*thread)->fc].pc = (*thread)->pc;
    (*thread)->frames[(*thread)->fc].fp = (*thread)->fp;
    (*thread)->frames[(*thread)->fc].locals = locals;
    ++(*thread)->fc;
    (*thread)->fp = (*thread)->sp;

    (*thread)->frames[(*thread)->fc].gc_mark = vm_heap_new_gc_mark((*thread)->state->heap);
}

void vm_do_pop_frame(vm_thread_t **thread) {
    vm_heap_gc_collect((*thread)->state->heap, &((*thread)->frames[(*thread)->fc].gc_mark), true, thread);
#ifdef VM_HEAP_SHRINK_AFTER_GC
    vm_heap_shrink((*thread)->state->heap);
#endif

    (*thread)->sp = (*thread)->fp;
    vm_frame_t frame = (*thread)->frames[--(*thread)->fc];
    (*thread)->sp -= frame.locals;
    (*thread)->fp = frame.fp;
    (*thread)->pc = frame.pc;
}

// vm
void vm_step(vm_thread_t **thread) {
    if ((*thread) == NULL)
        return;

    if ((*thread)->state == NULL || (*thread)->halted == true) {
        (*thread)->status = VM_ERR_FAIL;
        return;
    }

    if ((*thread)->pc > (*thread)->state->program_len - 1) {
        (*thread)->status = VM_ERR_PRG_END;
        (*thread)->halted = true;
        return;
    }

    vm_errors_t err = VM_ERR_OK;
    vm_state_t *state = (*thread)->state;
    bool indirect = false;
    int8_t ind_inc = 0;

    switch (OP_INDIRECT(state->program[(*thread)->pc])) {
        case 0x40:
            indirect = true;
            break;
        case 0x80:
            indirect = true;
            ind_inc = -1;
            break;
        case 0xc0:
            indirect = true;
            ind_inc = 1;
            break;
    }

    state->program[(*thread)->pc] &= 0x3f;

    switch (state->program[(*thread)->pc++]) {
        case PUSH_NULL:
        case PUSH_NULL_N: {
            uint8_t n = (state->program[(*thread)->pc - 1] == PUSH_NULL) ? 1 : (*thread)->state->program[(*thread)->pc++];

            memset(&STK_NEW(thread), 0, sizeof(vm_value_t) * n);
            (*thread)->sp += n;
        }
            break;

        case PUSH_NEW_HEAP_OBJ: {
            vm_value_t lib_idx = vm_do_pop(thread);
            if (lib_idx.type != VM_VAL_UINT || lib_idx.number.uinteger > (*thread)->state->lib_qty)
                err = VM_ERR_BAD_VALUE;

            vm_value_t ref = { .type = VM_VAL_LIB_OBJ };
            vm_heap_object_t obj = { .type = VM_VAL_LIB_OBJ, .lib_obj.identifier = 0, .lib_obj.addr = NULL, .lib_obj.lib_idx = lib_idx.number.uinteger,

            };
            uint32_t heap_ref = vm_heap_save((*thread)->state->heap, obj, &((*thread)->frames[(*thread)->fc].gc_mark));
            ref.lib_obj.heap_ref = heap_ref;
            vm_do_push(thread, ref);
            (*thread)->state->lib[lib_idx.number.uinteger](thread, VM_EDFAT_NEW, lib_idx.number.uinteger, heap_ref);
        }
            break;

        case PUSH_HEAP_OBJECT: {
            vm_value_t value = STK_TOP(thread);

            if (value.type != VM_VAL_UINT)
                err = VM_ERR_BAD_VALUE;
            else {
                uint32_t idx = value.number.uinteger;
                vm_heap_object_t *obj = vm_heap_load((*thread)->state->heap, idx);
                if (obj->type == VM_VAL_NULL)
                    err = VM_ERR_HEAPNOTEXIST;
                else {
                    switch (obj->type) {
                        case VM_VAL_GENERIC: {
                            STK_TOP(thread)= obj->value;
                        }
                        break;
                        case VM_VAL_LIB_OBJ: {
                            value.type = VM_VAL_LIB_OBJ;
                            value.lib_obj.heap_ref = idx;
                            value.lib_obj.lib_idx = obj->lib_obj.lib_idx;
                            STK_TOP(thread) = value;
                            err = (*thread)->state->lib[obj->lib_obj.lib_idx](thread, VM_EDFAT_PUSH, obj->lib_obj.lib_idx, idx);
                        }
                        break;
                        default:
                        break;
                    }
                }
            }
        }
        break;

        case PUSH_TRUE: {
            vm_new_bool(val, true);
            vm_do_push(thread, val);
        }
        break;

        case PUSH_FALSE: {
            vm_new_bool(val, false);
            vm_do_push(thread, val);
        }
        break;

        case PUSH_INT: {
            STK_NEW(thread).type = VM_VAL_INT;
            STK_NEW(thread).number.integer = vm_read_i32(thread, &(*thread)->pc);
            (*thread)->sp += 1;
        }
        break;

        case PUSH_UINT: {
            STK_NEW(thread).type = VM_VAL_UINT;
            STK_NEW(thread).number.integer = vm_read_u32(thread, &(*thread)->pc);
            (*thread)->sp += 1;
        }
        break;

        case PUSH_0: {
            STK_NEW(thread).type = VM_VAL_UINT;
            STK_NEW(thread).number.integer = 0;
            (*thread)->sp += 1;
        }
        break;

        case PUSH_1: {
            STK_NEW(thread).type = VM_VAL_UINT;
            STK_NEW(thread).number.integer = 1;
            (*thread)->sp += 1;
        }
        break;

        case PUSH_CHAR: {
            STK_NEW(thread).type = VM_VAL_UINT;
            STK_NEW(thread).number.integer = (*thread)->state->program[(*thread)->pc];
            (*thread)->sp += 1;
            (*thread)->pc += 1;
        }
        break;

        case PUSH_FLOAT: {
            vm_new_float(val, vm_read_f32(thread, &(*thread)->pc));
            vm_do_push(thread, val);
        }
        break;

        case PUSH_CONST_UINT8:
        case PUSH_CONST_INT8:
        case PUSH_CONST_UINT16:
        case PUSH_CONST_INT16:
        case PUSH_CONST_UINT32:
        case PUSH_CONST_INT32:
        case PUSH_CONST_FLOAT:
        case PUSH_CONST_STRING: {
            uint8_t type = state->program[(*thread)->pc - 1] - PUSH_CONST_UINT8;
            uint32_t const_pc = vm_read_u32(thread, &(*thread)->pc);

            if (indirect) {
                if ((*thread)->indirect > 0xffffffff - const_pc) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                const_pc += (*thread)->indirect;
            }

            uint32_t pc_tmp = (*thread)->pc;

            switch (type) {
                case 0: { // uint8
                    STK_NEW(thread).type = VM_VAL_UINT;
                    STK_NEW(thread).number.uinteger = vm_read_byte(thread, &const_pc);
                }
                break;
                case 1: { // int8
                    STK_NEW(thread).type = VM_VAL_INT;
                    STK_NEW(thread).number.integer = (int8_t) vm_read_byte(thread, &const_pc);
                }
                break;
                case 2: { // uint16
                    STK_NEW(thread).type = VM_VAL_UINT;
                    STK_NEW(thread).number.uinteger = vm_read_u16(thread, &const_pc);
                }
                break;
                case 3: { // int16
                    STK_NEW(thread).type = VM_VAL_INT;
                    STK_NEW(thread).number.integer = vm_read_i16(thread, &const_pc);
                }
                break;
                case 4: { // uint32
                    STK_NEW(thread).type = VM_VAL_UINT;
                    STK_NEW(thread).number.uinteger = vm_read_u32(thread, &const_pc);
                }
                break;
                case 5: { // int32
                    STK_NEW(thread).type = VM_VAL_INT;
                    STK_NEW(thread).number.integer = vm_read_i32(thread, &const_pc);
                }
                break;
                case 6: { // float
                    STK_NEW(thread).type = VM_VAL_FLOAT;
                    STK_NEW(thread).number.real = vm_read_f32(thread, &const_pc);
                }
                break;
                case 7: { // string
                    STK_NEW(thread).type = VM_VAL_CONST_STRING;
                    STK_NEW(thread).cstr.addr = (char*)((*thread)->state->program + const_pc);
                    STK_NEW(thread).cstr.is_program = true;
                }
                break;
                default:
                err = VM_ERR_CONST_BADTYPE;
            }

            (*thread)->sp += 1;
            (*thread)->pc = pc_tmp;
        }
        break;

        case PUSH_ARRAY: {
            vm_value_t heap_id = vm_do_pop(thread);
            if (heap_id.type == VM_VAL_UINT) {
                uint32_t ref = heap_id.number.uinteger;
                heap_id.heap_ref = ref;
                heap_id.type = VM_VAL_ARRAY;

                vm_do_push(thread, heap_id);
            } else
            err = VM_ERR_BAD_VALUE;
            break;
        }

        case NEW_ARRAY: {
            uint16_t n_fields = vm_read_u16(thread, &(*thread)->pc);
            if (n_fields > 0) {
                vm_heap_object_t arr;
                arr.array.fields = malloc(sizeof(vm_value_t) * n_fields);
                memcpy(arr.array.fields, &STK_OBJ(thread, (*thread)->sp - n_fields), sizeof(vm_value_t) * n_fields);
                arr.type = VM_VAL_ARRAY;
                arr.array.qty = n_fields;
                uint32_t heap_id = vm_heap_save((*thread)->state->heap, arr, &((*thread)->frames[(*thread)->fc].gc_mark));

                (*thread)->sp -= n_fields;

                if (heap_id == 0xffffffff)
                err = VM_ERR_OUTOFMEMORY;
                else {
                    vm_value_t val;
                    val.type = VM_VAL_ARRAY;
                    val.heap_ref = heap_id;

                    vm_do_push(thread, val);
                }
            } else
            err = VM_ERR_BAD_VALUE;
        }
        break;

        case GET_ARRAY_VALUE: {
            uint16_t index = vm_read_u16(thread, &(*thread)->pc);
            vm_heap_object_t *arr = vm_heap_load((*thread)->state->heap, STK_TOP(thread).heap_ref);

            if (indirect) {
                uint32_t _indx = index + (*thread)->indirect;
                if (_indx > 0xffff) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                index = _indx;
            }

            if (arr->type == VM_VAL_ARRAY && index >= 0 && index < arr->array.qty)
            vm_do_push(thread, arr->array.fields[index]);
            else
            err = VM_ERR_BAD_VALUE;
        }
        break;

        case SET_ARRAY_VALUE: {
            uint16_t index = vm_read_u16(thread, &(*thread)->pc);

            if (indirect) {
                uint32_t _indx = index + (*thread)->indirect;
                if (_indx > 0xffff) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                index = _indx;
            }

            vm_heap_object_t *arr = vm_heap_load((*thread)->state->heap, STK_SND(thread).heap_ref);
            vm_value_t val = vm_do_pop(thread);

            if (arr->type == VM_VAL_ARRAY && index >= 0 && index < arr->array.qty)
            arr->array.fields[index] = val;
            else
            err = VM_ERR_BAD_VALUE;
        }
        break;

#define BIN_OP(OP, operator)                                                          \
    case OP: {                                                                        \
        vm_value_t *a = &STK_SND(thread);                                             \
        --(*thread)->sp;                                                              \
        vm_value_t b = STK_NEW(thread);                                               \
        if (a->type == VM_VAL_INT && b.type == VM_VAL_INT) {                          \
            a->number.integer = a->number.integer operator b.number.integer;          \
        } else if (a->type == VM_VAL_UINT && b.type == VM_VAL_UINT) {                 \
            a->number.uinteger = a->number.uinteger operator b.number.uinteger;       \
        } else if (a->type == VM_VAL_FLOAT && b.type == VM_VAL_FLOAT) {               \
            a->number.real = a->number.real operator b.number.real;                   \
        } else if (a->type == VM_VAL_INT) {                                           \
            a->type = VM_VAL_FLOAT;                                                   \
            a->number.real = (float)a->number.integer operator b.number.real;         \
        } else {                                                                      \
            a->number.real = a->number.real operator(float) b.number.integer;         \
        }                                                                             \
    } break;

#define BIN_OP_INT_UINT(OP, operator)                                                            \
    case OP: {                                                                                   \
        vm_value_t val2 = vm_do_pop(thread);                                                     \
        vm_value_t val1 = vm_do_pop(thread);                                                     \
        if(val1.type == VM_VAL_UINT && val2.type == VM_VAL_UINT) {                               \
        	vm_new_uint(val, val1.number.uinteger operator val2.number.uinteger);                \
            vm_do_push(thread, val);                                                             \
        } else {                                                                                 \
        	vm_new_int(val, (int32_t)val1.number.integer operator (int32_t)val2.number.integer); \
            vm_do_push(thread, val);                                                             \
        }                                                                                        \
    } break;

#define REL_OP(OP, operator)                                                      \
    case OP: {                                                                    \
        vm_value_t *a = &STK_SND(thread);                                         \
        --(*thread)->sp;                                                          \
        vm_value_t b = STK_NEW(thread);                                           \
        if (a->type == VM_VAL_FLOAT && b.type == VM_VAL_FLOAT) {                  \
            a->type = VM_VAL_BOOL;                                                \
            a->number.boolean = a->number.real operator b.number.real;            \
        } else if (a->type == VM_VAL_INT && b.type == VM_VAL_INT) {               \
            a->type = VM_VAL_BOOL;                                                \
            a->number.boolean = a->number.integer operator b.number.integer;      \
        } else if (a->type == VM_VAL_UINT && b.type == VM_VAL_UINT) {             \
            a->type = VM_VAL_BOOL;                                                \
            a->number.boolean = a->number.uinteger operator b.number.uinteger;    \
        } else if (a->type == VM_VAL_INT) {                                       \
            a->type = VM_VAL_BOOL;                                                \
            a->number.boolean = (float)a->number.integer operator b.number.real;  \
        } else if (a->type == VM_VAL_UINT) {                                      \
            a->type = VM_VAL_BOOL;                                                \
            a->number.boolean = (float)a->number.uinteger operator b.number.real; \
        } else {                                                                  \
            a->type = VM_VAL_BOOL;                                                \
            a->number.boolean = a->number.real operator(float) b.number.integer;  \
        }                                                                         \
    } break;

        BIN_OP(ADD, +)
        BIN_OP(SUB, -)
        BIN_OP(MUL, *)

        case DIV: {
            vm_value_t *a = &STK_SND(thread);
            --(*thread)->sp;
            vm_value_t b = STK_NEW(thread);
            if (b.number.uinteger == 0)
            err = VM_ERR_DIVBYZERO;
            else {
                if (a->type == VM_VAL_INT && b.type == VM_VAL_INT) {
                    a->number.integer = a->number.integer / b.number.integer;
                } else if (a->type == VM_VAL_UINT && b.type == VM_VAL_UINT) {
                    a->number.uinteger = a->number.uinteger / b.number.uinteger;
                } else if (a->type == VM_VAL_FLOAT && b.type == VM_VAL_FLOAT) {
                    a->number.real = a->number.real / b.number.real;
                } else if (a->type == VM_VAL_INT) {
                    a->type = VM_VAL_FLOAT;
                    a->number.real = (float) a->number.integer / b.number.real;
                } else {
                    a->number.real = a->number.real / (float) b.number.integer;
                }
            }
        }
        break;

        BIN_OP_INT_UINT(MOD, %)
        BIN_OP_INT_UINT(OR, |)
        BIN_OP_INT_UINT(AND, &)

        REL_OP(LT, <)
        REL_OP(GT, >)
        REL_OP(GTE, >=)
        REL_OP(LTE, <=)

#undef BIN_OP
#undef BIN_OP_INT_UINT
#undef REL_OP

        case INC: {
            switch (STK_TOP(thread).type) {
                case VM_VAL_UINT:
                ++STK_TOP(thread).number.uinteger;
                break;
                case VM_VAL_INT:
                ++STK_TOP(thread).number.integer;
                break;
                case VM_VAL_FLOAT:
                ++STK_TOP(thread).number.real;
                break;
            }
        }
        break;

        case DEC: {
            switch (STK_TOP(thread).type) {
                case VM_VAL_UINT:
                --STK_TOP(thread).number.uinteger;
                break;
                case VM_VAL_INT:
                --STK_TOP(thread).number.integer;
                break;
                case VM_VAL_FLOAT:
                --STK_TOP(thread).number.real;
                break;
            }
        }
        break;

        case EQU: {
            vm_value_t b = vm_do_pop(thread);
            vm_value_t a = vm_do_pop(thread);

            vm_new_bool(val, vm_are_values_equal(thread, a, b));
            vm_do_push(thread, val);
        }
        break;

        case NOT: {
            vm_value_t a = vm_do_pop(thread);
            if (a.type != VM_VAL_BOOL) {
                err = VM_ERR_BAD_VALUE;
            } else {
                vm_new_bool(val, !a.number.boolean);
                vm_do_push(thread, val);
            }
        }
        break;

        case SET_GLOBAL: {
            uint32_t var_idx = vm_read_u32(thread, &(*thread)->pc);

            if(var_idx == 0xffffffff) { // is indirect register
                vm_value_t value = vm_do_pop(thread);
                if(value.type != VM_VAL_UINT) {
                    err = VM_ERR_BAD_VALUE;
                    break;
                }

                (*thread)->indirect = value.number.uinteger;
                break;
            }

            vm_heap_object_t value = {
                .type = VM_VAL_GENERIC,
                .value = vm_do_pop(thread)
            };

            if (var_idx > VM_MAX_GLOBAL_VARS)
            err = VM_ERR_OUTOFRANGE;
            else {
                if (var_idx < (*thread)->global_vars_qty) {
                    if (!vm_heap_set((*thread)->state->heap, value, (*thread)->global_vars[var_idx]))
                    err = VM_ERR_OUTOFRANGE;
                } else {
                    uint32_t heap_id = vm_heap_save((*thread)->state->heap, value, &((*thread)->frames[0].gc_mark));
                    (*thread)->global_vars[var_idx] = heap_id;
                    ++(*thread)->global_vars_qty;
                }
            }
        }
        break;

        case GET_GLOBAL: {
            uint32_t var_idx = vm_read_u32(thread, &(*thread)->pc);

            if (var_idx == 0xffffffff) { // is indirect register
                vm_value_t value = {
                    .type = VM_VAL_UINT,
                    .number.uinteger = (*thread)->indirect
                };

                vm_do_push(thread, value);
                break;
            }

            if (var_idx > (*thread)->global_vars_qty - 1)
            err = VM_ERR_OUTOFRANGE;
            else {
                vm_heap_object_t *value = vm_heap_load((*thread)->state->heap, (*thread)->global_vars[var_idx]);
                vm_do_push(thread, value->value);
            }
        }
        break;

        case GOTO: {
            uint32_t new_pc = vm_read_u32(thread, &(*thread)->pc);

            if (indirect) {
                if ((*thread)->indirect > 0xffffffff - new_pc) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                new_pc += (*thread)->indirect;
            }

            (*thread)->pc = new_pc;
        }
        break;

        case GOTOZ: {
            uint32_t new_pc = vm_read_u32(thread, &(*thread)->pc);

            if (indirect) {
                if ((*thread)->indirect > 0xffffffff - new_pc) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                new_pc += (*thread)->indirect;
            }

            vm_value_t val = vm_do_pop(thread);

            if (val.type != VM_VAL_BOOL && val.type != VM_VAL_UINT && val.type != VM_VAL_UINT && val.type != VM_VAL_FLOAT) {
                err = VM_ERR_BAD_VALUE;
            } else if (val.number.boolean == false || val.number.integer == 0 || val.number.uinteger == 0 || val.number.real == 0)
            (*thread)->pc = new_pc;
        }
        break;

        case CALL: {
            uint8_t nargs = (*thread)->state->program[(*thread)->pc++];
            uint32_t pc_idx = vm_read_u32(thread, &(*thread)->pc);

            if (indirect) {
                if ((*thread)->indirect > 0xffffffff - pc_idx) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                pc_idx += (*thread)->indirect;
            }

            if ((*thread)->fc < VM_THREAD_MAX_CALL_DEPTH) {
                vm_do_push_frame(thread, nargs);
                (*thread)->pc = pc_idx;
            } else
            err = VM_ERR_TOOMANYTHREADS;
        }
        break;

        case RETURN: {
            vm_value_t vm_value_null = {VM_VAL_NULL};
            (*thread)->ret_val = vm_value_null;
            if ((*thread)->fc > 0)
            vm_do_pop_frame(thread);
            else
            err = VM_ERR_INVALIDRETURN;
        }
        break;

        case RETURN_VALUE: {
            (*thread)->ret_val = vm_do_pop(thread);
            if ((*thread)->fc > 0)
            vm_do_pop_frame(thread);
            else
            err = VM_ERR_INVALIDRETURN;
        }
        break;

        case CALL_FOREIGN: {
            uint8_t fn = (*thread)->state->program[(*thread)->pc++];
            uint32_t f_idx = vm_read_u32(thread, &(*thread)->pc);

            if (indirect) {
                if ((*thread)->indirect > 0xffffffff - f_idx) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                f_idx += (*thread)->indirect;
            }

            if (f_idx <= (*thread)->state->foreign_functions_qty - 1) {
                (*thread)->ret_val = (*thread)->state->foreign_functions[f_idx](thread, fn);
            } else
            err = VM_ERR_FOREINGFNUNKN;
        }
        break;

        case LIB_FN: {
            if (STK_TOP(thread).type == VM_VAL_LIB_OBJ) {
                uint8_t calltype = vm_read_byte(thread, &(*thread)->pc);
                uint32_t arg = vm_read_u32(thread, &(*thread)->pc);
                err = (*thread)->state->lib[STK_TOP(thread).lib_obj.lib_idx](thread, calltype, STK_TOP(thread).lib_obj.lib_idx, arg);
            } else
            err = VM_ERR_BAD_VALUE;
        }
        break;

        case GET_LOCAL_FF:
        case GET_LOCAL: {
            uint32_t local_idx =
            (state->program[(*thread)->pc - 1] == GET_LOCAL) ?
            vm_read_u32(thread, &(*thread)->pc) :
            (*thread)->state->program[(*thread)->pc++];

            if (local_idx < (*thread)->frames[(*thread)->fc - 1].locals)
            vm_do_push(thread, (*thread)->stack[(*thread)->fp - (local_idx + 1)]);
            else
            err = VM_ERR_LOCALNOTEXIST;
        }
        break;

        case SET_LOCAL_FF:
        case SET_LOCAL: {
            uint32_t local_idx = (state->program[(*thread)->pc - 1] == SET_LOCAL) ?
            vm_read_u32(thread, &(*thread)->pc) :
            (*thread)->state->program[(*thread)->pc++];

            if (local_idx < (*thread)->frames[(*thread)->fc - 1].locals) {
                vm_value_t val = vm_do_pop(thread);
                (*thread)->stack[(*thread)->fp - (local_idx + 1)] = val;
            } else
            err = VM_ERR_LOCALNOTEXIST;
        }
        break;

        case GET_RETVAL: {
            vm_do_push(thread, (*thread)->ret_val);
        }
        break;

        case TO_TYPE: {
            if (STK_TOP(thread).type == VM_VAL_LIB_OBJ) {
                uint32_t arg = vm_read_u32(thread, &(*thread)->pc);
                err = (*thread)->state->lib[STK_TOP(thread).lib_obj.lib_idx](thread, VM_EDFAT_TOTYPE, STK_TOP(thread).lib_obj.lib_idx, arg);
            }
#ifdef VM_ENABLE_TOTYPES
            uint8_t type = (*thread)->state->program[(*thread)->pc++];
            if (STK_TOP(thread).type == type)
            return;

            vm_value_t tmp;
            switch (type) {
                case VM_VAL_UINT: {
                    switch (STK_TOP(thread).type) {
                        case VM_VAL_INT: {
                            tmp.number.uinteger = STK_TOP(thread).number.integer;
                            STK_TOP(thread).number.uinteger = tmp.number.uinteger;
                            STK_TOP(thread).type = VM_VAL_UINT;
                        }
                        break;
                        case VM_VAL_FLOAT: {
                            tmp.number.uinteger = STK_TOP(thread).number.real;
                            STK_TOP(thread).number.uinteger = tmp.number.uinteger;
                            STK_TOP(thread).type = VM_VAL_UINT;
                        }
                        break;
                    }
                }
                break;
                case VM_VAL_INT: {
                    switch (STK_TOP(thread).type) {
                        case VM_VAL_UINT: {
                            tmp.number.integer = STK_TOP(thread).number.uinteger;
                            STK_TOP(thread).number.integer = tmp.number.integer;
                            STK_TOP(thread).type = VM_VAL_INT;
                        }
                        break;
                        case VM_VAL_FLOAT: {
                            tmp.number.integer = STK_TOP(thread).number.real;
                            STK_TOP(thread).number.integer = tmp.number.integer;
                            STK_TOP(thread).type = VM_VAL_INT;
                        }
                        break;
                    }
                }
                break;
                case VM_VAL_FLOAT: {
                    switch (STK_TOP(thread).type) {
                        case VM_VAL_UINT: {
                            tmp.number.real = STK_TOP(thread).number.uinteger;
                            STK_TOP(thread).number.real = tmp.number.real;
                            STK_TOP(thread).type = VM_VAL_FLOAT;
                        }
                        break;
                        case VM_VAL_INT: {

                            tmp.number.real = STK_TOP(thread).number.integer;
                            STK_TOP(thread).number.real = tmp.number.real;
                            STK_TOP(thread).type = VM_VAL_FLOAT;
                        }
                        break;
                    }
                }
                break;
            }
#else
            ++(*thread)->pc;
#endif
        }

        break;

        case DROP: {
            STK_DROP(thread);
        }
        break;

        case HALT: {
            (*thread)->exit_value = (*thread)->state->program[(*thread)->pc];
            err = VM_ERR_HALT;
        }
        break;

        default:
        err = VM_ERR_UNKNOWNOP;
    }

    (*thread)->indirect += ind_inc;

    (*thread)->status = err;
    (*thread)->halted = (err != VM_ERR_OK);
}

void* vm_alloc(size_t size, void *userdata) {
    void *ret = NULL;

    if (size == 0 && userdata != NULL) {
        free(userdata);
        userdata = NULL;
    } else {
        if (userdata == NULL)
            ret = calloc(1, size);
        else
            ret = realloc(userdata, size);
    }

    return ret;
}

void vm_create_thread(vm_thread_t **thread) {
    (*thread) = calloc(1, sizeof(vm_thread_t));
    (*thread)->state = calloc(1, sizeof(vm_state_t));
    (*thread)->state->heap = vm_heap_create(1);
    (*thread)->halted = false;
    (*thread)->global_vars_qty = 0;
    (*thread)->indirect = 0;
    (*thread)->userdata = NULL;
    (*thread)->frames[0].gc_mark = vm_heap_new_gc_mark((*thread)->state->heap);
}

void vm_destroy_thread(vm_thread_t **thread) {
    vm_heap_gc_collect((*thread)->state->heap, &((*thread)->frames[0].gc_mark), true, thread);
    vm_heap_destroy((*thread)->state->heap, thread);
    if ((*thread)->state->lib_qty > 0)
        free((*thread)->state->lib);
    if ((*thread)->state->foreign_functions_qty > 0)
        free((*thread)->state->foreign_functions);
    for (uint32_t n = 0; n < VM_THREAD_STACK_SIZE; ++n)
        if ((*thread)->stack[n].type == VM_VAL_CONST_STRING && (*thread)->stack[n].cstr.is_program == false)
            free((*thread)->stack[n].cstr.addr);
    free((*thread)->state);
    free((*thread));
}
