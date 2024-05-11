#include <memory.h>
#include <smmintrin.h>

#include "matrix.h"

enum {
    I32_PACK_LEN = 4,
    U32_PACK_LEN = 4,
    F32_PACK_LEN = 4,
    D64_PACK_LEN = 2
};

typedef mtx_int32_t mtx_v4si_t __attribute__ ((vector_size (16)));

typedef void (*mat_init_fn)(ptr_matrix_t);
typedef ptr_matrix_t (*mat_oper_fn)(ptr_matrix_t, ptr_matrix_t, ptr_matrix_t);
typedef ptr_matrix_t (*i32_scr_oper_fn)(ptr_matrix_t, mtx_int32_t, ptr_matrix_t);
typedef ptr_matrix_t (*u32_scr_oper_fn)(ptr_matrix_t, mtx_uint32_t, ptr_matrix_t);
typedef ptr_matrix_t (*f32_scr_oper_fn)(ptr_matrix_t, mtx_float32_t, ptr_matrix_t);
typedef ptr_matrix_t (*d64_scr_oper_fn)(ptr_matrix_t, mtx_double64_t, ptr_matrix_t);

typedef struct MATRIX_OPERATION_T {
    mat_init_fn init_identity[2];   /* Initialization of identity matrix. */
    mat_init_fn init_zeros[2];      /* Initialization of zero-matrix. */
    mat_init_fn init_ones[2];       /* Initialization of one-matrix. */

    mat_oper_fn mat_add[2]; /* Matrix addtion. */
    mat_oper_fn mat_sub[2]; /* Matrix subtraction. */
    mat_oper_fn mat_mul[2]; /* Matrix multiplication. */

    union {
        i32_scr_oper_fn i32_scr_mul[2]; /* Scalar multiplication for int32. */
        u32_scr_oper_fn u32_scr_mul[2]; /* Scalar multiplication for uint32. */
        f32_scr_oper_fn f32_scr_mul[2]; /* Scalar multiplication for float32. */
        d64_scr_oper_fn d64_scr_mul[2]; /* Scalar multiplication for double64. */
    };
} mtx_operation_t, *mtx_operation_ptr_t;

typedef struct MATRIX_T {
    mtx_count_t row_cnt;           /* The actual number of rows. */ 
    mtx_count_t col_cnt;           /* The actual number of columns. */
    mtx_count_t padded_row_cnt;    /* Round to the power of <value type>_PACK_LEN. */
    mtx_count_t padded_col_cnt;    /* Round to the power of <value type>_PACK_LEN. */
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
        mtx_int32_t *    i32_vals[1];
        mtx_uint32_t *   u32_vals[1];
        mtx_float32_t *  f32_vals[1];
        mtx_double64_t * d64_vals[1];
    };
} matrix_t; 

inline static mtx_count_t mtx_round_to_multiples_of(mtx_count_t cnt, mtx_count_t n)
{
    return (cnt & ~(n - 1)) + (cnt & (n - 1)) ? n : 0;
} /* mtx_round_to_multiples_of */

