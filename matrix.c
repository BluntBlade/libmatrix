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
    return (cnt & ~(n - 1)) + ((cnt & (n - 1)) ? n : 0);
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

inline static void prepare_rhs_pack(v4si_t * rhs_pack, ptr_matrix_t rhs, unsigned int itn_base, unsigned int col)
{
    rhs_pack[0] = __builtin_ia32_vec_set_v4si(rhs_pack[0], rhs->i32_vals[itn_base +  0][col], 0);
    rhs_pack[0] = __builtin_ia32_vec_set_v4si(rhs_pack[0], rhs->i32_vals[itn_base +  1][col], 1);
    rhs_pack[0] = __builtin_ia32_vec_set_v4si(rhs_pack[0], rhs->i32_vals[itn_base +  2][col], 2);
    rhs_pack[0] = __builtin_ia32_vec_set_v4si(rhs_pack[0], rhs->i32_vals[itn_base +  3][col], 3);

    rhs_pack[1] = __builtin_ia32_vec_set_v4si(rhs_pack[1], rhs->i32_vals[itn_base +  4][col], 0);
    rhs_pack[1] = __builtin_ia32_vec_set_v4si(rhs_pack[1], rhs->i32_vals[itn_base +  5][col], 1);
    rhs_pack[1] = __builtin_ia32_vec_set_v4si(rhs_pack[1], rhs->i32_vals[itn_base +  6][col], 2);
    rhs_pack[1] = __builtin_ia32_vec_set_v4si(rhs_pack[1], rhs->i32_vals[itn_base +  7][col], 3);

    rhs_pack[2] = __builtin_ia32_vec_set_v4si(rhs_pack[2], rhs->i32_vals[itn_base +  8][col], 0);
    rhs_pack[2] = __builtin_ia32_vec_set_v4si(rhs_pack[2], rhs->i32_vals[itn_base +  9][col], 1);
    rhs_pack[2] = __builtin_ia32_vec_set_v4si(rhs_pack[2], rhs->i32_vals[itn_base + 10][col], 2);
    rhs_pack[2] = __builtin_ia32_vec_set_v4si(rhs_pack[2], rhs->i32_vals[itn_base + 11][col], 3);

    rhs_pack[3] = __builtin_ia32_vec_set_v4si(rhs_pack[3], rhs->i32_vals[itn_base + 12][col], 0);
    rhs_pack[3] = __builtin_ia32_vec_set_v4si(rhs_pack[3], rhs->i32_vals[itn_base + 13][col], 1);
    rhs_pack[3] = __builtin_ia32_vec_set_v4si(rhs_pack[3], rhs->i32_vals[itn_base + 14][col], 2);
    rhs_pack[3] = __builtin_ia32_vec_set_v4si(rhs_pack[3], rhs->i32_vals[itn_base + 15][col], 3);
} /* prepare_rhs_pack */

inline static void multiply_lhs_and_rhs_packs(ptr_matrix_t mtx, ptr_matrix_t lhs, unsigned int row, unsigned int itn, unsigned int col, v4si_t * rhs_pack)
{
    v4si_t lhs_pack[4] = {0};

    /* For utilizing instruction pipelining and raising cache-hit rate, get similar calculations together. */ 
    lhs_pack[0] = __builtin_ia32_pmulld128(lhs->i32_packs[itn + 0], rhs_pack[0]);
    lhs_pack[1] = __builtin_ia32_pmulld128(lhs->i32_packs[itn + 1], rhs_pack[1]);
    lhs_pack[2] = __builtin_ia32_pmulld128(lhs->i32_packs[itn + 2], rhs_pack[2]);
    lhs_pack[3] = __builtin_ia32_pmulld128(lhs->i32_packs[itn + 3], rhs_pack[3]);

    mtx->i32_vals[row][col] += v4si_sum_up(&lhs_pack[0]);
    mtx->i32_vals[row][col] += v4si_sum_up(&lhs_pack[1]);
    mtx->i32_vals[row][col] += v4si_sum_up(&lhs_pack[2]);
    mtx->i32_vals[row][col] += v4si_sum_up(&lhs_pack[3]);
} /* multiply_lhs_and_rhs_packs */

