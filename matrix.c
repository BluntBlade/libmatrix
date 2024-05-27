#include <omp.h>
#include <memory.h>
#include <smmintrin.h>
#include <stdio.h>

#include "config.h"
#include "matrix.h"

/* ==== Definitions of the matrix_t type ==== */

enum {
    I32_VALS_IN_V4SI = 4,
    I32_VALS_IN_V8SI = 8,
    U32_VALS_IN_V4SI = 4,
    U32_VALS_IN_V8SI = 8,
    F32_VALS_IN_V4SF = 4,
    F32_VALS_IN_V8SF = 8,
    D64_VALS_IN_V2DF = 2,
    D64_VALS_IN_V4DF = 4
};

enum {
    CPU_CACHE_LINE_BYTES = 64
};

#if defined(MTX_SSE41)
typedef int32_t v4si_t __attribute__ ((vector_size (16)));
#endif // MTX_SSE41

#if defined(MTX_AVX2)
typedef int32_t v8si_t __attribute__ ((vector_size (32)));
#endif // MTX_AVX2

typedef void (*mat_init_fn)(ptr_matrix_t);
typedef ptr_matrix_t (*mat_oper_fn)(ptr_matrix_t, ptr_matrix_t, ptr_matrix_t);
typedef void (*i32_scr_oper_fn)(ptr_matrix_t, int32_t, ptr_matrix_t);
typedef void (*u32_scr_oper_fn)(ptr_matrix_t, uint32_t, ptr_matrix_t);
typedef void (*f32_scr_oper_fn)(ptr_matrix_t, float, ptr_matrix_t);
typedef void (*d64_scr_oper_fn)(ptr_matrix_t, double, ptr_matrix_t);

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
} operation_t, *ptr_operation_t;

typedef struct MATRIX_T {
    ptr_operation_t ops;                /* Pointers to operation functions. */

    unsigned int    rows;               /* The number of rows. */
    unsigned int    cols;               /* The number of columns. */
    unsigned int    cols_padded;        /* The number of columns, include padding ones. */
    unsigned int    pcks_in_row;        /* The number of packs in one row. */
    unsigned int    vals_in_pck;        /* The number of values in one pack. */
    size_t          byte_cnt;           /* Allocated bytes, including pads. */
    size_t          val_size;           /* The size of one value. */

    void * val_buff;

    union {
#if defined(MTX_SSE41)
        v4si_t *    v4si_pcks;
#endif // MTX_SSE41

#if defined(MTX_AVX2)
        v8si_t *    v8si_pcks;
#endif // MTX_AVX2

        void *      val_data;
    };

    union {
        int32_t *   i32_vals[0];
        uint32_t *  u32_vals[0];
        float *     f32_vals[0];
        double *    d64_vals[0];
        void *      val_ptrs[0];
    };
} matrix_t;

inline static unsigned int round_count_to_multiples_of(unsigned int cnt, unsigned int n)
{
    return (cnt + (n - 1)) & (~(n - 1));
} /* round_count_to_multiples_of */

static ptr_matrix_t mtx_allocate(unsigned int rows, unsigned int cols, size_t val_size, size_t vals_in_pck, ptr_operation_t ops)
{
    ptr_matrix_t m = NULL;
    unsigned int padded_col_cnt = 0;
    unsigned int i = 0;
    size_t alignment = 0;

    m = calloc(sizeof(matrix_t) + sizeof(void *) * rows, 1);
    if (! m) {
        return NULL;
    } /* if */

    // NOTE: Append padding values to each row for memory alignment,
    //       and this speeds up loading values.
    padded_col_cnt = round_count_to_multiples_of(cols, vals_in_pck);

    m->rows = rows;
    m->cols = cols;
    m->cols_padded = padded_col_cnt;
    m->byte_cnt = val_size * rows * padded_col_cnt;
    m->pcks_in_row = padded_col_cnt / vals_in_pck;
    m->vals_in_pck = vals_in_pck;
    m->val_size = val_size;

    m->ops = ops;

    alignment = val_size * vals_in_pck;
    m->val_buff = malloc(m->byte_cnt + alignment);
    if (! m->val_buff) {
        free(m);
        return NULL;
    } /* if */

    /* NOTE: Align to the vector type's size, otherwise it will be segmentation fault when access to packs. */
    m->val_data = (void *)( ((size_t)m->val_buff + (alignment - 1)) & (~(alignment - 1)));
    for (i = 0; i < m->rows; i += 1) {
        m->val_ptrs[i] = m->val_data  + i * m->val_size * m->cols_padded;
    } /* for */
    return m;
} /* mtx_allocate */

