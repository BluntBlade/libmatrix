#include <string.h>
#include <avx2intrin.h>

#include "config.h"
#include "mx_types.h"
#include "mx_storage.h"

inline static uint32_t round_to_multiples(uint32_t cnt, uint32_t n)
{
    return (cnt + (n - 1)) & (~(n - 1));
} // round_to_multiples

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
        void *      ptrs[0];        // Pointers to the start address of each row.
        int32_t *   i32_vals[0];
        v8si_t *    v8si_pcks[0];
    };
} mx_stor_t;

static create(uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t vals_in_pck)
{
    uint32_t i = 0;
    mx_stor_ptr ms = NULL;

    ms = malloc(sizeof(mx_stor_t) + sizeof(void *) * rows);
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
    m->ptrs[0] = (void *)( ((size_t)ms->buf + (ms->alignment - 1)) & (~(ms->alignment - 1)));
    for (i = 1; i < ms->rows; i += 1) {
        m->ptrs[i] = m->ptrs[i - 1] + ms->val_sz * ms->cols_padded;
    } /* for */
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

// ---- V8SI related definitions ----

static uint8_t v8si_mask_sel[16][2] = {
    {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0},
    {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5}, {8, 6}, {8, 7}, {8, 8}
};

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

void mstr_v8si_init_zeros(ms_stor_ptr ms)
{
    memset(ms->ptrs[0], 0, ms->bytes);
} // mstr_v8si_init_zeros

void mstr_v8si_init_ident(ms_stor_ptr ms)
{
    uint32_t i = 0;
    uint32_t j = 0;

    mstr_v8si_init_zeros(ms);
    for (i = 0; i < ms->rows; i += 1) {
        for (j = 0; j < ms->cols; j += 1) {
            ms->i32_vals[i][j] = 1;
        } // for
    } // for
} // mstr_v8si_init_ident

void mstr_v8si_fill(ms_stor_ptr ms, int32_t val)
{
    v8si_t vals = {val, val, val, val, val, val, val, val};
    uint32_t i = 0;
    for (i = 0; i < ms->cols_padded / I32_VALS_IN_V8SI; i += 1) {
        ms->v8si_pcks[0][i] = vals;
    } // for
    for (i = 1; i < ms->rows; i += 1) {
        memcpy(&ms->v8si_pcks[i][0], &ms->v8si_pcks[0][0], ms->val_sz * ms->cols_padded);
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

    mask[0] = &v8si_mask[v8si_mask_sel[msks][0]];
    mask[1] = &v8si_mask[v8si_mask_sel[msks][1]];

    switch (itrs) {
        case 16:
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

        default:
            break;
    } // switch
} // v8si_assemble_chunk

void mstr_v8si_assemble_chunk_from_rows(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, mx_chunk_ptr chk)
{
    uint32_t rdelta = 0;
    uint32_t cdelta = 0;

    if (ms->rows <= ridx || ms->cols <= cidx) {
        // Out of range.
        memset(&chk->v8si_pcks[0][0], 0, sizeof(chk->v8si_pcks));
        return;
    } // if

    rdelta = (ms->rows - ridx) > I32_VALS_IN_CACHE_LINE ? I32_VALS_IN_CACHE_LINE : (ms->rows - ridx);
    cdelta = (ms->cols - cidx) > I32_VALS_IN_CACHE_LINE ? I32_VALS_IN_CACHE_LINE : (ms->cols - cidx);

    v8si_assemble_chunk(&chk->v8si_pcks[0][0], &ms->i32_vals[ridx][cidx], ms->cols_padded, 1, rdelta, cdelta);
} // mstr_v8si_assemble_chunk_from_rows

void mstr_v8si_assemble_chunk_from_columns(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, mx_chunk_ptr chk)
{
    uint32_t rdelta = 0;
    uint32_t cdelta = 0;

    if (ms->rows <= ridx || ms->cols <= cidx) {
        // Out of range.
        memset(&chk->v8si_pcks[0][0], 0, sizeof(chk->v8si_pcks));
        return;
    } // if

    rdelta = ((ms->rows - ridx) < I32_VALS_IN_CACHE_LINE) ? (ms->rows - ridx) : I32_VALS_IN_CACHE_LINE;
    cdelta = ((ms->cols - cidx) < I32_VALS_IN_CACHE_LINE) ? (ms->cols - cidx) : I32_VALS_IN_CACHE_LINE;

    v8si_assemble_chunk(&chk->v8si_pcks[0][0], &ms->i32_vals[ridx][cidx], 1, ms->cols_padded, cdelta, rdelta);
} // mstr_v8si_assemble_chunk_from_columns

void mstr_v8si_copy_from_chunk(ms_stor_ptr ms, uint32_t ridx, uint32_t cidx, mx_chunk_ptr chk)
{
    uint32_t i = ridx;
    uint32_t rows = I32_VALS_IN_CACHE_LINE;

    if (ms->rows - ridx < rows) {
        rows = ms->rows - ridx;
    } // if

    // TODO: When ridx or cidx is not the integer multiples of the number of values in one pack.
    switch (rows) {
        case 16:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case 15:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case 14:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case 13:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case 12:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case 11:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case 10:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case  9:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case  8:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case  7:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case  6:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case  5:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case  4:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case  3:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case  2:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        case  1:
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 0] = chk->v8si_pcks[i][0];
            ms->v8si_pcks[i][cidx / I32_VALS_IN_V8SI + 1] = chk->v8si_pcks[i][1];
            i += 1;
        default:
            break;
    } // switch
} // mstr_v8si_copy_from_chunk

void mstr_v8si_accumulate_from_chunk(ms_stor_ptr ms, uint32_t ridx, uint32_t cidx, mx_chunk_ptr chk)
{
} // mstr_v8si_accumulate_from_chunk
