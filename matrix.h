#ifndef MATRIX_H
#define MATRIX_H 1

#include <stdint.h>

typedef enum MATRIX_OPTION_T {
    MTX_PLAIN_CODE = 0x0000,
    MTX_SIMD_CODE = 0x0001,
} mtx_option_t;

typedef struct SUBMATRIX_T {
    /*
        For referencing one element at (x,y) in the sub-matrix,
        use the following expression, where `ref` is the pointer
        to this struct.

        ref->i32_vals[ ref->row_off + x ][ ref->col_off + y ]
    */
    unsigned int row_off; /* Offset of the sub-matrix at row dimension. */
    unsigned int col_off; /* Offset of the sub-matrix at column dimension. */
    unsigned int row_cnt; /* Total count of rows of the sub-matrix. */
    unsigned int col_cnt; /* Total count of columns of the sub-matrix. */

    union {
        int32_t **  i32_vals;
        uint32_t ** u32_vals;
        float **    f32_vals;
        double **   d64_vals;
        void **     val_ptrs;
    };
} submatrix_t, *ptr_submatrix_t;

struct MATRIX_T;
typedef struct MATRIX_T * ptr_matrix_t;

/* ==== Common functions. ==== */

extern ptr_matrix_t mtx_allocate_for_multiplying(ptr_matrix_t lhs, ptr_matrix_t rhs);
extern ptr_matrix_t mtx_allocate_for_transposing(ptr_matrix_t src);
extern ptr_matrix_t mtx_allocate_in_shape_of(ptr_matrix_t src);

extern ptr_matrix_t mtx_duplicate(ptr_matrix_t src);

extern void mtx_destroy(ptr_matrix_t mtx);

extern void mtx_initialize_identity(ptr_matrix_t mtx, mtx_option_t opt);
extern void mtx_initialize_zeros(ptr_matrix_t mtx, mtx_option_t opt);
extern void mtx_initialize_ones(ptr_matrix_t mtx, mtx_option_t opt);

extern int mtx_can_do_add(ptr_matrix_t lhs, ptr_matrix_t rhs);
extern int mtx_can_do_multiply(ptr_matrix_t lhs, ptr_matrix_t rhs);

extern unsigned int mtx_count_rows(ptr_matrix_t mtx);
extern unsigned int mtx_count_columns(ptr_matrix_t mtx);
extern unsigned int mtx_count_values(ptr_matrix_t mtx);

extern void mtx_get_submatrix(ptr_matrix_t mtx, unsigned int row, unsigned int col, unsigned int row_cnt, unsigned int col_cnt, ptr_submatrix_t ref);

extern void mtx_transpose_and_store(ptr_matrix_t mtx, ptr_matrix_t src);

extern void mtx_add_and_store(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt);
extern void mtx_sub_and_store(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt);
extern void mtx_multiply_and_store(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt);

/* ==== Type-Related functions. ==== */

extern ptr_matrix_t mtx_i32_allocate(unsigned int row_cnt, unsigned int col_cnt);

extern int32_t mtx_i32_get(ptr_matrix_t mtx, unsigned int row, unsigned int col);

extern void mtx_i32_set(ptr_matrix_t mtx, unsigned int row, unsigned int col, int32_t src_val);
extern void mtx_i32_set_each(ptr_matrix_t mtx, int32_t src_val);

extern void mtx_i32_scalar_multiply_and_store(ptr_matrix_t mtx, int32_t lhs, ptr_matrix_t rhs, mtx_option_t opt);

extern ptr_matrix_t mtx_u32_allocate(unsigned int row_cnt, unsigned int col_cnt); /* TODO */
extern ptr_matrix_t mtx_f32_allocate(unsigned int row_cnt, unsigned int col_cnt); /* TODO */
extern ptr_matrix_t mtx_d64_allocate(unsigned int row_cnt, unsigned int col_cnt); /* TODO */

/* ==== Wrapper functions. ==== */

inline static ptr_matrix_t mtx_i32_create_zeros(unsigned int row_cnt, unsigned int col_cnt, mtx_option_t opt)
{
    ptr_matrix_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initialize_zeros(mtx, opt);
    return mtx;
} /* mtx_i32_create_zeros */

inline static ptr_matrix_t mtx_i32_create_ones(unsigned int row_cnt, unsigned int col_cnt, mtx_option_t opt)
{
    ptr_matrix_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initialize_ones(mtx, opt);
    return mtx;
} /* mtx_i32_create_ones */

inline static ptr_matrix_t mtx_i32_create_identity(unsigned int row_cnt, unsigned int col_cnt, mtx_option_t opt)
{
    ptr_matrix_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initialize_identity(mtx, opt);
    return mtx;
} /* mtx_i32_create_identity */

inline static int mtx_can_do_sub(ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    return mtx_can_do_add(lhs, rhs);
} /* mtx_can_do_sub */

inline static ptr_matrix_t mtx_add(ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    ptr_matrix_t mtx = mtx_allocate_in_shape_of(lhs);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_add_and_store(mtx, lhs, rhs, opt);
    return mtx;
} /* mtx_add */

inline static ptr_matrix_t mtx_sub(ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    ptr_matrix_t mtx = mtx_allocate_in_shape_of(lhs);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_sub_and_store(mtx, lhs, rhs, opt);
    return mtx;
} /* mtx_sub */

inline static ptr_matrix_t mtx_multiply(ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    ptr_matrix_t mtx = mtx_allocate_for_multiplying(lhs, rhs);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_multiply_and_store(mtx, lhs, rhs, opt);
    return mtx;
} /* mtx_multiply */

inline static ptr_matrix_t mtx_i32_scalar_multiply(int32_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    ptr_matrix_t mtx = mtx_allocate_in_shape_of(rhs);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_i32_scalar_multiply_and_store(mtx, lhs, rhs, opt);
    return mtx;
} /* mtx_i32_scalar_multiply */

inline static ptr_matrix_t mtx_transpose(ptr_matrix_t src)
{
    ptr_matrix_t mtx = mtx_allocate_for_transposing(src);
    mtx_transpose_and_store(mtx, src);
    return mtx;
} /* mtx_transpose */

inline static void smtx_i32_set(ptr_submatrix_t ref, unsigned int row, unsigned int col, int32_t val)
{
    ref->i32_vals[ref->row_off + row][ref->col_off + col] = val;
} /* smtx_i32_set */

#endif /* MATRIX_H */

