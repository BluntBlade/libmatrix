#ifndef MATRIX_H
#define MATRIX_H 1

#include <stdint.h>

// ==== Declarations of common types ====

typedef enum MATRIX_OPTION_T {
    MX_PLAIN_CODE = 0x0000,
    MX_SIMD_CODE = 0x0001,
} mx_opt_t;

// ==== Declarations of the i32m_t type ====

struct I32_MATRIX_T;
typedef struct I32_MATRIX_T * i32_matrix_ptr;

/* ==== Common functions. ==== */

extern i32_matrix_ptr i32m_allocate(uint32_t row_cnt, uint32_t col_cnt);
extern i32_matrix_ptr i32m_allocate_for_multiplying(i32_matrix_ptr lhs, i32_matrix_ptr rhs);
extern i32_matrix_ptr i32m_allocate_for_transposing(i32_matrix_ptr src);
extern i32_matrix_ptr i32m_allocate_in_shape_of(i32_matrix_ptr src);

extern i32_matrix_ptr i32m_duplicate(i32_matrix_ptr src);

extern void i32m_destroy(i32_matrix_ptr m);

extern void i32m_init_identity(i32_matrix_ptr m, mx_opt_t opt);
extern void i32m_init_zeros(i32_matrix_ptr m, mx_opt_t opt);
extern void i32m_init_ones(i32_matrix_ptr m, mx_opt_t opt);

extern uint32_t i32m_rows(i32_matrix_ptr m);
extern uint32_t i32m_columns(i32_matrix_ptr m);
extern uint32_t i32m_values(i32_matrix_ptr m);

extern int32_t i32m_get(i32_matrix_ptr m, uint32_t row, uint32_t col);
extern void i32m_set(i32_matrix_ptr m, uint32_t row, uint32_t col, int32_t src_val);
extern void i32m_fill(i32_matrix_ptr m, int32_t src_val);

extern int i32m_can_add(i32_matrix_ptr lhs, i32_matrix_ptr rhs);
extern int i32m_can_multiply(i32_matrix_ptr lhs, i32_matrix_ptr rhs);

extern void i32m_transpose_and_store(i32_matrix_ptr m, i32_matrix_ptr src);

extern void i32m_add_and_store(i32_matrix_ptr m, i32_matrix_ptr lhs, i32_matrix_ptr rhs, mx_opt_t opt);
extern void i32m_sub_and_store(i32_matrix_ptr m, i32_matrix_ptr lhs, i32_matrix_ptr rhs, mx_opt_t opt);
extern void i32m_mul_and_store(i32_matrix_ptr m, i32_matrix_ptr lhs, i32_matrix_ptr rhs, mx_opt_t opt);

extern void i32m_scalar_multiply_and_store(i32_matrix_ptr m, int32_t lhs, i32_matrix_ptr rhs, mx_opt_t opt);

inline static i32_matrix_ptr i32m_create_zeros(uint32_t row_cnt, uint32_t col_cnt, mx_opt_t opt)
{
    i32_matrix_ptr m = i32m_allocate(row_cnt, col_cnt);
    if (m) {
        i32m_init_zeros(m, opt);
    } /* if */
    return m;
} /* i32m_create_zeros */

inline static i32_matrix_ptr i32m_create_ones(uint32_t row_cnt, uint32_t col_cnt, mx_opt_t opt)
{
    i32_matrix_ptr m = i32m_allocate(row_cnt, col_cnt);
    if (m) {
        i32m_init_ones(m, opt);
    } /* if */
    return m;
} /* i32m_create_ones */

inline static i32_matrix_ptr i32m_create_identity(uint32_t row_cnt, uint32_t col_cnt, mx_opt_t opt)
{
    i32_matrix_ptr m = i32m_allocate(row_cnt, col_cnt);
    if (m) {
        i32m_init_identity(m, opt);
    } /* if */
    return m;
} /* i32m_create_identity */

inline static i32_matrix_ptr i32m_transpose(i32_matrix_ptr src)
{
    i32_matrix_ptr m = i32m_allocate_for_transposing(src);
    if (m) {
        i32m_transpose_and_store(m, src);
    } // if
    return m;
} /* i32m_transpose */

inline static int i32m_can_sub(i32_matrix_ptr lhs, i32_matrix_ptr rhs)
{
    return i32m_can_add(lhs, rhs);
} /* i32m_can_sub */

inline static i32_matrix_ptr i32m_add(i32_matrix_ptr lhs, i32_matrix_ptr rhs, mx_opt_t opt)
{
    i32_matrix_ptr m = i32m_allocate_in_shape_of(lhs);
    if (m) {
        i32m_add_and_store(m, lhs, rhs, opt);
    } /* if */
    return m;
} /* i32m_add */

inline static i32_matrix_ptr i32m_sub(i32_matrix_ptr lhs, i32_matrix_ptr rhs, mx_opt_t opt)
{
    i32_matrix_ptr m = i32m_allocate_in_shape_of(lhs);
    if (m) {
        i32m_sub_and_store(m, lhs, rhs, opt);
    } /* if */
    return m;
} /* i32m_sub */

inline static i32_matrix_ptr i32m_multiply(i32_matrix_ptr lhs, i32_matrix_ptr rhs, mx_opt_t opt)
{
    i32_matrix_ptr m = i32m_allocate_for_multiplying(lhs, rhs);
    if (m) {
        i32m_mul_and_store(m, lhs, rhs, opt);
    } /* if */
    return m;
} /* i32m_multiply */

inline static i32_matrix_ptr i32m_scalar_multiply(int32_t lhs, i32_matrix_ptr rhs, mx_opt_t opt)
{
    i32_matrix_ptr m = i32m_allocate_in_shape_of(rhs);
    if (m) {
        i32m_scalar_multiply_and_store(m, lhs, rhs, opt);
    } /* if */
    return m;
} /* i32m_scalar_multiply */

#endif /* MATRIX_H */

