#include <memory.h>
#include <smmintrin.h>

#include "matrix.h"

#define IMTX_PACK_LENGTH 4

typedef int v4si __attribute__ ((vector_size (16)));

typedef struct MATRIX_T {
    mtx_count_t row_cnt;           /* The actual number of rows. */ 
    mtx_count_t col_cnt;           /* The actual number of columns. */
    mtx_count_t padded_row_cnt;    /* Round to the power of IMTX_PACK_LENGTH. */
    mtx_count_t padded_col_cnt;    /* Round to the power of IMTX_PACK_LENGTH. */
    mtx_count_t padded_byte_cnt;   /* Allocated bytes, including pads. */

    union {
       int *    padded_values;
       v4si *   packs;
    };

    mtx_count_t pack_cnt_per_row;
    
    int * values[1];
} matrix_t; 

typedef p_matrix_t (*mtx_matrix_operation_fn)(p_matrix_t, p_matrix_t, p_matrix_t);
typedef p_matrix_t (*mtx_scalar_operation_fn)(p_matrix_t, int, p_matrix_t);

inline static mtx_count_t mtx_round_to_multiples_of(mtx_count_t cnt, mtx_count_t n)
{
    return (cnt & ~(n - 1)) + (cnt & (n - 1)) ? n : 0;
} /* mtx_round_to_multiples_of */

p_matrix_t mtx_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt)
{
    p_matrix_t mtx = NULL;
    mtx_count_t i = 0;
    mtx_count_t padded_row_cnt = mtx_round_to_multiples_of(row_cnt, IMTX_PACK_LENGTH);

    mtx = calloc(sizeof(matrix_t) + sizeof(int *) * padded_row_cnt, 1);
    if (! mtx) {
        return NULL;
    } /* if */

    mtx->row_cnt = row_cnt;
    mtx->col_cnt = col_cnt;
    mtx->padded_row_cnt = padded_row_cnt;
    mtx->padded_col_cnt = mtx_round_to_multiples_of(col_cnt, IMTX_PACK_LENGTH);
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
} /* mtx_allocate */

p_matrix_t mtx_allocate_before_mulplicate(p_matrix_t lhs, p_matrix_t rhs)
{
    return mtx_allocate(lhs->row_cnt, rhs->col_cnt);
} /* mtx_allocate_before_mulplicate */

p_matrix_t mtx_allocate_before_transpose(p_matrix_t src)
{
    return mtx_allocate(src->col_cnt, src->row_cnt);
} /* mtx_allocate_before_transpose */

p_matrix_t mtx_allocate_in_shape_of(p_matrix_t src)
{
    return mtx_allocate(src->row_cnt, src->col_cnt);
} /* mtx_allocate_in_shape_of */

p_matrix_t mtx_create(mtx_count_t row_cnt, mtx_count_t col_cnt)
{
    p_matrix_t mtx = mtx_allocate(row_cnt, col_cnt);
    if (mtx) {
        memset(mtx->padded_values, 0, mtx->padded_byte_cnt);
    } /* if */
    return mtx;
} /* mtx_create */

p_matrix_t mtx_create_an_identity(mtx_count_t n)
{
    mtx_count_t i = 0;
    p_matrix_t mtx = mtx_zeros(n, n);
    if (! mtx) {
        return NULL;
    } /* if */

    for (i = 0; i < n; i += 1) {
        mtx->values[i][i] = 1;
    } /* for */
    return mtx;
} /* mtx_create_an_identity */

p_matrix_t mtx_duplicate(p_matrix_t src)
{
    p_matrix_t mtx = mtx_allocate(src->row_cnt, src->col_cnt);
    if (! mtx) {
        return NULL;
    } /* if */
    memcpy(mtx->padded_values, src->padded_values, src->padded_byte_cnt);
    return mtx;
} /* mtx_duplicate */

void mtx_destroy(p_matrix_t mtx)
{
    if (mtx) {
        free(mtx->padded_values);
    } /* if */
    free(mtx);
} /* mtx_destroy */

int mtx_can_do_add(p_matrix_t lhs, p_matrix_t rhs)
{
    return (lhs->row_cnt == rhs->row_cnt) && (lhs->col_cnt == rhs->col_cnt);
} /* mtx_can_do_add */

int mtx_can_do_multiply(p_matrix_t lhs, p_matrix_t rhs)
{
    return (lhs->col_cnt == rhs->row_cnt);
} /* mtx_can_do_multiply */

void mtx_set_at(p_matrix_t mtx, mtx_count_t row, mtx_count_t col, int src_val)
{
    mtx->values[row][col] = src_val;
} /* mtx_set_at */

void mtx_set_each_to(p_matrix_t mtx, int src_val)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->values[i][j] = src_val;
        } /* for */
    } /* for */
    return;
} /* mtx_set_each_to */

