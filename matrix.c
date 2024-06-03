#include <omp.h>
#include <memory.h>
#include <smmintrin.h>
#include <stdio.h>

#include "config.h"
#include "matrix.h"

/* ==== Definitions of the common types ==== */

typedef int32_t v4si_t __attribute__ ((vector_size (16)));
typedef int64_t v2di_t __attribute__ ((vector_size (16)));
typedef float v4sf_t __attribute__ ((vector_size (16)));
typedef double v2df_t __attribute__ ((vector_size (16)));

typedef int32_t v8si_t __attribute__ ((vector_size (32)));
typedef int64_t v4di_t __attribute__ ((vector_size (32)));
typedef float v8sf_t __attribute__ ((vector_size (32)));
typedef double v4df_t __attribute__ ((vector_size (32)));

enum {
    BYTES_IN_CACHE_LINE = 64
};

enum {
    I32_VALS_IN_V4SI = 4,
    I64_VALS_IN_V2DI = 2,
    F32_VALS_IN_V4SF = 4,
    D64_VALS_IN_V2DF = 2,

    I32_VALS_IN_V8SI = 8,
    I64_VALS_IN_V4DI = 4,
    F32_VALS_IN_V8SF = 8,
    D64_VALS_IN_V4DF = 4
};

enum {
    I32_VALS_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(int32_t), // 16 values in cache line.
    I64_VALS_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(int64_t), // 8 values in cache line.
    F32_VALS_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(float),   // 16 values in cache line.
    D64_VALS_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(double)   // 8 values in cache line.
};

enum {
    V4SI_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(v4si_t),      // 4 packs in cache line.
    V2DI_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(v2di_t),      // 2 packs in cache line.
    V4SF_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(v4sf_t),      // 4 packs in cache line.
    V2DF_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(v2df_t),      // 2 packs in cache line.

    V8SI_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(v8si_t),      // 8 packs in cache line.
    V4DI_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(v4di_t),      // 4 packs in cache line.
    V8SF_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(v8sf_t),      // 8 packs in cache line.
    V4DF_IN_CACHE_LINE = BYTES_IN_CACHE_LINE / sizeof(v4df_t)       // 4 packs in cache line.
};

inline static uint32_t round_to_multiples(uint32_t cnt, uint32_t n)
{
    return (cnt + (n - 1)) & (~(n - 1));
} // round_to_multiples

typedef struct META_T {
    uint32_t rows;          // The number of rows.
    uint32_t cols;          // The number of columns.
    uint32_t cols_padded;   // The number of columns, include padding ones.
    uint32_t pcks_in_row;   // The number of packs in one row.
    uint32_t vals_in_pck;   // The number of values in one pack.
    size_t   bytes;         // Allocated bytes, including pads.
    size_t   val_size;      // The size of one value.

    void *   buf;           // The buffer used to store values, perhaps non-aligned.
} meta_t, *p_meta_t;

static void * init_meta(p_meta_t mt, uint32_t rows, uint32_t cols, size_t val_size, size_t vals_in_pck)
{
    uint32_t padded_col_cnt = 0;
    size_t alignment = 0;

    // NOTE: Append padding values to each row for memory alignment,
    //       and this speeds up loading values.
    padded_col_cnt = round_to_multiples(cols, vals_in_pck);

    mt->rows        = rows;
    mt->cols        = cols;
    mt->cols_padded = padded_col_cnt;
    mt->bytes       = val_size * rows * padded_col_cnt;
    mt->pcks_in_row = padded_col_cnt / vals_in_pck;
    mt->vals_in_pck = vals_in_pck;
    mt->val_size    = val_size;

    alignment = val_size * vals_in_pck;
    mt->buf = malloc(mt->bytes + alignment);
    return (void *)( ((size_t)mt->buf + (alignment - 1)) & (~(alignment - 1)));
} // init_meta

struct MAT_MUL_INFO_T;
typedef struct MAT_MUL_INFO_T * p_mat_mul_info_t;

