#ifndef INT_MATRIX_H
#define INT_MATRIX_H 1

typedef struct INT_SUB_MATRIX_T {
    unsigned int row_cnt;
    unsigned int col_cnt;

    int ** vals;
} int_sub_matrix_t, *p_int_sub_matrix_t;

struct INT_MATRIX_T;
typedef struct INT_MATRIX_T * p_int_matrix_t;

extern p_int_matrix_t imtx_allocate(unsigned int row_cnt, unsigned int col_cnt);
extern p_int_matrix_t imtx_allocate_before_mulplicate(p_int_matrix_t lhs, p_int_matrix_t rhs);
extern p_int_matrix_t imtx_allocate_before_transpose(p_int_matrix_t src);
extern p_int_matrix_t imtx_allocate_like_shape_of(p_int_matrix_t src);

extern p_int_matrix_t imtx_create(unsigned int row_cnt, unsigned int col_cnt);
extern p_int_matrix_t imtx_create_an_identity(unsigned int n);
extern p_int_matrix_t imtx_duplicate(p_int_matrix_t src);

extern void imtx_destroy(p_int_matrix_t mtx);

extern int imtx_can_do_multiply(p_int_matrix_t lhs, p_int_matrix_t rhs);

extern int imtx_get_at(p_int_matrix_t mtx, unsigned int row, unsigned int col);
extern void imtx_get_slice_at(p_int_matrix_t mtx, unsigned int row, unsigned int col, int ** vals, unsigned int * max_cnt);

extern void imtx_set_at(p_int_matrix_t mtx, unsigned int row, unsigned int col, int src_val);
extern void imtx_set_each_to(p_int_matrix_t mtx, int src_val);
extern void imtx_set_slice_to(p_int_matrix_t mtx, unsigned int row, unsigned int col, int src_vals[], unsigned int val_cnt);
extern void imtx_set_from_array(p_int_matrix_t mtx, int * src_vals[]);

extern p_int_matrix_t imtx_multiply_and_store(p_int_matrix_t mtx, p_int_matrix_t lhs, p_int_matrix_t rhs);

inline static p_int_matrix_t imtx_multiply(p_int_matrix_t lhs, p_int_matrix_t rhs)
{
    p_int_matrix_t mtx = imtx_allocate_before_mulplicate(lhs, rhs);
    if (! mtx) {
        return NULL;
    } /* if */
    return imtx_multiply_and_store(mtx, lhs, rhs);
} /* imtx_multiply */

extern p_int_matrix_t imtx_fast_multiply_and_store(p_int_matrix_t mtx, p_int_matrix_t lhs, p_int_matrix_t rhs);

extern p_int_matrix_t imtx_multiply_by_scalar_and_store(p_int_matrix_t mtx, int lhs, p_int_matrix_t rhs);

inline static p_int_matrix_t imtx_multiply_by_scalar(int lhs, p_int_matrix_t rhs)
{
    p_int_matrix_t mtx = imtx_allocate_like_shape_of(rhs);
    if (! mtx) {
        return NULL;
    } /* if */
    return imtx_multiply_by_scalar_and_store(mtx, lhs, rhs);
} /* imtx_multiply_by_scalar */

extern p_int_matrix_t imtx_fast_multiply_by_scalar_and_store(p_int_matrix_t mtx, int lhs, p_int_matrix_t rhs);

inline static p_int_matrix_t imtx_fast_multiply_by_scalar(int lhs, p_int_matrix_t rhs)
{
    p_int_matrix_t mtx = imtx_allocate_like_shape_of(rhs);
    if (! mtx) {
        return NULL;
    } /* if */
    return imtx_fast_multiply_by_scalar_and_store(mtx, lhs, rhs);
} /* imtx_fast_multiply_by_scalar */

inline static p_int_matrix_t imtx_zeros(unsigned int row_cnt, unsigned int col_cnt)
{
    return imtx_create(row_cnt, col_cnt);
} /* imtx_zeros */

inline static p_int_matrix_t imtx_ones(unsigned int row_cnt, unsigned int col_cnt)
{
    p_int_matrix_t mtx = imtx_create(row_cnt, col_cnt);
    if (mtx) {
        imtx_set_each_to(mtx, 1);
    } /* if */
    return mtx;
} /* imtx_ones */

extern p_int_matrix_t imtx_transpose_and_store(p_int_matrix_t mtx, p_int_matrix_t src);

inline static p_int_matrix_t imtx_transpose(p_int_matrix_t src)
{
    p_int_matrix_t mtx = imtx_allocate_before_transpose(src);
    return imtx_transpose_and_store(mtx, src);
} /* imtx_transpose */

#endif /* INT_MATRIX_H */

