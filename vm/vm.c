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
 * @version 0.1
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

// memory
void* vm_malloc(vm_context_t *ctx, size_t size) {
    return ctx->alloc(size, ctx->userdata);
}

void* vm_realloc(vm_context_t *ctx, size_t size) {
    return ctx->alloc(size, ctx->userdata);
}

void vm_free(vm_context_t *ctx) {
    ctx->alloc(0, ctx->userdata);
}

// values
const vm_value_t vm_value_null = { VM_VAL_NULL };

static inline vm_value_t vm_new_bool(bool value) {
    vm_value_t val;

    val.type = VM_VAL_BOOL;
    val.number.boolean = value;

    return val;
}

static inline vm_value_t vm_new_int(int32_t i) {
    vm_value_t val;

    val.type = VM_VAL_INT;
    val.number.integer = i;

    return val;
}

static inline vm_value_t vm_new_uint(uint32_t i) {
    vm_value_t val;

    val.type = VM_VAL_UINT;
    val.number.uinteger = i;

    return val;
}

static inline vm_value_t vm_new_float(float f) {
    vm_value_t val;

    val.type = VM_VAL_FLOAT;
    val.number.real = f;

    return val;
}

static inline vm_value_t vm_new_const_string(const char *str) {
    vm_value_t val;
    if (str == NULL) {
        val.type = VM_VAL_NULL;
        return val;
    }

    val.type = VM_VAL_CONST_STRING;
    val.cstr = str;

    return val;
}

static vm_object_t* vm_new_array(vm_state_thread_t *thread, uint8_t n) {
    vm_object_t *obj = vm_malloc(&thread->ctx, sizeof(vm_object_t) + sizeof(vm_value_t) * n);

    obj->type = VM_VAL_ARRAY;
    obj->marked = 0;
    obj->array.qty = n;

    return obj;
}

static inline void vm_to_type(vm_value_t *value, uint8_t type) {
    if (value->type == type)
        return;

    vm_value_t tmp;
    switch (type) {
        case VM_VAL_UINT: {
            switch (value->type) {
                case VM_VAL_INT: {
                    tmp.number.uinteger = value->number.integer;
                    value->number.uinteger = tmp.number.uinteger;
                    value->type = VM_VAL_UINT;
                }
                    break;
                case VM_VAL_FLOAT: {
                    tmp.number.uinteger = value->number.real;
                    value->number.uinteger = tmp.number.uinteger;
                    value->type = VM_VAL_UINT;
                }
                    break;
            }
        }
            break;
        case VM_VAL_INT: {
            switch (value->type) {
                case VM_VAL_UINT: {
                    tmp.number.integer = value->number.uinteger;
                    value->number.integer = tmp.number.integer;
                    value->type = VM_VAL_INT;
                }
                    break;
                case VM_VAL_FLOAT: {
                    tmp.number.integer = value->number.real;
                    value->number.integer = tmp.number.integer;
                    value->type = VM_VAL_INT;
                }
                    break;
            }
        }
            break;
        case VM_VAL_FLOAT: {
            switch (value->type) {
                case VM_VAL_UINT: {
                    tmp.number.real = value->number.uinteger;
                    value->number.real = tmp.number.real;
                    value->type = VM_VAL_FLOAT;
                }
                    break;
                case VM_VAL_INT: {

                    tmp.number.real = value->number.integer;
                    value->number.real = tmp.number.real;
                    value->type = VM_VAL_FLOAT;
                }
                    break;
            }
        }
            break;
    }
}

size_t vm_string_len(const vm_value_t value) {
    if (value.type == VM_VAL_CONST_STRING)
        return strlen(value.cstr);
    if (value.type != VM_VAL_STRING)
        return 0;

    return value.obj->string.len;
}

const char* vm_to_string(const vm_value_t value) {
    if (value.type == VM_VAL_CONST_STRING)
        return value.cstr;
    if (value.type != VM_VAL_STRING)
        return NULL;

    return value.obj->string.ptr;
}