typedef void (*mat_load_rpcks_fn)(p_mat_mul_info_t, uint32_t, uint32_t, uint32_t, uint32_t);
typedef void (*mat_mul_pcks_and_store_fn)(p_mat_mul_info_t, uint32_t, uint32_t, uint32_t, uint32_t);
typedef void (*mat_mul_and_store_fn)(p_mat_mul_info_t, mat_mul_pcks_and_store_fn, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

/* Force memory alignment at 32-byte boundary to avoid segmentation fault. */
typedef union RIGHT_PACK_T {
    // 128 bits.
    v4si_t v4si_pcks[V4SI_IN_CACHE_LINE][I32_VALS_IN_CACHE_LINE];
    v2di_t v2di_pcks[V2DI_IN_CACHE_LINE][I32_VALS_IN_CACHE_LINE];
    v4sf_t v4sf_pcks[V4SF_IN_CACHE_LINE][F32_VALS_IN_CACHE_LINE];
    v2df_t v2df_pcks[V2DF_IN_CACHE_LINE][D64_VALS_IN_CACHE_LINE];

    // 256 bits.
    v8si_t v8si_pcks[V8SI_IN_CACHE_LINE][I32_VALS_IN_CACHE_LINE];
    v4di_t v4di_pcks[V4DI_IN_CACHE_LINE][I32_VALS_IN_CACHE_LINE];
    v8sf_t v8sf_pcks[V8SF_IN_CACHE_LINE][F32_VALS_IN_CACHE_LINE];
    v4df_t v4df_pcks[V4DF_IN_CACHE_LINE][D64_VALS_IN_CACHE_LINE];
} rpck_t, *ptr_rpck_t __attribute__ ((aligned (32)));

typedef struct MAT_MUL_INFO_T {
    void * m;
    void * lhs;
    void * rhs;

    p_meta_t lhs_mt;
    p_meta_t rhs_mt;

    mat_load_rpcks_fn load_rpcks_ifcf;
    mat_load_rpcks_fn load_rpcks_ifcp;
    mat_load_rpcks_fn load_rpcks_ipcf;
    mat_load_rpcks_fn load_rpcks_ipcp;

    mat_mul_pcks_and_store_fn mul_and_store_fully;
    mat_mul_pcks_and_store_fn mul_and_store_partly;

    mat_mul_and_store_fn mul_and_store;

    uint32_t val_size;
    uint32_t vals_in_pck;
    uint32_t pcks_per_bth;
    uint32_t vals_per_bth;
    uint32_t jbths;
    uint32_t jrmd;
    uint32_t kbths;
    uint32_t krmd;
    uint32_t ibths;
    uint32_t irmd;

    ptr_rpck_t rpck;
} mat_mul_info_t;

static void mat_multiply_and_store_simd_v2(p_mat_mul_info_t mi)
{
    mi->vals_per_bth = (BYTES_IN_CACHE_LINE / mi->val_size);
    mi->pcks_per_bth = mi->vals_per_bth / mi->vals_in_pck;
    mi->jbths = mi->rhs_mt->cols / mi->vals_per_bth;
    mi->jrmd = mi->rhs_mt->cols % mi->vals_per_bth;
    mi->kbths = mi->rhs_mt->rows / mi->vals_per_bth;
    mi->krmd = mi->rhs_mt->rows % mi->vals_per_bth;
    mi->ibths = (mi->lhs_mt->rows + (mi->vals_per_bth - 1)) / mi->vals_per_bth;
    mi->irmd = mi->lhs_mt->rows & (mi->vals_per_bth - 1);

#pragma omp parallel for schedule(static)
    for (uint32_t j = 0; j < mi->jbths; j += 1) {
        mat_mul_info_t mi2 = *mi;
        rpck_t rpck;
        uint32_t itn_base = 0;
        uint32_t col_base = j * mi->vals_per_bth;
        uint32_t pck_off = 0;

        mi2.rpck = &rpck;
        for (uint32_t k = 0; k < mi->kbths; k += 1) {
            (*mi->load_rpcks_ifcf)(&mi2, itn_base, 0, col_base, 0);
            (*mi->mul_and_store)(&mi2, mi->mul_and_store_fully, pck_off, mi->ibths, mi->irmd, col_base, 0);
            itn_base += mi->vals_per_bth;
            pck_off += mi->pcks_per_bth;
        } /* for */

        if (mi->krmd > 0) {
            memset(mi2.rpck, 0, sizeof(rpck));
            (*mi->load_rpcks_ipcf)(&mi2, itn_base, mi->krmd, col_base, 0);
            (*mi->mul_and_store)(&mi2, mi->mul_and_store_fully, pck_off, mi->ibths, mi->irmd, col_base, 0);
        } /* if */
    } /* for */

    if (mi->jrmd > 0) {
        mat_mul_info_t mi2 = *mi;
        rpck_t rpck;
        uint32_t pck_off = 0;
        uint32_t itn_base = 0;
        uint32_t col_base = mi->jbths * mi->vals_per_bth;

        mi2.rpck = &rpck;
        for (uint32_t k = 0; k < mi->kbths; k += 1) {
            memset(mi2.rpck, 0, sizeof(rpck));
            (*mi->load_rpcks_ifcp)(&mi2, itn_base, 0, col_base, mi->jrmd);
            (*mi->mul_and_store)(&mi2, mi->mul_and_store_partly, pck_off, mi->ibths, mi->irmd, col_base, mi->jrmd);
            itn_base += mi->vals_per_bth;
            pck_off += mi->pcks_per_bth;
        } /* for */

        if (mi->krmd > 0) {
            memset(mi2.rpck, 0, sizeof(rpck));
            (*mi->load_rpcks_ipcp)(&mi2, itn_base, mi->krmd, col_base, mi->jrmd);
            (*mi->mul_and_store)(&mi2, mi->mul_and_store_partly, pck_off, mi->ibths, mi->irmd, col_base, mi->jrmd);
        } /* if */
    } /* if */
} /* mat_multiply_and_store_simd_v2 */

/* ==== Definitions of the matrix_i32_t type ==== */

typedef struct MATRIX_I32_T {
    meta_t          mt;                 // Common fields of a matrix.

    union {
#if defined(MX_SSE41)
        v4si_t *    v4si_pcks;
#endif // MX_SSE41

#if defined(MX_AVX2)
        v8si_t *    v8si_pcks;
#endif // MX_AVX2

        void *      data;           // The buffer used to store values, aligned to 32-byte boundary.
    };

    int32_t *       i32_vals[0];
} matrix_i32_t;

p_mi32_t mi32_allocate(uint32_t rows, uint32_t cols)
{
    uint32_t i = 0;
    p_mi32_t m = NULL;

    m = calloc(sizeof(matrix_i32_t) + sizeof(void *) * rows, 1);
    if (! m) {
        return NULL;
    } /* if */

#if defined(MX_AVX2)
    m->data = init_meta(&m->mt, rows, cols, sizeof(int32_t), I32_VALS_IN_V8SI);
#elif defined(MX_SSE41)
    m->data = init_meta(&m->mt, rows, cols, sizeof(int32_t), I32_VALS_IN_V4SI);
#endif
    if (! m->data) {
        free(m);
        return NULL;
    } /* if */

    /* NOTE: Align to the vector type's size, otherwise it will be segmentation fault when access to packs. */
    for (i = 0; i < m->mt.rows; i += 1) {
        m->i32_vals[i] = m->data + i * m->mt.val_size * m->mt.cols_padded;
    } /* for */
    return m;
} /* mi32_allocate */

p_mi32_t mi32_allocate_for_multiplying(p_mi32_t lhs, p_mi32_t rhs)
{
    return mi32_allocate(lhs->mt.rows, rhs->mt.cols);
} /* mi32_allocate_for_multiplying */

p_mi32_t mi32_allocate_for_transposing(p_mi32_t src)
{
    return mi32_allocate(src->mt.cols, src->mt.rows);
} /* mi32_allocate_for_transposing */

p_mi32_t mi32_allocate_in_shape_of(p_mi32_t src)
{
    return mi32_allocate(src->mt.rows, src->mt.cols);
} /* mi32_allocate_in_shape_of */

p_mi32_t mi32_duplicate(p_mi32_t src)
{
    p_mi32_t m = mi32_allocate(src->mt.rows, src->mt.cols);
    if (m) {
        memcpy(m->data, src->data, src->mt.bytes);
    } // if
    return m;
} /* mi32_duplicate */

void mi32_destroy(p_mi32_t m)
{
    if (m) {
        free(m->mt.buf);
    } /* if */
    free(m);
} /* mi32_destroy */

void mi32_initialize_identity(p_mi32_t m, mx_opt_t opt)
{
    uint32_t i = 0;
    memset(m->data, 0, m->mt.bytes);
    for (i = 0; i < m->mt.rows; i += 1) {
        m->i32_vals[i][i] = 1;
    } /* for */
} /* mi32_initialize_identity */

void mi32_initialize_zeros(p_mi32_t m, mx_opt_t opt)
{
    memset(m->data, 0, m->mt.bytes);
} /* mi32_initialize_zeros */

void mi32_initialize_ones(p_mi32_t m, mx_opt_t opt)
{
    uint32_t i = 0;
    uint32_t j = 0;
    for (i = 0; i < m->mt.rows; i += 1) {
        for (j = 0; j < m->mt.cols; j += 1) {
            m->i32_vals[i][j] = 1;
        } /* for */
    } /* for */
} /* mi32_initialize_ones */

uint32_t mi32_rows(p_mi32_t m)
{
    return m->mt.rows;
} // mi32_rows

uint32_t mi32_columns(p_mi32_t m)
{
    return m->mt.cols;
} // mi32_columns

uint32_t mi32_values(p_mi32_t m)
{
    return (m->mt.rows * m->mt.cols);
} // mi32_values

int32_t mi32_get(p_mi32_t m, uint32_t row, uint32_t col)
{
    return m->i32_vals[row][col];
} /* mi32_get */

void mi32_set(p_mi32_t m, uint32_t row, uint32_t col, int32_t src_val)
{
    m->i32_vals[row][col] = src_val;
} /* mi32_set */

void mi32_set_each(p_mi32_t m, int32_t src_val)
{
    uint32_t i = 0;
    uint32_t j = 0;
    for (i = 0; i < m->mt.rows; i += 1) {
        for (j = 0; j < m->mt.cols; j += 1) {
            m->i32_vals[i][j] = src_val;
        } /* for */
    } /* for */
    return;
} /* mi32_set_each */

int mi32_can_do_add(p_mi32_t lhs, p_mi32_t rhs)
{
    return (lhs->mt.rows == rhs->mt.rows) && (lhs->mt.cols == rhs->mt.cols);
} /* mi32_can_do_add */

int mi32_can_do_multiply(p_mi32_t lhs, p_mi32_t rhs)
{
    return (lhs->mt.cols == rhs->mt.rows);
} /* mi32_can_do_multiply */

void mi32_transpose_and_store(p_mi32_t m, p_mi32_t src)
{
    uint32_t i = 0;
    uint32_t j = 0;

    for (i = 0; i < src->mt.rows; i += 1) {
        for (j = 0; j < src->mt.cols; j += 1) {
            m->i32_vals[j][i] = src->i32_vals[i][j];
        } /* for j */
    } /* for i */
} /* mi32_transpose_and_store */

void mi32_add_and_store(p_mi32_t m, p_mi32_t lhs, p_mi32_t rhs, mx_opt_t opt)
{
    uint32_t i = 0;
    uint32_t j = 0;
    for (i = 0; i < lhs->mt.rows; i += 1) {
        for (j = 0; j < lhs->mt.cols; j += 1) {
            m->i32_vals[i][j] = lhs->i32_vals[i][j] + rhs->i32_vals[i][j];
        } /* for */
    } /* for */
} // mi32_add_and_store

void mi32_sub_and_store(p_mi32_t m, p_mi32_t lhs, p_mi32_t rhs, mx_opt_t opt)
{
    uint32_t i = 0;
    uint32_t j = 0;
    for (i = 0; i < lhs->mt.rows; i += 1) {
        for (j = 0; j < lhs->mt.cols; j += 1) {
            m->i32_vals[i][j] = lhs->i32_vals[i][j] - rhs->i32_vals[i][j];
        } /* for */
    } /* for */
} // mi32_sub_and_store

static p_mi32_t i32_multiply_and_store_plain(p_mi32_t m, p_mi32_t lhs, p_mi32_t rhs)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;

    for (i = 0; i < lhs->mt.rows; i += 1) {
        for (j = 0; j < rhs->mt.cols; j += 1) {
            m->i32_vals[i][j] = 0;
            for (k = 0; k < lhs->mt.cols; k += 1) {
                m->i32_vals[i][j] += lhs->i32_vals[i][k] * rhs->i32_vals[k][j];
            } /* for k */
        } /* for j */
    } /* for i */
    return m;
} // i32_multiply_and_store_plain

