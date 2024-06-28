#include <criterion/criterion.h>

#ifndef MX_STOR_C
#include "src/mx_storage.c"
#define MX_STOR_C
#endif

#define check_vals_sz(ms, expect) \
    cr_expect(ms->val_sz == expect, "Wrong value size - Expect %d, got %d.", expect, ms->val_sz)

#define check_rows(ms, expect) \
    cr_expect(ms->rows == expect, "Wrong number of rows - Expect %d, got %d.", expect, ms->rows)

#define check_columns(ms, expect) \
    cr_expect(ms->cols == expect, "Wrong number of columns - Expect %d, got %d.", expect, ms->cols)

#define check_padded_columns(ms, expect) \
    cr_expect(ms->cols_padded == expect, "Wrong number of padded columns - Expect %d, got %d.", expect, ms->cols_padded)

#define check_allocated_bytes(ms, expect) \
    cr_expect(ms->bytes == expect, "Wrong size of allocated bytes of the buffer - Expect %d, got %d.", expect, ms->bytes)

#define check_pack_length(ms, expect) \
    cr_expect(ms->pck_len == expect, "Wrong pack length - Expect %d, got %d.", expect, ms->pck_len)

#define check_chunk_length(ms, expect) \
    cr_expect(ms->chk_len == expect, "Wrong chunk length - Expect %d, got %d.", expect, ms->chk_len)

#define check_last_chunk_width(ms, expect) \
    cr_expect(ms->last_chk_width == expect, "Wrong width of the last chunk - Expect %d, got %d.", expect, ms->last_chk_width)

#define check_last_chunk_height(ms, expect) \
    cr_expect(ms->last_chk_height == expect, "Wrong height of the last chunk - Expect %d, got %d.", expect, ms->last_chk_height)

#define check_chunks_in_width(ms, expect) \
    cr_expect(ms->chks_in_width == expect, "Wrong number of chunks in width - Expect %d, got %d.", expect, ms->chks_in_width)

#define check_chunks_in_height(ms, expect) \
    cr_expect(ms->chks_in_height == expect, "Wrong number of chunks in height - Expect %d, got %d.", expect, ms->chks_in_height)

#define check_alignment(ms, expect) \
    cr_expect(ms->alignment == expect, "Wrong alignment boundary - Expect %d, got %d.", expect, ms->alignment)

#define check_aligned_buffer(ms, mask, expect) \
    cr_expect(((size_t)ms->data & mask) == expect, "The buffer is not aligned correctly - Expect %d, got %d.", expect, ((size_t)ms->data & mask))

#define check_base(ms, base, off) \
    cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

#define check_rows_in_chunk(rows_in_chk, expect) \
    cr_expect(rows_in_chk == expect, "Wrong number of rows in the chunk - Expect %d, got %d.", expect, rows_in_chk)

#define check_columns_in_chunk(cols_in_chk, expect) \
    cr_expect(cols_in_chk == expect, "Wrong number of columns in the chunk - Expect %d, got %d.", expect, cols_in_chk)

#define check_full_flag(full, expect) \
    cr_expect(full == expect, "Wrong full flag of the chunk - Expect %d, got %d.", expect, full)

#define check_value(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %d, got %d.", expect, val)

#define check_value_at(val, expect, i, j) \
    cr_expect(val == expect, "Wrong value at (%d,%d) - Expect %d, got %d.", i, j, expect, val)

#define check_equal_poitners(val, expect) \
    cr_expect(val == expect, "Should be equal pointers - Expect %p, got %p.", expect, val)

#define check_not_equal_poitners(val, expect) \
    cr_expect(val != expect, "Should be not equal pointers - Expect %p, got %p.", expect, val)

// ==== TESTS FOR THE STORAGE MODULE OF MATRIX ==== //

