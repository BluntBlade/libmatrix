#ifndef MATRIX_H
#define MATRIX_H 1

#include <stdint.h>

// ==== Declarations of common types ====

typedef enum MATRIX_OPTION_T {
    MTX_PLAIN_CODE = 0x0000,
    MTX_SIMD_CODE = 0x0001,
} mx_opt_t;

// ==== Declarations of the mi32_t type ====

struct I32_MATRIX_T;
typedef struct I32_MATRIX_T * p_i32m_t;

/* ==== Common functions. ==== */

extern p_i32m_t mtx_allocate_for_multiplying(p_i32m_t lhs, p_i32m_t rhs);
extern p_i32m_t mtx_allocate_for_transposing(p_i32m_t src);
extern p_i32m_t mtx_allocate_in_shape_of(p_i32m_t src);

extern p_i32m_t mtx_duplicate(p_i32m_t src);

extern void mtx_destroy(p_i32m_t mtx);

extern void mtx_initialize_identity(p_i32m_t mtx, mx_opt_t opt);
extern void mtx_initialize_zeros(p_i32m_t mtx, mx_opt_t opt);
extern void mtx_initialize_ones(p_i32m_t mtx, mx_opt_t opt);

extern int mtx_can_do_add(p_i32m_t lhs, p_i32m_t rhs);
extern int mtx_can_do_multiply(p_i32m_t lhs, p_i32m_t rhs);

extern unsigned int mtx_count_rows(p_i32m_t mtx);
extern unsigned int mtx_count_columns(p_i32m_t mtx);
extern unsigned int mtx_count_values(p_i32m_t mtx);

extern void mtx_transpose_and_store(p_i32m_t mtx, p_i32m_t src);

extern void mtx_add_and_store(p_i32m_t mtx, p_i32m_t lhs, p_i32m_t rhs, mx_opt_t opt);
extern void mtx_sub_and_store(p_i32m_t mtx, p_i32m_t lhs, p_i32m_t rhs, mx_opt_t opt);
extern void mtx_multiply_and_store(p_i32m_t mtx, p_i32m_t lhs, p_i32m_t rhs, mx_opt_t opt);

inline static int mtx_can_do_sub(p_i32m_t lhs, p_i32m_t rhs)
{
    return mtx_can_do_add(lhs, rhs);
} /* mtx_can_do_sub */

inline static p_i32m_t mtx_add(p_i32m_t lhs, p_i32m_t rhs, mx_opt_t opt)
{
    p_i32m_t mtx = mtx_allocate_in_shape_of(lhs);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_add_and_store(mtx, lhs, rhs, opt);
    return mtx;
} /* mtx_add */

inline static p_i32m_t mtx_sub(p_i32m_t lhs, p_i32m_t rhs, mx_opt_t opt)
{
    p_i32m_t mtx = mtx_allocate_in_shape_of(lhs);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_sub_and_store(mtx, lhs, rhs, opt);
    return mtx;
} /* mtx_sub */

inline static p_i32m_t mtx_multiply(p_i32m_t lhs, p_i32m_t rhs, mx_opt_t opt)
{
    p_i32m_t mtx = mtx_allocate_for_multiplying(lhs, rhs);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_multiply_and_store(mtx, lhs, rhs, opt);
    return mtx;
} /* mtx_multiply */

inline static p_i32m_t mtx_transpose(p_i32m_t src)
{
    p_i32m_t mtx = mtx_allocate_for_transposing(src);
    mtx_transpose_and_store(mtx, src);
    return mtx;
} /* mtx_transpose */

/* ==== Type-Related functions. ==== */

// ---- int32_t ----

extern p_i32m_t mtx_i32_allocate(unsigned int row_cnt, unsigned int col_cnt);

extern int32_t mtx_i32_get(p_i32m_t mtx, unsigned int row, unsigned int col);

extern void mtx_i32_set(p_i32m_t mtx, unsigned int row, unsigned int col, int32_t src_val);
extern void mtx_i32_set_each(p_i32m_t mtx, int32_t src_val);

extern void mtx_i32_scalar_multiply_and_store(p_i32m_t mtx, int32_t lhs, p_i32m_t rhs, mx_opt_t opt);

inline static p_i32m_t mtx_i32_create_zeros(unsigned int row_cnt, unsigned int col_cnt, mx_opt_t opt)
{
    p_i32m_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initialize_zeros(mtx, opt);
    return mtx;
} /* mtx_i32_create_zeros */

inline static p_i32m_t mtx_i32_create_ones(unsigned int row_cnt, unsigned int col_cnt, mx_opt_t opt)
{
    p_i32m_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initialize_ones(mtx, opt);
    return mtx;
} /* mtx_i32_create_ones */

inline static p_i32m_t mtx_i32_create_identity(unsigned int row_cnt, unsigned int col_cnt, mx_opt_t opt)
{
    p_i32m_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initialize_identity(mtx, opt);
    return mtx;
} /* mtx_i32_create_identity */

inline static p_i32m_t mtx_i32_scalar_multiply(int32_t lhs, p_i32m_t rhs, mx_opt_t opt)
{
    p_i32m_t mtx = mtx_allocate_in_shape_of(rhs);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_i32_scalar_multiply_and_store(mtx, lhs, rhs, opt);
    return mtx;
} /* mtx_i32_scalar_multiply */

#endif /* MATRIX_H */

