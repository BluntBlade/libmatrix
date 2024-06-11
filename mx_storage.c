#include "config.h"
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
        v8si_t *    i32_vals[0];    // Pointers to the start address of each row.
        void *      ptrs[0];        // Pointers to the start address of each row.
    };
} mx_stor_t;

mx_stor_ptr mstr_create(uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t vals_in_pck)
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
        m->ptrs[i] = m->ptrs[i - 1] + ms->val_size * ms->cols_padded;
    } /* for */
    return ms;
} // mstr_create

void mstr_destroy(mx_stor_ptr ms)
{
    if (ms) {
        free(ms->buf);
    } // if
    free(ms);
} // mstr_destroy

uint32_t mstr_copy_row_to_vector(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, void * buf, uint32_t pcks, mx_vec_opt_t opt);
uint32_t mstr_copy_column_to_vector(mx_stor_ptr ms, uint32_t ridx, uint32_t cidx, void * buf, uint32_t pcks, mx_vec_opt_t opt);

// ---- V8SI related definitions ----

static v8si_t v8si_zero = {0, 0, 0, 0, 0, 0, 0, 0};
static v8si_t v8si_mask = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
static v8si_t v8si_idx[2] = {{0, 1, 2, 3, 4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14, 15}};

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
    v8si_t mask[2];

    idx[0] = v8si_idx[0] * idx_span;
    idx[1] = v8si_idx[1] * idx_span;

    mask[0] = v8si_mask;
    mask[1] = v8si_mask;
    memset((void *)&mask[msks], 0, sizeof(int32_t) * (I32_VALS_IN_CACHE_LINE - msks));

    switch (itrs) {
        case 16:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 15:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 14:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 13:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 12:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 11:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case 10:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  9:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  8:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  7:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  6:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  5:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  4:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  3:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  2:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));
            chk_pck += 2;
            src += src_span;

        case  1:
            chk_pck[0] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[0], mask[0], sizeof(int32_t));
            chk_pck[1] = __builtin_ia32_gathersiv8si(v8si_zero, src, idx[1], mask[1], sizeof(int32_t));

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
