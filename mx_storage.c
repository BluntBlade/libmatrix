#include <string.h>
#include <immintrin.h>

#include "config.h"
#include "mx_storage.h"

inline static uint32_t round_to_multiples(uint32_t cnt, uint32_t n)
{
    return (cnt + (n - 1)) & (~(n - 1));
} // round_to_multiples

inline static uint32_t round_to_multiples_of_8(uint32_t cnt)
{
    return (cnt + (8 - 1)) & (~(8 - 1));
} // round_to_multiples_of_8

inline static uint32_t round_to_multiples_of_16(uint32_t cnt)
{
    return (cnt + (16 - 1)) & (~(16 - 1));
} // round_to_multiples_of_16

inline static uint32_t ceil_to_or_less_than_16(uint32_t cnt)
{
    return (cnt < 16) ? cnt : 16;
} // ceil_to_or_less_than_16

typedef struct MX_STORAGE {
    uint32_t    val_sz;         // The size of one value, in bytes.
    uint32_t    vals_in_pck;    // The number of values in one pack.
    uint32_t    rows;           // The number of rows.
    uint32_t    cols;           // The number of columns.
    uint32_t    cols_padded;    // The actual number of columns, including padding ones.
    uint32_t    alignment;      // The number of bytes to align with.
    size_t      bytes;          // Allocated bytes of the buffer.

    void *      buf;            // Non-aligned buffer of values, only for calling free().
    union {
        void *      data;       // Aligned buffer of values.
        int32_t *   i32_vals;
        v8si_t *    v8si_pcks;
    };
} mx_stor_t;

static mx_stor_ptr create(uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t vals_in_pck)
{
    uint32_t i = 0;
    mx_stor_ptr ms = NULL;

    ms = malloc(sizeof(mx_stor_t));
    if (! ms) {
        return NULL;
    } // if

    ms->val_sz = val_sz;
    ms->rows = rows;
    ms->cols = cols;
    ms->cols_padded = round_to_multiples(cols, vals_in_pck);
    ms->bytes = ms->val_sz * ms->rows * ms->cols_padded;
    ms->vals_in_pck = vals_in_pck;

    ms->alignment = ms->val_sz * ms->vals_in_pck;
    ms->buf = malloc(ms->bytes + ms->alignment);
    if (! ms->buf) {
        free(ms);
        return NULL;
    } /* if */
    
    /* NOTE: Align to the vector type's size, otherwise it will be segmentation fault when access to packs. */
    ms->data = (void *)( ((size_t)ms->buf + (ms->alignment - 1)) & (~(ms->alignment - 1)));
    return ms;
} // create

mx_stor_ptr mstr_v8si_create(uint32_t rows, uint32_t cols)
{
    return create(rows, cols, sizeof(int32_t), I32_VALS_IN_V8SI);
} // mstr_v8si_create

void mstr_destroy(mx_stor_ptr ms)
{
    if (ms) {
        free(ms->buf);
    } // if
    free(ms);
} // mstr_destroy

uint32_t mstr_chunks_in_row(mx_stor_ptr ms)
{
    return round_to_multiples(ms->rows, ms->vals_in_pck) / ms->vals_in_pck;
} // mstr_chunks_in_row

uint32_t mstr_chunks_in_column(mx_stor_ptr ms)
{
    return round_to_multiples(ms->cols, ms->vals_in_pck) / ms->vals_in_pck;
} // mstr_chunks_in_column

// ---- V8SI related definitions ----

static v8si_t v8si_mask[9] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {~0,  0,  0,  0,  0,  0,  0,  0},
    {~0, ~0,  0,  0,  0,  0,  0,  0},
    {~0, ~0, ~0,  0,  0,  0,  0,  0},
    {~0, ~0, ~0, ~0,  0,  0,  0,  0},
    {~0, ~0, ~0, ~0, ~0,  0,  0,  0},
    {~0, ~0, ~0, ~0, ~0, ~0,  0,  0},
    {~0, ~0, ~0, ~0, ~0, ~0, ~0,  0},
    {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0}
};

static v8si_t v8si_idx[2] = {{0, 1, 2, 3, 4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14, 15}};

