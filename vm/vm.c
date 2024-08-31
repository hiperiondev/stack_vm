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
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
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
    bool result = false;

    if (a.type != b.type) {
        return false;
    }

    switch (a.type) {
        case VM_VAL_NULL:
            result = true;
            break;

        case VM_VAL_BOOL:
            result = a.number.boolean == b.number.boolean;
            break;

        case VM_VAL_INT:
            result = a.number.integer == b.number.integer;
            break;

        case VM_VAL_UINT:
            result = a.number.uinteger == b.number.uinteger;
            break;

        case VM_VAL_FLOAT:
            result = a.number.real == b.number.real;
            break;

        case VM_VAL_LIB_OBJ:
            result = (*thread)->externals->lib[a.lib_obj.lib_idx](thread, VM_EDFAT_CMP, a.lib_obj.lib_idx, 2) == VM_ERR_OK;
            break;

        case VM_VAL_ARRAY:
            vm_heap_object_t *a1 = vm_heap_load((*thread)->heap, a.heap_ref);
            vm_heap_object_t *a2 = vm_heap_load((*thread)->heap, b.heap_ref);
            result = a1->array.fields == a2->array.fields;
            break;

        case VM_VAL_CONST_STRING:
            result = (strcmp(a.cstr.addr, b.cstr.addr) == 0);
            break;

        default:
    }

    return result;
}

void vm_push_frame(vm_thread_t **thread, uint8_t locals) {
    (*thread)->frames[(*thread)->fc].pc = (*thread)->pc;
    (*thread)->frames[(*thread)->fc].fp = (*thread)->fp;
    (*thread)->frames[(*thread)->fc].locals = locals;
    ++(*thread)->fc;

#ifdef VM_ENABLE_FRAMES_ALIVE
    vm_wordpos_set_bit((*thread)->frame_exist, (*thread)->fc);
#endif
    (*thread)->fp = (*thread)->sp;
    (*thread)->frames[(*thread)->fc].gc_mark = vm_heap_new_gc_mark((*thread)->heap);
}

void vm_pop_frame(vm_thread_t **thread) {
    vm_heap_gc_collect((*thread)->heap, &((*thread)->frames[(*thread)->fc].gc_mark), true, thread, false);
#ifdef VM_HEAP_SHRINK_AFTER_GC
    vm_heap_shrink((*thread)->heap);
#endif

#ifdef VM_ENABLE_FRAMES_ALIVE
    vm_wordpos_unset_bit((*thread)->frame_exist, (*thread)->fc);
#endif
    (*thread)->sp = (*thread)->fp;
    vm_frame_t frame = (*thread)->frames[--(*thread)->fc];
    (*thread)->sp -= frame.locals;
    (*thread)->fp = frame.fp;
    (*thread)->pc = frame.pc;
}

