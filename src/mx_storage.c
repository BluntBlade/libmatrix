#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <immintrin.h>

#include "src/mx_common.h"
#include "src/mx_storage.h"

mx_stor_ptr mstr_init(mx_stor_ptr ms, uint32_t rows, uint32_t cols, uint32_t val_sz, uint32_t pck_len, uint32_t chk_len){
    ms->val_sz = val_sz;
    ms->rows = rows;
    ms->cols = cols;
    ms->cols_padded = mx_ceil_to_multiples(cols, pck_len);
    ms->bytes = ms->val_sz * ms->rows * ms->cols_padded;
    ms->pck_len = pck_len;
    ms->chk_len = chk_len;
    ms->chks_in_width = mx_ceil_to_multiples(cols, chk_len) / chk_len;
    ms->chks_in_height = mx_ceil_to_multiples(rows, chk_len) / chk_len;

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
    addr[1] = base + ms->val_sz * ((row - base_ridx) * mx_ceil_to_multiples_of_8(cols_in_chk) + (col - base_cidx));

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
    addr[1] = base + ms->val_sz * ms->chk_len * ms->chk_len + ms->val_sz * (row - base_ridx) * mx_ceil_to_multiples_of_8(cols_in_chk);

    memcpy(addr[dir], addr[!dir], ms->val_sz * cnt);
} // mstr_transfer_row_vector

void mstr_transfer_column_vector(mx_stor_ptr ms, uint32_t row, uint32_t col, uint32_t off, uint32_t dir, void * vec)
{
    void * addr[2];
    void * base;
    uint32_t base_ridx;
    uint32_t base_cidx;
    uint32_t rows_in_chk;
    uint32_t cols_in_chk;
    uint32_t cnt;

    addr[0] = vec + ms->val_sz * off;

    base_ridx = row & (~(ms->chk_len - 1)); // Refer to values.
    base_cidx = col & (~(ms->chk_len - 1)); // Refer to values.
    rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    cols_in_chk = mx_ceil_to_or_less_than_16(ms->cols - base_cidx);

    cnt = ms->pck_len - off;
    if (row + cnt >= base_ridx + rows_in_chk) {
        cnt = base_ridx + rows_in_chk - row;
    } // if

    do {
        base = mstr_calc_base(ms, base_ridx, base_cidx, rows_in_chk);
        addr[1] = base + ms->val_sz * ((row - base_ridx) * mx_ceil_to_multiples_of_8(cols_in_chk) + (col - base_cidx));

        switch (cnt) {
            default: assert(1); break;
            case 16: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case 15: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case 14: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case 13: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case 12: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case 11: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case 10: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case  9: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case  8: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case  7: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case  6: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case  5: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case  4: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case  3: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case  2: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
            case  1: memcpy(addr[dir], addr[!dir], ms->val_sz); addr[0] += ms->val_sz; addr[1] += ms->val_sz * ms->chk_len;
        } // switch

        base_ridx = (row += cnt);
        rows_in_chk = mx_ceil_to_or_less_than_16(ms->rows - base_ridx);
    } while ((cnt = ms->pck_len - off - cnt) > 0);
} // mstr_transfer_column_vector

static bool next_position_in_row(mx_iter_ptr itr, uint32_t step)
{
    assert(itr->row < itr->row_end);
    assert(itr->col < itr->col_end);

    if ((itr->col + step) >= itr->col_end) {
        if (++itr->row >= itr->row_end) {
            itr->col = itr->col_end;
            return false;
        } // if
        itr->col = itr->col_begin;
    } else {
        itr->col += step;
    } // if
    return true;
} // next_position_in_row

static bool next_position_in_column(mx_iter_ptr itr, uint32_t step)
{
    assert(itr->row < itr->row_end);
    assert(itr->col < itr->col_end);

    if ((itr->row + step) >= itr->row_end) {
        if (++itr->col >= itr->col_end) {
            itr->row = itr->row_end;
            return false;
        } // if
        itr->row = itr->row_begin;
    } else {
        itr->row += step;
    } // if
    return true;
} // next_position_in_column