inline static void * v8si_calc_base(mx_stor_ptr ms, uint32_t row_idx, uint32_t col_idx, uint32_t rows_in_chk)
{
    return ms->data + (row_idx * ms->cols_padded + rows_in_chk * col_idx) * sizeof(int32_t);
} // v8si_calc_base

void mstr_v8si_init_zeros(mx_stor_ptr ms)
{
    memset(ms->data, 0, ms->bytes);
} // mstr_v8si_init_zeros

void mstr_v8si_init_ident(mx_stor_ptr ms)
{
    uint32_t i = 0;
    uint32_t cols_in_last_chk = 0;
    mx_chunk_ptr chk = NULL;
    int32_t * base = NULL;

    mstr_v8si_init_zeros(ms);

    for (i = 0; i < mstr_chunks_in_row(ms) - 1; i += 1) {
        chk = v8si_calc_base(ms, i, i, 16);
        chk->i32_vals[ 0][ 0] = 1;
        chk->i32_vals[ 1][ 1] = 1;
        chk->i32_vals[ 2][ 2] = 1;
        chk->i32_vals[ 3][ 3] = 1;
        chk->i32_vals[ 4][ 4] = 1;
        chk->i32_vals[ 5][ 5] = 1;
        chk->i32_vals[ 6][ 6] = 1;
        chk->i32_vals[ 7][ 7] = 1;
        chk->i32_vals[ 8][ 8] = 1;
        chk->i32_vals[ 9][ 9] = 1;
        chk->i32_vals[10][10] = 1;
        chk->i32_vals[11][11] = 1;
        chk->i32_vals[12][12] = 1;
        chk->i32_vals[13][13] = 1;
        chk->i32_vals[14][14] = 1;
        chk->i32_vals[15][15] = 1;
    } // for

    cols_in_last_chk = ms->cols_padded - ms->cols;
    base = v8si_calc_base(ms, i, i, cols_in_last_chk);
    switch (cols_in_last_chk) {
        case 15: base[14 * cols_in_last_chk + 14] = 1;
        case 14: base[13 * cols_in_last_chk + 13] = 1;
        case 13: base[12 * cols_in_last_chk + 12] = 1;
        case 12: base[11 * cols_in_last_chk + 11] = 1;
        case 11: base[10 * cols_in_last_chk + 10] = 1;
        case 10: base[ 9 * cols_in_last_chk +  9] = 1;
        case  9: base[ 7 * cols_in_last_chk +  7] = 1;
        case  8: base[ 7 * cols_in_last_chk +  7] = 1;
        case  7: base[ 6 * cols_in_last_chk +  6] = 1;
        case  6: base[ 5 * cols_in_last_chk +  5] = 1;
        case  5: base[ 4 * cols_in_last_chk +  4] = 1;
        case  4: base[ 3 * cols_in_last_chk +  3] = 1;
        case  3: base[ 2 * cols_in_last_chk +  2] = 1;
        case  2: base[ 1 * cols_in_last_chk +  1] = 1;
        case  1: base[ 0 * cols_in_last_chk +  0] = 1;
        default: break;
    } // switch
} // mstr_v8si_init_ident

void mstr_v8si_fill(mx_stor_ptr ms, int32_t val)
{
    v8si_t vals = {val, val, val, val, val, val, val, val};
    v8si_t edge = vals;
    uint32_t i = 0;
    void * base = NULL;

    switch (ms->cols_padded - ms->cols) {
        case 7: edge[7] = 0;
        case 6: edge[6] = 0;
        case 5: edge[5] = 0;
        case 4: edge[4] = 0;
        case 3: edge[3] = 0;
        case 2: edge[2] = 0;
        case 1: edge[1] = 0;
        default: break;
    } // switch

    for (i = 0; i < ms->cols_padded / I32_VALS_IN_V8SI - 1; i += 1) {
        ms->v8si_pcks[i] = vals;
    } // for
    ms->v8si_pcks[i] = edge;

    base = &ms->v8si_pcks[i];
    for (i = 1; i < ms->rows; i += 1) {
        memcpy(base, &ms->v8si_pcks[0], ms->val_sz * ms->cols_padded);
        base += ms->val_sz * ms->cols_padded;
    } // for
} // mstr_v8si_fill