Test(Creat, v8si_create)
{
    mx_stor_ptr ms = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        check_vals_sz(ms, 4);
        check_rows(ms, 1);
        check_columns(ms, 1);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 1 * 8);
        check_pack_length(ms, 8);
        check_chunk_length(ms, 16);
        check_last_chunk_width(ms, 1);
        check_last_chunk_height(ms, 1);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);
        check_alignment(ms, 32);
        check_aligned_buffer(ms, 0x1F, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 7x7 matrix -- //
    {
        ms = mstr_v8si_create(7, 7);

        check_rows(ms, 7);
        check_columns(ms, 7);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 7 * 8);
        check_last_chunk_width(ms, 7);
        check_last_chunk_height(ms, 7);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        check_rows(ms, 8);
        check_columns(ms, 8);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 8 * 8);
        check_last_chunk_width(ms, 8);
        check_last_chunk_height(ms, 8);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);

        check_rows(ms, 9);
        check_columns(ms, 9);
        check_padded_columns(ms, 16);
        check_allocated_bytes(ms, 4 * 9 * 16);
        check_last_chunk_width(ms, 9);
        check_last_chunk_height(ms, 9);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_v8si_create(15, 15);

        check_rows(ms, 15);
        check_columns(ms, 15);
        check_padded_columns(ms, 16);
        check_allocated_bytes(ms, 4 * 15 * 16);
        check_last_chunk_width(ms, 15);
        check_last_chunk_height(ms, 15);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        check_rows(ms, 16);
        check_columns(ms, 16);
        check_padded_columns(ms, 16);
        check_allocated_bytes(ms, 4 * 16 * 16);
        check_last_chunk_width(ms, 16);
        check_last_chunk_height(ms, 16);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        check_rows(ms, 17);
        check_columns(ms, 17);
        check_padded_columns(ms, 24);
        check_allocated_bytes(ms, 4 * 17 * 24);
        check_last_chunk_width(ms, 1);
        check_last_chunk_height(ms, 1);
        check_chunks_in_width(ms, 2);
        check_chunks_in_height(ms, 2);

        mstr_v8si_destroy(ms);
    }

    // -- 2x7 matrix -- //
    {
        ms = mstr_v8si_create(2, 7);

        check_rows(ms, 2);
        check_columns(ms, 7);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 2 * 8);
        check_last_chunk_width(ms, 7);
        check_last_chunk_height(ms, 2);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 2x8 matrix -- //
    {
        ms = mstr_v8si_create(2, 8);

        check_rows(ms, 2);
        check_columns(ms, 8);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 2 * 8);
        check_last_chunk_width(ms, 8);
        check_last_chunk_height(ms, 2);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 2x9 matrix -- //
    {
        ms = mstr_v8si_create(2, 9);

        check_rows(ms, 2);
        check_columns(ms, 9);
        check_padded_columns(ms, 16);
        check_allocated_bytes(ms, 4 * 2 * 16);
        check_last_chunk_width(ms, 9);
        check_last_chunk_height(ms, 2);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 2x15 matrix -- //
    {
        ms = mstr_v8si_create(2, 15);

        check_rows(ms, 2);
        check_columns(ms, 15);
        check_padded_columns(ms, 16);
        check_allocated_bytes(ms, 4 * 2 * 16);
        check_last_chunk_width(ms, 15);
        check_last_chunk_height(ms, 2);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 2x16 matrix -- //
    {
        ms = mstr_v8si_create(2, 16);

        check_rows(ms, 2);
        check_columns(ms, 16);
        check_padded_columns(ms, 16);
        check_allocated_bytes(ms, 4 * 2 * 16);
        check_last_chunk_width(ms, 16);
        check_last_chunk_height(ms, 2);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 2x17 matrix -- //
    {
        ms = mstr_v8si_create(2, 17);

        check_rows(ms, 2);
        check_columns(ms, 17);
        check_padded_columns(ms, 24);
        check_allocated_bytes(ms, 4 * 2 * 24);
        check_last_chunk_width(ms, 1);
        check_last_chunk_height(ms, 2);
        check_chunks_in_width(ms, 2);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 7x2 matrix -- //
    {
        ms = mstr_v8si_create(7, 2);

        check_rows(ms, 7);
        check_columns(ms, 2);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 7 * 8);
        check_last_chunk_width(ms, 2);
        check_last_chunk_height(ms, 7);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 8x2 matrix -- //
    {
        ms = mstr_v8si_create(8, 2);

        check_rows(ms, 8);
        check_columns(ms, 2);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 8 * 8);
        check_last_chunk_width(ms, 2);
        check_last_chunk_height(ms, 8);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 9x2 matrix -- //
    {
        ms = mstr_v8si_create(9, 2);

        check_rows(ms, 9);
        check_columns(ms, 2);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 9 * 8);
        check_last_chunk_width(ms, 2);
        check_last_chunk_height(ms, 9);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 15x2 matrix -- //
    {
        ms = mstr_v8si_create(15, 2);

        check_rows(ms, 15);
        check_columns(ms, 2);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 15 * 8);
        check_last_chunk_width(ms, 2);
        check_last_chunk_height(ms, 15);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 16x2 matrix -- //
    {
        ms = mstr_v8si_create(16, 2);

        check_rows(ms, 16);
        check_columns(ms, 2);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 16 * 8);
        check_last_chunk_width(ms, 2);
        check_last_chunk_height(ms, 16);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 1);

        mstr_v8si_destroy(ms);
    }

    // -- 17x2 matrix -- //
    {
        ms = mstr_v8si_create(17, 2);

        check_rows(ms, 17);
        check_columns(ms, 2);
        check_padded_columns(ms, 8);
        check_allocated_bytes(ms, 4 * 17 * 8);
        check_last_chunk_width(ms, 2);
        check_last_chunk_height(ms, 1);
        check_chunks_in_width(ms, 1);
        check_chunks_in_height(ms, 2);

        mstr_v8si_destroy(ms);
    }
}

Test(Locate, v8si_calc_base)
{
    mx_stor_ptr ms = NULL;
    void * base = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        base = v8si_calc_base(ms, 0, 0, ms->last_chk_height);
        check_base(ms, base, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        base = v8si_calc_base(ms, 0, 0, ms->last_chk_height);
        check_base(ms, base, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_v8si_create(15, 15);

        base = v8si_calc_base(ms, 0, 0, ms->last_chk_height);
        check_base(ms, base, 0);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        base = v8si_calc_base(ms, 0, 0, ms->last_chk_height);
        check_base(ms, base, 0);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        base = v8si_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = v8si_calc_base(ms, 0, 16, 16);
        check_base(ms, base, 4 * 16 * 16);

        base = v8si_calc_base(ms, 16, 0, ms->last_chk_height);
        check_base(ms, base, 4 * 16 * 16 + 4 * 16 * 8);

        base = v8si_calc_base(ms, 16, 16, ms->last_chk_height);
        check_base(ms, base, 4 * 16 * 24 + 4 * 1 * 16);

        mstr_v8si_destroy(ms);
    }

    // -- 2x17 matrix -- //
    {
        ms = mstr_v8si_create(2, 17);

        base = v8si_calc_base(ms, 0, 0, 2);
        check_base(ms, base, 0);

        base = v8si_calc_base(ms, 0, 16, ms->last_chk_height);
        check_base(ms, base, 4 * 2 * 16);

        mstr_v8si_destroy(ms);
    }

    // -- 2x33 matrix -- //
    {
        ms = mstr_v8si_create(2, 33);

        base = v8si_calc_base(ms, 0, 0, 2);
        check_base(ms, base, 0);

        base = v8si_calc_base(ms, 0, 16, 2);
        check_base(ms, base, 4 * 2 * 16);

        base = v8si_calc_base(ms, 0, 32, ms->last_chk_height);
        check_base(ms, base, 4 * 2 * 16 * 2);

        mstr_v8si_destroy(ms);
    }

    // -- 17x33 matrix -- //
    {
        ms = mstr_v8si_create(17, 33);

        base = v8si_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = v8si_calc_base(ms, 0, 16, 16);
        check_base(ms, base, 4 * 16 * 16);

        base = v8si_calc_base(ms, 0, 32, 16);
        check_base(ms, base, 4 * 16 * 16 * 2);

        base = v8si_calc_base(ms, 16, 0, ms->last_chk_height);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8);
        
        base = v8si_calc_base(ms, 16, 16, ms->last_chk_height);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8 + 4 * 1 * 16);

        base = v8si_calc_base(ms, 16, 32, ms->last_chk_height);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8 + 4 * 1 * 16 * 2);

        mstr_v8si_destroy(ms);
    }

    // -- 17x2 matrix -- //
    {
        ms = mstr_v8si_create(17, 2);

        base = v8si_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = v8si_calc_base(ms, 16, 0, ms->last_chk_height);
        check_base(ms, base, 4 * 16 * 8);

        mstr_v8si_destroy(ms);
    }

    // -- 33x2 matrix -- //
    {
        ms = mstr_v8si_create(33, 2);

        base = v8si_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = v8si_calc_base(ms, 16, 0, 16);
        check_base(ms, base, 4 * 16 * 8);

        base = v8si_calc_base(ms, 32, 0, ms->last_chk_height);
        check_base(ms, base, 4 * 32 * 8);

        mstr_v8si_destroy(ms);
    }

    // -- 33x17 matrix -- //
    {
        ms = mstr_v8si_create(33, 17);

        base = v8si_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = v8si_calc_base(ms, 0, 16, 16);
        check_base(ms, base, 4 * 16 * 16);

        base = v8si_calc_base(ms, 16, 0, 16);
        check_base(ms, base, 4 * 16 * 16 + 4 * 16 * 8);

        base = v8si_calc_base(ms, 16, 16, 16);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8);

        base = v8si_calc_base(ms, 32, 0, ms->last_chk_height);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8 * 2);

        base = v8si_calc_base(ms, 32, 16, ms->last_chk_height);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8 * 2 + 4 * 1 * 16);
    }
}

