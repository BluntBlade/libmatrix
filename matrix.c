#include <memory.h>
#include <smmintrin.h>

#include "matrix.h"

enum {
    MTX_I32_PACK_LENGTH = 4,
    MTX_U32_PACK_LENGTH = 4,
    MTX_F32_PACK_LENGTH = 4,
    MTX_D64_PACK_LENGTH = 2
};

typedef mtx_int32_t mtx_v4si_t __attribute__ ((vector_size (16)));

typedef void (*mtx_matrix_initiation_fn)(p_matrix_t);
typedef p_matrix_t (*mtx_matrix_operation_fn)(p_matrix_t, p_matrix_t, p_matrix_t);
typedef p_matrix_t (*mtx_i32_scalar_operation_fn)(p_matrix_t, mtx_int32_t, p_matrix_t);
typedef p_matrix_t (*mtx_u32_scalar_operation_fn)(p_matrix_t, mtx_uint32_t, p_matrix_t);
typedef p_matrix_t (*mtx_f32_scalar_operation_fn)(p_matrix_t, mtx_float32_t, p_matrix_t);
typedef p_matrix_t (*mtx_d64_scalar_operation_fn)(p_matrix_t, mtx_double64_t, p_matrix_t);

typedef struct MATRIX_OPERATION_T {
    mtx_matrix_initiation_fn init_identity[2];
    mtx_matrix_initiation_fn init_zeros[2];
    mtx_matrix_initiation_fn init_ones[2];

    mtx_matrix_operation_fn matrix_addition[2];
    mtx_matrix_operation_fn matrix_subtraction[2];
    mtx_matrix_operation_fn matrix_multiplication[2];

    union {
        mtx_i32_scalar_operation_fn i32_scalar_multiplication[2];
        mtx_u32_scalar_operation_fn u32_scalar_multiplication[2];
        mtx_f32_scalar_operation_fn f32_scalar_multiplication[2];
        mtx_d64_scalar_operation_fn d64_scalar_multiplication[2];
    };
} mtx_operation_t, *mtx_operation_ptr_t;

typedef struct MATRIX_T {
    mtx_count_t row_cnt;           /* The actual number of rows. */ 
    mtx_count_t col_cnt;           /* The actual number of columns. */
    mtx_count_t padded_row_cnt;    /* Round to the power of <value type>_PACK_LENGTH. */
    mtx_count_t padded_col_cnt;    /* Round to the power of <value type>_PACK_LENGTH. */
    mtx_count_t padded_byte_cnt;   /* Allocated bytes, including pads. */
    mtx_count_t pack_cnt_per_row;
    mtx_count_t pack_len;
    mtx_count_t value_size;

    mtx_operation_ptr_t ops;

    union {
        mtx_int32_t *    i32_padded;
        mtx_v4si_t *     i32_packs;
        void *           data;
    };
    
    union {
        mtx_int32_t *    i32_values[1];
        mtx_uint32_t *   u32_values[1];
        mtx_float32_t *  f32_values[1];
        mtx_double64_t * d64_values[1];
    };
} matrix_t; 

inline static mtx_count_t mtx_round_to_multiples_of(mtx_count_t cnt, mtx_count_t n)
{
    return (cnt & ~(n - 1)) + (cnt & (n - 1)) ? n : 0;
} /* mtx_round_to_multiples_of */

static p_matrix_t mtx_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt, size_t val_size, mtx_count_t pack_len, mtx_operation_ptr_t ops)
{
    p_matrix_t mtx = NULL;
    mtx_count_t padded_row_cnt = mtx_round_to_multiples_of(row_cnt, pack_len);

    mtx = calloc(sizeof(matrix_t) + sizeof(void *) * padded_row_cnt, 1);
    if (! mtx) {
        return NULL;
    } /* if */

    mtx->row_cnt = row_cnt;
    mtx->col_cnt = col_cnt;
    mtx->padded_row_cnt = padded_row_cnt;
    mtx->padded_col_cnt = mtx_round_to_multiples_of(col_cnt, pack_len);
    mtx->padded_byte_cnt = val_size * mtx->padded_row_cnt * mtx->padded_col_cnt;
    mtx->pack_cnt_per_row = mtx->padded_col_cnt / pack_len;
    mtx->pack_len = pack_len;
    mtx->value_size = val_size;

    mtx->ops = ops;

    mtx->data = malloc(mtx->padded_byte_cnt);
    if (! mtx->data) {
        free(mtx);
        return NULL;
    } /* if */

    return mtx;
} /* mtx_allocate */

p_matrix_t mtx_allocate_for_multiplying(p_matrix_t lhs, p_matrix_t rhs)
{
    return mtx_allocate(lhs->row_cnt, rhs->col_cnt, lhs->value_size, lhs->pack_len, lhs->ops);
} /* mtx_allocate_for_multiplying */

p_matrix_t mtx_allocate_for_transposing(p_matrix_t src)
{
    return mtx_allocate(src->col_cnt, src->row_cnt, src->value_size, src->pack_len, src->ops);
} /* mtx_allocate_for_transposing */