//// NAME:
////   v8si_assemble_chunk
////
//// ACTION:
////   Copy int32_t values from given source addresses and assemble them into packs.
////
//// PARAMETERS:
////   chk_pck    The address of the chunk packs to hold assembled values.
////   src        The first address of the source.
////   src_span   Specify the interval between two source addresses, which shall be skiped before next copy.
////   idx_span   Specify the interval between each values being packed from one source address.
////              This controls the copying from row (1) or column (16 for 16x16 values storage, or `cols_padded` for flatten storage).
////   itrs       The total number of copies, one for each source address.
////   msks       Specify how many values should be copied from one source address.
////
//// RETURN VALUE:
////   none
static void v8si_assemble_chunk(v8si_t * chk_pck, int32_t * src, uint32_t src_span, uint32_t idx_span, uint32_t itrs, uint32_t msks)
{
    v8si_t idx[2];
    v8si_t * mask[2];

    idx[0] = v8si_idx[0] * idx_span;
    idx[1] = v8si_idx[1] * idx_span;

    mask[0] = &v8si_mask[(msks <= 8) ? (msks) : 8];
    mask[1] = &v8si_mask[(msks <= 8) ? 0 : (msks - 8)];

    switch (itrs) {
        default:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 15:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 14:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 13:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 12:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 11:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 10:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  9:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  8:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  7:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  6:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  5:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  4:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  3:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  2:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  1:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], *mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], *mask[1], sizeof(int32_t));
    } // switch
} // v8si_assemble_chunk

inline static void * v8si_locate_chunk(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, uint32_t * rows_in_chk, uint32_t * cols_in_chk, bool * full)
{
    uint32_t row_idx = ridx * I32_VALS_IN_CACHE_LINE; // Refer to values.
    uint32_t col_idx = cidx * I32_VALS_IN_CACHE_LINE; // Refer to values.

    *rows_in_chk = ceil_to_or_less_than_16(ms->rows - row_idx);
    *cols_in_chk = ceil_to_or_less_than_16(ms->cols - col_idx);

    *full = (*rows_in_chk == I32_VALS_IN_CACHE_LINE) && (*cols_in_chk == I32_VALS_IN_CACHE_LINE);
    return v8si_calc_base(ms, row_idx, col_idx, *rows_in_chk);
} // v8si_locate_chunk

mx_chunk_ptr mstr_v8si_copy_chunk(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, mx_chunk_ptr chk, uint32_t * rows_in_chk, uint32_t * cols_in_chk, bool * full)
{
    void * base = v8si_locate_chunk(ms, ridx, cidx, rows_in_chk, cols_in_chk, full);

    if (*full) {
        // If the source chunk is full of 16x16 values, return the pointer to it directly.
        return base;
    } // if

    // Otherwise copy it totally.
    v8si_assemble_chunk(&chk->v8si_pcks[0][0], base, round_to_multiples_of_8(*cols_in_chk), 1, *rows_in_chk, *cols_in_chk);
    return chk;
} // mstr_v8si_copy_chunk

mx_chunk_ptr mstr_v8si_transpose_chunk(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, mx_chunk_ptr chk, uint32_t * rows_in_chk, uint32_t * cols_in_chk, bool * full)
{
    void * base = v8si_locate_chunk(ms, ridx, cidx, rows_in_chk, cols_in_chk, full);
    v8si_assemble_chunk(&chk->v8si_pcks[0][0], base, 1, round_to_multiples_of_8(*cols_in_chk), *cols_in_chk, *rows_in_chk);
    return chk;
} // mstr_v8si_transpose_chunk

void mstr_v8si_store_chunk(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, mx_chunk_ptr chk, uint32_t rows_in_chk, uint32_t cols_in_chk)
{
    void * base = v8si_calc_base(ms, ridx * I32_VALS_IN_CACHE_LINE, cidx * I32_VALS_IN_CACHE_LINE, rows_in_chk);
    memcpy(base, chk->v8si_pcks, rows_in_chk * round_to_multiples_of_8(cols_in_chk) * sizeof(int32_t));
} // mstr_v8si_store_chunk
