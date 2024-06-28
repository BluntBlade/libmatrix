#ifndef MATRIX_H
#define MATRIX_H 1

#include <stdint.h>

// ==== Declarations of common types ====

typedef enum MATRIX_OPTION_T {
    MX_PLAIN_CODE = 0x0000,
    MX_SIMD_CODE = 0x0001,
} mx_opt_t;

// ==== Declarations of the mi32_t type ====

struct MATRIX_I32_T;
typedef struct MATRIX_I32_T * p_mi32_t;

/* ==== Common functions. ==== */

extern p_mi32_t mi32_allocate(uint32_t row_cnt, uint32_t col_cnt);
extern p_mi32_t mi32_allocate_for_multiplying(p_mi32_t lhs, p_mi32_t rhs);
extern p_mi32_t mi32_allocate_for_transposing(p_mi32_t src);
extern p_mi32_t mi32_allocate_in_shape_of(p_mi32_t src);

extern p_mi32_t mi32_duplicate(p_mi32_t src);

extern void mi32_destroy(p_mi32_t m);

extern void mi32_init_identity(p_mi32_t m, mx_opt_t opt);
extern void mi32_init_zeros(p_mi32_t m, mx_opt_t opt);
extern void mi32_init_ones(p_mi32_t m, mx_opt_t opt);

extern uint32_t mi32_rows(p_mi32_t m);
extern uint32_t mi32_columns(p_mi32_t m);
extern uint32_t mi32_values(p_mi32_t m);

extern int32_t mi32_get(p_mi32_t m, uint32_t row, uint32_t col);
extern void mi32_set(p_mi32_t m, uint32_t row, uint32_t col, int32_t src_val);
extern void mi32_set_each(p_mi32_t m, int32_t src_val);

extern int mi32_can_do_add(p_mi32_t lhs, p_mi32_t rhs);
extern int mi32_can_do_multiply(p_mi32_t lhs, p_mi32_t rhs);

extern void mi32_transpose_and_store(p_mi32_t m, p_mi32_t src);

extern void mi32_add_and_store(p_mi32_t m, p_mi32_t lhs, p_mi32_t rhs, mx_opt_t opt);
extern void mi32_sub_and_store(p_mi32_t m, p_mi32_t lhs, p_mi32_t rhs, mx_opt_t opt);
extern void mi32_mul_and_store(p_mi32_t m, p_mi32_t lhs, p_mi32_t rhs, mx_opt_t opt);

extern void mi32_scalar_multiply_and_store(p_mi32_t m, int32_t lhs, p_mi32_t rhs, mx_opt_t opt);

inline static p_mi32_t mi32_create_zeros(uint32_t row_cnt, uint32_t col_cnt, mx_opt_t opt)
{
    p_mi32_t m = mi32_allocate(row_cnt, col_cnt);
    if (m) {
        mi32_init_zeros(m, opt);
    } /* if */
    return m;
} /* mi32_create_zeros */

inline static p_mi32_t mi32_create_ones(uint32_t row_cnt, uint32_t col_cnt, mx_opt_t opt)
{
    p_mi32_t m = mi32_allocate(row_cnt, col_cnt);
    if (m) {
        mi32_init_ones(m, opt);
    } /* if */
    return m;
} /* mi32_create_ones */

inline static p_mi32_t mi32_create_identity(uint32_t row_cnt, uint32_t col_cnt, mx_opt_t opt)
{
    p_mi32_t m = mi32_allocate(row_cnt, col_cnt);
    if (m) {
        mi32_init_identity(m, opt);
    } /* if */
    return m;
} /* mi32_create_identity */

inline static p_mi32_t mi32_transpose(p_mi32_t src)
{
    p_mi32_t m = mi32_allocate_for_transposing(src);
    if (m) {
        mi32_transpose_and_store(m, src);
    } // if
    return m;
} /* mi32_transpose */

inline static int mi32_can_do_sub(p_mi32_t lhs, p_mi32_t rhs)
{
    return mi32_can_do_add(lhs, rhs);
} /* mi32_can_do_sub */

inline static p_mi32_t mi32_add(p_mi32_t lhs, p_mi32_t rhs, mx_opt_t opt)
{
    p_mi32_t m = mi32_allocate_in_shape_of(lhs);
    if (m) {
        mi32_add_and_store(m, lhs, rhs, opt);
    } /* if */
    return m;
} /* mi32_add */

inline static p_mi32_t mi32_sub(p_mi32_t lhs, p_mi32_t rhs, mx_opt_t opt)
{
    p_mi32_t m = mi32_allocate_in_shape_of(lhs);
    if (m) {
        mi32_sub_and_store(m, lhs, rhs, opt);
    } /* if */
    return m;
} /* mi32_sub */

inline static p_mi32_t mi32_multiply(p_mi32_t lhs, p_mi32_t rhs, mx_opt_t opt)
{
    p_mi32_t m = mi32_allocate_for_multiplying(lhs, rhs);
    if (m) {
        mi32_mul_and_store(m, lhs, rhs, opt);
    } /* if */
    return m;
} /* mi32_multiply */

inline static p_mi32_t mi32_scalar_multiply(int32_t lhs, p_mi32_t rhs, mx_opt_t opt)
{
    p_mi32_t m = mi32_allocate_in_shape_of(rhs);
    if (m) {
        mi32_scalar_multiply_and_store(m, lhs, rhs, opt);
    } /* if */
    return m;
} /* mi32_scalar_multiply */

#endif /* MATRIX_H */

