/*  
    This file is part of Vigil.

    Vigil is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Vigil is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Vigil.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <misc/kcon.h>
#include <tools/panic.h>

#define MODULE_NAME "ubsan"

struct ubsan_type_descriptor {
    uint16_t type_kind;
    uint16_t type_info;
    char type_name[];
};

struct ubsan_source_location {
    const char *filename;
    uint32_t line;
    uint32_t column;
};

struct ubsan_overflow_data {
    struct ubsan_source_location loc;
    struct ubsan_type_descriptor *type;
};

struct ubsan_shift_out_of_bounds_data {
    struct ubsan_source_location loc;
    struct ubsan_type_descriptor *left_type;
    struct ubsan_type_descriptor *right_type;
};

struct ubsan_invalid_value_data {
    struct ubsan_source_location loc;
    struct ubsan_type_descriptor *type;
};

struct ubsan_array_out_of_bounds_data {
    struct ubsan_source_location loc;
    struct ubsan_type_descriptor *array_type;
    struct ubsan_type_descriptor *index_type;
};

struct ubsan_type_mismatch_v1_data {
    struct ubsan_source_location loc;
    struct ubsan_type_descriptor *type;
	unsigned char log_alignment;
	unsigned char type_check_kind;
};

struct ubsan_negative_vla_data {
    struct ubsan_source_location loc;
    struct ubsan_type_descriptor *type;
};

struct ubsan_nonnull_return_data {
    struct ubsan_source_location loc;
};

struct ubsan_nonnull_arg_data {
    struct ubsan_source_location loc;
};

struct ubsan_unreachable_data {
    struct ubsan_source_location loc;
};

struct ubsan_invalid_builtin_data {
    struct ubsan_source_location loc;
    unsigned char kind;
};

void __ubsan_handle_add_overflow(struct ubsan_overflow_data *data) {
    panic(MODULE_NAME, "Addition overflow at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
};

void __ubsan_handle_sub_overflow(struct ubsan_overflow_data *data) {
    panic(MODULE_NAME, "Substracion underflow at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column)
}

void __ubsan_handle_mul_overflow(struct ubsan_overflow_data *data) {
    panic(MODULE_NAME, "Multplication overflow at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column)
}

void __ubsan_handle_divrem_overflow(struct ubsan_overflow_data *data) {
    panic(MODULE_NAME, "Division overflow at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column)
}

void __ubsan_handle_negate_overflow(struct ubsan_overflow_data *data) {
    panic(MODULE_NAME, "Negation overflow at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_pointer_overflow(struct ubsan_overflow_data *data, uintptr_t base, uintptr_t result) {
    panic(MODULE_NAME, "Pointer overflow at %s:%d:%d\nBase pointer: %p\nResult: %p", data->loc.filename, data->loc.line, data->loc.column, base, result);
}

void __ubsan_handle_shift_out_of_bounds(struct ubsan_shift_out_of_bounds_data *data) {
    panic(MODULE_NAME, "Shift out of bounds at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_load_invalid_value(struct ubsan_invalid_value_data *data) {
    panic(MODULE_NAME, "Invalid value load at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_out_of_bounds(struct ubsan_array_out_of_bounds_data *data) {
    panic(MODULE_NAME, "Array out of bounds at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_type_mismatch_v1(struct ubsan_type_mismatch_v1_data *data, uintptr_t ptr) {
    if (!ptr) {
        panic(MODULE_NAME, "Use of null pointer at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
    } else if (ptr & ((1 << data->log_alignment) - 1)) {
        panic(MODULE_NAME, "Use of misaligned pointer at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
    } else {
        panic(MODULE_NAME, "No space for object at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
    }
}

void __ubsan_handle_vla_bound_not_positive(struct ubsan_negative_vla_data *data) {
    panic(MODULE_NAME, "Variable-length argument is negative at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_nonnull_return(struct ubsan_nonnull_return_data *data) {
    panic(MODULE_NAME, "Non-null return is null at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_nonnull_arg(struct ubsan_nonnull_arg_data *data) {
    panic(MODULE_NAME, "Non-null argument is null at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_builtin_unreachable(struct ubsan_unreachable_data *data) {
    panic(MODULE_NAME, "Unreachable code reached at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_invalid_builtin(struct ubsan_invalid_builtin_data *data) {
    panic(MODULE_NAME, "Invalid builtin at %s:%d:%d", data->loc.filename, data->loc.line, data->loc.column);
}