#if defined(MX_SSE41)

#define v4si_load_rpcks_from_col_to_byte_fully(rpck, rhs, itn, col_base, byte_idx) \
    { \
        rpck[ 0] = __builtin_ia32_vec_set_v4si(rpck[ 0], rhs->i32_vals[itn][col_base +  0], byte_idx); \
        rpck[ 1] = __builtin_ia32_vec_set_v4si(rpck[ 1], rhs->i32_vals[itn][col_base +  1], byte_idx); \
        rpck[ 2] = __builtin_ia32_vec_set_v4si(rpck[ 2], rhs->i32_vals[itn][col_base +  2], byte_idx); \
        rpck[ 3] = __builtin_ia32_vec_set_v4si(rpck[ 3], rhs->i32_vals[itn][col_base +  3], byte_idx); \
        rpck[ 4] = __builtin_ia32_vec_set_v4si(rpck[ 4], rhs->i32_vals[itn][col_base +  4], byte_idx); \
        rpck[ 5] = __builtin_ia32_vec_set_v4si(rpck[ 5], rhs->i32_vals[itn][col_base +  5], byte_idx); \
        rpck[ 6] = __builtin_ia32_vec_set_v4si(rpck[ 6], rhs->i32_vals[itn][col_base +  6], byte_idx); \
        rpck[ 7] = __builtin_ia32_vec_set_v4si(rpck[ 7], rhs->i32_vals[itn][col_base +  7], byte_idx); \
        rpck[ 8] = __builtin_ia32_vec_set_v4si(rpck[ 8], rhs->i32_vals[itn][col_base +  8], byte_idx); \
        rpck[ 9] = __builtin_ia32_vec_set_v4si(rpck[ 9], rhs->i32_vals[itn][col_base +  9], byte_idx); \
        rpck[10] = __builtin_ia32_vec_set_v4si(rpck[10], rhs->i32_vals[itn][col_base + 10], byte_idx); \
        rpck[11] = __builtin_ia32_vec_set_v4si(rpck[11], rhs->i32_vals[itn][col_base + 11], byte_idx); \
        rpck[12] = __builtin_ia32_vec_set_v4si(rpck[12], rhs->i32_vals[itn][col_base + 12], byte_idx); \
        rpck[13] = __builtin_ia32_vec_set_v4si(rpck[13], rhs->i32_vals[itn][col_base + 13], byte_idx); \
        rpck[14] = __builtin_ia32_vec_set_v4si(rpck[14], rhs->i32_vals[itn][col_base + 14], byte_idx); \
        rpck[15] = __builtin_ia32_vec_set_v4si(rpck[15], rhs->i32_vals[itn][col_base + 15], byte_idx); \
    }

#define v4si_load_rpcks_from_col_to_byte_partly(rpck, rhs, itn, col_base, cols, byte_idx) \
    switch (cols) { \
        case 15: rpck[14] = __builtin_ia32_vec_set_v4si(rpck[14], rhs->i32_vals[itn][col_base + 14], byte_idx); \
        case 14: rpck[13] = __builtin_ia32_vec_set_v4si(rpck[13], rhs->i32_vals[itn][col_base + 13], byte_idx); \
        case 13: rpck[12] = __builtin_ia32_vec_set_v4si(rpck[12], rhs->i32_vals[itn][col_base + 12], byte_idx); \
        case 12: rpck[11] = __builtin_ia32_vec_set_v4si(rpck[11], rhs->i32_vals[itn][col_base + 11], byte_idx); \
        case 11: rpck[10] = __builtin_ia32_vec_set_v4si(rpck[10], rhs->i32_vals[itn][col_base + 10], byte_idx); \
        case 10: rpck[ 9] = __builtin_ia32_vec_set_v4si(rpck[ 9], rhs->i32_vals[itn][col_base +  9], byte_idx); \
        case  9: rpck[ 8] = __builtin_ia32_vec_set_v4si(rpck[ 8], rhs->i32_vals[itn][col_base +  8], byte_idx); \
        case  8: rpck[ 7] = __builtin_ia32_vec_set_v4si(rpck[ 7], rhs->i32_vals[itn][col_base +  7], byte_idx); \
        case  7: rpck[ 6] = __builtin_ia32_vec_set_v4si(rpck[ 6], rhs->i32_vals[itn][col_base +  6], byte_idx); \
        case  6: rpck[ 5] = __builtin_ia32_vec_set_v4si(rpck[ 5], rhs->i32_vals[itn][col_base +  5], byte_idx); \
        case  5: rpck[ 4] = __builtin_ia32_vec_set_v4si(rpck[ 4], rhs->i32_vals[itn][col_base +  4], byte_idx); \
        case  4: rpck[ 3] = __builtin_ia32_vec_set_v4si(rpck[ 3], rhs->i32_vals[itn][col_base +  3], byte_idx); \
        case  3: rpck[ 2] = __builtin_ia32_vec_set_v4si(rpck[ 2], rhs->i32_vals[itn][col_base +  2], byte_idx); \
        case  2: rpck[ 1] = __builtin_ia32_vec_set_v4si(rpck[ 1], rhs->i32_vals[itn][col_base +  1], byte_idx); \
        case  1: rpck[ 0] = __builtin_ia32_vec_set_v4si(rpck[ 0], rhs->i32_vals[itn][col_base +  0], byte_idx); \
        default: break; \
    }