Test(Locate, mstr_v8si_locate_chunk)
{
    mx_stor_ptr ms = NULL;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;;
    bool full = false;
    void * chk = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        chk = mstr_v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 1);
        check_full_flag(full, false);

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        chk = mstr_v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 8);
        check_columns_in_chunk(cols_in_chk, 8);
        check_full_flag(full, false);

        mstr_v8si_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_v8si_create(15, 15);

        chk = mstr_v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 15);
        check_columns_in_chunk(cols_in_chk, 15);
        check_full_flag(full, false);

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        chk = mstr_v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 16);
        check_full_flag(full, true);

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        chk = mstr_v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 16);
        check_full_flag(full, true);

        chk = mstr_v8si_locate_chunk(ms, 0, 1, &rows_in_chk, &cols_in_chk, &full);
        check_base(ms, chk, 4 * 16 * 16);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 1);
        check_full_flag(full, false);

        chk = mstr_v8si_locate_chunk(ms, 1, 0, &rows_in_chk, &cols_in_chk, &full);
        check_base(ms, chk, 4 * 16 * 16 + 4 * 16 * 8);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 16);
        check_full_flag(full, false);

        chk = mstr_v8si_locate_chunk(ms, 1, 1, &rows_in_chk, &cols_in_chk, &full);
        check_base(ms, chk, 4 * 16 * 16 + 4 * 16 * 8 + 4 * 1 * 16);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 1);
        check_full_flag(full, false);

        mstr_v8si_destroy(ms);
    }
}

