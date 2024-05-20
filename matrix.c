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

    unsigned int    row_cnt;            /* The actual number of rows. */ 
    unsigned int    col_cnt;            /* The actual number of columns. */
    unsigned int    padded_row_cnt;     /* Round to the power of <value type>_PACK_LEN. */
    unsigned int    padded_col_cnt;     /* Round to the power of <value type>_PACK_LEN. */
    unsigned int    pack_cnt_per_row;
    size_t          pack_len;
    size_t          padded_byte_cnt;    /* Allocated bytes, including pads. */
    size_t          value_size;

    union {
        int32_t *   i32_padded;
        v4si_t *    i32_packs;
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

static ptr_matrix_t mtx_allocate(unsigned int row_cnt, unsigned int col_cnt, size_t val_size, size_t pack_len, ptr_operation_t ops)
{
    unsigned int i = 0;
    ptr_matrix_t mtx = NULL;
    unsigned int count_boundary = 0;
    unsigned int padded_row_cnt = 0;

    count_boundary = ((CPU_CACHE_LINE_BYTES / val_size) < pack_len) ? pack_len : (CPU_CACHE_LINE_BYTES / val_size);
    padded_row_cnt = round_count_to_multiples_of(row_cnt, count_boundary);

    mtx = calloc(sizeof(matrix_t) + sizeof(void *) * padded_row_cnt, 1);
    if (! mtx) {
        return NULL;
    } /* if */

    mtx->row_cnt = row_cnt;
    mtx->col_cnt = col_cnt;
    mtx->padded_row_cnt = padded_row_cnt;
    mtx->padded_col_cnt = round_count_to_multiples_of(col_cnt, count_boundary);
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

    for (i = 0; i < mtx->padded_row_cnt; i += 1) {
        mtx->val_ptrs[i] = mtx->data + i * mtx->value_size * mtx->padded_col_cnt;
    } /* for */
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

unsigned int mtx_count_rows(ptr_matrix_t mtx)
{
    return mtx->row_cnt;
} /* mtx_count_rows */

unsigned int mtx_count_columns(ptr_matrix_t mtx)
{
    return mtx->col_cnt;
} /* mtx_count_columns */

unsigned int mtx_count_values(ptr_matrix_t mtx)
{
    return (mtx->row_cnt * mtx->col_cnt);
} /* mtx_count_values */

void mtx_get_submatrix(ptr_matrix_t mtx, unsigned int row, unsigned int col, unsigned int row_cnt, unsigned int col_cnt, ptr_submatrix_t ref)
{
    if (mtx->row_cnt <= row) {
        /* Beyond the right boundary. */
        ref->row_off = mtx->row_cnt;
    } else {
        ref->row_off = row;
    } /* if */
    if (mtx->col_cnt <= col) {
        /* Beyond the bottom boundary. */
        ref->col_off = mtx->col_cnt;
    } else {
        ref->col_off = col;
    } /* if */

    ref->row_cnt = (row_cnt <= mtx->row_cnt - ref->row_off) ? row_cnt : (mtx->row_cnt - ref->row_off);
    ref->col_cnt = (col_cnt <= mtx->col_cnt - ref->col_off) ? col_cnt : (mtx->col_cnt - ref->col_off);

    if (ref->row_cnt == 0 || ref->col_cnt == 0) {
        /* If the target sub-matrix is beyond the right or bottom boundary, return an empty one. */
        ref->row_cnt = 0;
        ref->col_cnt = 0;
    } /* if */

    ref->val_ptrs = mtx->val_ptrs;
} /* mtx_get_submatrix */

void mtx_transpose_and_store(ptr_matrix_t mtx, ptr_matrix_t src)
{
    unsigned int i = 0;
    unsigned int j = 0;

    for (i = 0; i < src->row_cnt; i += 1) {
        for (j = 0; j < src->col_cnt; j += 1) {
            mtx->i32_vals[j][i] = src->i32_vals[i][j];
        } /* for j */
    } /* for i */
} /* mtx_transpose_and_store */

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

/* ==== Type-Related functions. ==== */

int32_t mtx_i32_get(ptr_matrix_t mtx, unsigned int row, unsigned int col)
{
    return mtx->i32_vals[row][col];
} /* mtx_i32_get */

void mtx_i32_set(ptr_matrix_t mtx, unsigned int row, unsigned int col, int32_t src_val)
{
    mtx->i32_vals[row][col] = src_val;
} /* mtx_i32_set */

void mtx_i32_set_each(ptr_matrix_t mtx, int32_t src_val)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->i32_vals[i][j] = src_val;
        } /* for */
    } /* for */
    return;
} /* mtx_i32_set_each */

static ptr_matrix_t mat_add_and_store_plain(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < lhs->col_cnt; j += 1) {
            mtx->i32_vals[i][j] = lhs->i32_vals[i][j] + rhs->i32_vals[i][j];
        } /* for */
    } /* for */
    return mtx;
} /* mat_add_and_store_plain */

static ptr_matrix_t mat_sub_and_store_plain(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < lhs->row_cnt; i += 1) {
        for (j = 0; j < lhs->col_cnt; j += 1) {
            mtx->i32_vals[i][j] = lhs->i32_vals[i][j] - rhs->i32_vals[i][j];
        } /* for */
    } /* for */
    return mtx;
} /* mat_sub_and_store_plain */

static ptr_matrix_t mat_multiply_and_store_plain(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    int32_t sum = 0;

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
} /* mat_multiply_and_store_plain */

inline static int32_t v4si_sum_up(v4si_t * src)
{
    int32_t * vals = (int32_t *) src;
    return vals[0] + vals[1] + vals[2] + vals[3];
} /* v4si_sum_up */

inline static int32_t v4si_sum_up_v2(v4si_t * src)
{
    v4si_t ret = {0};
    int32_t * vals = (int32_t *) &ret;

    ret = __builtin_ia32_paddd128(src[0], src[1]);
    ret = __builtin_ia32_paddd128(ret, src[2]);
    ret = __builtin_ia32_paddd128(ret, src[3]);

    return vals[0] + vals[1] + vals[2] + vals[3];
} /* v4si_sum_up_v2 */

static ptr_matrix_t mat_multiply_and_store_simd(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    v4si_t pack_lhs = {0};
    v4si_t pack_rhs = {0};
    v4si_t ret = {0};

    for (k = 0; k < rhs->padded_row_cnt; k += I32_PACK_LEN) {
        for (j = 0; j < rhs->col_cnt; j += 1) {
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_vals[k + 0][j], 0);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_vals[k + 1][j], 1);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_vals[k + 2][j], 2);
            pack_rhs = __builtin_ia32_vec_set_v4si(pack_rhs, rhs->i32_vals[k + 3][j], 3);

            for (i = 0; i < lhs->row_cnt; i += 1) {
                pack_lhs = lhs->i32_packs[i * lhs->pack_cnt_per_row + k / I32_PACK_LEN];
                ret = __builtin_ia32_pmulld128(pack_lhs, pack_rhs);
                mtx->i32_vals[i][j] += v4si_sum_up(&ret);
            } /* for */
        } /* for */
    } /* for */
    return mtx;
} /* mat_multiply_and_store_simd */