static void v4si_load_rpcks_ifcf(p_mat_mul_info_t mi, uint32_t itn_base, uint32_t itn_rmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t rhs = mi->rhs;
    v4si_t * rpck0 = mi->rpck->v4si_pcks[0];
    v4si_t * rpck1 = mi->rpck->v4si_pcks[1];
    v4si_t * rpck2 = mi->rpck->v4si_pcks[2];
    v4si_t * rpck3 = mi->rpck->v4si_pcks[3];

    v4si_load_rpcks_from_col_to_byte_fully(rpck0, rhs, itn_base +  0, col_base, 0);
    v4si_load_rpcks_from_col_to_byte_fully(rpck0, rhs, itn_base +  1, col_base, 1);
    v4si_load_rpcks_from_col_to_byte_fully(rpck0, rhs, itn_base +  2, col_base, 2);
    v4si_load_rpcks_from_col_to_byte_fully(rpck0, rhs, itn_base +  3, col_base, 3);
    v4si_load_rpcks_from_col_to_byte_fully(rpck1, rhs, itn_base +  4, col_base, 0);
    v4si_load_rpcks_from_col_to_byte_fully(rpck1, rhs, itn_base +  5, col_base, 1);
    v4si_load_rpcks_from_col_to_byte_fully(rpck1, rhs, itn_base +  6, col_base, 2);
    v4si_load_rpcks_from_col_to_byte_fully(rpck1, rhs, itn_base +  7, col_base, 3);
    v4si_load_rpcks_from_col_to_byte_fully(rpck2, rhs, itn_base +  8, col_base, 0);
    v4si_load_rpcks_from_col_to_byte_fully(rpck2, rhs, itn_base +  9, col_base, 1);
    v4si_load_rpcks_from_col_to_byte_fully(rpck2, rhs, itn_base + 10, col_base, 2);
    v4si_load_rpcks_from_col_to_byte_fully(rpck2, rhs, itn_base + 11, col_base, 3);
    v4si_load_rpcks_from_col_to_byte_fully(rpck3, rhs, itn_base + 12, col_base, 0);
    v4si_load_rpcks_from_col_to_byte_fully(rpck3, rhs, itn_base + 13, col_base, 1);
    v4si_load_rpcks_from_col_to_byte_fully(rpck3, rhs, itn_base + 14, col_base, 2);
    v4si_load_rpcks_from_col_to_byte_fully(rpck3, rhs, itn_base + 15, col_base, 3);
} /* v4si_load_rpcks_ifcf */

static void v4si_load_rpcks_ifcp(p_mat_mul_info_t mi, uint32_t itn_base, uint32_t itn_rmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t rhs = mi->rhs;
    v4si_t * rpck0 = mi->rpck->v4si_pcks[0];
    v4si_t * rpck1 = mi->rpck->v4si_pcks[1];
    v4si_t * rpck2 = mi->rpck->v4si_pcks[2];
    v4si_t * rpck3 = mi->rpck->v4si_pcks[3];

    v4si_load_rpcks_from_col_to_byte_partly(rpck0, rhs, itn_base +  0, col_base, col_rmd, 0);
    v4si_load_rpcks_from_col_to_byte_partly(rpck0, rhs, itn_base +  1, col_base, col_rmd, 1);
    v4si_load_rpcks_from_col_to_byte_partly(rpck0, rhs, itn_base +  2, col_base, col_rmd, 2);
    v4si_load_rpcks_from_col_to_byte_partly(rpck0, rhs, itn_base +  3, col_base, col_rmd, 3);
    v4si_load_rpcks_from_col_to_byte_partly(rpck1, rhs, itn_base +  4, col_base, col_rmd, 0);
    v4si_load_rpcks_from_col_to_byte_partly(rpck1, rhs, itn_base +  5, col_base, col_rmd, 1);
    v4si_load_rpcks_from_col_to_byte_partly(rpck1, rhs, itn_base +  6, col_base, col_rmd, 2);
    v4si_load_rpcks_from_col_to_byte_partly(rpck1, rhs, itn_base +  7, col_base, col_rmd, 3);
    v4si_load_rpcks_from_col_to_byte_partly(rpck2, rhs, itn_base +  8, col_base, col_rmd, 0);
    v4si_load_rpcks_from_col_to_byte_partly(rpck2, rhs, itn_base +  9, col_base, col_rmd, 1);
    v4si_load_rpcks_from_col_to_byte_partly(rpck2, rhs, itn_base + 10, col_base, col_rmd, 2);
    v4si_load_rpcks_from_col_to_byte_partly(rpck2, rhs, itn_base + 11, col_base, col_rmd, 3);
    v4si_load_rpcks_from_col_to_byte_partly(rpck3, rhs, itn_base + 12, col_base, col_rmd, 0);
    v4si_load_rpcks_from_col_to_byte_partly(rpck3, rhs, itn_base + 13, col_base, col_rmd, 1);
    v4si_load_rpcks_from_col_to_byte_partly(rpck3, rhs, itn_base + 14, col_base, col_rmd, 2);
    v4si_load_rpcks_from_col_to_byte_partly(rpck3, rhs, itn_base + 15, col_base, col_rmd, 3);
} /* v4si_load_rpcks_ifcp */

static void v4si_load_rpcks_ipcf(p_mat_mul_info_t mi, uint32_t itn_base, uint32_t itn_rmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t rhs = mi->rhs;
    v4si_t * rpck0 = mi->rpck->v4si_pcks[0];
    v4si_t * rpck1 = mi->rpck->v4si_pcks[1];
    v4si_t * rpck2 = mi->rpck->v4si_pcks[2];
    v4si_t * rpck3 = mi->rpck->v4si_pcks[3];

    switch (itn_rmd) {
        case 15: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck3, rhs, itn_base + itn_rmd, col_base, 2);
        case 14: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck3, rhs, itn_base + itn_rmd, col_base, 1);
        case 13: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck3, rhs, itn_base + itn_rmd, col_base, 0);
        case 12: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck2, rhs, itn_base + itn_rmd, col_base, 3);
        case 11: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck2, rhs, itn_base + itn_rmd, col_base, 2);
        case 10: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck2, rhs, itn_base + itn_rmd, col_base, 1);
        case  9: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck2, rhs, itn_base + itn_rmd, col_base, 0);
        case  8: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck1, rhs, itn_base + itn_rmd, col_base, 3);
        case  7: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck1, rhs, itn_base + itn_rmd, col_base, 2);
        case  6: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck1, rhs, itn_base + itn_rmd, col_base, 1);
        case  5: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck1, rhs, itn_base + itn_rmd, col_base, 0);
        case  4: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck0, rhs, itn_base + itn_rmd, col_base, 3);
        case  3: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck0, rhs, itn_base + itn_rmd, col_base, 2);
        case  2: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck0, rhs, itn_base + itn_rmd, col_base, 1);
        case  1: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_fully(rpck0, rhs, itn_base + itn_rmd, col_base, 0);
    } /* switch */
} /* v4si_load_rpcks_ipcf */

