#include <omp.h>
#include <memory.h>
#include <smmintrin.h>

#include "matrix.h"

/* ==== Definitions of the mtx_block_t type ==== */

typedef struct BLOCK_T {
    mtx_block_t     block;          /* The block struct use by client code. */
    void *          row_ptrs[0];    /* Pointers to each row of the block. */
} block_t, *ptr_block_t;

inline static ptr_mtx_block_t iblk_to_eblk(ptr_block_t iblk)
{
    return &iblk->block;
} /* iblk_to_eblk */

inline static ptr_block_t eblk_to_iblk(ptr_mtx_block_t eblk)
{
    return (ptr_block_t)(eblk - (&(((ptr_block_t)(0))->block) - 0));
} /* eblk_to_iblk */

ptr_mtx_block_t mtx_blk_create(unsigned int row_max, unsigned int col_max)
{
    ptr_block_t iblk = calloc(sizeof(block_t) + sizeof(void *) * row_max, 1);
    if (! iblk) {
        return NULL;
    } /* if */
    iblk->block.row_max = row_max;
    iblk->block.col_max = col_max;
    iblk->block.row_cnt = 0;
    iblk->block.col_cnt = 0;
    iblk->block.i32_vals = (int32_t **)&iblk->row_ptrs[0];
    return iblk_to_eblk(iblk);
} /* mtx_blk_create */

void mtx_blk_destroy(ptr_mtx_block_t eblk)
{
    free(eblk_to_iblk(eblk));
} /* mtx_blk_destroy */

/* ==== Definitions of the matrix_t type ==== */

enum {
    I32_PACK_LEN = 4,
    U32_PACK_LEN = 4,
    F32_PACK_LEN = 4,
    D64_PACK_LEN = 2
};

enum {
    CPU_CACHE_LINE_BYTES = 64
};