Test(GetterSetter, v8si_get)
{
    mx_stor_ptr ms = NULL;
    int32_t * base = NULL;
    int32_t val = 0;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);
        base = ms->data;

        base[0 * 8 + 0] = 4567;
        val = mstr_v8si_get(ms, 0, 0);
        check_value(val, 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_v8si_create(5, 5);
        base = ms->data;

        base[4 * 8 + 4] = 4567;
        val = mstr_v8si_get(ms, 4, 4);
        check_value(val, 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);
        base = ms->data;

        base[7 * 8] = 4567;
        val = mstr_v8si_get(ms, 7, 0);
        check_value(val, 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);
        base = ms->data;

        base[8] = 4567;
        val = mstr_v8si_get(ms, 0, 8);
        check_value(val, 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);
        base = ms->data;

        base[15 * 16 + 15] = 4567;
        val = mstr_v8si_get(ms, 15, 15);
        check_value(val, 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);
        base = ms->data;

        base[16 * 16 + 0 + 0] = 9876;
        val = mstr_v8si_get(ms, 0, 16);
        check_value(val, 9876);

        base[16 * 16 + 16 * 8 + 0] = 1234;
        val = mstr_v8si_get(ms, 16, 0);
        check_value(val, 1234);

        base[16 * 16 + 16 * 8 + 1 * 16 + 0] = 4567;
        val = mstr_v8si_get(ms, 16, 16);
        check_value(val, 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 34x34 matrix -- //
    {
        ms = mstr_v8si_create(34, 34);
        base = ms->data;

        base[16 * 16 * 2 + 6 * 8 + 1] = 4567;
        val = mstr_v8si_get(ms, 6, 33);
        check_value(val, 4567);

        base[16 * 16 * 2 + 16 * 8 + 1 * 16 + 1] = 9876;
        val = mstr_v8si_get(ms, 17, 1);
        check_value(val, 9876);

        base[16 * 16 * 2 + 16 * 8 + 16 * 16 + 8 * 16 + 8] = 1234;
        val = mstr_v8si_get(ms, 24, 24);
        check_value(val, 1234);

        mstr_v8si_destroy(ms);
    }
}

Test(GetterSetter, v8si_set)
{
    mx_stor_ptr ms = NULL;
    int32_t * base = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);
        base = ms->data;

        mstr_v8si_set(ms, 0, 0, 4567);
        check_value(base[0 * 8 + 0], 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_v8si_create(5, 5);
        base = ms->data;

        mstr_v8si_set(ms, 4, 4, 4567);
        check_value(base[4 * 8 + 4], 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);
        base = ms->data;

        mstr_v8si_set(ms, 7, 0, 4567);
        check_value(base[7 * 8], 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);
        base = ms->data;

        mstr_v8si_set(ms, 0, 8, 4567);
        check_value(base[8], 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);
        base = ms->data;

        mstr_v8si_set(ms, 15, 15, 4567);
        check_value(base[15 * 16 + 15], 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);
        base = ms->data;

        mstr_v8si_set(ms, 0, 16, 9876);
        check_value(base[16 * 16 + 0 + 0], 9876);

        mstr_v8si_set(ms, 16, 0, 1234);
        check_value(base[16 * 16 + 16 * 8 + 0], 1234);

        mstr_v8si_set(ms, 16, 16, 4567);
        check_value(base[16 * 16 + 16 * 8 + 1 * 16 + 0], 4567);

        mstr_v8si_destroy(ms);
    }

    // -- 34x34 matrix -- //
    {
        ms = mstr_v8si_create(34, 34);
        base = ms->data;

        mstr_v8si_set(ms, 6, 33, 4567);
        check_value(base[16 * 16 * 2 + 6 * 8 + 1], 4567);

        mstr_v8si_set(ms, 17, 1, 9876);
        check_value(base[16 * 16 * 2 + 16 * 8 + 1 * 16 + 1], 9876);

        mstr_v8si_set(ms, 24, 24, 1234);
        check_value(base[16 * 16 * 2 + 16 * 8 + 16 * 16 + 8 * 16 + 8], 1234);

        mstr_v8si_destroy(ms);
    }
}