bool vm_are_values_equal(vm_value_t a, vm_value_t b) {
    bool both_strings = ((a.type == VM_VAL_CONST_STRING && b.type == VM_VAL_STRING) || (a.type == VM_VAL_STRING && b.type == VM_VAL_CONST_STRING));

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

    if (a.type == VM_VAL_STRING) {
        size_t a_len = vm_string_len(a);
        size_t b_len = vm_string_len(b);

        if (a_len != b_len) {
            return false;
        }

        return strncmp(a.obj->string.ptr, vm_to_string(b), a_len) == 0;
    }

    if (a.type == VM_VAL_CONST_STRING) {
        if (b.type == VM_VAL_CONST_STRING && a.cstr == b.cstr) {
            return true;
        }

        size_t a_len = vm_string_len(a);
        size_t b_len = vm_string_len(b);

        if (a_len != b_len) {
            return false;
        }

        return strncmp(a.cstr, vm_to_string(b), a_len) == 0;
    }

    if (a.type == VM_VAL_NATIVE) {
        return a.obj->native.addr == b.obj->native.addr;
    }

    if (a.type == VM_VAL_LIGHT_NATIVE) {
        return a.addr == b.addr;
    }

    if (a.type == VM_VAL_ARRAY) {
        return a.obj == b.obj;
    }

    return false;
}

// data program

static inline uint8_t vm_read_byte(vm_state_t *state, uint32_t *pc) {
    return state->program[(*pc)++];
}

static inline int16_t vm_read_i16(vm_state_t *state, uint32_t *pc) {
    vm_value_t val;
    val.number.byte[0] = state->program[*pc];
    val.number.byte[1] = state->program[*pc + 1];
    *pc += 2;

    return val.number.integer;
}

static inline uint16_t vm_read_u16(vm_state_t *state, uint32_t *pc) {
    vm_value_t val;
    val.number.byte[0] = state->program[*pc];
    val.number.byte[1] = state->program[*pc + 1];
    *pc += 2;

    return val.number.uinteger;
}

static inline int32_t vm_read_i32(vm_state_t *state, uint32_t *pc) {
    vm_value_t val;
    val.number.byte[0] = state->program[*pc];
    val.number.byte[1] = state->program[*pc + 1];
    val.number.byte[2] = state->program[*pc + 2];
    val.number.byte[3] = state->program[*pc + 3];
    *pc += 4;

    return val.number.integer;
}

static inline uint32_t vm_read_u32(vm_state_t *state, uint32_t *pc) {
    vm_value_t val;
    val.number.byte[0] = (uint32_t) state->program[*pc];
    val.number.byte[1] = (uint32_t) state->program[*pc + 1];
    val.number.byte[2] = (uint32_t) state->program[*pc + 2];
    val.number.byte[3] = (uint32_t) state->program[*pc + 3];
    *pc += 4;

    return val.number.integer;
}

static inline float vm_read_f32(vm_state_t *state, uint32_t *pc) {
    union {
        float real;
        uint8_t byte[4];
    } val;
    val.byte[0] = state->program[*pc];
    val.byte[1] = state->program[*pc + 1];
    val.byte[2] = state->program[*pc + 2];
    val.byte[3] = state->program[*pc + 3];
    *pc += 4;

    return val.real;
}

// stack

static inline void vm_do_push(vm_state_thread_t *thread, vm_value_t value) {
    thread->stack[thread->sp++] = value;
}

static inline vm_value_t vm_do_pop(vm_state_thread_t *thread) {
    return thread->stack[--thread->sp];
}

static inline void vm_do_push_indir(vm_state_thread_t *thread, uint8_t nargs) {
    thread->frames[thread->fc++] = (vm_frame_t ) { thread->pc, thread->fp, nargs };
    thread->fp = thread->sp;

}

static inline void vm_do_pop_indir(vm_state_thread_t *thread) {
    thread->sp = thread->fp;

    vm_frame_t frame = thread->frames[--thread->fc];

    thread->sp -= frame.nargs;
    thread->fp = frame.fp;
    thread->pc = frame.pc;
}