ptr_matrix_t mtx_allocate_for_multiplying(ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    return mtx_allocate(lhs->rows, rhs->cols, lhs->val_size, lhs->vals_in_pck, lhs->ops);
} /* mtx_allocate_for_multiplying */

ptr_matrix_t mtx_allocate_for_transposing(ptr_matrix_t src)
{
    return mtx_allocate(src->cols, src->rows, src->val_size, src->vals_in_pck, src->ops);
} /* mtx_allocate_for_transposing */

ptr_matrix_t mtx_allocate_in_shape_of(ptr_matrix_t src)
{
    return mtx_allocate(src->rows, src->cols, src->val_size, src->vals_in_pck, src->ops);
} /* mtx_allocate_in_shape_of */

ptr_matrix_t mtx_duplicate(ptr_matrix_t src)
{
    ptr_matrix_t m = mtx_allocate(src->rows, src->cols, src->val_size, src->vals_in_pck, src->ops);
    if (! m) {
        return NULL;
    } /* if */
    memcpy(m->val_data, src->val_data, src->byte_cnt);
    return m;
} /* mtx_duplicate */

void mtx_destroy(ptr_matrix_t m)
{
    if (m) {
        free(m->val_buff);
    } /* if */
    free(m);
} /* mtx_destroy */

void mtx_initialize_identity(ptr_matrix_t m, mtx_option_t opt)
{
    (*m->ops->init_identity[opt & 0x3])(m);
} /* mtx_initialize_identity */

void mtx_initialize_zeros(ptr_matrix_t m, mtx_option_t opt)
{
    (*m->ops->init_zeros[opt & 0x3])(m);
} /* mtx_initialize_zeros */

void mtx_initialize_ones(ptr_matrix_t m, mtx_option_t opt)
{
    (*m->ops->init_ones[opt & 0x3])(m);
} /* mtx_initialize_ones */

int mtx_can_do_add(ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    return (lhs->rows == rhs->rows) && (lhs->cols == rhs->cols);
} /* mtx_can_do_add */

int mtx_can_do_multiply(ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    return (lhs->cols == rhs->rows);
} /* mtx_can_do_multiply */

unsigned int mtx_count_rows(ptr_matrix_t m)
{
    return m->rows;
} /* mtx_count_rows */

unsigned int mtx_count_columns(ptr_matrix_t m)
{
    return m->cols;
} /* mtx_count_columns */

unsigned int mtx_count_values(ptr_matrix_t m)
{
    return (m->rows * m->cols);
} /* mtx_count_values */

void mtx_transpose_and_store(ptr_matrix_t m, ptr_matrix_t src)
{
    unsigned int i = 0;
    unsigned int j = 0;

    for (i = 0; i < src->rows; i += 1) {
        for (j = 0; j < src->cols; j += 1) {
            m->i32_vals[j][i] = src->i32_vals[i][j];
        } /* for j */
    } /* for i */
} /* mtx_transpose_and_store */

void mtx_add_and_store(ptr_matrix_t m, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*m->ops->mat_add[opt & 0x3])(m, lhs, rhs);
} /* mtx_add_and_store */

void mtx_sub_and_store(ptr_matrix_t m, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*m->ops->mat_sub[opt & 0x3])(m, lhs, rhs);
} /* mtx_sub_and_store */

void mtx_multiply_and_store(ptr_matrix_t m, ptr_matrix_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*m->ops->mat_mul[opt & 0x3])(m, lhs, rhs);
} /* mtx_multiply_and_store */

/* ==== Type-Related functions. ==== */

int32_t mtx_i32_get(ptr_matrix_t m, unsigned int row, unsigned int col)
{
    return m->i32_vals[row][col];
} /* mtx_i32_get */

void mtx_i32_set(ptr_matrix_t m, unsigned int row, unsigned int col, int32_t src_val)
{
    m->i32_vals[row][col] = src_val;
} /* mtx_i32_set */