static void v4si_load_rpcks_ipcp(p_mat_mul_info_t mi, uint32_t itn_base, uint32_t itn_rmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t rhs = mi->rhs;
    v4si_t * rpck0 = mi->rpck->v4si_pcks[0];
    v4si_t * rpck1 = mi->rpck->v4si_pcks[1];
    v4si_t * rpck2 = mi->rpck->v4si_pcks[2];
    v4si_t * rpck3 = mi->rpck->v4si_pcks[3];

    switch (itn_rmd) {
        case 15: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck3, rhs, itn_base + itn_rmd, col_base, col_rmd, 2);
        case 14: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck3, rhs, itn_base + itn_rmd, col_base, col_rmd, 1);
        case 13: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck3, rhs, itn_base + itn_rmd, col_base, col_rmd, 0);
        case 12: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck2, rhs, itn_base + itn_rmd, col_base, col_rmd, 3);
        case 11: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck2, rhs, itn_base + itn_rmd, col_base, col_rmd, 2);
        case 10: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck2, rhs, itn_base + itn_rmd, col_base, col_rmd, 1);
        case  9: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck2, rhs, itn_base + itn_rmd, col_base, col_rmd, 0);
        case  8: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck1, rhs, itn_base + itn_rmd, col_base, col_rmd, 3);
        case  7: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck1, rhs, itn_base + itn_rmd, col_base, col_rmd, 2);
        case  6: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck1, rhs, itn_base + itn_rmd, col_base, col_rmd, 1);
        case  5: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck1, rhs, itn_base + itn_rmd, col_base, col_rmd, 0);
        case  4: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck0, rhs, itn_base + itn_rmd, col_base, col_rmd, 3);
        case  3: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck0, rhs, itn_base + itn_rmd, col_base, col_rmd, 2);
        case  2: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck0, rhs, itn_base + itn_rmd, col_base, col_rmd, 1);
        case  1: itn_rmd -= 1; v4si_load_rpcks_from_col_to_byte_partly(rpck0, rhs, itn_base + itn_rmd, col_base, col_rmd, 0);
    } /* switch */
} /* v4si_load_rpcks_ipcp */

/* For utilizing instruction pipelining and raising cache-hit rate, get similar calculations together. */
#define v4si_mul_pkcs_and_store(ret, vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx) \
    { \
        tmp[0] = __builtin_ia32_pmulld128(lpck[0], rpck0[idx]); \
        tmp[1] = __builtin_ia32_pmulld128(lpck[1], rpck1[idx]); \
        tmp[2] = __builtin_ia32_pmulld128(lpck[2], rpck2[idx]); \
        tmp[3] = __builtin_ia32_pmulld128(lpck[3], rpck3[idx]); \
        tmp[0] = __builtin_ia32_paddd128(tmp[0], tmp[1]); \
        tmp[0] = __builtin_ia32_paddd128(tmp[0], tmp[2]); \
        tmp[0] = __builtin_ia32_paddd128(tmp[0], tmp[3]); \
        ret += vals[0] + vals[1] + vals[2] + vals[3]; \
    }

static void v4si_mul_and_store_fully(p_mat_mul_info_t mi, unsigned pck_off, uint32_t row, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t m = mi->m;
    p_mi32_t lhs = mi->lhs;
    v4si_t tmp[4] = {0, 0, 0, 0};
    v4si_t * lpck = &lhs->v4si_pcks[pck_off];
    v4si_t * rpck0 = mi->rpck->v4si_pcks[0];
    v4si_t * rpck1 = mi->rpck->v4si_pcks[1];
    v4si_t * rpck2 = mi->rpck->v4si_pcks[2];
    v4si_t * rpck3 = mi->rpck->v4si_pcks[3];
    int32_t * vals = (int32_t *) &tmp;
    uint32_t col = col_base;

    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  0); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  1); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  2); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  3); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  4); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  5); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  6); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  7); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  8); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3,  9); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, 10); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, 11); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, 12); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, 13); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, 14); ++col;
    v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, 15); ++col;
} /* v4si_mul_and_store_fully */

static void v4si_mul_and_store_partly(p_mat_mul_info_t mi, unsigned pck_off, uint32_t row, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t m = mi->m;
    p_mi32_t lhs = mi->lhs;
    v4si_t tmp[4] = {0, 0, 0, 0};
    v4si_t * lpck = &lhs->v4si_pcks[pck_off];
    v4si_t * rpck0 = mi->rpck->v4si_pcks[0];
    v4si_t * rpck1 = mi->rpck->v4si_pcks[1];
    v4si_t * rpck2 = mi->rpck->v4si_pcks[2];
    v4si_t * rpck3 = mi->rpck->v4si_pcks[3];
    int32_t * vals = (int32_t *) &tmp;
    uint32_t idx = 0;
    uint32_t col = col_base;

    switch (col_rmd) {
        case 15: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case 14: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case 13: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case 12: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case 11: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case 10: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case  9: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case  8: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case  7: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case  6: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case  5: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case  4: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case  3: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case  2: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        case  1: v4si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, rpck2, rpck3, idx); ++col; ++idx;
        default: break;
    } /* switch */
} /* v4si_mul_and_store_partly */

static void v4si_mul_and_store(p_mat_mul_info_t mi, mat_mul_pcks_and_store_fn op, uint32_t pck_off, uint32_t ibths, uint32_t irmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t lhs = mi->lhs;
    uint32_t row = 0;
    switch (irmd) {
            do {
        case  0: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 15: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 14: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 13: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 12: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 11: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 10: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  9: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  8: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  7: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  6: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  5: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  4: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  3: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  2: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  1: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
            } while (--ibths > 0);
        default: break;
    } /* switch */
} /* v4si_mul_and_store */

#endif // MX_SSE41

#if defined(MX_AVX2)

static void v8si_load_rpcks_ifcf(p_mat_mul_info_t mi, uint32_t itn_base, uint32_t itn_rmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t rhs = mi->rhs;
    v8si_t src = {0, 0, 0, 0, 0, 0, 0, 0};
    v8si_t idx0 = {0, 1, 2, 3, 4, 5, 6, 7};
    v8si_t idx1 = {8, 9, 10, 11, 12, 13, 14, 15};
    v8si_t mask = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];

    idx0 *= rhs->mt.cols_padded;
    idx1 *= rhs->mt.cols_padded;

    rpck0[ 0] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  0], idx0, mask, 4);
    rpck1[ 0] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  0], idx1, mask, 4);
    rpck0[ 1] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  1], idx0, mask, 4);
    rpck1[ 1] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  1], idx1, mask, 4);
    rpck0[ 2] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  2], idx0, mask, 4);
    rpck1[ 2] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  2], idx1, mask, 4);
    rpck0[ 3] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  3], idx0, mask, 4);
    rpck1[ 3] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  3], idx1, mask, 4);
    rpck0[ 4] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  4], idx0, mask, 4);
    rpck1[ 4] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  4], idx1, mask, 4);
    rpck0[ 5] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  5], idx0, mask, 4);
    rpck1[ 5] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  5], idx1, mask, 4);
    rpck0[ 6] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  6], idx0, mask, 4);
    rpck1[ 6] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  6], idx1, mask, 4);
    rpck0[ 7] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  7], idx0, mask, 4);
    rpck1[ 7] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  7], idx1, mask, 4);
    rpck0[ 8] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  8], idx0, mask, 4);
    rpck1[ 8] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  8], idx1, mask, 4);
    rpck0[ 9] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  9], idx0, mask, 4);
    rpck1[ 9] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  9], idx1, mask, 4);
    rpck0[10] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 10], idx0, mask, 4);
    rpck1[10] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 10], idx1, mask, 4);
    rpck0[11] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 11], idx0, mask, 4);
    rpck1[11] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 11], idx1, mask, 4);
    rpck0[12] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 12], idx0, mask, 4);
    rpck1[12] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 12], idx1, mask, 4);
    rpck0[13] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 13], idx0, mask, 4);
    rpck1[13] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 13], idx1, mask, 4);
    rpck0[14] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 14], idx0, mask, 4);
    rpck1[14] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 14], idx1, mask, 4);
    rpck0[15] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 15], idx0, mask, 4);
    rpck1[15] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 15], idx1, mask, 4);
} /* v8si_load_rpcks_ifcf */

