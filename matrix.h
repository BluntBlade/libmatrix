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

typedef struct SUBMATRIX_T {
    /*
        For referencing one element at (x,y) in the sub-matrix,
        use the following expression, where `ref` is the pointer
        to this struct.

        ref->i32_vals[ ref->row_off + x ][ ref->col_off + y ]
    */
    mtx_count_t row_off; /* Offset of the sub-matrix at row dimension. */
    mtx_count_t col_off; /* Offset of the sub-matrix at column dimension. */
    mtx_count_t row_cnt; /* Total count of rows of the sub-matrix. */
    mtx_count_t col_cnt; /* Total count of columns of the sub-matrix. */

    union {
        mtx_int32_t **    i32_vals;
        mtx_uint32_t **   u32_vals;
        mtx_float32_t **  f32_vals;
        mtx_double64_t ** d64_vals;
        void **           data;
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

extern mtx_count_t mtx_count_rows(ptr_matrix_t mtx);
extern mtx_count_t mtx_count_columns(ptr_matrix_t mtx);
extern mtx_count_t mtx_count_values(ptr_matrix_t mtx);

extern void mtx_add_and_store(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt);
extern void mtx_sub_and_store(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt);
extern void mtx_multiply_and_store(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt);

extern void mtx_transpose_and_store(ptr_matrix_t mtx, ptr_matrix_t src);

extern void mtx_get_submatrix(ptr_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_count_t row_cnt, mtx_count_t col_cnt, ptr_submatrix_t ref);

/* ==== Type-Related functions. ==== */

extern ptr_matrix_t mtx_i32_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt);

extern void mtx_i32_initialize_from_array(ptr_matrix_t mtx, mtx_int32_t * src_vals[]);

extern mtx_int32_t mtx_i32_get(ptr_matrix_t mtx, mtx_count_t row, mtx_count_t col);

extern void mtx_i32_set(ptr_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_int32_t src_val);
extern void mtx_i32_set_each(ptr_matrix_t mtx, mtx_int32_t src_val);
extern void mtx_i32_set_row_slice(ptr_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_int32_t src_vals[], mtx_count_t val_cnt);

extern void mtx_i32_scalar_multiply_and_store(ptr_matrix_t mtx, mtx_int32_t lhs, ptr_matrix_t rhs, mtx_option_t opt);

extern ptr_matrix_t mtx_u32_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt); /* TODO */
extern ptr_matrix_t mtx_f32_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt); /* TODO */
extern ptr_matrix_t mtx_d64_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt); /* TODO */

/* ==== Wrapper functions. ==== */

inline static ptr_matrix_t mtx_i32_create_zeros(mtx_count_t row_cnt, mtx_count_t col_cnt, mtx_option_t opt)
{
    ptr_matrix_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initialize_zeros(mtx, opt);
    return mtx;
} /* mtx_i32_create_zeros */

inline static ptr_matrix_t mtx_i32_create_ones(mtx_count_t row_cnt, mtx_count_t col_cnt, mtx_option_t opt)
{
    ptr_matrix_t mtx = mtx_i32_allocate(row_cnt, col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    mtx_initialize_ones(mtx, opt);
    return mtx;
} /* mtx_i32_create_ones */

inline static ptr_matrix_t mtx_i32_create_identity(mtx_count_t row_cnt, mtx_count_t col_cnt, mtx_option_t opt)
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

inline static ptr_matrix_t mtx_i32_scalar_multiply(mtx_int32_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
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

#endif /* MATRIX_H */