void mtx_i32_set_each(ptr_matrix_t m, int32_t src_val)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < m->rows; i += 1) {
        for (j = 0; j < m->cols; j += 1) {
            m->i32_vals[i][j] = src_val;
        } /* for */
    } /* for */
    return;
} /* mtx_i32_set_each */

static ptr_matrix_t i32_add_and_store_plain(ptr_matrix_t m, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < lhs->rows; i += 1) {
        for (j = 0; j < lhs->cols; j += 1) {
            m->i32_vals[i][j] = lhs->i32_vals[i][j] + rhs->i32_vals[i][j];
        } /* for */
    } /* for */
    return m;
} /* i32_add_and_store_plain */

static ptr_matrix_t i32_sub_and_store_plain(ptr_matrix_t m, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < lhs->rows; i += 1) {
        for (j = 0; j < lhs->cols; j += 1) {
            m->i32_vals[i][j] = lhs->i32_vals[i][j] - rhs->i32_vals[i][j];
        } /* for */
    } /* for */
    return m;
} /* i32_sub_and_store_plain */

static ptr_matrix_t i32_multiply_and_store_plain(ptr_matrix_t m, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;

    for (i = 0; i < lhs->rows; i += 1) {
        for (j = 0; j < rhs->cols; j += 1) {
            m->i32_vals[i][j] = 0;
            for (k = 0; k < lhs->cols; k += 1) {
                m->i32_vals[i][j] += lhs->i32_vals[i][k] * rhs->i32_vals[k][j];
            } /* for k */
        } /* for j */
    } /* for i */
    return m;
} /* i32_multiply_and_store_plain */

struct MAT_MUL_INFO_T;
typedef struct MAT_MUL_INFO_T * ptr_mat_mul_info_t;