static void v8si_load_rpcks_ifcp(p_mat_mul_info_t mi, uint32_t itn_base, uint32_t itn_rmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t rhs = mi->rhs;
    v8si_t src = {0, 0, 0, 0, 0, 0, 0, 0};
    v8si_t idx0 = {0, 1, 2, 3, 4, 5, 6, 7};
    v8si_t idx1 = {8, 9, 10, 11, 12, 13, 14, 15};
    v8si_t mask = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];

    idx0 *= rhs->mt.cols_padded;
    idx1 *= rhs->mt.cols_padded;

    switch (col_rmd) {
        case 15:    rpck1[14] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 14], idx1, mask, 4);
                    rpck0[14] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 14], idx0, mask, 4);
        case 14:    rpck1[13] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 13], idx1, mask, 4);
                    rpck0[13] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 13], idx0, mask, 4);
        case 13:    rpck1[12] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 12], idx1, mask, 4);
                    rpck0[12] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 12], idx0, mask, 4);
        case 12:    rpck1[11] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 11], idx1, mask, 4);
                    rpck0[11] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 11], idx0, mask, 4);
        case 11:    rpck1[10] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 10], idx1, mask, 4);
                    rpck0[10] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 10], idx0, mask, 4);
        case 10:    rpck1[ 9] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  9], idx1, mask, 4);
                    rpck0[ 9] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  9], idx0, mask, 4);
        case  9:    rpck1[ 8] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  8], idx1, mask, 4);
                    rpck0[ 8] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  8], idx0, mask, 4);
        case  8:    rpck1[ 7] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  7], idx1, mask, 4);
                    rpck0[ 7] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  7], idx0, mask, 4);
        case  7:    rpck1[ 6] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  6], idx1, mask, 4);
                    rpck0[ 6] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  6], idx0, mask, 4);
        case  6:    rpck1[ 5] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  5], idx1, mask, 4);
                    rpck0[ 5] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  5], idx0, mask, 4);
        case  5:    rpck1[ 4] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  4], idx1, mask, 4);
                    rpck0[ 4] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  4], idx0, mask, 4);
        case  4:    rpck1[ 3] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  3], idx1, mask, 4);
                    rpck0[ 3] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  3], idx0, mask, 4);
        case  3:    rpck1[ 2] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  2], idx1, mask, 4);
                    rpck0[ 2] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  2], idx0, mask, 4);
        case  2:    rpck1[ 1] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  1], idx1, mask, 4);
                    rpck0[ 1] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  1], idx0, mask, 4);
        case  1:    rpck1[ 0] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  0], idx1, mask, 4);
                    rpck0[ 0] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  0], idx0, mask, 4);
        default:    break;
    } /* switch */
} /* v8si_load_rpcks_ifcp */

static void v8si_load_rpcks_ipcf(p_mat_mul_info_t mi, uint32_t itn_base, uint32_t itn_rmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t rhs = mi->rhs;
    v8si_t src = {0, 0, 0, 0, 0, 0, 0, 0};
    v8si_t idx0 = {0, 1, 2, 3, 4, 5, 6, 7};
    v8si_t idx1 = {8, 9, 10, 11, 12, 13, 14, 15};
    v8si_t mask0 = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t mask1 = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];

    idx0 *= rhs->mt.cols_padded;
    idx1 *= rhs->mt.cols_padded;

    switch (itn_rmd) {
        case  1: mask0[1] = 0;
        case  2: mask0[2] = 0;
        case  3: mask0[3] = 0;
        case  4: mask0[4] = 0;
        case  5: mask0[5] = 0;
        case  6: mask0[6] = 0;
        case  7: mask0[7] = 0;
        case  8: mask1[0] = 0;
        case  9: mask1[1] = 0;
        case 10: mask1[2] = 0;
        case 11: mask1[3] = 0;
        case 12: mask1[4] = 0;
        case 13: mask1[5] = 0;
        case 14: mask1[6] = 0;
        case 15: mask1[7] = 0;
        default: break;
    } /* switch */

    rpck0[ 0] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  0], idx0, mask0, 4);
    rpck1[ 0] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  0], idx1, mask1, 4);
    rpck0[ 1] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  1], idx0, mask0, 4);
    rpck1[ 1] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  1], idx1, mask1, 4);
    rpck0[ 2] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  2], idx0, mask0, 4);
    rpck1[ 2] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  2], idx1, mask1, 4);
    rpck0[ 3] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  3], idx0, mask0, 4);
    rpck1[ 3] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  3], idx1, mask1, 4);
    rpck0[ 4] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  4], idx0, mask0, 4);
    rpck1[ 4] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  4], idx1, mask1, 4);
    rpck0[ 5] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  5], idx0, mask0, 4);
    rpck1[ 5] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  5], idx1, mask1, 4);
    rpck0[ 6] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  6], idx0, mask0, 4);
    rpck1[ 6] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  6], idx1, mask1, 4);
    rpck0[ 7] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  7], idx0, mask0, 4);
    rpck1[ 7] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  7], idx1, mask1, 4);
    rpck0[ 8] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  8], idx0, mask0, 4);
    rpck1[ 8] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  8], idx1, mask1, 4);
    rpck0[ 9] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  9], idx0, mask0, 4);
    rpck1[ 9] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  9], idx1, mask1, 4);
    rpck0[10] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 10], idx0, mask0, 4);
    rpck1[10] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 10], idx1, mask1, 4);
    rpck0[11] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 11], idx0, mask0, 4);
    rpck1[11] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 11], idx1, mask1, 4);
    rpck0[12] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 12], idx0, mask0, 4);
    rpck1[12] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 12], idx1, mask1, 4);
    rpck0[13] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 13], idx0, mask0, 4);
    rpck1[13] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 13], idx1, mask1, 4);
    rpck0[14] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 14], idx0, mask0, 4);
    rpck1[14] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 14], idx1, mask1, 4);
    rpck0[15] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 15], idx0, mask0, 4);
    rpck1[15] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 15], idx1, mask1, 4);
} /* v8si_load_rpcks_ipcf */