Test(Initialization, v8si_init_zeros)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr ms = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        mstr_v8si_init_zeros(ms);
        check_value_at(mstr_v8si_get(ms, 0, 0), 0, 0, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_v8si_create(5, 5);

        mstr_v8si_init_zeros(ms);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        mstr_v8si_init_zeros(ms);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);

        mstr_v8si_init_zeros(ms);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        mstr_v8si_init_zeros(ms);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        mstr_v8si_init_zeros(ms);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 33x33 matrix -- //
    {
        ms = mstr_v8si_create(33, 33);

        mstr_v8si_init_zeros(ms);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }
}

Test(Initialization, v8si_init_identity)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr ms = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        mstr_v8si_init_identity(ms);
        check_value_at(mstr_v8si_get(ms, 0, 0), 1, 0, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_v8si_create(5, 5);

        mstr_v8si_init_identity(ms);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                if (i == j) {
                    check_value_at(mstr_v8si_get(ms, i, j), 1, i, j);
                } else {
                    check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
                } // if
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        mstr_v8si_init_identity(ms);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                if (i == j) {
                    check_value_at(mstr_v8si_get(ms, i, j), 1, i, j);
                } else {
                    check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
                } // if
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);

        mstr_v8si_init_identity(ms);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                if (i == j) {
                    check_value_at(mstr_v8si_get(ms, i, j), 1, i, j);
                } else {
                    check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
                } // if
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        mstr_v8si_init_identity(ms);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                if (i == j) {
                    check_value_at(mstr_v8si_get(ms, i, j), 1, i, j);
                } else {
                    check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
                } // if
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        mstr_v8si_init_identity(ms);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                if (i == j) {
                    check_value_at(mstr_v8si_get(ms, i, j), 1, i, j);
                } else {
                    check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
                } // if
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 33x33 matrix -- //
    {
        ms = mstr_v8si_create(33, 33);

        mstr_v8si_init_identity(ms);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 33; j += 1) {
                if (i == j) {
                    check_value_at(mstr_v8si_get(ms, i, j), 1, i, j);
                } else {
                    check_value_at(mstr_v8si_get(ms, i, j), 0, i, j);
                } // if
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }
}