static ptr_matrix_t mtx_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt, size_t val_size, mtx_count_t pack_len, mtx_operation_ptr_t ops)
{
    ptr_matrix_t mtx = NULL;
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

ptr_matrix_t mtx_allocate_for_multiplying(ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    return mtx_allocate(lhs->row_cnt, rhs->col_cnt, lhs->value_size, lhs->pack_len, lhs->ops);
} /* mtx_allocate_for_multiplying */

ptr_matrix_t mtx_allocate_for_transposing(ptr_matrix_t src)
{
    return mtx_allocate(src->col_cnt, src->row_cnt, src->value_size, src->pack_len, src->ops);
} /* mtx_allocate_for_transposing */

ptr_matrix_t mtx_allocate_in_shape_of(ptr_matrix_t src)
{
    return mtx_allocate(src->row_cnt, src->col_cnt, src->value_size, src->pack_len, src->ops);
} /* mtx_allocate_in_shape_of */

ptr_matrix_t mtx_duplicate(ptr_matrix_t src)
{
    ptr_matrix_t mtx = mtx_allocate(src->row_cnt, src->col_cnt, src->value_size, src->pack_len, src->ops);
    if (! mtx) {
        return NULL;
    } /* if */
    memcpy(mtx->data, src->data, src->padded_byte_cnt);
    return mtx;
} /* mtx_duplicate */

void mtx_destroy(ptr_matrix_t mtx)
{
    if (mtx) {
        free(mtx->data);
    } /* if */
    free(mtx);
} /* mtx_destroy */

void mtx_initialize_identity(ptr_matrix_t mtx, mtx_option_t opt)
{
    (*mtx->ops->init_identity[opt & 0x3])(mtx);
} /* mtx_initialize_identity */

void mtx_initialize_zeros(ptr_matrix_t mtx, mtx_option_t opt)
{
    (*mtx->ops->init_zeros[opt & 0x3])(mtx);
} /* mtx_initialize_zeros */

void mtx_initialize_ones(ptr_matrix_t mtx, mtx_option_t opt)
{
    (*mtx->ops->init_ones[opt & 0x3])(mtx);
} /* mtx_initialize_ones */

int mtx_can_do_add(ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    return (lhs->row_cnt == rhs->row_cnt) && (lhs->col_cnt == rhs->col_cnt);
} /* mtx_can_do_add */

int mtx_can_do_multiply(ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    return (lhs->col_cnt == rhs->row_cnt);
} /* mtx_can_do_multiply */

mtx_count_t mtx_count_rows(ptr_matrix_t mtx)
{
    return mtx->row_cnt;
} /* mtx_count_rows */

mtx_count_t mtx_count_columns(ptr_matrix_t mtx)
{
    return mtx->col_cnt;
} /* mtx_count_columns */

mtx_count_t mtx_count_values(ptr_matrix_t mtx)
{
    return (mtx->row_cnt * mtx->col_cnt);
} /* mtx_count_values */

void mtx_add_and_store(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*mtx->ops->mat_add[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_add_and_store */

void mtx_sub_and_store(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*mtx->ops->mat_sub[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_sub_and_store */

void mtx_multiply_and_store(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*mtx->ops->mat_mul[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_multiply_and_store */

void mtx_transpose_and_store(ptr_matrix_t mtx, ptr_matrix_t src)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;

    for (i = 0; i < src->row_cnt; i += 1) {
        for (j = 0; j < src->col_cnt; j += 1) {
            mtx->i32_vals[j][i] = src->i32_vals[i][j];
        } /* for j */
    } /* for i */
} /* mtx_transpose_and_store */

void mtx_get_submatrix(ptr_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_count_t row_cnt, mtx_count_t col_cnt, ptr_submatrix_t ref)
{
    /* If out of range, then return the whole matrix. */
    ref->row_off = (mtx->row_cnt < row) ? 0 : row;
    ref->col_off = (mtx->col_cnt < col) ? 0 : col;
    ref->row_cnt = (row_cnt <= mtx->row_cnt - ref->row_off) ? row_cnt : (mtx->row_cnt - ref->row_off);
    ref->col_cnt = (col_cnt <= mtx->col_cnt - ref->col_off) ? col_cnt : (mtx->col_cnt - ref->col_off);

    ref->data = mtx->data;
} /* mtx_get_submatrix */

/* ==== Type-Related functions. ==== */

void mtx_i32_initialize_from_array(ptr_matrix_t mtx, mtx_int32_t * src_vals[])
{
    mtx_count_t i = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        memcpy(mtx->i32_vals[i], src_vals[i], sizeof(mtx->i32_padded[0]) * mtx->col_cnt);
    } /* for */
} /* mtx_i32_initialize_from_array */

mtx_int32_t mtx_i32_get(ptr_matrix_t mtx, mtx_count_t row, mtx_count_t col)
{
    return mtx->i32_vals[row][col];
} /* mtx_i32_get */

void mtx_i32_set(ptr_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_int32_t src_val)
{
    mtx->i32_vals[row][col] = src_val;
} /* mtx_i32_set */

void mtx_i32_set_each(ptr_matrix_t mtx, mtx_int32_t src_val)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->i32_vals[i][j] = src_val;
        } /* for */
    } /* for */
    return;
} /* mtx_i32_set_each */

void mtx_i32_set_row_slice(ptr_matrix_t mtx, mtx_count_t row, mtx_count_t col, mtx_int32_t src_vals[], mtx_count_t val_cnt)
{
    mtx_count_t end = col + val_cnt;
    if (mtx->col_cnt < end) {
        /* Copy enough values from the source and don't cross the row boundary. */
        end = mtx->col_cnt;
    } /* if */
    memcpy(&mtx->i32_vals[row][col], src_vals, sizeof(mtx->i32_padded[0]) * (end - col));
} /* mtx_i32_set_row_slice */

static ptr_matrix_t mtx_add_and_store_plain(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < lhs->col_cnt; j += 1) {
            mtx->i32_vals[i][j] = lhs->i32_vals[i][j] + rhs->i32_vals[i][j];
        } /* for */
    } /* for */
    return mtx;
} /* mtx_add_and_store_plain */

static ptr_matrix_t mtx_sub_and_store_plain(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < lhs->col_cnt; j += 1) {
            mtx->i32_vals[i][j] = lhs->i32_vals[i][j] - rhs->i32_vals[i][j];
        } /* for */
    } /* for */
    return mtx;
} /* mtx_sub_and_store_plain */