void vm_step(vm_state_thread_t **thread) {
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

    switch (state->program[(*thread)->pc]) {
        case PUSH_NULL: {
            ++(*thread)->pc;
            vm_do_push((*thread), vm_value_null);
        }
            break;

        case PUSH_NULL_N: {
            ++(*thread)->pc;
            uint8_t n = (*thread)->state->program[(*thread)->pc++];

            memset(&(*thread)->stack[(*thread)->sp], 0, sizeof(vm_value_t) * n);
            (*thread)->sp += n;
        }
            break;

        case PUSH_TRUE: {
            ++(*thread)->pc;
            vm_do_push((*thread), vm_new_bool(true));
        }
            break;

        case PUSH_FALSE: {
            ++(*thread)->pc;
            vm_do_push((*thread), vm_new_bool(false));
        }
            break;

        case PUSH_INT: {
            ++(*thread)->pc;
            (*thread)->stack[(*thread)->sp].type = VM_VAL_INT;
            (*thread)->stack[(*thread)->sp].number.integer = vm_read_i32((*thread)->state, &(*thread)->pc);
            (*thread)->sp += 1;
        }
            break;

        case PUSH_UINT: {
            ++(*thread)->pc;
            (*thread)->stack[(*thread)->sp].type = VM_VAL_UINT;
            (*thread)->stack[(*thread)->sp].number.integer = vm_read_u32((*thread)->state, &(*thread)->pc);
            (*thread)->sp += 1;
        }
            break;

        case PUSH_0: {
            ++(*thread)->pc;
            (*thread)->stack[(*thread)->sp].type = VM_VAL_INT;
            (*thread)->stack[(*thread)->sp].number.integer = 0;
            (*thread)->sp += 1;
        }
            break;

        case PUSH_1: {
            ++(*thread)->pc;
            (*thread)->stack[(*thread)->sp].type = VM_VAL_INT;
            (*thread)->stack[(*thread)->sp].number.integer = 1;
            (*thread)->sp += 1;
        }
            break;

        case PUSH_CHAR: {
            ++(*thread)->pc;
            (*thread)->stack[(*thread)->sp].type = VM_VAL_INT;
            (*thread)->stack[(*thread)->sp].number.integer = (*thread)->state->program[(*thread)->pc];
            (*thread)->sp += 1;
            (*thread)->pc += 1;
        }
            break;

        case PUSH_FLOAT: {
            ++(*thread)->pc;
            vm_do_push((*thread), vm_new_float(vm_read_f32((*thread)->state, &(*thread)->pc)));
        }
            break;

        case PUSH_STRING: {
            ++(*thread)->pc;
            uint32_t string_index = vm_read_u32((*thread)->state, &(*thread)->pc);
            vm_do_push((*thread), vm_new_const_string((*thread)->state->strings[string_index]));
        }
            break;

        case PUSH_STRING_FF: {
            ++(*thread)->pc;
            uint8_t s_index = (*thread)->state->program[(*thread)->pc++];
            vm_do_push((*thread), vm_new_const_string((*thread)->state->strings[s_index]));
        }
            break;

        case NEW_ARRAY: {
            ++(*thread)->pc;
            uint8_t n_fields = (*thread)->state->program[(*thread)->pc++];
            if (n_fields > 0) {
                vm_object_t *obj = vm_new_array((*thread), n_fields);

                memcpy(obj->array.fields, &(*thread)->stack[(*thread)->sp - n_fields], sizeof(vm_value_t) * n_fields);
                (*thread)->sp -= n_fields;

                vm_value_t val;
                val.type = VM_VAL_ARRAY;
                val.obj = obj;
                vm_do_push(*thread, vm_new_uint(vm_heap_save((*thread)->state->heap, val)));
            } else
                err = VM_ERR_BAD_VALUE;
        }
            break;

        case PUSH_ARRAY: {
            ++(*thread)->pc;
            vm_value_t st = vm_do_pop(*thread);
            if (st.type == VM_VAL_UINT)
                vm_do_push(*thread, vm_heap_load((*thread)->state->heap, st.number.uinteger));
            else
                err = VM_ERR_BAD_VALUE;
            break;
        }

        case GET_ARRAY_VALUE: {
            ++(*thread)->pc;
            uint8_t i = (*thread)->state->program[(*thread)->pc++];
            if (i >= 0 && i < (*thread)->stack[(*thread)->sp - 1].obj->array.qty)
                vm_do_push(*thread, (*thread)->stack[(*thread)->sp - 1].obj->array.fields[i]);
            else
                err = VM_ERR_BAD_VALUE;
        }
            break;

        case SET_ARRAY_VALUE: {
            ++(*thread)->pc;
            uint8_t i = (*thread)->state->program[(*thread)->pc++];
            vm_value_t vstruct = (*thread)->stack[(*thread)->sp - 2];
            vm_value_t val = vm_do_pop(*thread);

            if (i >= 0 && i < vstruct.obj->array.qty)
                vstruct.obj->array.fields[i] = val;
            else
                err = VM_ERR_BAD_VALUE;
        }
            break;

#define BIN_OP(OP, operator)                                                          \
    case OP: {                                                                        \
        vm_value_t *a = &(*thread)->stack[(*thread)->sp - 2];                         \
        vm_value_t b = (*thread)->stack[--(*thread)->sp];                             \
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
        ++(*thread)->pc;                                                              \
    } break;

#define BIN_OP_INT_UINT(OP, operator)                                                                              \
    case OP: {                                                                                                     \
        vm_value_t val2 = vm_do_pop((*thread));                                                                    \
        vm_value_t val1 = vm_do_pop((*thread));                                                                    \
        if(val1.type == VM_VAL_UINT && val2.type == VM_VAL_UINT)                                                   \
            vm_do_push((*thread), vm_new_uint(val1.number.uinteger operator val2.number.uinteger));                \
        else                                                                                                       \
            vm_do_push((*thread), vm_new_int((int32_t)val1.number.integer operator (int32_t)val2.number.integer)); \
        ++(*thread)->pc;                                                                                           \
    } break;

#define REL_OP(OP, operator)                                                      \
    case OP: {                                                                    \
        vm_value_t *a = &(*thread)->stack[(*thread)->sp - 2];                     \
        vm_value_t b = (*thread)->stack[--(*thread)->sp];                         \
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
        ++(*thread)->pc;                                                          \
    } break;

        BIN_OP(ADD, +)
        BIN_OP(SUB, -)
        BIN_OP(MUL, *)

        case DIV: {
            vm_value_t *a = &(*thread)->stack[(*thread)->sp - 2];
            vm_value_t b = (*thread)->stack[--(*thread)->sp];
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
            ++(*thread)->pc;
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
            ++(*thread)->pc;
            switch ((*thread)->stack[(*thread)->sp - 1].type) {
                case VM_VAL_UINT:
                    ++(*thread)->stack[(*thread)->sp - 1].number.uinteger;
                    break;
                case VM_VAL_INT:
                    ++(*thread)->stack[(*thread)->sp - 1].number.integer;
                    break;
                case VM_VAL_FLOAT:
                    ++(*thread)->stack[(*thread)->sp - 1].number.real;
                    break;
            }
        }
            break;

        case DEC: {
            ++(*thread)->pc;
            switch ((*thread)->stack[(*thread)->sp - 1].type) {
                case VM_VAL_UINT:
                    --(*thread)->stack[(*thread)->sp - 1].number.uinteger;
                    break;
                case VM_VAL_INT:
                    --(*thread)->stack[(*thread)->sp - 1].number.integer;
                    break;
                case VM_VAL_FLOAT:
                    --(*thread)->stack[(*thread)->sp - 1].number.real;
                    break;
            }
        }
            break;

        case EQU: {
            ++(*thread)->pc;
            vm_value_t b = vm_do_pop((*thread));
            vm_value_t a = vm_do_pop((*thread));

            vm_do_push((*thread), vm_new_bool(vm_are_values_equal(a, b)));
        }
            break;

        case NOT: {
            ++(*thread)->pc;
            vm_value_t a = vm_do_pop((*thread));
            if (a.type != VM_VAL_BOOL) {
                err = VM_ERR_BAD_VALUE;
            } else
                vm_do_push((*thread), vm_new_bool(!a.number.boolean));
        }
            break;

        case SET_GLOBAL: {
            ++(*thread)->pc;
            uint32_t var_idx = vm_read_u32((*thread)->state, &(*thread)->pc);
            if (var_idx > (*thread)->global_vars_qty - 1)
                err = VM_ERR_OUTOFRANGE;
            else
                (*thread)->global_vars[var_idx] = vm_do_pop((*thread));
        }
            break;

        case GET_GLOBAL: {
            ++(*thread)->pc;
            uint32_t var_idx = vm_read_u32((*thread)->state, &(*thread)->pc);
            if (var_idx > (*thread)->global_vars_qty - 1)
                err = VM_ERR_OUTOFRANGE;
            else
                vm_do_push((*thread), (*thread)->global_vars[var_idx]);
        }
            break;

        case GOTO: {
            ++(*thread)->pc;
            uint32_t new_pc = vm_read_u32((*thread)->state, &(*thread)->pc);
            (*thread)->pc = new_pc;
        }
            break;

        case GOTOZ: {
            ++(*thread)->pc;
            uint32_t new_pc = vm_read_u32((*thread)->state, &(*thread)->pc);
            vm_value_t val = vm_do_pop((*thread));

            if (val.type != VM_VAL_BOOL && val.type != VM_VAL_UINT && val.type != VM_VAL_UINT && val.type != VM_VAL_FLOAT) {
                err = VM_ERR_BAD_VALUE;
            } else if (val.number.boolean == false || val.number.integer == 0 || val.number.uinteger == 0 || val.number.real == 0)
                (*thread)->pc = new_pc;
        }
            break;

        case CALL: {
            ++(*thread)->pc;
            uint8_t nargs = (*thread)->state->program[(*thread)->pc++];
            uint32_t pc_idx = vm_read_u32((*thread)->state, &(*thread)->pc);

            if ((*thread)->fc < VM_THREAD_MAX_CALL_DEPTH) {
                vm_do_push_indir((*thread), nargs);
                (*thread)->pc = state->functions[pc_idx];
            } else
                err = VM_ERR_TOOMANYTHREADS;
        }
            break;

        case RETURN: {
            (*thread)->ret_val = vm_value_null;
            if ((*thread)->fc > 0)
                vm_do_pop_indir((*thread));
            else
                err = VM_ERR_INVALIDRETURN;
        }
            break;

        case RETURN_VALUE: {
            (*thread)->ret_val = vm_do_pop((*thread));
            if ((*thread)->fc > 0)
                vm_do_pop_indir((*thread));
            else
                err = VM_ERR_INVALIDRETURN;
        }
            break;

        case CALL_FOREIGN: {
            ++(*thread)->pc;
            uint8_t nargs = (*thread)->state->program[(*thread)->pc++];
            uint32_t f_idx = vm_read_u32((*thread)->state, &(*thread)->pc);
            if (f_idx <= (*thread)->state->foreign_functions_qty - 1) {
                uint32_t prev_size = (*thread)->sp - nargs; // the state of the stack prior to the function arguments being pushed

                (*thread)->ret_val = (*thread)->state->foreign_functions[f_idx]((*thread), &(*thread)->stack[prev_size], nargs);
                (*thread)->sp = prev_size; // Resize the stack so that it has the arguments removed
            } else
                err = VM_ERR_FOREINGFNUNKN;
        }
            break;

        case GET_LOCAL: {
            ++(*thread)->pc;
            uint32_t local_idx = vm_read_u32((*thread)->state, &(*thread)->pc);
            vm_do_push((*thread), (*thread)->stack[(*thread)->fp - local_idx]);
        }
            break;

        case GET_LOCAL_W: {
            ++(*thread)->pc;
            uint8_t local_idx = (*thread)->state->program[(*thread)->pc++];
            vm_do_push((*thread), (*thread)->stack[(*thread)->fp - local_idx]);
        }
            break;

        case SET_LOCAL: {
            ++(*thread)->pc;
            uint32_t local_idx = vm_read_u32((*thread)->state, &(*thread)->pc);
            vm_value_t val = vm_do_pop((*thread));
            (*thread)->stack[(*thread)->fp - local_idx] = val;
        }
            break;

        case GET_RETVAL: {
            ++(*thread)->pc;
            vm_do_push((*thread), (*thread)->ret_val);
        }
            break;

        case GET_CONST: {
            ++(*thread)->pc;
            uint8_t type = vm_read_byte((*thread)->state, &(*thread)->pc);
            uint32_t const_pc = vm_read_u16((*thread)->state, &(*thread)->pc) + vm_read_u32((*thread)->state, &(*thread)->pc);
            uint32_t pc_tmp = (*thread)->pc;
            switch (type) {
                case 0: // uint8
                    (*thread)->stack[(*thread)->sp].type = VM_VAL_UINT;
                    (*thread)->stack[(*thread)->sp].number.uinteger = vm_read_byte((*thread)->state, &const_pc);
                    break;
                case 1: // int8
                    (*thread)->stack[(*thread)->sp].type = VM_VAL_INT;
                    (*thread)->stack[(*thread)->sp].number.integer = (int8_t) vm_read_byte((*thread)->state, &const_pc);
                    break;
                case 2: // uint16
                    (*thread)->stack[(*thread)->sp].type = VM_VAL_UINT;
                    (*thread)->stack[(*thread)->sp].number.uinteger = vm_read_u16((*thread)->state, &const_pc);
                    break;
                case 3: // int16
                    (*thread)->stack[(*thread)->sp].type = VM_VAL_INT;
                    (*thread)->stack[(*thread)->sp].number.integer = vm_read_i16((*thread)->state, &const_pc);
                    break;
                case 4: // uint32
                    (*thread)->stack[(*thread)->sp].type = VM_VAL_UINT;
                    (*thread)->stack[(*thread)->sp].number.uinteger = vm_read_u32((*thread)->state, &const_pc);
                    break;
                case 5: // int32
                    (*thread)->stack[(*thread)->sp].type = VM_VAL_INT;
                    (*thread)->stack[(*thread)->sp].number.integer = vm_read_i32((*thread)->state, &const_pc);
                    break;
                case 6: // float
                    (*thread)->stack[(*thread)->sp].type = VM_VAL_FLOAT;
                    (*thread)->stack[(*thread)->sp].number.real = vm_read_f32((*thread)->state, &const_pc);
                    break;
                case 7: // string
                    (*thread)->stack[(*thread)->sp].type = VM_VAL_CONST_STRING;
                    (*thread)->stack[(*thread)->sp].cstr = (char*) ((*thread)->state->program + const_pc);
                    break;
                default:
                    err = VM_ERR_CONST_BADTYPE;
            }

            (*thread)->sp += 1;
            (*thread)->pc = pc_tmp;
        }
            break;

        case TO_TYPE: {
            ++(*thread)->pc;
            vm_to_type(&((*thread)->stack[(*thread)->sp - 1]), (*thread)->state->program[(*thread)->pc++]);
        }
            break;

        case DROP: {
            ++(*thread)->pc;
            vm_do_pop(*thread);
        }
            break;

        case HALT: {
            ++(*thread)->pc;
            (*thread)->exit_value = (*thread)->state->program[(*thread)->pc];
            err = VM_ERR_HALT;
        }
            break;

        default:
            err = VM_ERR_UNKNOWNOP;
    }

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
            ret = malloc(size);
        else
            ret = realloc(userdata, size);
    }

    return ret;
}

