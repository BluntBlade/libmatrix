#include <memory.h>
#include <smmintrin.h>

#include "int_matrix.h"

#define IMTX_PACK_LENGTH 4

typedef int v4si __attribute__ ((vector_size (16)));

struct INT_MATRIX_T {
    unsigned int row_cnt;
    unsigned int col_cnt;
    unsigned int row_cnt_rounded;   /* Round to the power of IMTX_PACK_LENGTH (4). */
    unsigned int col_cnt_rounded;   /* Round to the power of IMTX_PACK_LENGTH (4). */
    unsigned int pack_cnt_per_row;
    unsigned int pack_cnt;
    unsigned int byte_cnt;

    union {
       int *    vals;
       v4si *   packs;
    };
}; 

typedef struct INT_MATRIX_T int_matrix_t;

inline static unsigned int imtx_round_to_multiples_of(unsigned int cnt, unsigned int un)
{
    return (cnt & ~(un - 1)) + (cnt & (un - 1)) ? un : 0;
} /* imtx_round_to_multiples_of */

p_int_matrix_t imtx_allocate(unsigned int row_cnt, unsigned int col_cnt)
{
    p_int_matrix_t mtx = NULL;

    mtx = calloc(sizeof(int_matrix_t), 1);
    if (! mtx) {
        return NULL;
    } /* if */

    mtx->row_cnt = row_cnt;
    mtx->col_cnt = col_cnt;
    mtx->row_cnt_rounded = imtx_round_to_multiples_of(row_cnt, IMTX_PACK_LENGTH);
    mtx->col_cnt_rounded = imtx_round_to_multiples_of(col_cnt, IMTX_PACK_LENGTH);
    mtx->pack_cnt_per_row = mtx->col_cnt_rounded / IMTX_PACK_LENGTH;
    mtx->pack_cnt = mtx->row_cnt * mtx->pack_cnt_per_row;
    mtx->byte_cnt = sizeof(mtx->vals[0]) * mtx->row_cnt_rounded * mtx->col_cnt_rounded;

    mtx->vals = (int *)malloc(mtx->byte_cnt);
    if (! mtx->vals) {
        free(mtx);
        return NULL;
    } /* if */

    return mtx;
} /* imtx_allocate */

p_int_matrix_t imtx_allocate_before_mulplicate(p_int_matrix_t lhs, p_int_matrix_t rhs)
{
    return imtx_allocate(lhs->row_cnt, rhs->col_cnt);
} /* imtx_allocate_before_mulplicate */

p_int_matrix_t imtx_allocate_before_transpose(p_int_matrix_t src)
{
    return imtx_allocate(src->col_cnt, src->row_cnt);
} /* imtx_allocate_before_transpose */

p_int_matrix_t imtx_allocate_like_shape_of(p_int_matrix_t src)
{
    p_int_matrix_t mtx = imtx_allocate(src->row_cnt, src->col_cnt);
    return mtx;
} /* imtx_allocate_like_shape_of */

p_int_matrix_t imtx_create(unsigned int row_cnt, unsigned int col_cnt)
{
    p_int_matrix_t mtx = imtx_allocate(row_cnt, col_cnt);
    if (mtx) {
        memset(mtx->vals, 0, mtx->byte_cnt);
    } /* if */
    return mtx;
} /* imtx_create */

p_int_matrix_t imtx_create_an_identity(unsigned int n)
{
    unsigned int i = 0;
    p_int_matrix_t mtx = imtx_zeros(n, n);
    if (! mtx) {
        return NULL;
    } /* if */

    for (i = 0; i < n; i += 1) {
        mtx->vals[i * mtx->row_cnt + i] = 1;
    } /* for */
    return mtx;
} /* imtx_create_an_identity */

p_int_matrix_t imtx_duplicate(p_int_matrix_t src)
{
    p_int_matrix_t dup = imtx_allocate(src->row_cnt, src->col_cnt);
    if (! dup) {
        return NULL;
    } /* if */

    dup->row_cnt = src->row_cnt;
    dup->col_cnt = src->col_cnt;
    dup->row_cnt_rounded = src->row_cnt_rounded;
    dup->col_cnt_rounded = src->col_cnt_rounded;
    dup->pack_cnt_per_row = src->pack_cnt_per_row;
    dup->pack_cnt = src->pack_cnt;
    dup->byte_cnt = src->byte_cnt;

    memcpy(dup->vals, src->vals, src->byte_cnt);

    return dup;
} /* imtx_duplicate */

void imtx_destroy(p_int_matrix_t mtx)
{
    if (mtx) {
        free(mtx->vals);
    } /* if */
    free(mtx);
} /* imtx_destroy */

int imtx_can_do_multiply(p_int_matrix_t lhs, p_int_matrix_t rhs)
{
    return (lhs->col_cnt == rhs->row_cnt);
} /* imtx_can_do_multiply */

void imtx_set_at(p_int_matrix_t mtx, unsigned int row, unsigned int col, int val)
{
    mtx->vals[row * mtx->col_cnt + col] = val;
} /* imtx_set_at */

