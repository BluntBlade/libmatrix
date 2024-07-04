#ifndef I32_MATRIX_H
#define I32_MATRIX_H 1

#include <stdbool.h>
#include <stdint.h>

// ==== Declarations of the i32m_t type ====

struct I32_MATRIX;
typedef struct I32_MATRIX * i32_matrix_ptr;

extern i32_matrix_ptr i32m_allocate(uint32_t rows, uint32_t cols);
extern i32_matrix_ptr i32m_allocate_for_multiplying(i32_matrix_ptr lhs, i32_matrix_ptr rhs);
extern i32_matrix_ptr i32m_allocate_for_transposing(i32_matrix_ptr src);
extern i32_matrix_ptr i32m_allocate_in_shape_of(i32_matrix_ptr src);

extern i32_matrix_ptr i32m_duplicate(i32_matrix_ptr src);

extern void i32m_destroy(i32_matrix_ptr mx);

extern void i32m_init_identity(i32_matrix_ptr mx);
extern void i32m_init_zeros(i32_matrix_ptr mx);
extern void i32m_init_ones(i32_matrix_ptr mx);

extern uint32_t i32m_rows(i32_matrix_ptr mx);
extern uint32_t i32m_columns(i32_matrix_ptr mx);
extern uint32_t i32m_values(i32_matrix_ptr mx);

extern int32_t i32m_get(i32_matrix_ptr mx, uint32_t row, uint32_t col);
extern void i32m_set(i32_matrix_ptr mx, uint32_t row, uint32_t col, int32_t src_val);
extern void i32m_fill(i32_matrix_ptr mx, int32_t src_val);

extern bool i32m_can_add(i32_matrix_ptr lhs, i32_matrix_ptr rhs);
extern bool i32m_can_multiply(i32_matrix_ptr lhs, i32_matrix_ptr rhs);

extern void i32m_transpose_and_store(i32_matrix_ptr mx, i32_matrix_ptr src);

extern void i32m_add_and_store(i32_matrix_ptr mx, i32_matrix_ptr lhs, i32_matrix_ptr rhs);
extern void i32m_subtract_and_store(i32_matrix_ptr mx, i32_matrix_ptr lhs, i32_matrix_ptr rhs);
extern void i32m_multiply_and_store(i32_matrix_ptr mx, i32_matrix_ptr lhs, i32_matrix_ptr rhs);

extern void i32m_multiply_scalar_and_store(i32_matrix_ptr mx, i32_matrix_ptr src, int32_t val);

inline static i32_matrix_ptr i32m_create_zeros(uint32_t rows, uint32_t cols)
{
    i32_matrix_ptr mx = i32m_allocate(rows, cols);
    if (mx) {
        i32m_init_zeros(mx);
    } // if
    return mx;
} // i32m_create_zeros

inline static i32_matrix_ptr i32m_create_ones(uint32_t rows, uint32_t cols)
{
    i32_matrix_ptr mx = i32m_allocate(rows, cols);
    if (mx) {
        i32m_init_ones(mx);
    } // if
    return mx;
} // i32m_create_ones

inline static i32_matrix_ptr i32m_create_identity(uint32_t rows, uint32_t cols)
{
    i32_matrix_ptr mx = i32m_allocate(rows, cols);
    if (mx) {
        i32m_init_identity(mx);
    } // if
    return mx;
} // i32m_create_identity

inline static i32_matrix_ptr i32m_transpose(i32_matrix_ptr src)
{
    i32_matrix_ptr mx = i32m_allocate_for_transposing(src);
    if (mx) {
        i32m_transpose_and_store(mx, src);
    } // if
    return mx;
} // i32m_transpose

inline static bool i32m_can_sub(i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    return i32m_can_add(lhs, rhs);
} // i32m_can_sub

inline static i32_matrix_ptr i32m_add(i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    i32_matrix_ptr mx = i32m_allocate_in_shape_of(lhs);
    if (mx) {
        i32m_add_and_store(mx, lhs, rhs);
    } // if
    return mx;
} // i32m_add

inline static i32_matrix_ptr i32m_subtract(i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    i32_matrix_ptr mx = i32m_allocate_in_shape_of(lhs);
    if (mx) {
        i32m_subtract_and_store(mx, lhs, rhs);
    } // if
    return mx;
} // i32m_subtract

inline static i32_matrix_ptr i32m_multiply(i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    i32_matrix_ptr mx = i32m_allocate_for_multiplying(lhs, rhs);
    if (mx) {
        i32m_multiply_and_store(mx, lhs, rhs);
    } // if
    return mx;
} // i32m_multiply

inline static i32_matrix_ptr i32m_multiply_scalar(i32_matrix_ptr src, int32_t val)
{
    i32_matrix_ptr mx = i32m_allocate_in_shape_of(src);
    if (mx) {
        i32m_multiply_scalar_and_store(mx, src, val);
    } // if
    return mx;
} // i32m_scalar_multiply

#endif // I32_MATRIX_H