static ptr_matrix_t mtx_multiply_and_store_plain(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    mtx_count_t k = 0;
    mtx_int32_t sum = 0;

    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < rhs->col_cnt; j += 1) {
            sum = 0;
            for (k = 0; k < lhs->col_cnt; k += 1) {
                sum += lhs->i32_vals[i][k] * rhs->i32_vals[k][j];
            } /* for k */
            mtx->i32_vals[i][j] = sum;
        } /* for j */
    } /* for i */
    return mtx;
} /* mtx_multiply_and_store_plain */

inline static mtx_int32_t mtx_sum(mtx_v4si_t * src)
{
    mtx_int32_t * vals = (mtx_int32_t *) src;
    return vals[0] + vals[1] + vals[2] + vals[3];
} /* mtx_sum */

static ptr_matrix_t mtx_multiply_and_store_simd(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    mtx_count_t k = 0;
    mtx_v4si_t pack_lhs = {0};
    mtx_v4si_t pack_rhs = {0};
    mtx_v4si_t ret = {0};

    for (k = 0; k < rhs->padded_row_cnt; k += I32_PACK_LEN) {
        for (j = 0; j < rhs->col_cnt; j += 1) {
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_vals[k + 0][j], 0);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_vals[k + 1][j], 1);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_vals[k + 2][j], 2);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_vals[k + 3][j], 3);

            for (i = 0; i < lhs->row_cnt; i += 1) {
                pack_lhs = lhs->i32_packs[i * lhs->pack_cnt_per_row + k / I32_PACK_LEN];
                ret = __builtin_ia32_pmuldq128(pack_lhs, pack_rhs);
                mtx->i32_vals[i][j] += mtx_sum(&ret);
            } /* for */
        } /* for */
    } /* for */
    return mtx;
} /* mtx_multiply_and_store_simd */

static ptr_matrix_t mtx_i32_scalar_multiply_and_store_plain(ptr_matrix_t mtx, mtx_int32_t lhs, ptr_matrix_t rhs)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;

    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->i32_vals[i][j] = lhs * rhs->i32_vals[i][j];
        } /* for j */
    } /* for i */
    return mtx;
} /* mtx_i32_scalar_multiply_and_store_plain */

ptr_matrix_t mtx_i32_scalar_multiply_and_store_simd(ptr_matrix_t mtx, mtx_int32_t lhs, ptr_matrix_t rhs)
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
} /* mtx_i32_scalar_multiply_and_store_simd */

void mtx_i32_scalar_multiply_and_store(ptr_matrix_t mtx, mtx_int32_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*mtx->ops->i32_scr_mul[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_i32_scalar_multiply_and_store */

static void mtx_i32_init_identity_plain(ptr_matrix_t mtx)
{
    mtx_count_t i = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        mtx->i32_vals[i][i] = 1;
    } /* for */
} /* mtx_i32_init_identity_plain */

static void mtx_i32_init_zeros_plain(ptr_matrix_t mtx)
{
    memset(mtx->i32_padded, 0, mtx->padded_byte_cnt);
} /* mtx_i32_init_zeros_plain */

static void mtx_i32_init_ones_plain(ptr_matrix_t mtx)
{
    mtx_count_t i = 0;
    mtx_count_t j = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (i = 0; i < mtx->col_cnt; i += 1) {
            mtx->i32_vals[i][j] = 1;
        } /* for */
    } /* for */
} /* mtx_i32_init_ones_plain */

static mtx_operation_t i32_ops = {
    {
        &mtx_i32_init_identity_plain,
        &mtx_i32_init_identity_plain
    },
    {
        &mtx_i32_init_zeros_plain,
        &mtx_i32_init_zeros_plain
    },
    {
        &mtx_i32_init_ones_plain,
        &mtx_i32_init_ones_plain
    },
    {&mtx_add_and_store_plain, &mtx_add_and_store_plain},
    {&mtx_sub_and_store_plain, &mtx_sub_and_store_plain},
    {&mtx_multiply_and_store_plain, &mtx_multiply_and_store_simd},
    {NULL, NULL},
};

ptr_matrix_t mtx_i32_allocate(mtx_count_t row_cnt, mtx_count_t col_cnt)
{
    mtx_count_t i = 0;
    ptr_matrix_t mtx = NULL;

    mtx = mtx_allocate(row_cnt, col_cnt, sizeof(mtx_int32_t), I32_PACK_LEN, &i32_ops);
    if (! mtx) {
        return NULL;
    } /* if */

    for (i = 0; i < mtx->row_cnt; i += 1) {
        mtx->i32_vals[i] = &mtx->i32_padded[i * mtx->padded_col_cnt];
    } /* for */
    return mtx;
} /* mtx_i32_allocate */