inline static void prepare_rpck(v4si_t * rpck, ptr_matrix_t rhs, unsigned int itn_base, unsigned int col)
{
    rpck[0] = __builtin_ia32_vec_set_v4si(rpck[0], rhs->i32_vals[itn_base +  0][col], 0);
    rpck[0] = __builtin_ia32_vec_set_v4si(rpck[0], rhs->i32_vals[itn_base +  1][col], 1);
    rpck[0] = __builtin_ia32_vec_set_v4si(rpck[0], rhs->i32_vals[itn_base +  2][col], 2);
    rpck[0] = __builtin_ia32_vec_set_v4si(rpck[0], rhs->i32_vals[itn_base +  3][col], 3);

    rpck[1] = __builtin_ia32_vec_set_v4si(rpck[1], rhs->i32_vals[itn_base +  4][col], 0);
    rpck[1] = __builtin_ia32_vec_set_v4si(rpck[1], rhs->i32_vals[itn_base +  5][col], 1);
    rpck[1] = __builtin_ia32_vec_set_v4si(rpck[1], rhs->i32_vals[itn_base +  6][col], 2);
    rpck[1] = __builtin_ia32_vec_set_v4si(rpck[1], rhs->i32_vals[itn_base +  7][col], 3);

    rpck[2] = __builtin_ia32_vec_set_v4si(rpck[2], rhs->i32_vals[itn_base +  8][col], 0);
    rpck[2] = __builtin_ia32_vec_set_v4si(rpck[2], rhs->i32_vals[itn_base +  9][col], 1);
    rpck[2] = __builtin_ia32_vec_set_v4si(rpck[2], rhs->i32_vals[itn_base + 10][col], 2);
    rpck[2] = __builtin_ia32_vec_set_v4si(rpck[2], rhs->i32_vals[itn_base + 11][col], 3);

    rpck[3] = __builtin_ia32_vec_set_v4si(rpck[3], rhs->i32_vals[itn_base + 12][col], 0);
    rpck[3] = __builtin_ia32_vec_set_v4si(rpck[3], rhs->i32_vals[itn_base + 13][col], 1);
    rpck[3] = __builtin_ia32_vec_set_v4si(rpck[3], rhs->i32_vals[itn_base + 14][col], 2);
    rpck[3] = __builtin_ia32_vec_set_v4si(rpck[3], rhs->i32_vals[itn_base + 15][col], 3);
} /* prepare_rpck */