typedef int32_t v4si_t __attribute__ ((vector_size (16)));

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
    unsigned int    pcks_in_row;        /* The number of packs in one row. */
    unsigned int    vals_in_pck;        /* The number of values in one pack. */
    size_t          byte_cnt;           /* Allocated bytes, including pads. */
    size_t          val_size;           /* The size of one value. */

    union {
        v4si_t *    i32_pcks;
        void *      data;
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

    m = calloc(sizeof(matrix_t) + sizeof(void *) * rows, 1);
    if (! m) {
        return NULL;
    } /* if */

    padded_col_cnt = round_count_to_multiples_of(cols, vals_in_pck);

    m->rows = rows;
    m->cols = cols;
    m->byte_cnt = val_size * rows * padded_col_cnt;
    m->pcks_in_row = padded_col_cnt / vals_in_pck;
    m->vals_in_pck = vals_in_pck;
    m->val_size = val_size;

    m->ops = ops;

    m->data = malloc(m->byte_cnt);
    if (! m->data) {
        free(m);
        return NULL;
    } /* if */

    for (i = 0; i < m->rows; i += 1) {
        m->val_ptrs[i] = m->data + i * m->val_size * padded_col_cnt;
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
    memcpy(m->data, src->data, src->byte_cnt);
    return m;
} /* mtx_duplicate */

void mtx_destroy(ptr_matrix_t m)
{
    if (m) {
        free(m->data);
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

void mtx_get_submatrix(ptr_matrix_t m, unsigned int row, unsigned int col, unsigned int rows, unsigned int cols, ptr_submatrix_t ref)
{
    if (m->rows <= row) {
        /* Beyond the right boundary. */
        ref->row_off = m->rows;
    } else {
        ref->row_off = row;
    } /* if */
    if (m->cols <= col) {
        /* Beyond the bottom boundary. */
        ref->col_off = m->cols;
    } else {
        ref->col_off = col;
    } /* if */

    ref->row_cnt = (rows <= m->rows - ref->row_off) ? rows : (m->rows - ref->row_off);
    ref->col_cnt = (cols <= m->cols - ref->col_off) ? cols : (m->cols - ref->col_off);

    if (ref->row_cnt == 0 || ref->col_cnt == 0) {
        /* If the target sub-matrix is beyond the right or bottom boundary, return an empty one. */
        ref->row_cnt = 0;
        ref->col_cnt = 0;
    } /* if */

    ref->val_ptrs = m->val_ptrs;
} /* mtx_get_submatrix */

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

static void v4si_load_rpcks_ifcf(v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2, v4si_t * rpck3, ptr_matrix_t rhs, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
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

static void v4si_load_rpcks_ifcp(v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2, v4si_t * rpck3, ptr_matrix_t rhs, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
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

static void v4si_load_rpcks_ipcf(v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2, v4si_t * rpck3, ptr_matrix_t rhs, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
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

static void v4si_load_rpcks_ipcp(v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2, v4si_t * rpck3, ptr_matrix_t rhs, unsigned int itn_base, unsigned int itn_rmd, unsigned int col_base, unsigned int col_rmd)
{
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

static void v4si_mul_and_store_fully(ptr_matrix_t m, v4si_t * lpck, v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2, v4si_t * rpck3, unsigned int row, unsigned int col_base, unsigned int col_rmd)
{
    v4si_t tmp[4] = {0};
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

static void v4si_mul_and_store_partly(ptr_matrix_t m, v4si_t * lpck, v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2, v4si_t * rpck3, unsigned int row, unsigned int col_base, unsigned int col_rmd)
{
    v4si_t tmp[4] = {0, 0, 0, 0};
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

typedef void (*mul_pcks_and_store)(ptr_matrix_t, v4si_t *, v4si_t *, v4si_t *, v4si_t *, v4si_t *, unsigned int, unsigned int, unsigned int);

static void v4si_do_mul_and_store(ptr_matrix_t m, mul_pcks_and_store mul_and_store, unsigned int ibths, unsigned int irmd, ptr_matrix_t lhs, unsigned pck_off, v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2, v4si_t * rpck3, unsigned int col_base, unsigned int col_rmd)
{
    unsigned int row = 0;
    switch (irmd) {
            do {
        case  0: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 15: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 14: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 13: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 12: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 11: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case 10: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  9: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  8: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  7: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  6: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  5: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  4: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  3: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  2: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
        case  1: (*mul_and_store)(m, &lhs->i32_pcks[pck_off], rpck0, rpck1, rpck2, rpck3, row, col_base, col_rmd); ++row; pck_off += lhs->pcks_in_row;
            } while (--ibths > 0);
        default: break;
    } /* switch */
} /* v4si_do_mul_and_store */

static ptr_matrix_t i32_multiply_and_store_simd_v2(ptr_matrix_t m, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    const unsigned int vals_per_bth = (CPU_CACHE_LINE_BYTES / lhs->val_size);
    const unsigned int pcks_per_bth = vals_per_bth / lhs->vals_in_pck;
    const unsigned int jbths = rhs->cols / vals_per_bth;
    const unsigned int jrmd = rhs->cols % vals_per_bth;
    const unsigned int kbths = rhs->rows / vals_per_bth;
    const unsigned int krmd = rhs->rows % vals_per_bth;
    const unsigned int ibths = (lhs->rows + (vals_per_bth - 1)) / vals_per_bth;
    const unsigned int irmd = lhs->rows & (vals_per_bth - 1);
    const unsigned int bytes = lhs->val_size * lhs->vals_in_pck * pcks_per_bth * vals_per_bth;

#pragma omp parallel for schedule(static)
    for (unsigned int j = 0; j < jbths; j += 1) {
        v4si_t rpck[pcks_per_bth][vals_per_bth];
        unsigned int itn_base = 0;
        unsigned int col_base = j * vals_per_bth;
        unsigned int pck_off = 0;

        for (unsigned int k = 0; k < kbths; k += 1) {
            v4si_load_rpcks_ifcf(rpck[0], rpck[1], rpck[2], rpck[3], rhs, itn_base, 0, col_base, 0);
            v4si_do_mul_and_store(m, v4si_mul_and_store_fully, ibths, irmd, lhs, pck_off, rpck[0], rpck[1], rpck[2], rpck[3], col_base, 0);
            itn_base += vals_per_bth;
            pck_off += pcks_per_bth;
        } /* for */

        if (krmd > 0) {
            memset(rpck, 0, bytes);
            v4si_load_rpcks_ipcf(rpck[0], rpck[1], rpck[2], rpck[3], rhs, itn_base, krmd, col_base, 0);
            v4si_do_mul_and_store(m, v4si_mul_and_store_fully, ibths, irmd, lhs, pck_off, rpck[0], rpck[1], rpck[2], rpck[3], col_base, 0);
        } /* if */
    } /* for */

    {
        v4si_t rpck[pcks_per_bth][vals_per_bth];
        unsigned int itn_base = 0;
        unsigned int col_base = jbths * vals_per_bth;
        unsigned int pck_off = 0;

        for (unsigned int k = 0; k < kbths; k += 1) {
            memset(rpck, 0, bytes);
            v4si_load_rpcks_ifcp(rpck[0], rpck[1], rpck[2], rpck[3], rhs, itn_base, 0, col_base, jrmd);
            v4si_do_mul_and_store(m, v4si_mul_and_store_partly, ibths, irmd, lhs, pck_off, rpck[0], rpck[1], rpck[2], rpck[3], col_base, jrmd);
            itn_base += vals_per_bth;
            pck_off += pcks_per_bth;
        } /* for */

        if (krmd > 0) {
            memset(rpck, 0, bytes);
            v4si_load_rpcks_ipcp(rpck[0], rpck[1], rpck[2], rpck[3], rhs, itn_base, krmd, col_base, jrmd);
            v4si_do_mul_and_store(m, v4si_mul_and_store_partly, ibths, irmd, lhs, pck_off, rpck[0], rpck[1], rpck[2], rpck[3], col_base, jrmd);
        } /* if */
    }
    return m;
} /* i32_multiply_and_store_simd_v2 */

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
    v4si_t scr = {0};
    unsigned int itrs = 0;
    unsigned int rmd = 0;
    unsigned int pck_cnt = rhs->rows * rhs->pcks_in_row;
    
    scr = __builtin_ia32_vec_set_v4si(scr, lhs, 0);
    scr = __builtin_ia32_vec_set_v4si(scr, lhs, 1);
    scr = __builtin_ia32_vec_set_v4si(scr, lhs, 2);
    scr = __builtin_ia32_vec_set_v4si(scr, lhs, 3);

    itrs = pck_cnt / 16;
    rmd = pck_cnt % 16;

    for (unsigned int i = 0; i < itrs; i += 1) {
        m->i32_pcks[i * 16 +  0] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  0], scr);
        m->i32_pcks[i * 16 +  1] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  1], scr);
        m->i32_pcks[i * 16 +  2] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  2], scr);
        m->i32_pcks[i * 16 +  3] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  3], scr);
        m->i32_pcks[i * 16 +  4] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  4], scr);
        m->i32_pcks[i * 16 +  5] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  5], scr);
        m->i32_pcks[i * 16 +  6] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  6], scr);
        m->i32_pcks[i * 16 +  7] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  7], scr);
        m->i32_pcks[i * 16 +  8] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  8], scr);
        m->i32_pcks[i * 16 +  9] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 +  9], scr);
        m->i32_pcks[i * 16 + 10] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 + 10], scr);
        m->i32_pcks[i * 16 + 11] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 + 11], scr);
        m->i32_pcks[i * 16 + 12] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 + 12], scr);
        m->i32_pcks[i * 16 + 13] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 + 13], scr);
        m->i32_pcks[i * 16 + 14] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 + 14], scr);
        m->i32_pcks[i * 16 + 15] = __builtin_ia32_pmulld128(rhs->i32_pcks[i * 16 + 15], scr);
    } /* for i */

    switch (rmd) {
        case 15: m->i32_pcks[itrs * 16 + 14] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 + 14], scr);
        case 14: m->i32_pcks[itrs * 16 + 13] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 + 13], scr);
        case 13: m->i32_pcks[itrs * 16 + 12] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 + 12], scr);
        case 12: m->i32_pcks[itrs * 16 + 11] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 + 11], scr);
        case 11: m->i32_pcks[itrs * 16 + 10] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 + 10], scr);
        case 10: m->i32_pcks[itrs * 16 +  9] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  9], scr);
        case  9: m->i32_pcks[itrs * 16 +  8] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  8], scr);
        case  8: m->i32_pcks[itrs * 16 +  7] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  7], scr);
        case  7: m->i32_pcks[itrs * 16 +  6] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  6], scr);
        case  6: m->i32_pcks[itrs * 16 +  5] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  5], scr);
        case  5: m->i32_pcks[itrs * 16 +  4] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  4], scr);
        case  4: m->i32_pcks[itrs * 16 +  3] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  3], scr);
        case  3: m->i32_pcks[itrs * 16 +  2] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  2], scr);
        case  2: m->i32_pcks[itrs * 16 +  1] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  1], scr);
        case  1: m->i32_pcks[itrs * 16 +  0] = __builtin_ia32_pmulld128(rhs->i32_pcks[itrs * 16 +  0], scr);
        default: break;
    } /* switch */
} /* i32_scr_multiply_and_store_simd */

void mtx_i32_scalar_multiply_and_store(ptr_matrix_t m, int32_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*m->ops->i32_scr_mul[opt & 0x3])(m, lhs, rhs);
} /* mtx_i32_scalar_multiply_and_store */

static void i32_init_identity_plain(ptr_matrix_t m)
{
    unsigned int i = 0;
    memset(m->data, 0, m->byte_cnt);
    for (i = 0; i < m->rows; i += 1) {
        m->i32_vals[i][i] = 1;
    } /* for */
} /* i32_init_identity_plain */

static void i32_init_zeros_plain(ptr_matrix_t m)
{
    memset(m->data, 0, m->byte_cnt);
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
    {&i32_multiply_and_store_plain, &i32_multiply_and_store_simd_v2},
    {&i32_scr_multiply_and_store_plain, &i32_scr_multiply_and_store_simd},
};

ptr_matrix_t mtx_i32_allocate(unsigned int rows, unsigned int cols)
{
    unsigned int i = 0;
    ptr_matrix_t m = NULL;

    m = mtx_allocate(rows, cols, sizeof(int32_t), I32_PACK_LEN, &i32_ops);
    if (! m) {
        return NULL;
    } /* if */
    return m;
} /* mtx_i32_allocate */