Test(Initialization, v8si_fill)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr ms = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        mstr_v8si_fill(ms, 4567);
        check_value_at(mstr_v8si_get(ms, 0, 0), 4567, 0, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_v8si_create(5, 5);

        mstr_v8si_fill(ms, 4567);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 4567, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        mstr_v8si_fill(ms, 4567);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 4567, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);

        mstr_v8si_fill(ms, 4567);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 4567, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        mstr_v8si_fill(ms, 4567);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 4567, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        mstr_v8si_fill(ms, 4567);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 4567, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 33x33 matrix -- //
    {
        ms = mstr_v8si_create(33, 33);

        mstr_v8si_fill(ms, 4567);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), 4567, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }
}

Test(Operation, mstr_v8si_store_chunk)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr ms = NULL;
    mx_chunk_t src = {
        .i32_vals = {
            {  0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15},
            { 16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31},
            { 32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47},
            { 48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63},
            { 64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79},
            { 80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95},
            { 96,   97,   98,   99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111},
            {112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127},
            {128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143},
            {144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159},
            {160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  170,  171,  172,  173,  174,  175},
            {176,  177,  178,  179,  180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  190,  191},
            {192,  193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207},
            {208,  209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,  221,  222,  223},
            {224,  225,  226,  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239},
            {240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,  255}
        }
    };

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        mstr_v8si_init_zeros(ms);
        mstr_v8si_store_chunk(ms, 0, 0, &src);
        check_value_at(mstr_v8si_get(ms, 0, 0), 0, 0, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_v8si_create(5, 5);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        mstr_v8si_init_zeros(ms);
        mstr_v8si_store_chunk(ms, 0, 0, &src);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), src.i32_vals[i][j], i, j);
            } // for
        } // for

        check_value_at(mstr_v8si_get(ms, 0, 16), 0, 0, 16);
        check_value_at(mstr_v8si_get(ms, 16, 0), 0, 16, 0);
        check_value_at(mstr_v8si_get(ms, 16, 16), 0, 16, 16);

        mstr_v8si_destroy(ms);
    }

    // -- 2x15 matrix -- //
    {
        ms = mstr_v8si_create(2, 15);

        mstr_v8si_init_zeros(ms);
        mstr_v8si_store_chunk(ms, 0, 0, &src);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 15; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 15x2 matrix -- //
    {
        ms = mstr_v8si_create(15, 2);

        mstr_v8si_init_zeros(ms);
        mstr_v8si_store_chunk(ms, 0, 0, &src);
        for (i = 0; i < 15; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mstr_v8si_get(ms, i, j), src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }
}

Test(Operation, mstr_v8si_copy_chunk)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;
    bool full = false;
    mx_stor_ptr ms = NULL;
    mx_chunk_ptr ret = NULL;
    mx_chunk_t src = {
        .i32_vals = {
            {  0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15},
            { 16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31},
            { 32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47},
            { 48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63},
            { 64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79},
            { 80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95},
            { 96,   97,   98,   99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111},
            {112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127},
            {128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143},
            {144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159},
            {160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  170,  171,  172,  173,  174,  175},
            {176,  177,  178,  179,  180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  190,  191},
            {192,  193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207},
            {208,  209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,  221,  222,  223},
            {224,  225,  226,  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239},
            {240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,  255}
        }
    };
    mx_chunk_t dst;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_copy_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 1);
        check_full_flag(full, false);
        check_value_at(dst.i32_vals[0][0], 0, 0, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_v8si_create(5, 5);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_copy_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 5);
        check_columns_in_chunk(cols_in_chk, 5);
        check_full_flag(full, false);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_copy_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 8);
        check_columns_in_chunk(cols_in_chk, 8);
        check_full_flag(full, false);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_copy_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 9);
        check_columns_in_chunk(cols_in_chk, 9);
        check_full_flag(full, false);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_copy_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, v8si_calc_base(ms, 0, 0, 16));
        check_not_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 16);
        check_full_flag(full, true);

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 4321);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_copy_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, v8si_calc_base(ms, 0, 0, 16));
        check_not_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 16);
        check_full_flag(full, true);

        ret = mstr_v8si_copy_chunk(ms, 0, 1, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 1);
        check_full_flag(full, false);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(dst.i32_vals[i][j], 4321, i, j);
            } // for
        } // for

        ret = mstr_v8si_copy_chunk(ms, 1, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 16);
        check_full_flag(full, false);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(dst.i32_vals[i][j], 4321, i, j);
            } // for
        } // for

        ret = mstr_v8si_copy_chunk(ms, 1, 1, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 1);
        check_full_flag(full, false);
        check_value_at(dst.i32_vals[0][0], 4321, 0, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 2x5 matrix -- //
    {
        ms = mstr_v8si_create(2, 5);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_copy_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 2);
        check_columns_in_chunk(cols_in_chk, 5);
        check_full_flag(full, false);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 5x2 matrix -- //
    {
        ms = mstr_v8si_create(5, 2);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_copy_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 5);
        check_columns_in_chunk(cols_in_chk, 2);
        check_full_flag(full, false);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }
}