// vm
void vm_step(vm_thread_t **thread, vm_program_t *program) {
    if ((*thread) == NULL)
        return;

    if ((*thread)->pc > program->prog_len - 1) {
        (*thread)->status = VM_ERR_PRG_END;
        (*thread)->halted = true;
        return;
    }

    vm_errors_t err = VM_ERR_OK;
    bool modifier = false;
    int8_t ind_inc = 0;

    switch (OP_MODIFIER(program->prog[(*thread)->pc])) {
        case 0x40:
            modifier = true;
            break;
        case 0x80:
            modifier = true;
            ind_inc = -1;
            break;
        case 0xc0:
            modifier = true;
            ind_inc = 1;
            break;
    }

    program->prog[(*thread)->pc] &= 0x3f;

    switch (program->prog[(*thread)->pc++]) {
        case PUSH_NULL:
        case PUSH_NULL_N: {
            uint8_t n = (program->prog[(*thread)->pc - 1] == PUSH_NULL) ? 1 : program->prog[(*thread)->pc++];

            memset(&STK_NEW(thread), 0, sizeof(vm_value_t) * n);
            (*thread)->sp += n;
        }
            break;

        case NEW_HEAP_OBJECT:
        case NEW_LIB_OBJ: {
            vm_value_t value = vm_pop(thread);
            vm_heap_object_t obj;
            vm_value_t ref;
            bool is_new_lib = program->prog[(*thread)->pc - 1] == NEW_LIB_OBJ ? true : false;

            if (is_new_lib) {
                if (value.type != VM_VAL_UINT || value.number.uinteger > (*thread)->externals->lib_qty) {
                    err = VM_ERR_BAD_VALUE;
                    break;
                }

                ref.type = VM_VAL_LIB_OBJ;

                obj.type = VM_VAL_LIB_OBJ;
                obj.static_obj = false;
                obj.lib_obj.identifier = 0;
                obj.lib_obj.addr = NULL;
                obj.lib_obj.lib_idx = value.number.uinteger;
            } else {
                ref.type = VM_VAL_HEAP_REF;

                obj.type = VM_VAL_GENERIC;
                obj.static_obj = false;
                obj.value = value;
            }

            if (modifier)
                obj.static_obj = true;

            uint32_t heap_ref = vm_heap_save((*thread)->heap, obj, &((*thread)->frames[(*thread)->fc].gc_mark));

            if (is_new_lib) {
                ref.lib_obj.heap_ref = heap_ref;
                vm_push(thread, ref);
                (*thread)->externals->lib[value.number.uinteger](thread, VM_EDFAT_NEW, value.number.uinteger, heap_ref);
            } else {
                ref.heap_ref = heap_ref;
                vm_push(thread, ref);
            }

        }
            break;

        case PUSH_HEAP_OBJECT: {
            vm_value_t value = STK_TOP(thread);

            if (value.type != VM_VAL_LIB_OBJ && value.type != VM_VAL_HEAP_REF)
                err = VM_ERR_BAD_VALUE;
            else {
                uint32_t idx;
                if (value.type == VM_VAL_LIB_OBJ)
                    idx = value.lib_obj.heap_ref;
                else
                    idx = value.heap_ref;

                vm_heap_object_t *obj = vm_heap_load((*thread)->heap, idx);
                if (obj->type == VM_VAL_NULL)
                    err = VM_ERR_HEAPNOTEXIST;
                else {
                    switch (obj->type) {
                        case VM_VAL_GENERIC: {
                            STK_TOP(thread) = obj->value;
                        }
                        break;
                        case VM_VAL_LIB_OBJ: {
                            value.type = VM_VAL_LIB_OBJ;
                            value.lib_obj.heap_ref = idx;
                            value.lib_obj.lib_idx = obj->lib_obj.lib_idx;
                            STK_TOP(thread) = value;
                            err = (*thread)->externals->lib[obj->lib_obj.lib_idx](thread, VM_EDFAT_PUSH, obj->lib_obj.lib_idx, idx);
                        }
                        break;
                        default:
                        break;
                    }
                }
            }
        }
        break;

        case FREE_HEAP_OBJECT: {
            vm_value_t value = vm_pop(thread);

            if (value.type != VM_VAL_LIB_OBJ && value.type != VM_VAL_HEAP_REF) {
                err = VM_ERR_BAD_VALUE;
            } else {
                uint32_t idx;
                if (value.type == VM_VAL_LIB_OBJ) {
                    idx = value.lib_obj.heap_ref;
                } else {
                    idx = value.heap_ref;
                    vm_heap_free((*thread)->heap, idx);
                }
            }
        }
        break;

        case PUSH_TRUE: {
            vm_new_bool(val, true);
            vm_push(thread, val);
        }
        break;

        case PUSH_FALSE: {
            vm_new_bool(val, false);
            vm_push(thread, val);
        }
        break;

        case PUSH_INT: {
            STK_NEW(thread).type = VM_VAL_INT;
            STK_NEW(thread).number.integer = vm_read_i32(thread, program, &(*thread)->pc);
            (*thread)->sp += 1;
        }
        break;

        case PUSH_UINT: {
            STK_NEW(thread).type = VM_VAL_UINT;
            STK_NEW(thread).number.integer = vm_read_u32(thread, program, &(*thread)->pc);
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
            STK_NEW(thread).number.integer = program->prog[(*thread)->pc];
            (*thread)->sp += 1;
            (*thread)->pc += 1;
        }
        break;

        case PUSH_FLOAT: {
            vm_new_float(val, vm_read_f32(thread, program, &(*thread)->pc));
            vm_push(thread, val);
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
            uint8_t type = program->prog[(*thread)->pc - 1] - PUSH_CONST_UINT8;
            uint32_t const_pc = vm_read_u32(thread, program, &(*thread)->pc);

            if (modifier) {
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
                    STK_NEW(thread).number.uinteger = vm_read_byte(thread, program, &const_pc);
                }
                break;
                case 1: { // int8
                    STK_NEW(thread).type = VM_VAL_INT;
                    STK_NEW(thread).number.integer = (int8_t) vm_read_byte(thread, program, &const_pc);
                }
                break;
                case 2: { // uint16
                    STK_NEW(thread).type = VM_VAL_UINT;
                    STK_NEW(thread).number.uinteger = vm_read_u16(thread, program, &const_pc);
                }
                break;
                case 3: { // int16
                    STK_NEW(thread).type = VM_VAL_INT;
                    STK_NEW(thread).number.integer = vm_read_i16(thread, program, &const_pc);
                }
                break;
                case 4: { // uint32
                    STK_NEW(thread).type = VM_VAL_UINT;
                    STK_NEW(thread).number.uinteger = vm_read_u32(thread, program, &const_pc);
                }
                break;
                case 5: { // int32
                    STK_NEW(thread).type = VM_VAL_INT;
                    STK_NEW(thread).number.integer = vm_read_i32(thread, program, &const_pc);
                }
                break;
                case 6: { // float
                    STK_NEW(thread).type = VM_VAL_FLOAT;
                    STK_NEW(thread).number.real = vm_read_f32(thread, program, &const_pc);
                }
                break;
                case 7: { // string
                    STK_NEW(thread).type = VM_VAL_CONST_STRING;
                    STK_NEW(thread).cstr.addr = (char*)(program->prog + const_pc);
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
            vm_value_t heap_id = vm_pop(thread);
            if (heap_id.type == VM_VAL_UINT) {
                uint32_t ref = heap_id.number.uinteger;
                heap_id.heap_ref = ref;
                heap_id.type = VM_VAL_ARRAY;

                vm_push(thread, heap_id);
            } else
                err = VM_ERR_BAD_VALUE;
            break;
        }

        case NEW_ARRAY: {
            uint16_t n_fields = vm_read_u16(thread, program, &(*thread)->pc);
            if (n_fields > 0) {
                vm_heap_object_t arr;
                arr.array.fields = malloc(sizeof(vm_value_t) * n_fields);
                memcpy(arr.array.fields, &STK_OBJ(thread, (*thread)->sp - n_fields), sizeof(vm_value_t) * n_fields);
                arr.type = VM_VAL_ARRAY;
                arr.array.qty = n_fields;
                uint32_t heap_id = vm_heap_save((*thread)->heap, arr, &((*thread)->frames[(*thread)->fc].gc_mark));

                (*thread)->sp -= n_fields;

                if (heap_id == 0xffffffff)
                    err = VM_ERR_OUTOFMEMORY;
                else {
                    vm_value_t val;
                    val.type = VM_VAL_ARRAY;
                    val.heap_ref = heap_id;

                    vm_push(thread, val);
                }
            } else
                err = VM_ERR_BAD_VALUE;
        }
        break;

        case GET_ARRAY_VALUE: {
            uint16_t index = vm_read_u16(thread, program, &(*thread)->pc);
            vm_heap_object_t *arr = vm_heap_load((*thread)->heap, STK_TOP(thread).heap_ref);

            if (modifier) {
                uint32_t _indx = index + (*thread)->indirect;
                if (_indx > 0xffff) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                index = _indx;
            }

            if (arr->type == VM_VAL_ARRAY && index >= 0 && index < arr->array.qty)
                vm_push(thread, arr->array.fields[index]);
            else
                err = VM_ERR_BAD_VALUE;
        }
        break;

        case SET_ARRAY_VALUE: {
            uint16_t index = vm_read_u16(thread, program, &(*thread)->pc);

            if (modifier) {
                uint32_t _indx = index + (*thread)->indirect;
                if (_indx > 0xffff) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                index = _indx;
            }

            vm_heap_object_t *arr = vm_heap_load((*thread)->heap, STK_SND(thread).heap_ref);
            vm_value_t val = vm_pop(thread);

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
        vm_value_t val2 = vm_pop(thread);                                                        \
        vm_value_t val1 = vm_pop(thread);                                                        \
        if(val1.type == VM_VAL_UINT && val2.type == VM_VAL_UINT) {                               \
        	vm_new_uint(val, val1.number.uinteger operator val2.number.uinteger);                \
            vm_push(thread, val);                                                                \
        } else {                                                                                 \
        	vm_new_int(val, (int32_t)val1.number.integer operator (int32_t)val2.number.integer); \
            vm_push(thread, val);                                                                \
        }                                                                                        \
    } break;

#define REL_OP(OP, operator)                                                       \
    case OP: {                                                                     \
    	uint32_t new_pc = 0;                                                       \
        if(modifier)                                                               \
		    new_pc = vm_read_u32(thread, program, &(*thread)->pc);                 \
        vm_value_t b = vm_pop(thread);                                             \
        vm_value_t a = vm_pop(thread);                                             \
        if (a.type == VM_VAL_FLOAT && b.type == VM_VAL_FLOAT) {                    \
            if(modifier)                                                           \
                (*thread)->pc = new_pc;                                            \
            else {                                                                 \
			    vm_new_bool(val, a.number.real operator b.number.real);            \
			    vm_push(thread, val);                                              \
            }                                                                      \
        } else if (a.type == VM_VAL_INT && b.type == VM_VAL_INT) {                 \
            if(modifier)                                                           \
                (*thread)->pc = new_pc;                                            \
            else {                                                                 \
                vm_new_bool(val, a.number.integer operator b.number.integer);      \
                vm_push(thread, val);                                              \
            }                                                                      \
        } else if (a.type == VM_VAL_UINT && b.type == VM_VAL_UINT) {               \
            if(modifier)                                                           \
                (*thread)->pc = new_pc;                                            \
            else {                                                                 \
                vm_new_bool(val, a.number.uinteger operator b.number.uinteger);    \
                vm_push(thread, val);                                              \
            }                                                                      \
        } else if (a.type == VM_VAL_INT) {                                         \
            if(modifier)                                                           \
                (*thread)->pc = new_pc;                                            \
            else {                                                                 \
                vm_new_bool(val, (float)a.number.integer operator b.number.real);  \
                vm_push(thread, val);                                              \
            }                                                                      \
        } else if (a.type == VM_VAL_UINT) {                                        \
            if(modifier)                                                           \
                (*thread)->pc = new_pc;                                            \
            else {                                                                 \
                vm_new_bool(val, (float)a.number.uinteger operator b.number.real); \
                vm_push(thread, val);                                              \
            }                                                                      \
        } else {                                                                   \
            if(modifier)                                                           \
                (*thread)->pc = new_pc;                                            \
            else {                                                                 \
                vm_new_bool(val, a.number.real operator(float) b.number.integer);  \
                vm_push(thread, val);                                              \
            }                                                                      \
        }                                                                          \
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
            uint32_t new_pc = 0;
            if(modifier) {
                new_pc = vm_read_u32(thread, program, &(*thread)->pc);
            }

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
                default:
            }

            if(modifier) {
                (*thread)->pc = new_pc;
            }
        }
        break;

        case DEC: {
            uint32_t new_pc = 0;
            if(modifier) {
                new_pc = vm_read_u32(thread, program, &(*thread)->pc);
            }

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
                default:
            }

            if(modifier) {
                (*thread)->pc = new_pc;
            }
        }
        break;

        case EQU: {
            uint32_t new_pc = 0;
            if(modifier) {
                new_pc = vm_read_u32(thread, program, &(*thread)->pc);
            }

            vm_value_t b = vm_pop(thread);
            vm_value_t a = vm_pop(thread);

            bool res = vm_are_values_equal(thread, a, b);

            if(modifier && res) {
                (*thread)->pc = new_pc;
            } else {
                vm_new_bool(val, res);
                vm_push(thread, val);
            }
        }
        break;

        case NOT: {
            vm_value_t a = vm_pop(thread);
            if (a.type != VM_VAL_BOOL) {
                err = VM_ERR_BAD_VALUE;
            } else {
                vm_new_bool(val, !a.number.boolean);
                vm_push(thread, val);
            }
        }
        break;

        case SET_GLOBAL: {
            uint32_t var_idx = vm_read_u32(thread, program, &(*thread)->pc);

            if(var_idx == 0xffffffff) { // is indirect register
                vm_value_t value = vm_pop(thread);
                if(value.type != VM_VAL_UINT) {
                    err = VM_ERR_BAD_VALUE;
                    break;
                }

                (*thread)->indirect = value.number.uinteger;
                break;
            }

            vm_heap_object_t value = {
                .type = VM_VAL_GENERIC,
                .value = vm_pop(thread)
            };

            if (var_idx > VM_MAX_GLOBAL_VARS)
                err = VM_ERR_OUTOFRANGE;
            else {
                if (var_idx < (*thread)->globals->global_vars_qty) {
                    if (!vm_heap_set((*thread)->heap, value, (*thread)->globals->global_vars[var_idx]))
                        err = VM_ERR_OUTOFRANGE;
                } else {
                    uint32_t heap_id = vm_heap_save((*thread)->heap, value, &((*thread)->frames[0].gc_mark));
                    (*thread)->globals->global_vars[var_idx] = heap_id;
                    ++(*thread)->globals->global_vars_qty;
                }
            }
        }
        break;

        case GET_GLOBAL: {
            uint32_t var_idx = vm_read_u32(thread, program, &(*thread)->pc);

            if (var_idx == 0xffffffff) { // is indirect register
                vm_value_t value = {
                    .type = VM_VAL_UINT,
                    .number.uinteger = (*thread)->indirect
                };

                vm_push(thread, value);
                break;
            }

            if (var_idx > (*thread)->globals->global_vars_qty - 1)
                err = VM_ERR_OUTOFRANGE;
            else {
                vm_heap_object_t *value = vm_heap_load((*thread)->heap, (*thread)->globals->global_vars[var_idx]);
                vm_push(thread, value->value);
            }
        }
        break;

        case GOTO: {
            uint32_t new_pc = vm_read_u32(thread, program, &(*thread)->pc);

            if (modifier) {
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
            uint32_t new_pc = vm_read_u32(thread, program, &(*thread)->pc);

            if (modifier) {
                if ((*thread)->indirect > 0xffffffff - new_pc) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                new_pc += (*thread)->indirect;
            }

            vm_value_t val = vm_pop(thread);

            if (val.type != VM_VAL_BOOL && val.type != VM_VAL_UINT && val.type != VM_VAL_UINT && val.type != VM_VAL_FLOAT) {
                err = VM_ERR_BAD_VALUE;
            } else if (val.number.boolean == false || val.number.integer == 0 || val.number.uinteger == 0 || val.number.real == 0)
                (*thread)->pc = new_pc;
        }
        break;

        case CALL: {
            uint8_t nargs = program->prog[(*thread)->pc++];
            uint32_t pc_idx = vm_read_u32(thread, program, &(*thread)->pc);

            if (modifier) {
                if ((*thread)->indirect > 0xffffffff - pc_idx) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                pc_idx += (*thread)->indirect;
            }

            if ((*thread)->fc < VM_THREAD_MAX_CALL_DEPTH) {
                vm_push_frame(thread, nargs);
                (*thread)->pc = pc_idx;
            } else
                err = VM_ERR_TOOMANYTHREADS;
        }
        break;

        case RETURN: {
            vm_value_t vm_value_null = {VM_VAL_NULL};
            (*thread)->ret_val = vm_value_null;
            if ((*thread)->fc > 0)
                vm_pop_frame(thread);
            else
                err = VM_ERR_INVALIDRETURN;
        }
        break;

        case RETURN_VALUE: {
            (*thread)->ret_val = vm_pop(thread);
            if ((*thread)->fc > 0)
                vm_pop_frame(thread);
            else
                err = VM_ERR_INVALIDRETURN;
        }
        break;

        case CALL_FOREIGN: {
            uint8_t fn = program->prog[(*thread)->pc++];
            uint32_t arg = vm_read_u32(thread, program, &(*thread)->pc);
            uint32_t f_idx = vm_read_u32(thread, program, &(*thread)->pc);

            if (modifier) {
                if ((*thread)->indirect > 0xffffffff - f_idx) {
                    err = VM_ERR_OVERFLOW;
                    break;
                }
                f_idx += (*thread)->indirect;
            }

            if (f_idx <= (*thread)->externals->foreign_functions_qty - 1) {
                (*thread)->ret_val = (*thread)->externals->foreign_functions[f_idx](thread, fn, arg);
            } else
                err = VM_ERR_FOREINGFNUNKN;
        }
        break;

        case LIB_FN: {
            if (STK_TOP(thread).type == VM_VAL_LIB_OBJ) {
                uint8_t calltype = vm_read_byte(thread, program, &(*thread)->pc);
                uint32_t arg = vm_read_u32(thread, program, &(*thread)->pc);
                err = (*thread)->externals->lib[STK_TOP(thread).lib_obj.lib_idx](thread, calltype, STK_TOP(thread).lib_obj.lib_idx, arg);
            } else
                err = VM_ERR_BAD_VALUE;
        }
        break;

        case GET_LOCAL_FF:
        case GET_LOCAL: {
            uint32_t local_idx =
            (program->prog[(*thread)->pc - 1] == GET_LOCAL) ?
            vm_read_u32(thread, program, &(*thread)->pc) :
            program->prog[(*thread)->pc++];

            if (local_idx < (*thread)->frames[(*thread)->fc - 1].locals)
                vm_push(thread, (*thread)->stack[(*thread)->fp - (*thread)->frames[(*thread)->fc - 1].locals + local_idx]);
            else
                err = VM_ERR_LOCALNOTEXIST;
        }
        break;

        case SET_LOCAL_FF:
        case SET_LOCAL: {
            uint32_t local_idx = (program->prog[(*thread)->pc - 1] == SET_LOCAL) ?
            vm_read_u32(thread, program, &(*thread)->pc) :
            program->prog[(*thread)->pc++];

            if (local_idx < (*thread)->frames[(*thread)->fc - 1].locals) {
                vm_value_t val = vm_pop(thread);
                (*thread)->stack[(*thread)->fp - (*thread)->frames[(*thread)->fc - 1].locals + local_idx] = val;
            } else
                err = VM_ERR_LOCALNOTEXIST;
        }
        break;

        case GET_RETVAL: {
            vm_push(thread, (*thread)->ret_val);
        }
        break;

        case TO_TYPE: {
#ifdef VM_ENABLE_TOTYPES
            if (STK_TOP(thread).type == VM_VAL_LIB_OBJ) {
                uint32_t arg = vm_read_u32(thread, program, &(*thread)->pc);
                err = (*thread)->externals->lib[STK_TOP(thread).lib_obj.lib_idx](thread, VM_EDFAT_TOTYPE, STK_TOP(thread).lib_obj.lib_idx, arg);
            }

            uint8_t type = program->prog[(*thread)->pc++];
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
                        default:
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
                        default:
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
                        default:
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

        case SWAP: {
            STK_SWAP(thread);
        }
        break;

        case HALT: {
            (*thread)->exit_value = program->prog[(*thread)->pc];
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
    (*thread)->globals = calloc(1, sizeof(vm_globals_t));
    (*thread)->stack = calloc(VM_THREAD_STACK_SIZE, sizeof(vm_value_t));
    (*thread)->heap = vm_heap_create(1);
    (*thread)->globals->global_vars_qty = 0;
    (*thread)->frames[0].gc_mark = vm_heap_new_gc_mark((*thread)->heap);
}

void vm_destroy_thread(vm_thread_t **thread) {
    vm_heap_gc_collect((*thread)->heap, &((*thread)->frames[0].gc_mark), true, thread, true);
    vm_heap_destroy((*thread)->heap, thread);
    free((*thread)->globals);
    for (uint32_t n = 0; n < VM_THREAD_STACK_SIZE; ++n)
        if ((*thread)->stack[n].type == VM_VAL_CONST_STRING && (*thread)->stack[n].cstr.is_program == false)
            free((*thread)->stack[n].cstr.addr);
    free((*thread)->stack);
    free((*thread));
}