void mtx_set_slice_to(p_matrix_t mtx, mtx_count_t row, mtx_count_t col, int src_vals[], mtx_count_t val_cnt)
{
    mtx_count_t end = col + val_cnt;
    if (mtx->col_cnt < end) {
        /* Copy enough values from the source and don't cross the row boundary. */
        end = mtx->col_cnt;
    } /* if */
    memcpy(&mtx->values[row][col], src_vals, sizeof(mtx->padded_values[0]) * (end - col));
} /* mtx_set_slice_to */

void mtx_set_from_array(p_matrix_t mtx, int * src_vals[])
{
    mtx_count_t i = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        memcpy(mtx->values[i], src_vals[i], sizeof(mtx->padded_values[0]) * mtx->col_cnt);
    } /* for */
} /* mtx_set_from_array */

static p_matrix_t mtx_add_and_store_plain_impl(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < lhs->col_cnt; j += 1) {
            mtx->values[i][j] = lhs->values[i][j] + rhs->values[i][j];
        } /* for */
    } /* for */
    return mtx;
} /* mtx_add_and_store_plain_impl */

static mtx_matrix_operation_fn mtx_matrix_addition_ops[2] = {
    &mtx_add_and_store_plain_impl,
    &mtx_add_and_store_plain_impl
};

p_matrix_t mtx_add_and_store(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    return (*mtx_matrix_addition_ops[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_add_and_store */

static p_matrix_t mtx_sub_and_store_plain_impl(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < lhs->col_cnt; j += 1) {
            mtx->values[i][j] = lhs->values[i][j] - rhs->values[i][j];
        } /* for */
    } /* for */
    return mtx;
} /* mtx_sub_and_store_plain_impl */

static mtx_matrix_operation_fn mtx_matrix_subtraction_ops[2] = {
    &mtx_sub_and_store_plain_impl,
    &mtx_sub_and_store_plain_impl
};

p_matrix_t mtx_sub_and_store(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    return (*mtx_matrix_subtraction_ops[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_sub_and_store */

static p_matrix_t mtx_multiply_and_store_plain_impl(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    mtx_count_t k = 0;
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
} /* mtx_multiply_and_store_plain_impl */

inline static int mtx_sum(v4si * src)
{
    int * vals = (int *) src;
    return vals[0] + vals[1] + vals[2] + vals[3];
} /* mtx_sum */

static p_matrix_t mtx_multiply_and_store_simd_impl(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    mtx_count_t k = 0;
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
                mtx->values[i][j] += mtx_sum(&ret);
            } /* for */
        } /* for */
    } /* for */
    return mtx;
} /* mtx_multiply_and_store_simd_impl */

static mtx_matrix_operation_fn mtx_matrix_multiplication_ops[2] = {
    &mtx_multiply_and_store_plain_impl,
    &mtx_multiply_and_store_simd_impl
};

p_matrix_t mtx_multiply_and_store(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    return (*mtx_matrix_multiplication_ops[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_multiply_and_store */

static p_matrix_t mtx_scalar_multiply_and_store_plain_impl(p_matrix_t mtx, int lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;

    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->values[i][j] = lhs * rhs->values[i][j];
        } /* for j */
    } /* for i */
    return mtx;
} /* mtx_scalar_multiply_and_store_plain_impl */

p_matrix_t mtx_scalar_multiply_and_store_simd_impl(p_matrix_t mtx, int lhs, p_matrix_t rhs)
{
#undef IMTX_CALL_INTRINSIC_AND_STORE
#define IMTX_CALL_INTRINSIC_AND_STORE(func, pack, val) (pack) = func((pack), (val))

    v4si scalar = {0};
    mtx_count_t i = 0;
    mtx_count_t itr_cnt = 0;
    mtx_count_t rmd_cnt = 0;
    mtx_count_t valid_pack_cnt = mtx->row_cnt * mtx->pack_cnt_per_row;
    
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
} /* mtx_scalar_multiply_and_store_simd_impl */

static mtx_scalar_operation_fn mtx_scalar_multiplication_ops[2] = {
    &mtx_scalar_multiply_and_store_plain_impl,
    &mtx_scalar_multiply_and_store_simd_impl
};

p_matrix_t mtx_scalar_multiply_and_store(p_matrix_t mtx, int lhs, p_matrix_t rhs, mtx_option_t opt)
{
    return (*mtx_scalar_multiplication_ops[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_scalar_multiply_and_store */

p_matrix_t mtx_transpose_and_store(p_matrix_t mtx, p_matrix_t src)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;

    for (i = 0; i < src->row_cnt; i += 1) {
        for (j = 0; j < src->col_cnt; j += 1) {
            mtx->values[j][i] = src->values[i][j];
        } /* for j */
    } /* for i */
    return mtx;
} /* mtx_transpose_and_store */

