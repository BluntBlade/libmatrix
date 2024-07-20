#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "src/mx_common.h"
#include "src/mx_storage.h"

mx_stor_ptr mstr_init(mx_stor_ptr ms, uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t pck_len, uint32_t chk_len){
    ms->val_sz = val_sz;
    ms->rows = rows;
    ms->cols = cols;
    ms->cols_padded = mx_round_to_multiples(cols, pck_len);
    ms->bytes = ms->val_sz * ms->rows * ms->cols_padded;
    ms->pck_len = pck_len;
    ms->chk_len = chk_len;
    ms->chks_in_width = mx_round_to_multiples(cols, chk_len) / chk_len;
    ms->chks_in_height = mx_round_to_multiples(rows, chk_len) / chk_len;

    ms->alignment = ms->val_sz * ms->pck_len;
    ms->buf = malloc(ms->bytes + ms->alignment);
    if (! ms->buf) {
        return NULL;
    } /* if */
    
    /* NOTE: Align to the vector type's size, otherwise it will be segmentation fault when access to packs. */
    ms->data = (void *)( ((size_t)ms->buf + (ms->alignment - 1)) & (~(ms->alignment - 1)));
    return ms;
} // mstr_init

void mstr_clean(mx_stor_ptr ms)
{
    free(ms->buf);
} // mstr_clean

mx_stor_ptr mstr_create(uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t pck_len, uint32_t chk_len)
{
    uint32_t i = 0;
    mx_stor_ptr ms = NULL;

    ms = calloc(sizeof(mx_stor_t), 1);
    if (! ms) {
        return NULL;
    } // if

    if (! mstr_init(ms, rows, cols, val_sz, pck_len, chk_len)) {
        free(ms);
        return NULL;
    } // if
    return ms;
} // mstr_create

void mstr_destroy(mx_stor_ptr ms)
{
    if (ms) {
        mstr_clean(ms);
        free(ms);
    } // if
} // mstr_destroy

void mstr_calibrate_index(mx_stor_ptr ms, uint32_t * row, uint32_t * col, int32_t * row_off, int32_t * col_off)
{
    // Calibrate the row index.
    if (*row_off < 0 && *row < abs(*row_off)) {
        // The calibrated row index is beyond the TOP boundary.
        *row_off += *row;
        *row = 0;
    } else {
        *row += *row_off;
        *row_off = 0;
    } // if

    // Calibrate the column index.
    if (*col_off < 0 && *col < abs(*col_off)) {
        // The actual column index is beyond the LEFT boundary.
        *col_off += *col;
        *col = 0;
    } else {
        *col += *col_off;
        *col_off = 0;
    } // if

    assert(*row_off <= 0);
    assert(*col_off <= 0);
} // mstr_calibrate_index

void mstr_transfer_row_vector(mx_stor_ptr ms, uint32_t row, uint32_t col, uint32_t off, uint32_t dir, void * vec)
{
    void * addr[2] = {NULL, NULL};
    void * base = NULL;
    uint32_t base_ridx = 0;
    uint32_t base_cidx = 0;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;
    uint32_t rmd = 0;
    uint32_t cnt = ms->pck_len - off;

    base_ridx = row & (~(ms->chk_len - 1)); // Refer to values.
    base_cidx = col & (~(ms->chk_len - 1)); // Refer to values.
    rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - base_cidx);

    base = mstr_calc_base(ms, base_ridx, base_cidx, rows_in_chk);
    rmd = cols_in_chk - (col - base_cidx);

    addr[0] = vec + ms->val_sz * off;
    addr[1] = base + ms->val_sz * ((row - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk) + (col - base_cidx));

    memcpy(addr[dir], addr[!dir], ms->val_sz * (rmd < cnt ? rmd : cnt));
    if ((rmd >= cnt) || (col += rmd) >= ms->cols) {
        return;
    } // if

    // The previous chunk is full, then the copying action will cross the boundary.
    assert(col == base_cidx + cols_in_chk);

    cnt -= rmd;
    off += rmd;

    // col must be equal to the base_cidx of the right adjacent chunk.
    cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - col);

    addr[0] = vec + ms->val_sz * off;
    addr[1] = base + ms->val_sz * ms->chk_len * ms->chk_len + ms->val_sz * (row - base_ridx) * mx_round_to_multiples_of_8(cols_in_chk);

    memcpy(addr[dir], addr[!dir], ms->val_sz * cnt);
} // mstr_transfer_row_vector
