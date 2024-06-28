#include <assert.h>
#include <string.h>
#include <immintrin.h>

#include "src/config.h"
#include "src/mx_common.h"
#include "src/v8si_storage.h"

// ---- V8SI related definitions ----

static v8si_t v8si_idx[2] = {{0, 1, 2, 3, 4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14, 15}};

void mstr_v8si_init_zeros(mx_stor_ptr ms)
{
    memset(ms->data, 0, ms->bytes);
} // mstr_v8si_init_zeros

void mstr_v8si_init_identity(mx_stor_ptr ms)
{
    uint32_t i = ms->chks_in_width;
    uint32_t last_rows = 0;
    uint32_t cols_padded = 0;
    uint32_t val_idx = 0;
    int32_t * base = NULL;

    mstr_v8si_init_zeros(ms);

    last_rows = ms->last_chk_height;
    cols_padded = mx_round_to_multiples_of_8(last_rows);
    val_idx = (i - 1) * I32_VALS_IN_CACHE_LINE;
    base = mstr_v8si_calc_base(ms, val_idx, val_idx, last_rows);
    switch (last_rows) {
        default: assert(1); break;
        case 16:
            do { *base = 1; base += cols_padded + 1;   // 0
        case 15: *base = 1; base += cols_padded + 1;   // 1
        case 14: *base = 1; base += cols_padded + 1;   // 2
        case 13: *base = 1; base += cols_padded + 1;   // 3
        case 12: *base = 1; base += cols_padded + 1;   // 4
        case 11: *base = 1; base += cols_padded + 1;   // 5
        case 10: *base = 1; base += cols_padded + 1;   // 6
        case  9: *base = 1; base += cols_padded + 1;   // 7
        case  8: *base = 1; base += cols_padded + 1;   // 8
        case  7: *base = 1; base += cols_padded + 1;   // 9
        case  6: *base = 1; base += cols_padded + 1;   // 10
        case  5: *base = 1; base += cols_padded + 1;   // 11
        case  4: *base = 1; base += cols_padded + 1;   // 12
        case  3: *base = 1; base += cols_padded + 1;   // 13
        case  2: *base = 1; base += cols_padded + 1;   // 14
        case  1: *base = 1;                            // 15
                if (--i == 0) {
                    break;
                } // if
                val_idx = (i - 1) * I32_VALS_IN_CACHE_LINE;
                base = mstr_v8si_calc_base(ms, val_idx, val_idx, 16);
                cols_padded = 16;
            } while (1);
            break;
    } // switch
} // mstr_v8si_init_identity

int32_t mstr_v8si_get(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx)
{
    uint32_t base_ridx = mx_floor_to_multiples_of_16(val_ridx);
    uint32_t base_cidx = mx_floor_to_multiples_of_16(val_cidx);
    uint32_t rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    uint32_t cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - base_cidx);
    int32_t * base = mstr_v8si_calc_base(ms, base_ridx, base_cidx, rows_in_chk);
    return base[(val_ridx - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk) + (val_cidx - base_cidx)];
} // mstr_v8si_get

void mstr_v8si_set(mx_stor_ptr ms, uint32_t val_ridx, uint32_t val_cidx, int32_t src)
{
    uint32_t base_ridx = mx_floor_to_multiples_of_16(val_ridx);
    uint32_t base_cidx = mx_floor_to_multiples_of_16(val_cidx);
    uint32_t rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    uint32_t cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - base_cidx);
    int32_t * base = mstr_v8si_calc_base(ms, base_ridx, base_cidx, rows_in_chk);
    base[(val_ridx - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk) + (val_cidx - base_cidx)] = src;
} // mstr_v8si_set

void mstr_v8si_fill(mx_stor_ptr ms, int32_t src)
{
    v8si_t vals = {src, src, src, src, src, src, src, src};
    uint32_t i = 0;
    v8si_t * base = NULL;

    base = ms->data;
    for (i = 0; i < ms->cols_padded / I32_VALS_IN_V8SI; i += 1) {
        base[i] = vals;
    } // for
    for (i = 1; i < ms->rows; i += 1) {
        base += ms->cols_padded / I32_VALS_IN_V8SI;
        memcpy(base, ms->data, sizeof(int32_t) * ms->cols_padded);
    } // for
    // NOTE: No need to zero out any padded int32_t values.
} // mstr_v8si_fill

//// NAME:
////   mstr_v8si_assemble_chunk
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
void mstr_v8si_assemble_chunk(v8si_t * chk_pck, int32_t * src, uint32_t src_span, uint32_t idx_span, uint32_t itrs, uint32_t msks)
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
} // mstr_v8si_assemble_chunk

mx_chunk_ptr mstr_v8si_copy_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr chk, uint32_t * rows_in_chk, uint32_t * cols_in_chk, bool * full)
{
    void * base = mstr_v8si_locate_chunk(ms, chk_ridx, chk_cidx, rows_in_chk, cols_in_chk, full);

    if (*full) {
        // If the source chunk is full of 16x16 values, return the pointer to it directly.
        return base;
    } // if

    // Otherwise copy it totally.
    mstr_v8si_assemble_chunk(&chk->v8si_pcks[0][0], base, mx_round_to_multiples_of_8(*cols_in_chk), 1, *rows_in_chk, *cols_in_chk);
    return chk;
} // mstr_v8si_copy_chunk

mx_chunk_ptr mstr_v8si_transpose_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr chk, uint32_t * rows_in_chk, uint32_t * cols_in_chk, bool * full)
{
    void * base = mstr_v8si_locate_chunk(ms, chk_ridx, chk_cidx, rows_in_chk, cols_in_chk, full);
    mstr_v8si_assemble_chunk(&chk->v8si_pcks[0][0], base, 1, mx_round_to_multiples_of_8(*cols_in_chk), *cols_in_chk, *rows_in_chk);
    // Swap the number of rows and columns of the target chunk since it is transposed.
    *rows_in_chk ^= *cols_in_chk;
    *cols_in_chk ^= *rows_in_chk;
    *rows_in_chk ^= *cols_in_chk;
    return chk;
} // mstr_v8si_transpose_chunk

void mstr_v8si_store_chunk(mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx, mx_chunk_ptr chk)
{
    uint32_t i = 0;
    uint32_t bytes = 0;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;
    bool full = false;
    void * base = mstr_v8si_locate_chunk(ms, chk_ridx, chk_cidx, &rows_in_chk, &cols_in_chk, &full);

    if (base == chk) {
        return;
    } // if

    // NOTE: Copy bytes depends on the number of columns in the target chunk.
    bytes = sizeof(int32_t) * mx_round_to_multiples_of_8(cols_in_chk);
    switch (rows_in_chk) {
        default: assert(1); break;
        case 16: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case 15: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case 14: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case 13: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case 12: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case 11: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case 10: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case  9: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case  8: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case  7: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case  6: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case  5: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case  4: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case  3: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case  2: memcpy(base, &chk->i32_vals[i][0], bytes); base += bytes; i += 1;
        case  1: memcpy(base, &chk->i32_vals[i][0], bytes);
    } // switch
    // NOTE: No need to zero out any padded int32_t values.
} // mstr_v8si_store_chunk