p_matrix_t mtx_allocate_in_shape_of(p_matrix_t src)
{
    return mtx_allocate(src->row_cnt, src->col_cnt, src->value_size, src->pack_len, src->ops);
} /* mtx_allocate_in_shape_of */

p_matrix_t mtx_duplicate(p_matrix_t src)
{
    p_matrix_t mtx = mtx_allocate(src->row_cnt, src->col_cnt, src->value_size, src->pack_len, src->ops);
    if (! mtx) {
        return NULL;
    } /* if */
    memcpy(mtx->data, src->data, src->padded_byte_cnt);
    return mtx;
} /* mtx_duplicate */

void mtx_destroy(p_matrix_t mtx)
{
    if (mtx) {
        free(mtx->data);
    } /* if */
    free(mtx);
} /* mtx_destroy */

void mtx_initialize_identity(p_matrix_t mtx, mtx_option_t opt)
{
    (*mtx->ops->init_identity[opt & 0x3])(mtx);
} /* mtx_initialize_identity */

void mtx_initialize_zeros(p_matrix_t mtx, mtx_option_t opt)
{
    (*mtx->ops->init_zeros[opt & 0x3])(mtx);
} /* mtx_initialize_zeros */

void mtx_initialize_ones(p_matrix_t mtx, mtx_option_t opt)
{
    (*mtx->ops->init_ones[opt & 0x3])(mtx);
} /* mtx_initialize_ones */

int mtx_can_do_add(p_matrix_t lhs, p_matrix_t rhs)
{
    return (lhs->row_cnt == rhs->row_cnt) && (lhs->col_cnt == rhs->col_cnt);
} /* mtx_can_do_add */

int mtx_can_do_multiply(p_matrix_t lhs, p_matrix_t rhs)
{
    return (lhs->col_cnt == rhs->row_cnt);
} /* mtx_can_do_multiply */

void mtx_add_and_store(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    (*mtx->ops->matrix_addition[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_add_and_store */

void mtx_sub_and_store(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    (*mtx->ops->matrix_subtraction[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_sub_and_store */

void mtx_multiply_and_store(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    (*mtx->ops->matrix_multiplication[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_multiply_and_store */

void mtx_transpose_and_store(p_matrix_t mtx, p_matrix_t src)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;

    for (i = 0; i < src->row_cnt; i += 1) {
        for (j = 0; j < src->col_cnt; j += 1) {
            mtx->i32_values[j][i] = src->i32_values[i][j];
        } /* for j */
    } /* for i */
} /* mtx_transpose_and_store */

/* ==== Type-Related functions. ==== */

void mtx_i32_set_at(p_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_int32_t src_val)
{
    mtx->i32_values[row][col] = src_val;
} /* mtx_i32_set_at */

void mtx_i32_set_all_to(p_matrix_t mtx, mtx_int32_t src_val)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->i32_values[i][j] = src_val;
        } /* for */
    } /* for */
    return;
} /* mtx_i32_set_all_to */

void mtx_i32_set_slice_to(p_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_int32_t src_vals[], mtx_count_t val_cnt)
{
    mtx_count_t end = col + val_cnt;
    if (mtx->col_cnt < end) {
        /* Copy enough values from the source and don't cross the row boundary. */
        end = mtx->col_cnt;
    } /* if */
    memcpy(&mtx->i32_values[row][col], src_vals, sizeof(mtx->i32_padded[0]) * (end - col));
} /* mtx_i32_set_slice_to */

void mtx_i32_set_from_array(p_matrix_t mtx, mtx_int32_t * src_vals[])
{
    mtx_count_t i = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        memcpy(mtx->i32_values[i], src_vals[i], sizeof(mtx->i32_padded[0]) * mtx->col_cnt);
    } /* for */
} /* mtx_i32_set_from_array */

static p_matrix_t mtx_add_and_store_plain_impl(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < lhs->col_cnt; j += 1) {
            mtx->i32_values[i][j] = lhs->i32_values[i][j] + rhs->i32_values[i][j];
        } /* for */
    } /* for */
    return mtx;
} /* mtx_add_and_store_plain_impl */

static p_matrix_t mtx_sub_and_store_plain_impl(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < lhs->col_cnt; j += 1) {
            mtx->i32_values[i][j] = lhs->i32_values[i][j] - rhs->i32_values[i][j];
        } /* for */
    } /* for */
    return mtx;
} /* mtx_sub_and_store_plain_impl */

static p_matrix_t mtx_multiply_and_store_plain_impl(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    mtx_count_t k = 0;
    mtx_int32_t sum = 0;

    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < rhs->col_cnt; j += 1) {
            sum = 0;
            for (k = 0; k < lhs->col_cnt; k += 1) {
                sum += lhs->i32_values[i][k] * rhs->i32_values[k][j];
            } /* for k */
            mtx->i32_values[i][j] = sum;
        } /* for j */
    } /* for i */
    return mtx;
} /* mtx_multiply_and_store_plain_impl */

inline static mtx_int32_t mtx_sum(mtx_v4si_t * src)
{
    mtx_int32_t * vals = (mtx_int32_t *) src;
    return vals[0] + vals[1] + vals[2] + vals[3];
} /* mtx_sum */