inline static void prepare_rpck_fully(v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2, v4si_t * rpck3, ptr_matrix_t rhs, unsigned int itn_base, unsigned col_base)
{
    rpck0[ 0] = __builtin_ia32_vec_set_v4si(rpck0[ 0], rhs->i32_vals[itn_base +  0][col_base +  0], 0);
    rpck0[ 1] = __builtin_ia32_vec_set_v4si(rpck0[ 1], rhs->i32_vals[itn_base +  0][col_base +  1], 0);
    rpck0[ 2] = __builtin_ia32_vec_set_v4si(rpck0[ 2], rhs->i32_vals[itn_base +  0][col_base +  2], 0);
    rpck0[ 3] = __builtin_ia32_vec_set_v4si(rpck0[ 3], rhs->i32_vals[itn_base +  0][col_base +  3], 0);
    rpck0[ 4] = __builtin_ia32_vec_set_v4si(rpck0[ 4], rhs->i32_vals[itn_base +  0][col_base +  4], 0);
    rpck0[ 5] = __builtin_ia32_vec_set_v4si(rpck0[ 5], rhs->i32_vals[itn_base +  0][col_base +  5], 0);
    rpck0[ 6] = __builtin_ia32_vec_set_v4si(rpck0[ 6], rhs->i32_vals[itn_base +  0][col_base +  6], 0);
    rpck0[ 7] = __builtin_ia32_vec_set_v4si(rpck0[ 7], rhs->i32_vals[itn_base +  0][col_base +  7], 0);
    rpck0[ 8] = __builtin_ia32_vec_set_v4si(rpck0[ 8], rhs->i32_vals[itn_base +  0][col_base +  8], 0);
    rpck0[ 9] = __builtin_ia32_vec_set_v4si(rpck0[ 9], rhs->i32_vals[itn_base +  0][col_base +  9], 0);
    rpck0[10] = __builtin_ia32_vec_set_v4si(rpck0[10], rhs->i32_vals[itn_base +  0][col_base + 10], 0);
    rpck0[11] = __builtin_ia32_vec_set_v4si(rpck0[11], rhs->i32_vals[itn_base +  0][col_base + 11], 0);
    rpck0[12] = __builtin_ia32_vec_set_v4si(rpck0[12], rhs->i32_vals[itn_base +  0][col_base + 12], 0);
    rpck0[13] = __builtin_ia32_vec_set_v4si(rpck0[13], rhs->i32_vals[itn_base +  0][col_base + 13], 0);
    rpck0[14] = __builtin_ia32_vec_set_v4si(rpck0[14], rhs->i32_vals[itn_base +  0][col_base + 14], 0);
    rpck0[15] = __builtin_ia32_vec_set_v4si(rpck0[15], rhs->i32_vals[itn_base +  0][col_base + 15], 0);
                                                                 
    rpck0[ 0] = __builtin_ia32_vec_set_v4si(rpck0[ 0], rhs->i32_vals[itn_base +  1][col_base +  0], 1);
    rpck0[ 1] = __builtin_ia32_vec_set_v4si(rpck0[ 1], rhs->i32_vals[itn_base +  1][col_base +  1], 1);
    rpck0[ 2] = __builtin_ia32_vec_set_v4si(rpck0[ 2], rhs->i32_vals[itn_base +  1][col_base +  2], 1);
    rpck0[ 3] = __builtin_ia32_vec_set_v4si(rpck0[ 3], rhs->i32_vals[itn_base +  1][col_base +  3], 1);
    rpck0[ 4] = __builtin_ia32_vec_set_v4si(rpck0[ 4], rhs->i32_vals[itn_base +  1][col_base +  4], 1);
    rpck0[ 5] = __builtin_ia32_vec_set_v4si(rpck0[ 5], rhs->i32_vals[itn_base +  1][col_base +  5], 1);
    rpck0[ 6] = __builtin_ia32_vec_set_v4si(rpck0[ 6], rhs->i32_vals[itn_base +  1][col_base +  6], 1);
    rpck0[ 7] = __builtin_ia32_vec_set_v4si(rpck0[ 7], rhs->i32_vals[itn_base +  1][col_base +  7], 1);
    rpck0[ 8] = __builtin_ia32_vec_set_v4si(rpck0[ 8], rhs->i32_vals[itn_base +  1][col_base +  8], 1);
    rpck0[ 9] = __builtin_ia32_vec_set_v4si(rpck0[ 9], rhs->i32_vals[itn_base +  1][col_base +  9], 1);
    rpck0[10] = __builtin_ia32_vec_set_v4si(rpck0[10], rhs->i32_vals[itn_base +  1][col_base + 10], 1);
    rpck0[11] = __builtin_ia32_vec_set_v4si(rpck0[11], rhs->i32_vals[itn_base +  1][col_base + 11], 1);
    rpck0[12] = __builtin_ia32_vec_set_v4si(rpck0[12], rhs->i32_vals[itn_base +  1][col_base + 12], 1);
    rpck0[13] = __builtin_ia32_vec_set_v4si(rpck0[13], rhs->i32_vals[itn_base +  1][col_base + 13], 1);
    rpck0[14] = __builtin_ia32_vec_set_v4si(rpck0[14], rhs->i32_vals[itn_base +  1][col_base + 14], 1);
    rpck0[15] = __builtin_ia32_vec_set_v4si(rpck0[15], rhs->i32_vals[itn_base +  1][col_base + 15], 1);

    rpck0[ 0] = __builtin_ia32_vec_set_v4si(rpck0[ 0], rhs->i32_vals[itn_base +  2][col_base +  0], 2);
    rpck0[ 1] = __builtin_ia32_vec_set_v4si(rpck0[ 1], rhs->i32_vals[itn_base +  2][col_base +  1], 2);
    rpck0[ 2] = __builtin_ia32_vec_set_v4si(rpck0[ 2], rhs->i32_vals[itn_base +  2][col_base +  2], 2);
    rpck0[ 3] = __builtin_ia32_vec_set_v4si(rpck0[ 3], rhs->i32_vals[itn_base +  2][col_base +  3], 2);
    rpck0[ 4] = __builtin_ia32_vec_set_v4si(rpck0[ 4], rhs->i32_vals[itn_base +  2][col_base +  4], 2);
    rpck0[ 5] = __builtin_ia32_vec_set_v4si(rpck0[ 5], rhs->i32_vals[itn_base +  2][col_base +  5], 2);
    rpck0[ 6] = __builtin_ia32_vec_set_v4si(rpck0[ 6], rhs->i32_vals[itn_base +  2][col_base +  6], 2);
    rpck0[ 7] = __builtin_ia32_vec_set_v4si(rpck0[ 7], rhs->i32_vals[itn_base +  2][col_base +  7], 2);
    rpck0[ 8] = __builtin_ia32_vec_set_v4si(rpck0[ 8], rhs->i32_vals[itn_base +  2][col_base +  8], 2);
    rpck0[ 9] = __builtin_ia32_vec_set_v4si(rpck0[ 9], rhs->i32_vals[itn_base +  2][col_base +  9], 2);
    rpck0[10] = __builtin_ia32_vec_set_v4si(rpck0[10], rhs->i32_vals[itn_base +  2][col_base + 10], 2);
    rpck0[11] = __builtin_ia32_vec_set_v4si(rpck0[11], rhs->i32_vals[itn_base +  2][col_base + 11], 2);
    rpck0[12] = __builtin_ia32_vec_set_v4si(rpck0[12], rhs->i32_vals[itn_base +  2][col_base + 12], 2);
    rpck0[13] = __builtin_ia32_vec_set_v4si(rpck0[13], rhs->i32_vals[itn_base +  2][col_base + 13], 2);
    rpck0[14] = __builtin_ia32_vec_set_v4si(rpck0[14], rhs->i32_vals[itn_base +  2][col_base + 14], 2);
    rpck0[15] = __builtin_ia32_vec_set_v4si(rpck0[15], rhs->i32_vals[itn_base +  2][col_base + 15], 2);

    rpck0[ 0] = __builtin_ia32_vec_set_v4si(rpck0[ 0], rhs->i32_vals[itn_base +  3][col_base +  0], 3);
    rpck0[ 1] = __builtin_ia32_vec_set_v4si(rpck0[ 1], rhs->i32_vals[itn_base +  3][col_base +  1], 3);
    rpck0[ 2] = __builtin_ia32_vec_set_v4si(rpck0[ 2], rhs->i32_vals[itn_base +  3][col_base +  2], 3);
    rpck0[ 3] = __builtin_ia32_vec_set_v4si(rpck0[ 3], rhs->i32_vals[itn_base +  3][col_base +  3], 3);
    rpck0[ 4] = __builtin_ia32_vec_set_v4si(rpck0[ 4], rhs->i32_vals[itn_base +  3][col_base +  4], 3);
    rpck0[ 5] = __builtin_ia32_vec_set_v4si(rpck0[ 5], rhs->i32_vals[itn_base +  3][col_base +  5], 3);
    rpck0[ 6] = __builtin_ia32_vec_set_v4si(rpck0[ 6], rhs->i32_vals[itn_base +  3][col_base +  6], 3);
    rpck0[ 7] = __builtin_ia32_vec_set_v4si(rpck0[ 7], rhs->i32_vals[itn_base +  3][col_base +  7], 3);
    rpck0[ 8] = __builtin_ia32_vec_set_v4si(rpck0[ 8], rhs->i32_vals[itn_base +  3][col_base +  8], 3);
    rpck0[ 9] = __builtin_ia32_vec_set_v4si(rpck0[ 9], rhs->i32_vals[itn_base +  3][col_base +  9], 3);
    rpck0[10] = __builtin_ia32_vec_set_v4si(rpck0[10], rhs->i32_vals[itn_base +  3][col_base + 10], 3);
    rpck0[11] = __builtin_ia32_vec_set_v4si(rpck0[11], rhs->i32_vals[itn_base +  3][col_base + 11], 3);
    rpck0[12] = __builtin_ia32_vec_set_v4si(rpck0[12], rhs->i32_vals[itn_base +  3][col_base + 12], 3);
    rpck0[13] = __builtin_ia32_vec_set_v4si(rpck0[13], rhs->i32_vals[itn_base +  3][col_base + 13], 3);
    rpck0[14] = __builtin_ia32_vec_set_v4si(rpck0[14], rhs->i32_vals[itn_base +  3][col_base + 14], 3);
    rpck0[15] = __builtin_ia32_vec_set_v4si(rpck0[15], rhs->i32_vals[itn_base +  3][col_base + 15], 3);

    rpck1[ 0] = __builtin_ia32_vec_set_v4si(rpck1[ 0], rhs->i32_vals[itn_base +  4][col_base +  0], 0);
    rpck1[ 1] = __builtin_ia32_vec_set_v4si(rpck1[ 1], rhs->i32_vals[itn_base +  4][col_base +  1], 0);
    rpck1[ 2] = __builtin_ia32_vec_set_v4si(rpck1[ 2], rhs->i32_vals[itn_base +  4][col_base +  2], 0);
    rpck1[ 3] = __builtin_ia32_vec_set_v4si(rpck1[ 3], rhs->i32_vals[itn_base +  4][col_base +  3], 0);
    rpck1[ 4] = __builtin_ia32_vec_set_v4si(rpck1[ 4], rhs->i32_vals[itn_base +  4][col_base +  4], 0);
    rpck1[ 5] = __builtin_ia32_vec_set_v4si(rpck1[ 5], rhs->i32_vals[itn_base +  4][col_base +  5], 0);
    rpck1[ 6] = __builtin_ia32_vec_set_v4si(rpck1[ 6], rhs->i32_vals[itn_base +  4][col_base +  6], 0);
    rpck1[ 7] = __builtin_ia32_vec_set_v4si(rpck1[ 7], rhs->i32_vals[itn_base +  4][col_base +  7], 0);
    rpck1[ 8] = __builtin_ia32_vec_set_v4si(rpck1[ 8], rhs->i32_vals[itn_base +  4][col_base +  8], 0);
    rpck1[ 9] = __builtin_ia32_vec_set_v4si(rpck1[ 9], rhs->i32_vals[itn_base +  4][col_base +  9], 0);
    rpck1[10] = __builtin_ia32_vec_set_v4si(rpck1[10], rhs->i32_vals[itn_base +  4][col_base + 10], 0);
    rpck1[11] = __builtin_ia32_vec_set_v4si(rpck1[11], rhs->i32_vals[itn_base +  4][col_base + 11], 0);
    rpck1[12] = __builtin_ia32_vec_set_v4si(rpck1[12], rhs->i32_vals[itn_base +  4][col_base + 12], 0);
    rpck1[13] = __builtin_ia32_vec_set_v4si(rpck1[13], rhs->i32_vals[itn_base +  4][col_base + 13], 0);
    rpck1[14] = __builtin_ia32_vec_set_v4si(rpck1[14], rhs->i32_vals[itn_base +  4][col_base + 14], 0);
    rpck1[15] = __builtin_ia32_vec_set_v4si(rpck1[15], rhs->i32_vals[itn_base +  4][col_base + 15], 0);

    rpck1[ 0] = __builtin_ia32_vec_set_v4si(rpck1[ 0], rhs->i32_vals[itn_base +  5][col_base +  0], 1);
    rpck1[ 1] = __builtin_ia32_vec_set_v4si(rpck1[ 1], rhs->i32_vals[itn_base +  5][col_base +  1], 1);
    rpck1[ 2] = __builtin_ia32_vec_set_v4si(rpck1[ 2], rhs->i32_vals[itn_base +  5][col_base +  2], 1);
    rpck1[ 3] = __builtin_ia32_vec_set_v4si(rpck1[ 3], rhs->i32_vals[itn_base +  5][col_base +  3], 1);
    rpck1[ 4] = __builtin_ia32_vec_set_v4si(rpck1[ 4], rhs->i32_vals[itn_base +  5][col_base +  4], 1);
    rpck1[ 5] = __builtin_ia32_vec_set_v4si(rpck1[ 5], rhs->i32_vals[itn_base +  5][col_base +  5], 1);
    rpck1[ 6] = __builtin_ia32_vec_set_v4si(rpck1[ 6], rhs->i32_vals[itn_base +  5][col_base +  6], 1);
    rpck1[ 7] = __builtin_ia32_vec_set_v4si(rpck1[ 7], rhs->i32_vals[itn_base +  5][col_base +  7], 1);
    rpck1[ 8] = __builtin_ia32_vec_set_v4si(rpck1[ 8], rhs->i32_vals[itn_base +  5][col_base +  8], 1);
    rpck1[ 9] = __builtin_ia32_vec_set_v4si(rpck1[ 9], rhs->i32_vals[itn_base +  5][col_base +  9], 1);
    rpck1[10] = __builtin_ia32_vec_set_v4si(rpck1[10], rhs->i32_vals[itn_base +  5][col_base + 10], 1);
    rpck1[11] = __builtin_ia32_vec_set_v4si(rpck1[11], rhs->i32_vals[itn_base +  5][col_base + 11], 1);
    rpck1[12] = __builtin_ia32_vec_set_v4si(rpck1[12], rhs->i32_vals[itn_base +  5][col_base + 12], 1);
    rpck1[13] = __builtin_ia32_vec_set_v4si(rpck1[13], rhs->i32_vals[itn_base +  5][col_base + 13], 1);
    rpck1[14] = __builtin_ia32_vec_set_v4si(rpck1[14], rhs->i32_vals[itn_base +  5][col_base + 14], 1);
    rpck1[15] = __builtin_ia32_vec_set_v4si(rpck1[15], rhs->i32_vals[itn_base +  5][col_base + 15], 1);

    rpck1[ 0] = __builtin_ia32_vec_set_v4si(rpck1[ 0], rhs->i32_vals[itn_base +  6][col_base +  0], 2);
    rpck1[ 1] = __builtin_ia32_vec_set_v4si(rpck1[ 1], rhs->i32_vals[itn_base +  6][col_base +  1], 2);
    rpck1[ 2] = __builtin_ia32_vec_set_v4si(rpck1[ 2], rhs->i32_vals[itn_base +  6][col_base +  2], 2);
    rpck1[ 3] = __builtin_ia32_vec_set_v4si(rpck1[ 3], rhs->i32_vals[itn_base +  6][col_base +  3], 2);
    rpck1[ 4] = __builtin_ia32_vec_set_v4si(rpck1[ 4], rhs->i32_vals[itn_base +  6][col_base +  4], 2);
    rpck1[ 5] = __builtin_ia32_vec_set_v4si(rpck1[ 5], rhs->i32_vals[itn_base +  6][col_base +  5], 2);
    rpck1[ 6] = __builtin_ia32_vec_set_v4si(rpck1[ 6], rhs->i32_vals[itn_base +  6][col_base +  6], 2);
    rpck1[ 7] = __builtin_ia32_vec_set_v4si(rpck1[ 7], rhs->i32_vals[itn_base +  6][col_base +  7], 2);
    rpck1[ 8] = __builtin_ia32_vec_set_v4si(rpck1[ 8], rhs->i32_vals[itn_base +  6][col_base +  8], 2);
    rpck1[ 9] = __builtin_ia32_vec_set_v4si(rpck1[ 9], rhs->i32_vals[itn_base +  6][col_base +  9], 2);
    rpck1[10] = __builtin_ia32_vec_set_v4si(rpck1[10], rhs->i32_vals[itn_base +  6][col_base + 10], 2);
    rpck1[11] = __builtin_ia32_vec_set_v4si(rpck1[11], rhs->i32_vals[itn_base +  6][col_base + 11], 2);
    rpck1[12] = __builtin_ia32_vec_set_v4si(rpck1[12], rhs->i32_vals[itn_base +  6][col_base + 12], 2);
    rpck1[13] = __builtin_ia32_vec_set_v4si(rpck1[13], rhs->i32_vals[itn_base +  6][col_base + 13], 2);
    rpck1[14] = __builtin_ia32_vec_set_v4si(rpck1[14], rhs->i32_vals[itn_base +  6][col_base + 14], 2);
    rpck1[15] = __builtin_ia32_vec_set_v4si(rpck1[15], rhs->i32_vals[itn_base +  6][col_base + 15], 2);

    rpck1[ 0] = __builtin_ia32_vec_set_v4si(rpck1[ 0], rhs->i32_vals[itn_base +  7][col_base +  0], 3);
    rpck1[ 1] = __builtin_ia32_vec_set_v4si(rpck1[ 1], rhs->i32_vals[itn_base +  7][col_base +  1], 3);
    rpck1[ 2] = __builtin_ia32_vec_set_v4si(rpck1[ 2], rhs->i32_vals[itn_base +  7][col_base +  2], 3);
    rpck1[ 3] = __builtin_ia32_vec_set_v4si(rpck1[ 3], rhs->i32_vals[itn_base +  7][col_base +  3], 3);
    rpck1[ 4] = __builtin_ia32_vec_set_v4si(rpck1[ 4], rhs->i32_vals[itn_base +  7][col_base +  4], 3);
    rpck1[ 5] = __builtin_ia32_vec_set_v4si(rpck1[ 5], rhs->i32_vals[itn_base +  7][col_base +  5], 3);
    rpck1[ 6] = __builtin_ia32_vec_set_v4si(rpck1[ 6], rhs->i32_vals[itn_base +  7][col_base +  6], 3);
    rpck1[ 7] = __builtin_ia32_vec_set_v4si(rpck1[ 7], rhs->i32_vals[itn_base +  7][col_base +  7], 3);
    rpck1[ 8] = __builtin_ia32_vec_set_v4si(rpck1[ 8], rhs->i32_vals[itn_base +  7][col_base +  8], 3);
    rpck1[ 9] = __builtin_ia32_vec_set_v4si(rpck1[ 9], rhs->i32_vals[itn_base +  7][col_base +  9], 3);
    rpck1[10] = __builtin_ia32_vec_set_v4si(rpck1[10], rhs->i32_vals[itn_base +  7][col_base + 10], 3);
    rpck1[11] = __builtin_ia32_vec_set_v4si(rpck1[11], rhs->i32_vals[itn_base +  7][col_base + 11], 3);
    rpck1[12] = __builtin_ia32_vec_set_v4si(rpck1[12], rhs->i32_vals[itn_base +  7][col_base + 12], 3);
    rpck1[13] = __builtin_ia32_vec_set_v4si(rpck1[13], rhs->i32_vals[itn_base +  7][col_base + 13], 3);
    rpck1[14] = __builtin_ia32_vec_set_v4si(rpck1[14], rhs->i32_vals[itn_base +  7][col_base + 14], 3);
    rpck1[15] = __builtin_ia32_vec_set_v4si(rpck1[15], rhs->i32_vals[itn_base +  7][col_base + 15], 3);

    rpck2[ 0] = __builtin_ia32_vec_set_v4si(rpck2[ 0], rhs->i32_vals[itn_base +  8][col_base +  0], 0);
    rpck2[ 1] = __builtin_ia32_vec_set_v4si(rpck2[ 1], rhs->i32_vals[itn_base +  8][col_base +  1], 0);
    rpck2[ 2] = __builtin_ia32_vec_set_v4si(rpck2[ 2], rhs->i32_vals[itn_base +  8][col_base +  2], 0);
    rpck2[ 3] = __builtin_ia32_vec_set_v4si(rpck2[ 3], rhs->i32_vals[itn_base +  8][col_base +  3], 0);
    rpck2[ 4] = __builtin_ia32_vec_set_v4si(rpck2[ 4], rhs->i32_vals[itn_base +  8][col_base +  4], 0);
    rpck2[ 5] = __builtin_ia32_vec_set_v4si(rpck2[ 5], rhs->i32_vals[itn_base +  8][col_base +  5], 0);
    rpck2[ 6] = __builtin_ia32_vec_set_v4si(rpck2[ 6], rhs->i32_vals[itn_base +  8][col_base +  6], 0);
    rpck2[ 7] = __builtin_ia32_vec_set_v4si(rpck2[ 7], rhs->i32_vals[itn_base +  8][col_base +  7], 0);
    rpck2[ 8] = __builtin_ia32_vec_set_v4si(rpck2[ 8], rhs->i32_vals[itn_base +  8][col_base +  8], 0);
    rpck2[ 9] = __builtin_ia32_vec_set_v4si(rpck2[ 9], rhs->i32_vals[itn_base +  8][col_base +  9], 0);
    rpck2[10] = __builtin_ia32_vec_set_v4si(rpck2[10], rhs->i32_vals[itn_base +  8][col_base + 10], 0);
    rpck2[11] = __builtin_ia32_vec_set_v4si(rpck2[11], rhs->i32_vals[itn_base +  8][col_base + 11], 0);
    rpck2[12] = __builtin_ia32_vec_set_v4si(rpck2[12], rhs->i32_vals[itn_base +  8][col_base + 12], 0);
    rpck2[13] = __builtin_ia32_vec_set_v4si(rpck2[13], rhs->i32_vals[itn_base +  8][col_base + 13], 0);
    rpck2[14] = __builtin_ia32_vec_set_v4si(rpck2[14], rhs->i32_vals[itn_base +  8][col_base + 14], 0);
    rpck2[15] = __builtin_ia32_vec_set_v4si(rpck2[15], rhs->i32_vals[itn_base +  8][col_base + 15], 0);
                                                                 
    rpck2[ 0] = __builtin_ia32_vec_set_v4si(rpck2[ 0], rhs->i32_vals[itn_base +  9][col_base +  0], 1);
    rpck2[ 1] = __builtin_ia32_vec_set_v4si(rpck2[ 1], rhs->i32_vals[itn_base +  9][col_base +  1], 1);
    rpck2[ 2] = __builtin_ia32_vec_set_v4si(rpck2[ 2], rhs->i32_vals[itn_base +  9][col_base +  2], 1);
    rpck2[ 3] = __builtin_ia32_vec_set_v4si(rpck2[ 3], rhs->i32_vals[itn_base +  9][col_base +  3], 1);
    rpck2[ 4] = __builtin_ia32_vec_set_v4si(rpck2[ 4], rhs->i32_vals[itn_base +  9][col_base +  4], 1);
    rpck2[ 5] = __builtin_ia32_vec_set_v4si(rpck2[ 5], rhs->i32_vals[itn_base +  9][col_base +  5], 1);
    rpck2[ 6] = __builtin_ia32_vec_set_v4si(rpck2[ 6], rhs->i32_vals[itn_base +  9][col_base +  6], 1);
    rpck2[ 7] = __builtin_ia32_vec_set_v4si(rpck2[ 7], rhs->i32_vals[itn_base +  9][col_base +  7], 1);
    rpck2[ 8] = __builtin_ia32_vec_set_v4si(rpck2[ 8], rhs->i32_vals[itn_base +  9][col_base +  8], 1);
    rpck2[ 9] = __builtin_ia32_vec_set_v4si(rpck2[ 9], rhs->i32_vals[itn_base +  9][col_base +  9], 1);
    rpck2[10] = __builtin_ia32_vec_set_v4si(rpck2[10], rhs->i32_vals[itn_base +  9][col_base + 10], 1);
    rpck2[11] = __builtin_ia32_vec_set_v4si(rpck2[11], rhs->i32_vals[itn_base +  9][col_base + 11], 1);
    rpck2[12] = __builtin_ia32_vec_set_v4si(rpck2[12], rhs->i32_vals[itn_base +  9][col_base + 12], 1);
    rpck2[13] = __builtin_ia32_vec_set_v4si(rpck2[13], rhs->i32_vals[itn_base +  9][col_base + 13], 1);
    rpck2[14] = __builtin_ia32_vec_set_v4si(rpck2[14], rhs->i32_vals[itn_base +  9][col_base + 14], 1);
    rpck2[15] = __builtin_ia32_vec_set_v4si(rpck2[15], rhs->i32_vals[itn_base +  9][col_base + 15], 1);

    rpck2[ 0] = __builtin_ia32_vec_set_v4si(rpck2[ 0], rhs->i32_vals[itn_base + 10][col_base +  0], 2);
    rpck2[ 1] = __builtin_ia32_vec_set_v4si(rpck2[ 1], rhs->i32_vals[itn_base + 10][col_base +  1], 2);
    rpck2[ 2] = __builtin_ia32_vec_set_v4si(rpck2[ 2], rhs->i32_vals[itn_base + 10][col_base +  2], 2);
    rpck2[ 3] = __builtin_ia32_vec_set_v4si(rpck2[ 3], rhs->i32_vals[itn_base + 10][col_base +  3], 2);
    rpck2[ 4] = __builtin_ia32_vec_set_v4si(rpck2[ 4], rhs->i32_vals[itn_base + 10][col_base +  4], 2);
    rpck2[ 5] = __builtin_ia32_vec_set_v4si(rpck2[ 5], rhs->i32_vals[itn_base + 10][col_base +  5], 2);
    rpck2[ 6] = __builtin_ia32_vec_set_v4si(rpck2[ 6], rhs->i32_vals[itn_base + 10][col_base +  6], 2);
    rpck2[ 7] = __builtin_ia32_vec_set_v4si(rpck2[ 7], rhs->i32_vals[itn_base + 10][col_base +  7], 2);
    rpck2[ 8] = __builtin_ia32_vec_set_v4si(rpck2[ 8], rhs->i32_vals[itn_base + 10][col_base +  8], 2);
    rpck2[ 9] = __builtin_ia32_vec_set_v4si(rpck2[ 9], rhs->i32_vals[itn_base + 10][col_base +  9], 2);
    rpck2[10] = __builtin_ia32_vec_set_v4si(rpck2[10], rhs->i32_vals[itn_base + 10][col_base + 10], 2);
    rpck2[11] = __builtin_ia32_vec_set_v4si(rpck2[11], rhs->i32_vals[itn_base + 10][col_base + 11], 2);
    rpck2[12] = __builtin_ia32_vec_set_v4si(rpck2[12], rhs->i32_vals[itn_base + 10][col_base + 12], 2);
    rpck2[13] = __builtin_ia32_vec_set_v4si(rpck2[13], rhs->i32_vals[itn_base + 10][col_base + 13], 2);
    rpck2[14] = __builtin_ia32_vec_set_v4si(rpck2[14], rhs->i32_vals[itn_base + 10][col_base + 14], 2);
    rpck2[15] = __builtin_ia32_vec_set_v4si(rpck2[15], rhs->i32_vals[itn_base + 10][col_base + 15], 2);

    rpck2[ 0] = __builtin_ia32_vec_set_v4si(rpck2[ 0], rhs->i32_vals[itn_base + 11][col_base +  0], 3);
    rpck2[ 1] = __builtin_ia32_vec_set_v4si(rpck2[ 1], rhs->i32_vals[itn_base + 11][col_base +  1], 3);
    rpck2[ 2] = __builtin_ia32_vec_set_v4si(rpck2[ 2], rhs->i32_vals[itn_base + 11][col_base +  2], 3);
    rpck2[ 3] = __builtin_ia32_vec_set_v4si(rpck2[ 3], rhs->i32_vals[itn_base + 11][col_base +  3], 3);
    rpck2[ 4] = __builtin_ia32_vec_set_v4si(rpck2[ 4], rhs->i32_vals[itn_base + 11][col_base +  4], 3);
    rpck2[ 5] = __builtin_ia32_vec_set_v4si(rpck2[ 5], rhs->i32_vals[itn_base + 11][col_base +  5], 3);
    rpck2[ 6] = __builtin_ia32_vec_set_v4si(rpck2[ 6], rhs->i32_vals[itn_base + 11][col_base +  6], 3);
    rpck2[ 7] = __builtin_ia32_vec_set_v4si(rpck2[ 7], rhs->i32_vals[itn_base + 11][col_base +  7], 3);
    rpck2[ 8] = __builtin_ia32_vec_set_v4si(rpck2[ 8], rhs->i32_vals[itn_base + 11][col_base +  8], 3);
    rpck2[ 9] = __builtin_ia32_vec_set_v4si(rpck2[ 9], rhs->i32_vals[itn_base + 11][col_base +  9], 3);
    rpck2[10] = __builtin_ia32_vec_set_v4si(rpck2[10], rhs->i32_vals[itn_base + 11][col_base + 10], 3);
    rpck2[11] = __builtin_ia32_vec_set_v4si(rpck2[11], rhs->i32_vals[itn_base + 11][col_base + 11], 3);
    rpck2[12] = __builtin_ia32_vec_set_v4si(rpck2[12], rhs->i32_vals[itn_base + 11][col_base + 12], 3);
    rpck2[13] = __builtin_ia32_vec_set_v4si(rpck2[13], rhs->i32_vals[itn_base + 11][col_base + 13], 3);
    rpck2[14] = __builtin_ia32_vec_set_v4si(rpck2[14], rhs->i32_vals[itn_base + 11][col_base + 14], 3);
    rpck2[15] = __builtin_ia32_vec_set_v4si(rpck2[15], rhs->i32_vals[itn_base + 11][col_base + 15], 3);

    rpck3[ 0] = __builtin_ia32_vec_set_v4si(rpck3[ 0], rhs->i32_vals[itn_base + 12][col_base +  0], 0);
    rpck3[ 1] = __builtin_ia32_vec_set_v4si(rpck3[ 1], rhs->i32_vals[itn_base + 12][col_base +  1], 0);
    rpck3[ 2] = __builtin_ia32_vec_set_v4si(rpck3[ 2], rhs->i32_vals[itn_base + 12][col_base +  2], 0);
    rpck3[ 3] = __builtin_ia32_vec_set_v4si(rpck3[ 3], rhs->i32_vals[itn_base + 12][col_base +  3], 0);
    rpck3[ 4] = __builtin_ia32_vec_set_v4si(rpck3[ 4], rhs->i32_vals[itn_base + 12][col_base +  4], 0);
    rpck3[ 5] = __builtin_ia32_vec_set_v4si(rpck3[ 5], rhs->i32_vals[itn_base + 12][col_base +  5], 0);
    rpck3[ 6] = __builtin_ia32_vec_set_v4si(rpck3[ 6], rhs->i32_vals[itn_base + 12][col_base +  6], 0);
    rpck3[ 7] = __builtin_ia32_vec_set_v4si(rpck3[ 7], rhs->i32_vals[itn_base + 12][col_base +  7], 0);
    rpck3[ 8] = __builtin_ia32_vec_set_v4si(rpck3[ 8], rhs->i32_vals[itn_base + 12][col_base +  8], 0);
    rpck3[ 9] = __builtin_ia32_vec_set_v4si(rpck3[ 9], rhs->i32_vals[itn_base + 12][col_base +  9], 0);
    rpck3[10] = __builtin_ia32_vec_set_v4si(rpck3[10], rhs->i32_vals[itn_base + 12][col_base + 10], 0);
    rpck3[11] = __builtin_ia32_vec_set_v4si(rpck3[11], rhs->i32_vals[itn_base + 12][col_base + 11], 0);
    rpck3[12] = __builtin_ia32_vec_set_v4si(rpck3[12], rhs->i32_vals[itn_base + 12][col_base + 12], 0);
    rpck3[13] = __builtin_ia32_vec_set_v4si(rpck3[13], rhs->i32_vals[itn_base + 12][col_base + 13], 0);
    rpck3[14] = __builtin_ia32_vec_set_v4si(rpck3[14], rhs->i32_vals[itn_base + 12][col_base + 14], 0);
    rpck3[15] = __builtin_ia32_vec_set_v4si(rpck3[15], rhs->i32_vals[itn_base + 12][col_base + 15], 0);

    rpck3[ 0] = __builtin_ia32_vec_set_v4si(rpck3[ 0], rhs->i32_vals[itn_base + 13][col_base +  0], 1);
    rpck3[ 1] = __builtin_ia32_vec_set_v4si(rpck3[ 1], rhs->i32_vals[itn_base + 13][col_base +  1], 1);
    rpck3[ 2] = __builtin_ia32_vec_set_v4si(rpck3[ 2], rhs->i32_vals[itn_base + 13][col_base +  2], 1);
    rpck3[ 3] = __builtin_ia32_vec_set_v4si(rpck3[ 3], rhs->i32_vals[itn_base + 13][col_base +  3], 1);
    rpck3[ 4] = __builtin_ia32_vec_set_v4si(rpck3[ 4], rhs->i32_vals[itn_base + 13][col_base +  4], 1);
    rpck3[ 5] = __builtin_ia32_vec_set_v4si(rpck3[ 5], rhs->i32_vals[itn_base + 13][col_base +  5], 1);
    rpck3[ 6] = __builtin_ia32_vec_set_v4si(rpck3[ 6], rhs->i32_vals[itn_base + 13][col_base +  6], 1);
    rpck3[ 7] = __builtin_ia32_vec_set_v4si(rpck3[ 7], rhs->i32_vals[itn_base + 13][col_base +  7], 1);
    rpck3[ 8] = __builtin_ia32_vec_set_v4si(rpck3[ 8], rhs->i32_vals[itn_base + 13][col_base +  8], 1);
    rpck3[ 9] = __builtin_ia32_vec_set_v4si(rpck3[ 9], rhs->i32_vals[itn_base + 13][col_base +  9], 1);
    rpck3[10] = __builtin_ia32_vec_set_v4si(rpck3[10], rhs->i32_vals[itn_base + 13][col_base + 10], 1);
    rpck3[11] = __builtin_ia32_vec_set_v4si(rpck3[11], rhs->i32_vals[itn_base + 13][col_base + 11], 1);
    rpck3[12] = __builtin_ia32_vec_set_v4si(rpck3[12], rhs->i32_vals[itn_base + 13][col_base + 12], 1);
    rpck3[13] = __builtin_ia32_vec_set_v4si(rpck3[13], rhs->i32_vals[itn_base + 13][col_base + 13], 1);
    rpck3[14] = __builtin_ia32_vec_set_v4si(rpck3[14], rhs->i32_vals[itn_base + 13][col_base + 14], 1);
    rpck3[15] = __builtin_ia32_vec_set_v4si(rpck3[15], rhs->i32_vals[itn_base + 13][col_base + 15], 1);

    rpck3[ 0] = __builtin_ia32_vec_set_v4si(rpck3[ 0], rhs->i32_vals[itn_base + 14][col_base +  0], 2);
    rpck3[ 1] = __builtin_ia32_vec_set_v4si(rpck3[ 1], rhs->i32_vals[itn_base + 14][col_base +  1], 2);
    rpck3[ 2] = __builtin_ia32_vec_set_v4si(rpck3[ 2], rhs->i32_vals[itn_base + 14][col_base +  2], 2);
    rpck3[ 3] = __builtin_ia32_vec_set_v4si(rpck3[ 3], rhs->i32_vals[itn_base + 14][col_base +  3], 2);
    rpck3[ 4] = __builtin_ia32_vec_set_v4si(rpck3[ 4], rhs->i32_vals[itn_base + 14][col_base +  4], 2);
    rpck3[ 5] = __builtin_ia32_vec_set_v4si(rpck3[ 5], rhs->i32_vals[itn_base + 14][col_base +  5], 2);
    rpck3[ 6] = __builtin_ia32_vec_set_v4si(rpck3[ 6], rhs->i32_vals[itn_base + 14][col_base +  6], 2);
    rpck3[ 7] = __builtin_ia32_vec_set_v4si(rpck3[ 7], rhs->i32_vals[itn_base + 14][col_base +  7], 2);
    rpck3[ 8] = __builtin_ia32_vec_set_v4si(rpck3[ 8], rhs->i32_vals[itn_base + 14][col_base +  8], 2);
    rpck3[ 9] = __builtin_ia32_vec_set_v4si(rpck3[ 9], rhs->i32_vals[itn_base + 14][col_base +  9], 2);
    rpck3[10] = __builtin_ia32_vec_set_v4si(rpck3[10], rhs->i32_vals[itn_base + 14][col_base + 10], 2);
    rpck3[11] = __builtin_ia32_vec_set_v4si(rpck3[11], rhs->i32_vals[itn_base + 14][col_base + 11], 2);
    rpck3[12] = __builtin_ia32_vec_set_v4si(rpck3[12], rhs->i32_vals[itn_base + 14][col_base + 12], 2);
    rpck3[13] = __builtin_ia32_vec_set_v4si(rpck3[13], rhs->i32_vals[itn_base + 14][col_base + 13], 2);
    rpck3[14] = __builtin_ia32_vec_set_v4si(rpck3[14], rhs->i32_vals[itn_base + 14][col_base + 14], 2);
    rpck3[15] = __builtin_ia32_vec_set_v4si(rpck3[15], rhs->i32_vals[itn_base + 14][col_base + 15], 2);

    rpck3[ 0] = __builtin_ia32_vec_set_v4si(rpck3[ 0], rhs->i32_vals[itn_base + 15][col_base +  0], 3);
    rpck3[ 1] = __builtin_ia32_vec_set_v4si(rpck3[ 1], rhs->i32_vals[itn_base + 15][col_base +  1], 3);
    rpck3[ 2] = __builtin_ia32_vec_set_v4si(rpck3[ 2], rhs->i32_vals[itn_base + 15][col_base +  2], 3);
    rpck3[ 3] = __builtin_ia32_vec_set_v4si(rpck3[ 3], rhs->i32_vals[itn_base + 15][col_base +  3], 3);
    rpck3[ 4] = __builtin_ia32_vec_set_v4si(rpck3[ 4], rhs->i32_vals[itn_base + 15][col_base +  4], 3);
    rpck3[ 5] = __builtin_ia32_vec_set_v4si(rpck3[ 5], rhs->i32_vals[itn_base + 15][col_base +  5], 3);
    rpck3[ 6] = __builtin_ia32_vec_set_v4si(rpck3[ 6], rhs->i32_vals[itn_base + 15][col_base +  6], 3);
    rpck3[ 7] = __builtin_ia32_vec_set_v4si(rpck3[ 7], rhs->i32_vals[itn_base + 15][col_base +  7], 3);
    rpck3[ 8] = __builtin_ia32_vec_set_v4si(rpck3[ 8], rhs->i32_vals[itn_base + 15][col_base +  8], 3);
    rpck3[ 9] = __builtin_ia32_vec_set_v4si(rpck3[ 9], rhs->i32_vals[itn_base + 15][col_base +  9], 3);
    rpck3[10] = __builtin_ia32_vec_set_v4si(rpck3[10], rhs->i32_vals[itn_base + 15][col_base + 10], 3);
    rpck3[11] = __builtin_ia32_vec_set_v4si(rpck3[11], rhs->i32_vals[itn_base + 15][col_base + 11], 3);
    rpck3[12] = __builtin_ia32_vec_set_v4si(rpck3[12], rhs->i32_vals[itn_base + 15][col_base + 12], 3);
    rpck3[13] = __builtin_ia32_vec_set_v4si(rpck3[13], rhs->i32_vals[itn_base + 15][col_base + 13], 3);
    rpck3[14] = __builtin_ia32_vec_set_v4si(rpck3[14], rhs->i32_vals[itn_base + 15][col_base + 14], 3);
    rpck3[15] = __builtin_ia32_vec_set_v4si(rpck3[15], rhs->i32_vals[itn_base + 15][col_base + 15], 3);
} /* prepare_rpck_fully */