void vm_create_thread(vm_state_thread_t **thread) {
    (*thread) = calloc(1, sizeof(vm_state_thread_t));
    (*thread)->state = calloc(1, sizeof(vm_state_t));
    (*thread)->state->heap = vm_heap_create(1);
    (*thread)->halted = false;
    (*thread)->userdata = NULL;
    (*thread)->ctx.alloc = vm_alloc;

}

void vm_destroy_thread(vm_state_thread_t **thread) {
    for (uint32_t n = 0; n < VM_MAX_STRINGS; n++) {
        if ((*thread)->state->strings[n] != NULL)
            free((*thread)->state->strings[n]);
    }
    if ((*thread)->ctx.userdata != NULL)
        free((*thread)->ctx.userdata);

    for (uint32_t n = 0; n < (*thread)->state->heap->size; n++) {
        if ((*thread)->state->heap->data[n].type == VM_VAL_ARRAY)
            free((*thread)->state->heap->data[n].obj);
    }

    vm_heap_destroy((*thread)->state->heap);
    free((*thread)->state);
    free((*thread));

}

/////////////////// API ///////////////////

void vm_api_push(vm_state_thread_t **thread, vm_value_t value) {
    vm_do_push(*thread, value);
}

void vm_api_push_indir(vm_state_thread_t **thread, uint8_t nargs) {
    vm_do_push_indir(*thread, nargs);
}