static void transfer_vectors_in_row(mx_iter_ptr itr, uint32_t dir, uint32_t vn, void * vec, va_list * args);
static void transfer_vectors_in_column(mx_iter_ptr itr, uint32_t dir, uint32_t vn, void * vec, va_list * args);

void mitr_init_for_iterating_in_rows(mx_iter_ptr itr, mx_stor_ptr ms, uint32_t row_begin, uint32_t col_begin, uint32_t row_end, uint32_t col_end)
{
    itr->ms = ms;

    itr->row_begin = row_begin;
    itr->col_begin = col_begin;
    itr->row_end = row_end;
    itr->col_end = col_end;

    itr->next = &next_position_in_row;
    itr->transfer = &transfer_vectors_in_row;

    itr->row = row_begin;
    itr->col = col_begin;

    memset(&itr->dval, 0, sizeof(itr->dval));
} // mitr_init_for_iterating_in_rows

void mitr_init_for_iterating_in_columns(mx_iter_ptr itr, mx_stor_ptr ms, uint32_t row_begin, uint32_t col_begin, uint32_t row_end, uint32_t col_end)
{
    itr->ms = ms;

    itr->row_begin = row_begin;
    itr->col_begin = col_begin;
    itr->row_end = row_end;
    itr->col_end = col_end;

    itr->next = &next_position_in_column;
    itr->transfer = &transfer_vectors_in_column;

    itr->row = row_begin;
    itr->col = col_begin;

    memset(&itr->dval, 0, sizeof(itr->dval));
} // mitr_init_for_iterating_in_columns

void mitr_init_for_iterating_in_chunk(mx_iter_ptr itr, mx_stor_ptr ms, uint32_t chk_ridx, uint32_t chk_cidx)
{
    itr->ms = ms;

    itr->row_begin = chk_ridx * ms->chk_len;
    itr->col_begin = chk_cidx * ms->chk_len;
    itr->row_end = itr->row_begin + ms->chk_len;
    itr->col_end = itr->col_begin + ms->chk_len;

    itr->next = &next_position_in_row;
    itr->transfer = &transfer_vectors_in_row;

    itr->row = itr->row_begin;
    itr->col = itr->col_begin;

    memset(&itr->dval, 0, sizeof(itr->dval));
} // mitr_init_for_iterating_in_chunk

void mitr_set_default_i32(mx_iter_ptr itr, int32_t dval)
{
    mx_type_reg(itr->dval.i32) = _mm256_set1_epi32(dval);
} // mitr_set_default_i32

void mitr_set_default_f32(mx_iter_ptr itr, float dval)
{
    mx_type_reg(itr->dval.f32) = _mm256_set1_ps(dval);
} // mitr_set_default_f32

static void set_default_values(mx_iter_ptr itr, uint32_t vi, uint32_t vk, void * val)
{
    uint32_t i = vi / 8;
    size_t bytes = itr->ms->val_sz * itr->ms->pck_len;

    switch (vi % 8) {
        default: assert(1); break;
        case 8: do {
                    memcpy(val, &itr->dval, bytes); val += bytes;
        case 7:     memcpy(val, &itr->dval, bytes); val += bytes;
        case 6:     memcpy(val, &itr->dval, bytes); val += bytes;
        case 5:     memcpy(val, &itr->dval, bytes); val += bytes;
        case 4:     memcpy(val, &itr->dval, bytes); val += bytes;
        case 3:     memcpy(val, &itr->dval, bytes); val += bytes;
        case 2:     memcpy(val, &itr->dval, bytes); val += bytes;
        case 1:     memcpy(val, &itr->dval, bytes); val += bytes;
                } while (i-- > 0);
    } // switch

    memcpy(val, &itr->dval, itr->ms->val_sz * vk);
} // set_default_values