inline static int32_t mul_pcks(v4si_t * lpck, v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2,  v4si_t * rpck3, unsigned k)
{
    v4si_t ret[4] = {0};

    /* For utilizing instruction pipelining and raising cache-hit rate, get similar calculations together. */ 
    ret[0] = __builtin_ia32_pmulld128(lpck[0], rpck0[k]);
    ret[1] = __builtin_ia32_pmulld128(lpck[1], rpck1[k]);
    ret[2] = __builtin_ia32_pmulld128(lpck[2], rpck2[k]);
    ret[3] = __builtin_ia32_pmulld128(lpck[3], rpck3[k]);

    return v4si_sum_up_v2(ret);
} /* mul_pcks */

inline static void mul_pcks_fully(ptr_matrix_t mtx, v4si_t * lpck, v4si_t * rpck0, v4si_t * rpck1, v4si_t * rpck2, v4si_t * rpck3, unsigned int row, unsigned int col_base)
{
    mtx->i32_vals[row][col_base +  0] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  0);
    mtx->i32_vals[row][col_base +  1] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  1);
    mtx->i32_vals[row][col_base +  2] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  2);
    mtx->i32_vals[row][col_base +  3] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  3);
    mtx->i32_vals[row][col_base +  4] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  4);
    mtx->i32_vals[row][col_base +  5] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  5);
    mtx->i32_vals[row][col_base +  6] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  6);
    mtx->i32_vals[row][col_base +  7] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  7);
    mtx->i32_vals[row][col_base +  8] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  8);
    mtx->i32_vals[row][col_base +  9] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3,  9);
    mtx->i32_vals[row][col_base + 10] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3, 10);
    mtx->i32_vals[row][col_base + 11] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3, 11);
    mtx->i32_vals[row][col_base + 12] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3, 12);
    mtx->i32_vals[row][col_base + 13] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3, 13);
    mtx->i32_vals[row][col_base + 14] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3, 14);
    mtx->i32_vals[row][col_base + 15] += mul_pcks(lpck, rpck0, rpck1, rpck2, rpck3, 15);
} /* mul_pcks_fully */