void imtx_set_each_to(p_int_matrix_t mtx, int val)
{
    unsigned int i = 0;
    for (i = 0; i < mtx->row_cnt * mtx->col_cnt; i += 1) {
        mtx->vals[i] = val;
    } /* for */
    return;
} /* imtx_set_each_to */

void imtx_set_slice_to(p_int_matrix_t mtx, unsigned int row, unsigned int col, int vals[], unsigned val_cnt)
{
    unsigned int end = col + val_cnt;
    if (mtx->col_cnt < end) {
        /* Copy enough values from the source and don't cross the row boundary. */
        end = mtx->col_cnt;
    } /* if */
    memcpy(&mtx->vals[row * mtx->col_cnt + col], vals, sizeof(mtx->vals[0]) * (end - col));
} /* imtx_set_slice_to */

void imtx_set_from_array(p_int_matrix_t mtx, int * vals[])
{
    unsigned int i = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        memcpy(&mtx->vals[i * mtx->col_cnt], vals[i], sizeof(mtx->vals[0]) * mtx->col_cnt);
    } /* for */
} /* imtx_set_from_array */

p_int_matrix_t imtx_multiply_and_store(p_int_matrix_t mtx, p_int_matrix_t lhs, p_int_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    int sum = 0;

    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < rhs->col_cnt; j += 1) {
            sum = 0;
            for (k = 0; k < lhs->col_cnt; k += 1) {
                sum += lhs->vals[i * lhs->col_cnt + k] * rhs->vals[k * rhs->col_cnt + j];
            } /* for k */
            mtx->vals[i * rhs->col_cnt + j] = sum;
        } /* for j */
    } /* for i */
    return mtx;
} /* imtx_multiply_and_store */

inline static int imtx_sum(v4si * src)
{
    int * vals = (int *) src;
    return vals[0] + vals[1] + vals[2] + vals[3];
} /* imtx_sum */

p_int_matrix_t imtx_fast_multiply_and_store(p_int_matrix_t mtx, p_int_matrix_t lhs, p_int_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    v4si pack_lhs = {0};
    v4si pack_rhs = {0};
    v4si ret = {0};

    for (k = 0; k < rhs->row_cnt_rounded; k += IMTX_PACK_LENGTH) {
        for (j = 0; j < rhs->col_cnt; j += 1) {
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->vals[(k + 0) * rhs->col_cnt_rounded + j], 0);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->vals[(k + 1) * rhs->col_cnt_rounded + j], 1);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->vals[(k + 2) * rhs->col_cnt_rounded + j], 2);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->vals[(k + 3) * rhs->col_cnt_rounded + j], 3);

            for (i = 0; i < lhs->row_cnt; i += 1) {
                pack_lhs = lhs->packs[i * lhs->pack_cnt_per_row + k / IMTX_PACK_LENGTH];
                ret = __builtin_ia32_pmuldq128(pack_lhs, pack_rhs);
                mtx->vals[i * rhs->col_cnt_rounded + j] += imtx_sum(&ret);
            } /* for */
        } /* for */
    } /* for */
    return mtx;
} /* imtx_fast_multiply_and_store */

p_int_matrix_t imtx_fast_multiply_by_scalar_and_store(p_int_matrix_t mtx, int lhs, p_int_matrix_t rhs)
{
#undef IMTX_CALL_INTRINSIC_AND_STORE
#define IMTX_CALL_INTRINSIC_AND_STORE(func, pack, val) (pack) = func((pack), (val))

    v4si scalar = {0};
    unsigned int i = 0;
    unsigned int itr_cnt = 0;
    unsigned int rmd_cnt = 0;
    
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 0);
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 1);
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 2);
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 3);

    itr_cnt = rhs->pack_cnt / 8;
    rmd_cnt = rhs->pack_cnt % 8;

    for (i = 0; i < rhs->pack_cnt; i += 8) {
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 0], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 1], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 2], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 3], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 4], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 5], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 6], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 7], scalar);
    } /* for i */

    switch (rmd_cnt) {
        case 7: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 6], scalar);
        case 6: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 5], scalar);
        case 5: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 4], scalar);
        case 4: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 3], scalar);
        case 3: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 2], scalar);
        case 2: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 1], scalar);
        case 1: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->packs[i + 0], scalar);
        default: break;
    } /* switch */
    return mtx;
#undef IMTX_CALL_INTRINSIC_AND_STORE
} /* imtx_fast_multiply_by_scalar_and_store */

p_int_matrix_t imtx_multiply_by_scalar_and_store(p_int_matrix_t mtx, int lhs, p_int_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;

    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->vals[i * mtx->col_cnt + j] = lhs * rhs->vals[i * mtx->col_cnt + j];
        } /* for j */
    } /* for i */
    return mtx;
} /* imtx_multiply_by_scalar_and_store */

p_int_matrix_t imtx_transpose_and_store(p_int_matrix_t mtx, p_int_matrix_t src)
{
    unsigned int i = 0;
    unsigned int j = 0;

    for (i = 0; i < src->row_cnt; i += 1) {
        for (j = 0; j < src->col_cnt; j += 1) {
            mtx->vals[j * src->row_cnt + i] = src->vals[i * src->col_cnt + j];
        } /* for j */
    } /* for i */
    return mtx;
} /* imtx_transpose_and_store */