static void transfer_values(mx_iter_ptr itr, uint32_t dir, uint32_t vn, void * val, va_list * args)
{
    va_list off_pairs;
    void * addr[2];

    if (dir == MSTR_LOAD_VALUES) {
        set_default_values(itr, (vn / itr->ms->pck_len), (vn % itr->ms->pck_len), val);
    } // if

    va_copy(off_pairs, *args);
    addr[0] = val;
    for (uint32_t i = 0; i < vn; i += 1) {
        uint32_t row = itr->row;
        uint32_t col = itr->col;
        int32_t row_off = va_arg(off_pairs, int32_t);
        int32_t col_off = va_arg(off_pairs, int32_t);

        mstr_calibrate_index(itr->ms, &row, &col, &row_off, &col_off);
        if (row_off == 0 && col_off == 0 && row < itr->row_end && col < itr->col_end) {
            uint32_t row_diff = row - mx_floor_to_multiples_of(row, itr->ms->chk_len);
            uint32_t col_diff = col - mx_floor_to_multiples_of(col, itr->ms->chk_len);
            uint32_t rows_in_chk;
            uint32_t cols_in_chk;
            addr[1] = mstr_locate_chunk_by_value_index(itr->ms, row, col, &rows_in_chk, &cols_in_chk);
            addr[1] += itr->ms->val_sz * (row_diff * mx_ceil_to_multiples(cols_in_chk, itr->ms->pck_len) + col_diff);
            memcpy(addr[dir], addr[!dir], itr->ms->val_sz);
        } // if

        addr[0] += itr->ms->val_sz;
    } // for
    va_end(off_pairs);
} // transfer_values

bool mitr_get_values(mx_iter_ptr itr, bool move, uint32_t vn, void * val, ...)
{
    va_list args;

    va_start(args, val);
    transfer_values(itr, MSTR_LOAD_VALUES, vn, val, &args);
    va_end(args);

    if (move) {
        return itr->next(itr, 1);
    } // if
    return true;
} // mitr_get_values

bool mitr_set_values(mx_iter_ptr itr, bool move, uint32_t vn, void * val, ...)
{
    va_list args;

    va_start(args, val);
    transfer_values(itr, MSTR_STORE_VALUES, vn, val, &args);
    va_end(args);

    if (move) {
        return itr->next(itr, 1);
    } // if
    return true;
} // mitr_set_values

static void transfer_vectors_in_row(mx_iter_ptr itr, uint32_t dir, uint32_t vn, void * vec, va_list * args)
{
    va_list args2;

    if (dir == MSTR_LOAD_VECTOR) {
        set_default_values(itr, vn, 0, vec);
    } // if

    va_copy(args2, *args);
    for (uint32_t i = 0; i < vn; i += 1) {
        uint32_t row = itr->row;
        uint32_t col = itr->col;
        int32_t row_off = va_arg(args2, int32_t);
        int32_t col_off = va_arg(args2, int32_t);

        mstr_calibrate_index(itr->ms, &row, &col, &row_off, &col_off);
        if (abs(col_off) <= itr->ms->pck_len) {
            mstr_transfer_row_vector(itr->ms, row, col, abs(col_off), dir, vec);
        } // if

        vec += itr->ms->val_sz * itr->ms->pck_len;
    } // for
    va_end(args2);
} // transfer_vectors_in_row

static void transfer_vectors_in_column(mx_iter_ptr itr, uint32_t dir, uint32_t vn, void * vec, va_list * args)
{
    va_list args2;

    if (dir == MSTR_LOAD_VECTOR) {
        set_default_values(itr, vn, 0, vec);
    } // if

    va_copy(args2, *args);
    for (uint32_t i = 0; i < vn; i += 1) {
        uint32_t row = itr->row;
        uint32_t col = itr->col;
        int32_t row_off = va_arg(args2, int32_t);
        int32_t col_off = va_arg(args2, int32_t);

        mstr_calibrate_index(itr->ms, &row, &col, &row_off, &col_off);
        if (abs(row_off) <= itr->ms->pck_len) {
            mstr_transfer_column_vector(itr->ms, row, col, abs(row_off), dir, vec);
        } // if

        vec += itr->ms->val_sz * itr->ms->pck_len;
    } // for
    va_end(args2);
} // transfer_vectors_in_column
