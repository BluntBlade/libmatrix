#ifndef MATRIX_H
#define MATRIX_H 1

typedef enum MATRIX_OPTION_T {
    MTX_PLAIN_CODE = 0x0000,
    MTX_SIMD_CODE = 0x0001,
} mtx_option_t;

typedef size_t mtx_count_t;

typedef int32_t mtx_int32_t;
typedef uint32_t mtx_uint32_t;
typedef float mtx_float32_t;
typedef double mtx_double64_t;

typedef struct INT_SUBMATRIX_T {
    mtx_count_t row_cnt;
    mtx_count_t col_cnt;

    mtx_int32_t ** vals;
} int_submatrix_t, *p_int_submatrix_t;

struct MATRIX_T;
typedef struct MATRIX_T * p_matrix_t;

extern p_matrix_t mtx_i32_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt);
extern p_matrix_t mtx_u32_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt); /* TODO */
extern p_matrix_t mtx_f32_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt); /* TODO */
extern p_matrix_t mtx_d64_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt); /* TODO */

extern p_matrix_t mtx_allocate_before_multiply(p_matrix_t lhs, p_matrix_t rhs);
extern p_matrix_t mtx_allocate_before_transpose(p_matrix_t src);
extern p_matrix_t mtx_allocate_in_shape_of(p_matrix_t src);

extern p_matrix_t mtx_duplicate(p_matrix_t src);

extern void mtx_destroy(p_matrix_t mtx);

extern void mtx_initiate_identity(p_matrix_t mtx, mtx_option_t opt); /* TODO */
extern void mtx_initiate_zeros(p_matrix_t mtx, mtx_option_t opt); /* TODO */
extern void mtx_initiate_ones(p_matrix_t mtx, mtx_option_t opt); /* TODO */

extern int mtx_can_do_add(p_matrix_t lhs, p_matrix_t rhs);
extern int mtx_can_do_multiply(p_matrix_t lhs, p_matrix_t rhs);

extern void mtx_i32_set_at(p_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_int32_t src_val);
extern void mtx_i32_set_all_to(p_matrix_t mtx, mtx_int32_t src_val);
extern void mtx_i32_set_slice_to(p_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_int32_t src_vals[], mtx_count_t val_cnt);
extern void mtx_i32_set_from_array(p_matrix_t mtx, mtx_int32_t * src_vals[]);

extern p_matrix_t mtx_add_and_store(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt);
extern p_matrix_t mtx_sub_and_store(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt);
extern p_matrix_t mtx_multiply_and_store(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt);
extern p_matrix_t mtx_scalar_multiply_and_store(p_matrix_t mtx, int lhs, p_matrix_t rhs, mtx_option_t opt);

extern p_matrix_t mtx_transpose_and_store(p_matrix_t mtx, p_matrix_t src);

inline static p_matrix_t mtx_i32_create_zeros(mtx_count_t row_cnt, mtx_count_t col_cnt, mtx_option_t opt)
{
    p_matrix_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initiate_zeros(mtx, opt);
    return mtx;
} /* mtx_i32_create_zeros */

inline static p_matrix_t mtx_i32_create_ones(mtx_count_t row_cnt, mtx_count_t col_cnt, mtx_option_t opt)
{
    p_matrix_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initiate_ones(mtx, opt);
    return mtx;
} /* mtx_i32_create_ones */

inline static p_matrix_t mtx_i32_create_identity(mtx_count_t row_cnt, mtx_count_t col_cnt, mtx_option_t opt)
{
    p_matrix_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initiate_identity(mtx, opt);
    return mtx;
} /* mtx_i32_create_identity */

inline static int mtx_can_do_sub(p_matrix_t lhs, p_matrix_t rhs)
{
    return mtx_can_do_add(lhs, rhs);
} /* mtx_can_do_sub */

inline static p_matrix_t mtx_add(p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    p_matrix_t mtx = mtx_allocate_in_shape_of(lhs);
    if (! mtx) {
        return NULL;
    } /* if */
    return mtx_add_and_store(mtx, lhs, rhs, opt);
} /* mtx_add */

inline static p_matrix_t mtx_sub(p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    p_matrix_t mtx = mtx_allocate_in_shape_of(lhs);
    if (! mtx) {
        return NULL;
    } /* if */
    return mtx_sub_and_store(mtx, lhs, rhs, opt);
} /* mtx_sub */

inline static p_matrix_t mtx_multiply(p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    p_matrix_t mtx = mtx_allocate_before_multiply(lhs, rhs);
    if (! mtx) {
        return NULL;
    } /* if */
    return mtx_multiply_and_store(mtx, lhs, rhs, opt);
} /* mtx_multiply */

inline static p_matrix_t mtx_scalar_multiply(int lhs, p_matrix_t rhs, mtx_option_t opt)
{
    p_matrix_t mtx = mtx_allocate_in_shape_of(rhs);
    if (! mtx) {
        return NULL;
    } /* if */
    return mtx_scalar_multiply_and_store(mtx, lhs, rhs, opt);
} /* mtx_scalar_multiply */

inline static p_matrix_t mtx_transpose(p_matrix_t src)
{
    p_matrix_t mtx = mtx_allocate_before_transpose(src);
    return mtx_transpose_and_store(mtx, src);
} /* mtx_transpose */

#endif /* MATRIX_H */