static void v8si_load_rpcks_ipcp(p_mat_mul_info_t mi, uint32_t itn_base, uint32_t itn_rmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t rhs = mi->rhs;
    v8si_t src = {0, 0, 0, 0, 0, 0, 0, 0};
    v8si_t idx0 = {0, 1, 2, 3, 4, 5, 6, 7};
    v8si_t idx1 = {8, 9, 10, 11, 12, 13, 14, 15};
    v8si_t mask0 = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t mask1 = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];

    idx0 *= rhs->mt.cols_padded;
    idx1 *= rhs->mt.cols_padded;

    switch (itn_rmd) {
        case  1: mask0[1] = 0;
        case  2: mask0[2] = 0;
        case  3: mask0[3] = 0;
        case  4: mask0[4] = 0;
        case  5: mask0[5] = 0;
        case  6: mask0[6] = 0;
        case  7: mask0[7] = 0;
        case  8: mask1[0] = 0;
        case  9: mask1[1] = 0;
        case 10: mask1[2] = 0;
        case 11: mask1[3] = 0;
        case 12: mask1[4] = 0;
        case 13: mask1[5] = 0;
        case 14: mask1[6] = 0;
        case 15: mask1[7] = 0;
        default: break;
    } /* switch */

    switch (col_rmd) {
        case 15:    rpck1[14] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 14], idx1, mask1, 4);
                    rpck0[14] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 14], idx0, mask0, 4);
        case 14:    rpck1[13] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 13], idx1, mask1, 4);
                    rpck0[13] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 13], idx0, mask0, 4);
        case 13:    rpck1[12] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 12], idx1, mask1, 4);
                    rpck0[12] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 12], idx0, mask0, 4);
        case 12:    rpck1[11] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 11], idx1, mask1, 4);
                    rpck0[11] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 11], idx0, mask0, 4);
        case 11:    rpck1[10] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 10], idx1, mask1, 4);
                    rpck0[10] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base + 10], idx0, mask0, 4);
        case 10:    rpck1[ 9] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  9], idx1, mask1, 4);
                    rpck0[ 9] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  9], idx0, mask0, 4);
        case  9:    rpck1[ 8] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  8], idx1, mask1, 4);
                    rpck0[ 8] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  8], idx0, mask0, 4);
        case  8:    rpck1[ 7] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  7], idx1, mask1, 4);
                    rpck0[ 7] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  7], idx0, mask0, 4);
        case  7:    rpck1[ 6] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  6], idx1, mask1, 4);
                    rpck0[ 6] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  6], idx0, mask0, 4);
        case  6:    rpck1[ 5] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  5], idx1, mask1, 4);
                    rpck0[ 5] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  5], idx0, mask0, 4);
        case  5:    rpck1[ 4] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  4], idx1, mask1, 4);
                    rpck0[ 4] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  4], idx0, mask0, 4);
        case  4:    rpck1[ 3] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  3], idx1, mask1, 4);
                    rpck0[ 3] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  3], idx0, mask0, 4);
        case  3:    rpck1[ 2] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  2], idx1, mask1, 4);
                    rpck0[ 2] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  2], idx0, mask0, 4);
        case  2:    rpck1[ 1] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  1], idx1, mask1, 4);
                    rpck0[ 1] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  1], idx0, mask0, 4);
        case  1:    rpck1[ 0] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  0], idx1, mask1, 4);
                    rpck0[ 0] = __builtin_ia32_gathersiv8si(src, &rhs->i32_vals[itn_base][col_base +  0], idx0, mask0, 4);
    } /* switch */
} /* v8si_load_rpcks_ipcp */

/* For utilizing instruction pipelining and raising cache-hit rate, get similar calculations together. */
#define v8si_mul_pkcs_and_store(ret, vals, tmp, lpck, rpck0, rpck1, idx) \
    { \
        tmp[0] = __builtin_ia32_pmulld256(lpck[0], rpck0[idx]); \
        tmp[1] = __builtin_ia32_pmulld256(lpck[1], rpck1[idx]); \
        tmp[0] = __builtin_ia32_paddd256(tmp[0], tmp[1]); \
        ret += vals[0] + vals[1] + vals[2] + vals[3] + vals[4] + vals[5] + vals[6] + vals[7]; \
    }

static void v8si_mul_and_store_fully(p_mat_mul_info_t mi, uint32_t pck_off, uint32_t row, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t m = mi->m;
    p_mi32_t lhs = mi->lhs;
    v8si_t tmp[2] = {0, 0};
    v8si_t * lpck = &lhs->v8si_pcks[pck_off];
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];
    int32_t * vals = (int32_t *) &tmp;
    uint32_t col = col_base;

    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  0); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  1); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  2); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  3); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  4); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  5); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  6); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  7); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  8); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1,  9); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, 10); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, 11); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, 12); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, 13); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, 14); ++col;
    v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, 15); ++col;
} /* v8si_mul_and_store_fully */

static void v8si_mul_and_store_partly(p_mat_mul_info_t mi, uint32_t pck_off, uint32_t row, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t m = mi->m;
    p_mi32_t lhs = mi->lhs;
    v8si_t tmp[2] = {0, 0};
    v8si_t * lpck = &lhs->v8si_pcks[pck_off];
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];
    int32_t * vals = (int32_t *) &tmp;
    uint32_t idx = 0;
    uint32_t col = col_base;

    switch (col_rmd) {
        case 15: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case 14: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case 13: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case 12: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case 11: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case 10: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case  9: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case  8: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case  7: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case  6: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case  5: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case  4: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case  3: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case  2: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        case  1: v8si_mul_pkcs_and_store(m->i32_vals[row][col], vals, tmp, lpck, rpck0, rpck1, idx); ++col; ++idx;
        default: break;
    } /* switch */
} /* v8si_mul_and_store_partly */

static void v8si_mul_and_store(p_mat_mul_info_t mi, mat_mul_pcks_and_store_fn op, uint32_t pck_off, uint32_t ibths, uint32_t irmd, uint32_t col_base, uint32_t col_rmd)
{
    p_mi32_t lhs = mi->lhs;
    uint32_t row = 0;
    switch (irmd) {
            do {
        case  0: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 15: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 14: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 13: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 12: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 11: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case 10: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  9: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  8: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  7: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  6: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  5: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  4: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  3: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  2: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
        case  1: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->mt.pcks_in_row;
            } while (--ibths > 0);
        default: break;
    } /* switch */
} /* v8si_mul_and_store */

#endif // MX_AVX2

static p_mi32_t i32_multiply_and_store_simd(p_mi32_t m, p_mi32_t lhs, p_mi32_t rhs)
{
    mat_mul_info_t mi = {0};

    mi.m = m;
    mi.lhs = lhs;
    mi.rhs = rhs;

    mi.lhs_mt = &lhs->mt;
    mi.rhs_mt = &rhs->mt;

    mi.val_size = lhs->mt.val_size;
    mi.vals_in_pck = lhs->mt.vals_in_pck;

#if defined(MX_AVX2)
    mi.load_rpcks_ifcf = &v8si_load_rpcks_ifcf;
    mi.load_rpcks_ipcf = &v8si_load_rpcks_ipcf;
    mi.load_rpcks_ifcp = &v8si_load_rpcks_ifcp;
    mi.load_rpcks_ipcp = &v8si_load_rpcks_ipcp;

    mi.mul_and_store_fully = &v8si_mul_and_store_fully;
    mi.mul_and_store_partly = &v8si_mul_and_store_partly;

    mi.mul_and_store = &v8si_mul_and_store;
#elif defined(MX_SSE41)
    mi.load_rpcks_ifcf = &v4si_load_rpcks_ifcf;
    mi.load_rpcks_ipcf = &v4si_load_rpcks_ipcf;
    mi.load_rpcks_ifcp = &v4si_load_rpcks_ifcp;
    mi.load_rpcks_ipcp = &v4si_load_rpcks_ipcp;

    mi.mul_and_store_fully = &v4si_mul_and_store_fully;
    mi.mul_and_store_partly = &v4si_mul_and_store_partly;

    mi.mul_and_store = &v4si_mul_and_store;
#endif

    mi32_initialize_zeros(m, MX_SIMD_CODE);
    mat_multiply_and_store_simd_v2(&mi);
    return m;
} /* i32_multiply_and_store_simd */