static ptr_matrix_t mat_multiply_and_store_simd_v2(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    unsigned int pcks_per_blk = (CPU_CACHE_LINE_BYTES / lhs->value_size) / lhs->pack_len;
    unsigned int pcks_per_row = lhs->pack_cnt_per_row;
    unsigned int pcks_per_itn = lhs->padded_col_cnt / lhs->pack_len;
    unsigned int pcks_per_col = rhs->padded_col_cnt / rhs->pack_len;
    unsigned int blks_per_row = round_count_to_multiples_of(pcks_per_row, pcks_per_blk) / pcks_per_blk;
    unsigned int blks_per_itn = round_count_to_multiples_of(pcks_per_itn, pcks_per_blk) / pcks_per_blk;
    unsigned int blks_per_col = round_count_to_multiples_of(pcks_per_col, pcks_per_blk) / pcks_per_blk;
    unsigned int vals_per_pck = lhs->pack_len;
    unsigned int vals_per_blk = vals_per_pck * pcks_per_blk;

#pragma omp parallel for schedule(static)
    for (unsigned int j = 0; j < blks_per_col; j += 1) {
        unsigned int col_base = j * pcks_per_blk * vals_per_pck;

        for (unsigned int k = 0; k < blks_per_itn; k += 1) {
            unsigned int itn_base = k * pcks_per_blk * vals_per_pck;
            v4si_t rpck[pcks_per_blk][vals_per_blk];

            prepare_rpck_fully(rpck[0], rpck[1], rpck[2], rpck[3], rhs, itn_base, col_base);

            for (unsigned int i = 0; i < blks_per_row; i += 1) {
                unsigned row_base = i * pcks_per_blk * vals_per_pck; 

                unsigned row = row_base; /* 0 */
                unsigned pck_off = row * lhs->pack_cnt_per_row + k * pcks_per_blk;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 1 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 2 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 3 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 4 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 5 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 6 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 7 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 8 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 9 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 10 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 11 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 12 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 13 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 14 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);

                row += 1; /* 15 */
                pck_off += lhs->pack_cnt_per_row;
                mul_pcks_fully(mtx, &lhs->i32_packs[pck_off], rpck[0], rpck[1], rpck[2], rpck[3], row, col_base);
            } /* for */
        } /* for */
    } /* for */
    return mtx;
} /* mat_multiply_and_store_simd_v2 */

