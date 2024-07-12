#ifndef F32_MATRIX_H
#define F32_MATRIX_H 1

#include <stdbool.h>
#include <stdint.h>

// ==== Declarations of the f32_matrix_t type ====

struct F32_MATRIX;
typedef struct F32_MATRIX * f32_matrix_ptr;

extern f32_matrix_ptr f32m_allocate(uint32_t rows, uint32_t cols);
extern f32_matrix_ptr f32m_allocate_for_multiplying(f32_matrix_ptr lhs, f32_matrix_ptr rhs);
extern f32_matrix_ptr f32m_allocate_for_transposing(f32_matrix_ptr src);
extern f32_matrix_ptr f32m_allocate_in_shape_of(f32_matrix_ptr src);

extern f32_matrix_ptr f32m_duplicate(f32_matrix_ptr src);

extern void f32m_destroy(f32_matrix_ptr mx);

extern void f32m_init_identity(f32_matrix_ptr mx);
extern void f32m_init_zeros(f32_matrix_ptr mx);
extern void f32m_init_ones(f32_matrix_ptr mx);

extern uint32_t f32m_rows(f32_matrix_ptr mx);
extern uint32_t f32m_columns(f32_matrix_ptr mx);
extern uint32_t f32m_values(f32_matrix_ptr mx);

extern float f32m_get(f32_matrix_ptr mx, uint32_t row, uint32_t col);
extern void f32m_set(f32_matrix_ptr mx, uint32_t row, uint32_t col, float src_val);
extern void f32m_fill(f32_matrix_ptr mx, float src_val);

extern bool f32m_can_add(f32_matrix_ptr lhs, f32_matrix_ptr rhs);
extern bool f32m_can_multiply(f32_matrix_ptr lhs, f32_matrix_ptr rhs);

extern void f32m_transpose_and_store(f32_matrix_ptr mx, f32_matrix_ptr src);

extern void f32m_add_and_store(f32_matrix_ptr mx, f32_matrix_ptr lhs, f32_matrix_ptr rhs);
extern void f32m_subtract_and_store(f32_matrix_ptr mx, f32_matrix_ptr lhs, f32_matrix_ptr rhs);
extern void f32m_multiply_and_store(f32_matrix_ptr mx, f32_matrix_ptr lhs, f32_matrix_ptr rhs);

extern void f32m_multiply_scalar_and_store(f32_matrix_ptr mx, f32_matrix_ptr src, float val);

inline static f32_matrix_ptr f32m_create_zeros(uint32_t rows, uint32_t cols)
{
    f32_matrix_ptr mx = f32m_allocate(rows, cols);
    if (mx) {
        f32m_init_zeros(mx);
    } // if
    return mx;
} // f32m_create_zeros

inline static f32_matrix_ptr f32m_create_ones(uint32_t rows, uint32_t cols)
{
    f32_matrix_ptr mx = f32m_allocate(rows, cols);
    if (mx) {
        f32m_init_ones(mx);
    } // if
    return mx;
} // f32m_create_ones

inline static f32_matrix_ptr f32m_create_identity(uint32_t rows, uint32_t cols)
{
    f32_matrix_ptr mx = f32m_allocate(rows, cols);
    if (mx) {
        f32m_init_identity(mx);
    } // if
    return mx;
} // f32m_create_identity

inline static f32_matrix_ptr f32m_transpose(f32_matrix_ptr src)
{
    f32_matrix_ptr mx = f32m_allocate_for_transposing(src);
    if (mx) {
        f32m_transpose_and_store(mx, src);
    } // if
    return mx;
} // f32m_transpose

inline static bool f32m_can_sub(f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    return f32m_can_add(lhs, rhs);
} // f32m_can_sub

inline static f32_matrix_ptr f32m_add(f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    f32_matrix_ptr mx = f32m_allocate_in_shape_of(lhs);
    if (mx) {
        f32m_add_and_store(mx, lhs, rhs);
    } // if
    return mx;
} // f32m_add

inline static f32_matrix_ptr f32m_subtract(f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    f32_matrix_ptr mx = f32m_allocate_in_shape_of(lhs);
    if (mx) {
        f32m_subtract_and_store(mx, lhs, rhs);
    } // if
    return mx;
} // f32m_subtract

inline static f32_matrix_ptr f32m_multiply(f32_matrix_ptr lhs, f32_matrix_ptr rhs)
{
    f32_matrix_ptr mx = f32m_allocate_for_multiplying(lhs, rhs);
    if (mx) {
        f32m_multiply_and_store(mx, lhs, rhs);
    } // if
    return mx;
} // f32m_multiply

inline static f32_matrix_ptr f32m_multiply_scalar(f32_matrix_ptr src, float val)
{
    f32_matrix_ptr mx = f32m_allocate_in_shape_of(src);
    if (mx) {
        f32m_multiply_scalar_and_store(mx, src, val);
    } // if
    return mx;
} // f32m_scalar_multiply

#endif // F32_MATRIX_H