typedef void (*mat_load_rpcks_fn)(ptr_mat_mul_info_t, unsigned int, unsigned int, unsigned int, unsigned int);
typedef void (*mat_mul_pcks_and_store_fn)(ptr_mat_mul_info_t, unsigned int, unsigned int, unsigned int, unsigned int);
typedef void (*mat_mul_and_store_fn)(ptr_mat_mul_info_t, mat_mul_pcks_and_store_fn, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

/* Force memory alignment at 32-byte boundary to avoid segmentation fault. */
typedef union RIGHT_PACK_T {
#if defined(MTX_SSE41)
    v4si_t v4si_pcks[CPU_CACHE_LINE_BYTES / sizeof(v4si_t)][CPU_CACHE_LINE_BYTES / sizeof(int32_t)];
#endif // MTX_SSE41

#if defined(MTX_AVX2)
    v8si_t v8si_pcks[CPU_CACHE_LINE_BYTES / sizeof(v8si_t)][CPU_CACHE_LINE_BYTES / sizeof(int32_t)];
#endif // MTX_AVX2
} rpck_t, *ptr_rpck_t;

typedef struct MAT_MUL_INFO_T {
    ptr_matrix_t m;
    ptr_matrix_t lhs;
    ptr_matrix_t rhs;

    mat_load_rpcks_fn load_rpcks_ifcf;
    mat_load_rpcks_fn load_rpcks_ifcp;
    mat_load_rpcks_fn load_rpcks_ipcf;
    mat_load_rpcks_fn load_rpcks_ipcp;

    mat_mul_pcks_and_store_fn mul_and_store_fully;
    mat_mul_pcks_and_store_fn mul_and_store_partly;

    mat_mul_and_store_fn mul_and_store;

    unsigned int val_size;
    unsigned int vals_in_pck;
    unsigned int pcks_per_bth;
    unsigned int vals_per_bth;
    unsigned int jbths;
    unsigned int jrmd;
    unsigned int kbths;
    unsigned int krmd;
    unsigned int ibths;
    unsigned int irmd;

    ptr_rpck_t rpck;
} mat_mul_info_t;

#if defined(MTX_SSE41)

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

static void v4si_load_rpcks_ifcf(ptr_mat_mul_info_t mi, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t rhs = mi->rhs;
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

static void v4si_load_rpcks_ifcp(ptr_mat_mul_info_t mi, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t rhs = mi->rhs;
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

static void v4si_load_rpcks_ipcf(ptr_mat_mul_info_t mi, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t rhs = mi->rhs;
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

static void v4si_load_rpcks_ipcp(ptr_mat_mul_info_t mi, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t rhs = mi->rhs;
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

static void v4si_mul_and_store_fully(ptr_mat_mul_info_t mi, unsigned pck_off, unsigned int row, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t m = mi->m;
    ptr_matrix_t lhs = mi->lhs;
    v4si_t tmp[4] = {0, 0, 0, 0};
    v4si_t * lpck = &lhs->v4si_pcks[pck_off];
    v4si_t * rpck0 = mi->rpck->v4si_pcks[0];
    v4si_t * rpck1 = mi->rpck->v4si_pcks[1];
    v4si_t * rpck2 = mi->rpck->v4si_pcks[2];
    v4si_t * rpck3 = mi->rpck->v4si_pcks[3];
    int32_t * vals = (int32_t *) &tmp;
    unsigned int col = col_base;

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

static void v4si_mul_and_store_partly(ptr_mat_mul_info_t mi, unsigned pck_off, unsigned int row, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t m = mi->m;
    ptr_matrix_t lhs = mi->lhs;
    v4si_t tmp[4] = {0, 0, 0, 0};
    v4si_t * lpck = &lhs->v4si_pcks[pck_off];
    v4si_t * rpck0 = mi->rpck->v4si_pcks[0];
    v4si_t * rpck1 = mi->rpck->v4si_pcks[1];
    v4si_t * rpck2 = mi->rpck->v4si_pcks[2];
    v4si_t * rpck3 = mi->rpck->v4si_pcks[3];
    int32_t * vals = (int32_t *) &tmp;
    unsigned int idx = 0;
    unsigned int col = col_base;

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

static void v4si_mul_and_store(ptr_mat_mul_info_t mi, mat_mul_pcks_and_store_fn op, unsigned int pck_off, unsigned int ibths, unsigned int irmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t lhs = mi->lhs;
    unsigned int row = 0;
    switch (irmd) {
            do {
        case  0: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 15: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 14: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 13: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 12: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 11: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 10: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  9: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  8: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  7: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  6: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  5: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  4: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  3: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  2: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  1: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
            } while (--ibths > 0);
        default: break;
    } /* switch */
} /* v4si_mul_and_store */

#endif // MTX_SSE41

#if defined(MTX_AVX2)

static void v8si_load_rpcks_ifcf(ptr_mat_mul_info_t mi, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t rhs = mi->rhs;
    v8si_t src = {0, 0, 0, 0, 0, 0, 0, 0};
    v8si_t idx0 = {0, 1, 2, 3, 4, 5, 6, 7};
    v8si_t idx1 = {8, 9, 10, 11, 12, 13, 14, 15};
    v8si_t mask = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];

    idx0 *= rhs->cols_padded;
    idx1 *= rhs->cols_padded;

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

static void v8si_load_rpcks_ifcp(ptr_mat_mul_info_t mi, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t rhs = mi->rhs;
    v8si_t src = {0, 0, 0, 0, 0, 0, 0, 0};
    v8si_t idx0 = {0, 1, 2, 3, 4, 5, 6, 7};
    v8si_t idx1 = {8, 9, 10, 11, 12, 13, 14, 15};
    v8si_t mask = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];

    idx0 *= rhs->cols_padded;
    idx1 *= rhs->cols_padded;

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

static void v8si_load_rpcks_ipcf(ptr_mat_mul_info_t mi, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t rhs = mi->rhs;
    v8si_t src = {0, 0, 0, 0, 0, 0, 0, 0};
    v8si_t idx0 = {0, 1, 2, 3, 4, 5, 6, 7};
    v8si_t idx1 = {8, 9, 10, 11, 12, 13, 14, 15};
    v8si_t mask0 = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t mask1 = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];

    idx0 *= rhs->cols_padded;
    idx1 *= rhs->cols_padded;

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

static void v8si_load_rpcks_ipcp(ptr_mat_mul_info_t mi, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t rhs = mi->rhs;
    v8si_t src = {0, 0, 0, 0, 0, 0, 0, 0};
    v8si_t idx0 = {0, 1, 2, 3, 4, 5, 6, 7};
    v8si_t idx1 = {8, 9, 10, 11, 12, 13, 14, 15};
    v8si_t mask0 = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t mask1 = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];

    idx0 *= rhs->cols_padded;
    idx1 *= rhs->cols_padded;

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

static void v8si_mul_and_store_fully(ptr_mat_mul_info_t mi, unsigned int pck_off, unsigned int row, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t m = mi->m;
    ptr_matrix_t lhs = mi->lhs;
    v8si_t tmp[2] = {0, 0};
    v8si_t * lpck = &lhs->v8si_pcks[pck_off];
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];
    int32_t * vals = (int32_t *) &tmp;
    unsigned int col = col_base;

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

static void v8si_mul_and_store_partly(ptr_mat_mul_info_t mi, unsigned int pck_off, unsigned int row, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t m = mi->m;
    ptr_matrix_t lhs = mi->lhs;
    v8si_t tmp[2] = {0, 0};
    v8si_t * lpck = &lhs->v8si_pcks[pck_off];
    v8si_t * rpck0 = mi->rpck->v8si_pcks[0];
    v8si_t * rpck1 = mi->rpck->v8si_pcks[1];
    int32_t * vals = (int32_t *) &tmp;
    unsigned int idx = 0;
    unsigned int col = col_base;

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

static void v8si_mul_and_store(ptr_mat_mul_info_t mi, mat_mul_pcks_and_store_fn op, unsigned int pck_off, unsigned int ibths, unsigned int irmd, unsigned int col_base, unsigned int col_rmd)
{
    ptr_matrix_t lhs = mi->lhs;
    unsigned int row = 0;
    switch (irmd) {
            do {
        case  0: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 15: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 14: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 13: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 12: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 11: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 10: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  9: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  8: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  7: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  6: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  5: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  4: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  3: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  2: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  1: (*op)(mi, pck_off, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
            } while (--ibths > 0);
        default: break;
    } /* switch */
} /* v8si_mul_and_store */

#endif // MTX_AVX2

static void mat_multiply_and_store_simd_v2(ptr_mat_mul_info_t mi)
{
    mi->vals_per_bth = (CPU_CACHE_LINE_BYTES / mi->val_size);
    mi->pcks_per_bth = mi->vals_per_bth / mi->vals_in_pck;
    mi->jbths = mi->rhs->cols / mi->vals_per_bth;
    mi->jrmd = mi->rhs->cols % mi->vals_per_bth;
    mi->kbths = mi->rhs->rows / mi->vals_per_bth;
    mi->krmd = mi->rhs->rows % mi->vals_per_bth;
    mi->ibths = (mi->lhs->rows + (mi->vals_per_bth - 1)) / mi->vals_per_bth;
    mi->irmd = mi->lhs->rows & (mi->vals_per_bth - 1);

#pragma omp parallel for schedule(static)
    for (unsigned int j = 0; j < mi->jbths; j += 1) {
        mat_mul_info_t mi2 = *mi;
        rpck_t rpck;
        unsigned int itn_base = 0;
        unsigned int col_base = j * mi->vals_per_bth;
        unsigned int pck_off = 0;

        mi2.rpck = &rpck;
        for (unsigned int k = 0; k < mi->kbths; k += 1) {
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
        unsigned int pck_off = 0;
        unsigned int itn_base = 0;
        unsigned int col_base = mi->jbths * mi->vals_per_bth;

        mi2.rpck = &rpck;
        for (unsigned int k = 0; k < mi->kbths; k += 1) {
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

static ptr_matrix_t i32_multiply_and_store_simd(ptr_matrix_t m, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    mat_mul_info_t mi = {0};

    mtx_initialize_zeros(m, MTX_SIMD_CODE);

    mi.m = m;
    mi.lhs = lhs;
    mi.rhs = rhs;
    mi.val_size = lhs->val_size;
    mi.vals_in_pck = lhs->vals_in_pck;

#if defined(MTX_AVX2)
    mi.load_rpcks_ifcf = &v8si_load_rpcks_ifcf;
    mi.load_rpcks_ipcf = &v8si_load_rpcks_ipcf;
    mi.load_rpcks_ifcp = &v8si_load_rpcks_ifcp;
    mi.load_rpcks_ipcp = &v8si_load_rpcks_ipcp;

    mi.mul_and_store_fully = &v8si_mul_and_store_fully;
    mi.mul_and_store_partly = &v8si_mul_and_store_partly;

    mi.mul_and_store = &v8si_mul_and_store;
#elif defined(MTX_SSE41)
    mi.load_rpcks_ifcf = &v4si_load_rpcks_ifcf;
    mi.load_rpcks_ipcf = &v4si_load_rpcks_ipcf;
    mi.load_rpcks_ifcp = &v4si_load_rpcks_ifcp;
    mi.load_rpcks_ipcp = &v4si_load_rpcks_ipcp;

    mi.mul_and_store_fully = &v4si_mul_and_store_fully;
    mi.mul_and_store_partly = &v4si_mul_and_store_partly;

    mi.mul_and_store = &v4si_mul_and_store;
#endif

    mat_multiply_and_store_simd_v2(&mi);
    return m;
} /* i32_multiply_and_store_simd */

static void i32_scr_multiply_and_store_plain(ptr_matrix_t m, int32_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;

    for (i = 0; i < rhs->rows; i += 1) {
        for (j = 0; j < rhs->cols; j += 1) {
            m->i32_vals[i][j] = lhs * rhs->i32_vals[i][j];
        } /* for j */
    } /* for i */
} /* i32_scr_multiply_and_store_plain */

static void i32_scr_multiply_and_store_simd(ptr_matrix_t m, int32_t lhs, ptr_matrix_t rhs)
{
    unsigned int itrs = 0;
    unsigned int rmd = 0;
    unsigned int pck_cnt = rhs->rows * rhs->pcks_in_row;

#if defined(MTX_AVX2)
    v8si_t scr = {lhs, lhs, lhs, lhs, lhs, lhs, lhs, lhs};
#elif defined(MTX_SSE41)
    v4si_t scr = {lhs, lhs, lhs, lhs};
#endif

    itrs = pck_cnt / 16;
    rmd = pck_cnt % 16;

#if defined(MTX_AVX2)
    for (unsigned int i = 0; i < itrs; i += 1) {
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
#elif defined(MTX_SSE41)
    for (unsigned int i = 0; i < itrs; i += 1) {
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
} /* i32_scr_multiply_and_store_simd */

void mtx_i32_scalar_multiply_and_store(ptr_matrix_t m, int32_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*m->ops->i32_scr_mul[opt & 0x3])(m, lhs, rhs);
} /* mtx_i32_scalar_multiply_and_store */

static void i32_init_identity_plain(ptr_matrix_t m)
{
    unsigned int i = 0;
    memset(m->val_data, 0, m->byte_cnt);
    for (i = 0; i < m->rows; i += 1) {
        m->i32_vals[i][i] = 1;
    } /* for */
} /* i32_init_identity_plain */

static void i32_init_zeros_plain(ptr_matrix_t m)
{
    memset(m->val_data, 0, m->byte_cnt);
} /* i32_init_zeros_plain */

static void i32_init_ones_plain(ptr_matrix_t m)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < m->rows; i += 1) {
        for (j = 0; j < m->cols; j += 1) {
            m->i32_vals[i][j] = 1;
        } /* for */
    } /* for */
} /* i32_init_ones_plain */

static operation_t i32_ops = {
    {
        &i32_init_identity_plain,
        &i32_init_identity_plain
    },
    {
        &i32_init_zeros_plain,
        &i32_init_zeros_plain
    },
    {
        &i32_init_ones_plain,
        &i32_init_ones_plain
    },
    {&i32_add_and_store_plain, &i32_add_and_store_plain},
    {&i32_sub_and_store_plain, &i32_sub_and_store_plain},
    {&i32_multiply_and_store_plain, &i32_multiply_and_store_simd},
    {&i32_scr_multiply_and_store_plain, &i32_scr_multiply_and_store_simd},
};

ptr_matrix_t mtx_i32_allocate(unsigned int rows, unsigned int cols)
{
    unsigned int i = 0;
    ptr_matrix_t m = NULL;

#if defined(MTX_AVX2)
    m = mtx_allocate(rows, cols, sizeof(int32_t), I32_VALS_IN_V8SI, &i32_ops);
#elif defined(MTX_SSE41)
    m = mtx_allocate(rows, cols, sizeof(int32_t), I32_VALS_IN_V4SI, &i32_ops);
#endif
    if (! m) {
        return NULL;
    } /* if */
    return m;
} /* mtx_i32_allocate */