static void i32_scr_multiply_and_store_plain(ptr_matrix_t mtx, int32_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;

    for (i = 0; i < rhs->row_cnt; i += 1) {
        for (j = 0; j < rhs->col_cnt; j += 1) {
            mtx->i32_vals[i][j] = lhs * rhs->i32_vals[i][j];
        } /* for j */
    } /* for i */
} /* i32_scr_multiply_and_store_plain */

static void i32_scr_multiply_and_store_simd(ptr_matrix_t mtx, int32_t lhs, ptr_matrix_t rhs)
{
    v4si_t scalar = {0};
    unsigned int i = 0;
    unsigned int itr_cnt = 0;
    unsigned int rmd_cnt = 0;
    unsigned int valid_pack_cnt = rhs->padded_row_cnt * rhs->pack_cnt_per_row;
    
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 0);
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 1);
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 2);
    scalar = __builtin_ia32_vec_set_v4si(scalar, lhs, 3);

    itr_cnt = valid_pack_cnt / 8;
    rmd_cnt = valid_pack_cnt % 8;

    for (i = 0; i < itr_cnt; i += 1) {
        mtx->i32_packs[i * 8 + 0] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 0], scalar);
        mtx->i32_packs[i * 8 + 1] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 1], scalar);
        mtx->i32_packs[i * 8 + 2] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 2], scalar);
        mtx->i32_packs[i * 8 + 3] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 3], scalar);
        mtx->i32_packs[i * 8 + 4] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 4], scalar);
        mtx->i32_packs[i * 8 + 5] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 5], scalar);
        mtx->i32_packs[i * 8 + 6] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 6], scalar);
        mtx->i32_packs[i * 8 + 7] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 7], scalar);
    } /* for i */

    switch (rmd_cnt) {
        case 7: mtx->i32_packs[i * 8 + 6] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 6], scalar);
        case 6: mtx->i32_packs[i * 8 + 5] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 5], scalar);
        case 5: mtx->i32_packs[i * 8 + 4] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 4], scalar);
        case 4: mtx->i32_packs[i * 8 + 3] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 3], scalar);
        case 3: mtx->i32_packs[i * 8 + 2] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 2], scalar);
        case 2: mtx->i32_packs[i * 8 + 1] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 1], scalar);
        case 1: mtx->i32_packs[i * 8 + 0] = __builtin_ia32_pmulld128(rhs->i32_packs[i * 8 + 0], scalar);
        default: break;
    } /* switch */
} /* i32_scr_multiply_and_store_simd */