static p_matrix_t mtx_multiply_and_store_simd_impl(p_matrix_t mtx, p_matrix_t lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    mtx_count_t k = 0;
    mtx_v4si_t pack_lhs = {0};
    mtx_v4si_t pack_rhs = {0};
    mtx_v4si_t ret = {0};

    for (k = 0; k < rhs->padded_row_cnt; k += MTX_I32_PACK_LENGTH) {
        for (j = 0; j < rhs->col_cnt; j += 1) {
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_values[k + 0][j], 0);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_values[k + 1][j], 1);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_values[k + 2][j], 2);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_values[k + 3][j], 3);

            for (i = 0; i < lhs->row_cnt; i += 1) {
                pack_lhs = lhs->i32_packs[i * lhs->pack_cnt_per_row + k / MTX_I32_PACK_LENGTH];
                ret = __builtin_ia32_pmuldq128(pack_lhs, pack_rhs);
                mtx->i32_values[i][j] += mtx_sum(&ret);
            } /* for */
        } /* for */
    } /* for */
    return mtx;
} /* mtx_multiply_and_store_simd_impl */

static p_matrix_t mtx_i32_scalar_multiply_and_store_plain_impl(p_matrix_t mtx, mtx_int32_t lhs, p_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;

    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->i32_values[i][j] = lhs * rhs->i32_values[i][j];
        } /* for j */
    } /* for i */
    return mtx;
} /* mtx_i32_scalar_multiply_and_store_plain_impl */

p_matrix_t mtx_i32_scalar_multiply_and_store_simd_impl(p_matrix_t mtx, mtx_int32_t lhs, p_matrix_t rhs)
{
#undef IMTX_CALL_INTRINSIC_AND_STORE
#define IMTX_CALL_INTRINSIC_AND_STORE(func, pack, val) (pack) = func((pack), (val))

    mtx_v4si_t scalar = {0};
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
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 0], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 1], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 2], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 3], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 4], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 5], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 6], scalar);
        IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 7], scalar);
    } /* for i */

    switch (rmd_cnt) {
        case 7: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 6], scalar);
        case 6: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 5], scalar);
        case 5: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 4], scalar);
        case 4: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 3], scalar);
        case 3: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 2], scalar);
        case 2: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 1], scalar);
        case 1: IMTX_CALL_INTRINSIC_AND_STORE(__builtin_ia32_pmuldq128, rhs->i32_packs[i + 0], scalar);
        default: break;
    } /* switch */
    return mtx;
#undef IMTX_CALL_INTRINSIC_AND_STORE
} /* mtx_i32_scalar_multiply_and_store_simd_impl */

void mtx_i32_scalar_multiply_and_store(p_matrix_t mtx, mtx_int32_t lhs, p_matrix_t rhs, mtx_option_t opt)
{
    (*mtx->ops->i32_scalar_multiplication[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_i32_scalar_multiply_and_store */

static void mtx_i32_init_identity_plain_impl(p_matrix_t mtx)
{
    mtx_count_t i = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        mtx->i32_values[i][i] = 1;
    } /* for */
} /* mtx_i32_init_identity_plain_impl */

static void mtx_i32_init_zeros_plain_impl(p_matrix_t mtx)
{
    memset(mtx->i32_padded, 0, mtx->padded_byte_cnt);
} /* mtx_i32_init_zeros_plain_impl */

static void mtx_i32_init_ones_plain_impl(p_matrix_t mtx)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (i = 0; i < mtx->col_cnt; i += 1) {
            mtx->i32_values[i][j] = 1;
        } /* for */
    } /* for */
} /* mtx_i32_init_ones_plain_impl */

static mtx_operation_t mtx_i32_operations = {
    {
        &mtx_i32_init_identity_plain_impl,
        &mtx_i32_init_identity_plain_impl
    },
    {
        &mtx_i32_init_zeros_plain_impl,
        &mtx_i32_init_zeros_plain_impl
    },
    {
        &mtx_i32_init_ones_plain_impl,
        &mtx_i32_init_ones_plain_impl
    },
    {&mtx_add_and_store_plain_impl, &mtx_add_and_store_plain_impl},
    {&mtx_sub_and_store_plain_impl, &mtx_sub_and_store_plain_impl},
    {&mtx_multiply_and_store_plain_impl, &mtx_multiply_and_store_simd_impl},
    {NULL, NULL},
};

p_matrix_t mtx_i32_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt)
{
    mtx_count_t i = 0;
    p_matrix_t mtx = NULL;

    mtx = mtx_allocate(row_cnt, col_cnt, sizeof(mtx_int32_t), MTX_I32_PACK_LENGTH, &mtx_i32_operations);
    if (! mtx) {
        return NULL;
    } /* if */

    for (i = 0; i < mtx->row_cnt; i += 1) {
        mtx->i32_values[i] = &mtx->i32_padded[i * mtx->padded_col_cnt];
    } /* for */
    return mtx;
} /* mtx_i32_allocate */