Test(Operation, mstr_v8si_transpose_chunk)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;
    bool full = false;
    mx_stor_ptr ms = NULL;
    mx_chunk_ptr ret = NULL;
    mx_chunk_t src = {
        .i32_vals = {
            {  0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15},
            { 16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31},
            { 32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47},
            { 48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63},
            { 64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79},
            { 80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95},
            { 96,   97,   98,   99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111},
            {112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127},
            {128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143},
            {144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159},
            {160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  170,  171,  172,  173,  174,  175},
            {176,  177,  178,  179,  180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  190,  191},
            {192,  193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207},
            {208,  209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,  221,  222,  223},
            {224,  225,  226,  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239},
            {240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,  255}
        }
    };
    mx_chunk_t dst;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_transpose_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 1);
        check_full_flag(full, false);
        check_value_at(dst.i32_vals[0][0], 0, 0, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_v8si_create(5, 5);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_transpose_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 5);
        check_columns_in_chunk(cols_in_chk, 5);
        check_full_flag(full, false);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[j][i], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_transpose_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 8);
        check_columns_in_chunk(cols_in_chk, 8);
        check_full_flag(full, false);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[j][i], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_transpose_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 9);
        check_columns_in_chunk(cols_in_chk, 9);
        check_full_flag(full, false);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[j][i], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_transpose_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 16);
        check_full_flag(full, true);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[j][i], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 4321);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_transpose_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 16);
        check_full_flag(full, true);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[j][i], i, j);
            } // for
        } // for

        ret = mstr_v8si_transpose_chunk(ms, 0, 1, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 16);
        check_full_flag(full, false);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(dst.i32_vals[i][j], 4321, i, j);
            } // for
        } // for

        ret = mstr_v8si_transpose_chunk(ms, 1, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 1);
        check_full_flag(full, false);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(dst.i32_vals[i][j], 4321, i, j);
            } // for
        } // for

        ret = mstr_v8si_transpose_chunk(ms, 1, 1, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 1);
        check_full_flag(full, false);
        check_value_at(dst.i32_vals[0][0], 4321, 0, 0);

        mstr_v8si_destroy(ms);
    }

    // -- 2x5 matrix -- //
    {
        ms = mstr_v8si_create(2, 5);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_transpose_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 5);
        check_columns_in_chunk(cols_in_chk, 2);
        check_full_flag(full, false);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[j][i], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }

    // -- 5x2 matrix -- //
    {
        ms = mstr_v8si_create(5, 2);

        rows_in_chk = 0;
        cols_in_chk = 0;
        full = false;
        memset(&dst, 0, sizeof(dst));
        mstr_v8si_fill(ms, 65535);

        mstr_v8si_store_chunk(ms, 0, 0, &src);
        ret = mstr_v8si_transpose_chunk(ms, 0, 0, &dst, &rows_in_chk, &cols_in_chk, &full);

        check_equal_poitners(ret, &dst);
        check_rows_in_chunk(rows_in_chk, 2);
        check_columns_in_chunk(cols_in_chk, 5);
        check_full_flag(full, false);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(dst.i32_vals[i][j], src.i32_vals[j][i], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ms);
    }
}