void mtx_i32_scalar_multiply_and_store(ptr_matrix_t mtx, int32_t lhs, ptr_matrix_t rhs, mtx_option_t opt)
{
    (*mtx->ops->i32_scr_mul[opt & 0x3])(mtx, lhs, rhs);
} /* mtx_i32_scalar_multiply_and_store */

static void i32_init_identity_plain(ptr_matrix_t mtx)
{
    unsigned int i = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        mtx->i32_vals[i][i] = 1;
    } /* for */
} /* i32_init_identity_plain */

static void i32_init_zeros_plain(ptr_matrix_t mtx)
{
    memset(mtx->i32_padded, 0, mtx->padded_byte_cnt);
} /* i32_init_zeros_plain */

static void i32_init_ones_plain(ptr_matrix_t mtx)
{
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < mtx->row_cnt; i += 1) {
        for (j = 0; j < mtx->col_cnt; j += 1) {
            mtx->i32_vals[i][j] = 1;
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
    {&mat_add_and_store_plain, &mat_add_and_store_plain},
    {&mat_sub_and_store_plain, &mat_sub_and_store_plain},
    {&mat_multiply_and_store_plain, &mat_multiply_and_store_simd_v2},
    {&i32_scr_multiply_and_store_plain, &i32_scr_multiply_and_store_simd},
};

ptr_matrix_t mtx_i32_allocate(unsigned int row_cnt, unsigned int col_cnt)
{
    unsigned int i = 0;
    ptr_matrix_t mtx = NULL;

    mtx = mtx_allocate(row_cnt, col_cnt, sizeof(int32_t), I32_PACK_LEN, &i32_ops);
    if (! mtx) {
        return NULL;
    } /* if */
    return mtx;
} /* mtx_i32_allocate */