void mi32_multiply_and_store(p_mi32_t m, p_mi32_t lhs, p_mi32_t rhs, mx_opt_t opt)
{
    if (opt & MX_SIMD_CODE) {
        i32_multiply_and_store_simd(m, lhs, rhs);
    } else {
        i32_multiply_and_store_plain(m, lhs, rhs);
    } // if
} /* mi32_multiply_and_store */

static void i32_scr_multiply_and_store_plain(p_mi32_t m, int32_t lhs, p_mi32_t rhs)
{
    uint32_t i = 0;
    uint32_t j = 0;

    for (i = 0; i < rhs->mt.rows; i += 1) {
        for (j = 0; j < rhs->mt.cols; j += 1) {
            m->i32_vals[i][j] = lhs * rhs->i32_vals[i][j];
        } /* for j */
    } /* for i */
} // i32_scr_multiply_and_store_plain

static void i32_scr_multiply_and_store_simd(p_mi32_t m, int32_t lhs, p_mi32_t rhs)
{
    uint32_t itrs = 0;
    uint32_t rmd = 0;
    uint32_t pck_cnt = rhs->mt.rows * rhs->mt.pcks_in_row;

#if defined(MX_AVX2)
    v8si_t scr = {lhs, lhs, lhs, lhs, lhs, lhs, lhs, lhs};
#elif defined(MX_SSE41)
    v4si_t scr = {lhs, lhs, lhs, lhs};
#endif

    itrs = pck_cnt / 16;
    rmd = pck_cnt % 16;

#if defined(MX_AVX2)
    for (uint32_t i = 0; i < itrs; i += 1) {
        m->v8si_pcks[i * 16 +  0] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  0], scr);
        m->v8si_pcks[i * 16 +  1] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  1], scr);
        m->v8si_pcks[i * 16 +  2] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  2], scr);
        m->v8si_pcks[i * 16 +  3] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  3], scr);
        m->v8si_pcks[i * 16 +  4] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  4], scr);
        m->v8si_pcks[i * 16 +  5] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  5], scr);
        m->v8si_pcks[i * 16 +  6] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  6], scr);
        m->v8si_pcks[i * 16 +  7] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  7], scr);
        m->v8si_pcks[i * 16 +  8] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  8], scr);
        m->v8si_pcks[i * 16 +  9] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 +  9], scr);
        m->v8si_pcks[i * 16 + 10] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 + 10], scr);
        m->v8si_pcks[i * 16 + 11] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 + 11], scr);
        m->v8si_pcks[i * 16 + 12] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 + 12], scr);
        m->v8si_pcks[i * 16 + 13] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 + 13], scr);
        m->v8si_pcks[i * 16 + 14] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 + 14], scr);
        m->v8si_pcks[i * 16 + 15] = __builtin_ia32_pmulld256(rhs->v8si_pcks[i * 16 + 15], scr);
    } /* for i */

    switch (rmd) {
        case 15: m->v8si_pcks[itrs * 16 + 14] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 + 14], scr);
        case 14: m->v8si_pcks[itrs * 16 + 13] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 + 13], scr);
        case 13: m->v8si_pcks[itrs * 16 + 12] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 + 12], scr);
        case 12: m->v8si_pcks[itrs * 16 + 11] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 + 11], scr);
        case 11: m->v8si_pcks[itrs * 16 + 10] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 + 10], scr);
        case 10: m->v8si_pcks[itrs * 16 +  9] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  9], scr);
        case  9: m->v8si_pcks[itrs * 16 +  8] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  8], scr);
        case  8: m->v8si_pcks[itrs * 16 +  7] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  7], scr);
        case  7: m->v8si_pcks[itrs * 16 +  6] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  6], scr);
        case  6: m->v8si_pcks[itrs * 16 +  5] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  5], scr);
        case  5: m->v8si_pcks[itrs * 16 +  4] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  4], scr);
        case  4: m->v8si_pcks[itrs * 16 +  3] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  3], scr);
        case  3: m->v8si_pcks[itrs * 16 +  2] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  2], scr);
        case  2: m->v8si_pcks[itrs * 16 +  1] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  1], scr);
        case  1: m->v8si_pcks[itrs * 16 +  0] = __builtin_ia32_pmulld256(rhs->v8si_pcks[itrs * 16 +  0], scr);
        default: break;
    } /* switch */
#elif defined(MX_SSE41)
    for (uint32_t i = 0; i < itrs; i += 1) {
        m->v4si_pcks[i * 16 +  0] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  0], scr);
        m->v4si_pcks[i * 16 +  1] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  1], scr);
        m->v4si_pcks[i * 16 +  2] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  2], scr);
        m->v4si_pcks[i * 16 +  3] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  3], scr);
        m->v4si_pcks[i * 16 +  4] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  4], scr);
        m->v4si_pcks[i * 16 +  5] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  5], scr);
        m->v4si_pcks[i * 16 +  6] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  6], scr);
        m->v4si_pcks[i * 16 +  7] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  7], scr);
        m->v4si_pcks[i * 16 +  8] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  8], scr);
        m->v4si_pcks[i * 16 +  9] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 +  9], scr);
        m->v4si_pcks[i * 16 + 10] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 + 10], scr);
        m->v4si_pcks[i * 16 + 11] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 + 11], scr);
        m->v4si_pcks[i * 16 + 12] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 + 12], scr);
        m->v4si_pcks[i * 16 + 13] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 + 13], scr);
        m->v4si_pcks[i * 16 + 14] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 + 14], scr);
        m->v4si_pcks[i * 16 + 15] = __builtin_ia32_pmulld128(rhs->v4si_pcks[i * 16 + 15], scr);
    } /* for i */

    switch (rmd) {
        case 15: m->v4si_pcks[itrs * 16 + 14] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 + 14], scr);
        case 14: m->v4si_pcks[itrs * 16 + 13] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 + 13], scr);
        case 13: m->v4si_pcks[itrs * 16 + 12] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 + 12], scr);
        case 12: m->v4si_pcks[itrs * 16 + 11] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 + 11], scr);
        case 11: m->v4si_pcks[itrs * 16 + 10] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 + 10], scr);
        case 10: m->v4si_pcks[itrs * 16 +  9] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  9], scr);
        case  9: m->v4si_pcks[itrs * 16 +  8] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  8], scr);
        case  8: m->v4si_pcks[itrs * 16 +  7] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  7], scr);
        case  7: m->v4si_pcks[itrs * 16 +  6] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  6], scr);
        case  6: m->v4si_pcks[itrs * 16 +  5] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  5], scr);
        case  5: m->v4si_pcks[itrs * 16 +  4] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  4], scr);
        case  4: m->v4si_pcks[itrs * 16 +  3] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  3], scr);
        case  3: m->v4si_pcks[itrs * 16 +  2] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  2], scr);
        case  2: m->v4si_pcks[itrs * 16 +  1] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  1], scr);
        case  1: m->v4si_pcks[itrs * 16 +  0] = __builtin_ia32_pmulld128(rhs->v4si_pcks[itrs * 16 +  0], scr);
        default: break;
    } /* switch */
#endif
} // i32_scr_multiply_and_store_simd

void mi32_scalar_multiply_and_store(p_mi32_t m, int32_t lhs, p_mi32_t rhs, mx_opt_t opt)
{
    if (opt & MX_SIMD_CODE) {
        i32_scr_multiply_and_store_simd(m, lhs, rhs);
    } else {
        i32_scr_multiply_and_store_plain(m, lhs, rhs);
    } // if
} // mi32_scalar_multiply_and_store

