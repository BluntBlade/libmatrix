#include <memory.h>
#include <smmintrin.h>

#include "int_matrix.h"

#define IMTX_PACK_LENGTH 4

typedef int v4si __attribute__ ((vector_size (16)));

typedef struct INT_MATRIX_T {
    unsigned int row_cnt;           /* The actual number of rows. */ 
    unsigned int col_cnt;           /* The actual number of columns. */
    unsigned int padded_row_cnt;    /* Round to the power of IMTX_PACK_LENGTH. */
    unsigned int padded_col_cnt;    /* Round to the power of IMTX_PACK_LENGTH. */
    unsigned int padded_byte_cnt;   /* Allocated bytes, including pads. */

    union {
       int *    padded_values;
       v4si *   packs;
    };

    unsigned int pack_cnt_per_row;
    
    int * values[1];
} int_matrix_t; 

inline static unsigned int imtx_round_to_multiples_of(unsigned int cnt, unsigned int n)
{
    return (cnt & ~(n - 1)) + (cnt & (n - 1)) ? n : 0;
} /* imtx_round_to_multiples_of */

p_int_matrix_t imtx_allocate(unsigned int row_cnt, unsigned int col_cnt)
{
    p_int_matrix_t mtx = NULL;
    unsigned int i = 0;
    unsigned int padded_row_cnt = imtx_round_to_multiples_of(row_cnt, IMTX_PACK_LENGTH);

    mtx = calloc(sizeof(int_matrix_t) + sizeof(int *) * padded_row_cnt, 1);
    if (! mtx) {
        return NULL;
    } /* if */

    mtx->row_cnt = row_cnt;
    mtx->col_cnt = col_cnt;
    mtx->padded_row_cnt = padded_row_cnt;
    mtx->padded_col_cnt = imtx_round_to_multiples_of(col_cnt, IMTX_PACK_LENGTH);
    mtx->pack_cnt_per_row = mtx->padded_col_cnt / IMTX_PACK_LENGTH;
    mtx->padded_byte_cnt = sizeof(mtx->padded_values[0]) * mtx->padded_row_cnt * mtx->padded_col_cnt;

    mtx->padded_values = (int *)malloc(mtx->padded_byte_cnt);
    if (! mtx->padded_values) {
        free(mtx);
        return NULL;
    } /* if */

    for (i = 0; i < mtx->row_cnt; i += 1) {
        mtx->values[i] = &mtx->padded_values[i * mtx->padded_col_cnt];
    } /* for */

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

p_int_matrix_t imtx_allocate_in_shape_of(p_int_matrix_t src)
{
    return imtx_allocate(src->row_cnt, src->col_cnt);
} /* imtx_allocate_in_shape_of */

p_int_matrix_t imtx_create(unsigned int row_cnt, unsigned int col_cnt)
{
    p_int_matrix_t mtx = imtx_allocate(row_cnt, col_cnt);
    if (mtx) {
        memset(mtx->padded_values, 0, mtx->padded_byte_cnt);
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
        mtx->values[i][i] = 1;
    } /* for */
    return mtx;
} /* imtx_create_an_identity */

p_int_matrix_t imtx_duplicate(p_int_matrix_t src)
{
    p_int_matrix_t mtx = imtx_allocate(src->row_cnt, src->col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    memcpy(mtx->padded_values, src->padded_values, src->padded_byte_cnt);
    return mtx;
} /* imtx_duplicate */

void imtx_destroy(p_int_matrix_t mtx)
{
    if (mtx) {
        free(mtx->padded_values);
    } /* if */
    free(mtx);
} /* imtx_destroy */

int imtx_can_do_add(p_int_matrix_t lhs, p_int_matrix_t rhs)
{
    return (lhs->row_cnt == rhs->row_cnt) && (lhs->col_cnt == rhs->col_cnt);
} /* imtx_can_do_add */

int imtx_can_do_multiply(p_int_matrix_t lhs, p_int_matrix_t rhs)
{
    return (lhs->col_cnt == rhs->row_cnt);
} /* imtx_can_do_multiply */

void imtx_set_at(p_int_matrix_t mtx, unsigned int row, unsigned int col, int src_val)
{
    mtx->values[row][col] = src_val;
} /* imtx_set_at */

void imtx_set_each_to(p_int_matrix_t mtx, int src_val)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->values[i][j] = src_val;
        } /* for */
    } /* for */
    return;
} /* imtx_set_each_to */

void imtx_set_slice_to(p_int_matrix_t mtx, unsigned int row, unsigned int col, int src_vals[], unsigned int val_cnt)
{
    unsigned int end = col + val_cnt;
    if (mtx->col_cnt < end) {
        /* Copy enough values from the source and don't cross the row boundary. */
        end = mtx->col_cnt;
    } /* if */
    memcpy(&mtx->values[row][col], src_vals, sizeof(mtx->padded_values[0]) * (end - col));
} /* imtx_set_slice_to */

void imtx_set_from_array(p_int_matrix_t mtx, int * src_vals[])
{
    unsigned int i = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        memcpy(mtx->values[i], src_vals[i], sizeof(mtx->padded_values[0]) * mtx->col_cnt);
    } /* for */
} /* imtx_set_from_array */

p_int_matrix_t imtx_add_and_store(p_int_matrix_t mtx, p_int_matrix_t lhs, p_int_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < lhs->col_cnt; j += 1) {
            mtx->values[i][j] = lhs->values[i][j] + rhs->values[i][j];
        } /* for */
    } /* for */
    return mtx;
} /* imtx_add_and_store */

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
                sum += lhs->values[i][k] * rhs->values[k][j];
            } /* for k */
            mtx->values[i][j] = sum;
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

    for (k = 0; k < rhs->padded_row_cnt; k += IMTX_PACK_LENGTH) {
        for (j = 0; j < rhs->col_cnt; j += 1) {
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->values[k + 0][j], 0);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->values[k + 1][j], 1);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->values[k + 2][j], 2);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->values[k + 3][j], 3);

            for (i = 0; i < lhs->row_cnt; i += 1) {
                pack_lhs = lhs->packs[i * lhs->pack_cnt_per_row + k / IMTX_PACK_LENGTH];
                ret = __builtin_ia32_pmuldq128(pack_lhs, pack_rhs);
                mtx->values[i][j] += imtx_sum(&ret);
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
    unsigned int valid_pack_cnt = mtx->row_cnt * mtx->pack_cnt_per_row;
    
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 0);
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 1);
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 2);
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 3);

    itr_cnt = valid_pack_cnt / 8;
    rmd_cnt = valid_pack_cnt % 8;

    for (i = 0; i < itr_cnt; i += 1) {
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
            mtx->values[i][j] = lhs * rhs->values[i][j];
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
            mtx->values[j][i] = src->values[i][j];
        } /* for j */
    } /* for i */
    return mtx;
} /* imtx_transpose_and_store */