static ptr_matrix_t mat_multiply_and_store_simd_v2(ptr_matrix_t mtx, ptr_matrix_t lhs, ptr_matrix_t rhs)
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    unsigned int row_base = 0;
    unsigned int itn_base = 0;
    unsigned int col_base = 0;
    unsigned int row = 0;
    unsigned int itn = 0;
    unsigned int pcks_per_blk = (CPU_CACHE_LINE_BYTES / lhs->value_size) / lhs->pack_len;
    unsigned int pcks_per_row = lhs->pack_cnt_per_row;
    unsigned int pcks_per_itn = lhs->padded_col_cnt / lhs->pack_len;
    unsigned int pcks_per_col = rhs->padded_col_cnt / rhs->pack_len;
    unsigned int blks_per_row = round_count_to_multiples_of(pcks_per_row, pcks_per_blk) / pcks_per_blk;
    unsigned int blks_per_itn = round_count_to_multiples_of(pcks_per_itn, pcks_per_blk) / pcks_per_blk;
    unsigned int blks_per_col = round_count_to_multiples_of(pcks_per_col, pcks_per_blk) / pcks_per_blk;
    unsigned int vals_per_pck = lhs->pack_len;
    unsigned int vals_per_blk = vals_per_pck * pcks_per_blk;
    v4si_t rhs_pack[vals_per_blk][pcks_per_blk];

    for (j = 0; j < blks_per_col; j += 1) {
        col_base = j * pcks_per_blk * vals_per_pck;

        for (k = 0; k < blks_per_itn; k += 1) {
            itn_base = k * pcks_per_blk * vals_per_pck;

            prepare_rhs_pack((v4si_t *)rhs_pack[ 0], rhs, itn_base, col_base +  0);
            prepare_rhs_pack((v4si_t *)rhs_pack[ 1], rhs, itn_base, col_base +  1);
            prepare_rhs_pack((v4si_t *)rhs_pack[ 2], rhs, itn_base, col_base +  2);
            prepare_rhs_pack((v4si_t *)rhs_pack[ 3], rhs, itn_base, col_base +  3);
            prepare_rhs_pack((v4si_t *)rhs_pack[ 4], rhs, itn_base, col_base +  4);
            prepare_rhs_pack((v4si_t *)rhs_pack[ 5], rhs, itn_base, col_base +  5);
            prepare_rhs_pack((v4si_t *)rhs_pack[ 6], rhs, itn_base, col_base +  6);
            prepare_rhs_pack((v4si_t *)rhs_pack[ 7], rhs, itn_base, col_base +  7);
            prepare_rhs_pack((v4si_t *)rhs_pack[ 8], rhs, itn_base, col_base +  8);
            prepare_rhs_pack((v4si_t *)rhs_pack[ 9], rhs, itn_base, col_base +  9);
            prepare_rhs_pack((v4si_t *)rhs_pack[10], rhs, itn_base, col_base + 10);
            prepare_rhs_pack((v4si_t *)rhs_pack[11], rhs, itn_base, col_base + 11);
            prepare_rhs_pack((v4si_t *)rhs_pack[12], rhs, itn_base, col_base + 12);
            prepare_rhs_pack((v4si_t *)rhs_pack[13], rhs, itn_base, col_base + 13);
            prepare_rhs_pack((v4si_t *)rhs_pack[14], rhs, itn_base, col_base + 14);
            prepare_rhs_pack((v4si_t *)rhs_pack[15], rhs, itn_base, col_base + 15);

            for (i = 0; i < blks_per_row; i += 1) {
                row_base = i * pcks_per_blk * vals_per_pck; 

                row = row_base + 0;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 1;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 2;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 3;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 4;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 5;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 6;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 7;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 8;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 9;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 10;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 11;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 12;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 13;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 14;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);

                row = row_base + 15;
                itn = row * lhs->pack_cnt_per_row + itn_base / vals_per_pck;
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  0, (v4si_t *)rhs_pack[ 0]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  1, (v4si_t *)rhs_pack[ 1]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  2, (v4si_t *)rhs_pack[ 2]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  3, (v4si_t *)rhs_pack[ 3]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  4, (v4si_t *)rhs_pack[ 4]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  5, (v4si_t *)rhs_pack[ 5]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  6, (v4si_t *)rhs_pack[ 6]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  7, (v4si_t *)rhs_pack[ 7]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  8, (v4si_t *)rhs_pack[ 8]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base +  9, (v4si_t *)rhs_pack[ 9]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 10, (v4si_t *)rhs_pack[10]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 11, (v4si_t *)rhs_pack[11]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 12, (v4si_t *)rhs_pack[12]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 13, (v4si_t *)rhs_pack[13]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 14, (v4si_t *)rhs_pack[14]);
                multiply_lhs_and_rhs_packs(mtx, lhs, row, itn, col_base + 15, (v4si_t *)rhs_pack[15]);
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