vm_value_t vm_api_pop(vm_state_thread_t **thread) {
    return vm_do_pop(*thread);
}

void vm_api_pop_indir(vm_state_thread_t **thread) {
    vm_do_pop_indir(*thread);
}

uint8_t vm_api_read_byte(vm_state_thread_t **thread, uint32_t *pc) {
    return vm_read_byte((*thread)->state, pc);
}

int16_t vm_api_read_i16(vm_state_thread_t **thread, uint32_t *pc) {
    return vm_read_i16((*thread)->state, pc);
}

uint16_t vm_api_read_u16(vm_state_thread_t **thread, uint32_t *pc) {
    return vm_read_u16((*thread)->state, pc);
}

int32_t vm_api_read_i32(vm_state_thread_t **thread, uint32_t *pc) {
    return vm_read_i32((*thread)->state, pc);
}

uint32_t vm_api_read_u32(vm_state_thread_t **thread, uint32_t *pc) {
    return vm_read_u32((*thread)->state, pc);
}

float vm_api_read_f32(vm_state_thread_t **thread, uint32_t *pc) {
    return vm_read_f32((*thread)->state, pc);
}

vm_errors_t vm_api_create_string(vm_state_thread_t **thread, uint32_t index, const char *str) {
    if (str == NULL)
        return VM_ERR_FAIL;

    if ((*thread)->state->strings[index] != 0 || index > VM_MAX_STRINGS - 1)
        return VM_ERR_OUTOFRANGE;

    (*thread)->state->strings[index] = strdup(str);
    return VM_ERR_OK;
}
