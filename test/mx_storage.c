#include <criterion/criterion.h>

#include "src/mx_storage.h"

#define check_base(ms, base, off) \
    cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

#define check_rows_in_chunk(rows_in_chk, expect) \
    cr_expect(rows_in_chk == expect, "Wrong number of rows in the chunk - Expect %d, got %d.", expect, rows_in_chk)

#define check_columns_in_chunk(cols_in_chk, expect) \
    cr_expect(cols_in_chk == expect, "Wrong number of columns in the chunk - Expect %d, got %d.", expect, cols_in_chk)

Test(Locate, mstr_calc_base)
{
    mx_stor_ptr ms = NULL;
    void * base = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_create(1, 1, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 1);
        check_base(ms, base, 0);

        mstr_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_create(8, 8, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 8);
        check_base(ms, base, 0);

        mstr_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_create(15, 15, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 15);
        check_base(ms, base, 0);

        mstr_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_create(16, 16, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        mstr_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_create(17, 17, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = mstr_calc_base(ms, 0, 16, 16);
        check_base(ms, base, 4 * 16 * 16);

        base = mstr_calc_base(ms, 16, 0, 1);
        check_base(ms, base, 4 * 16 * 16 + 4 * 16 * 8);

        base = mstr_calc_base(ms, 16, 16, 1);
        check_base(ms, base, 4 * 16 * 24 + 4 * 1 * 16);

        mstr_destroy(ms);
    }

    // -- 2x17 matrix -- //
    {
        ms = mstr_create(2, 17, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 2);
        check_base(ms, base, 0);

        base = mstr_calc_base(ms, 0, 16, 2);
        check_base(ms, base, 4 * 2 * 16);

        mstr_destroy(ms);
    }

    // -- 2x33 matrix -- //
    {
        ms = mstr_create(2, 33, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 2);
        check_base(ms, base, 0);

        base = mstr_calc_base(ms, 0, 16, 2);
        check_base(ms, base, 4 * 2 * 16);

        base = mstr_calc_base(ms, 0, 32, 2);
        check_base(ms, base, 4 * 2 * 16 * 2);

        mstr_destroy(ms);
    }

    // -- 17x33 matrix -- //
    {
        ms = mstr_create(17, 33, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = mstr_calc_base(ms, 0, 16, 16);
        check_base(ms, base, 4 * 16 * 16);

        base = mstr_calc_base(ms, 0, 32, 16);
        check_base(ms, base, 4 * 16 * 16 * 2);

        base = mstr_calc_base(ms, 16, 0, 1);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8);
        
        base = mstr_calc_base(ms, 16, 16, 1);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8 + 4 * 1 * 16);

        base = mstr_calc_base(ms, 16, 32, 1);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8 + 4 * 1 * 16 * 2);

        mstr_destroy(ms);
    }

    // -- 17x2 matrix -- //
    {
        ms = mstr_create(17, 2, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = mstr_calc_base(ms, 16, 0, 1);
        check_base(ms, base, 4 * 16 * 8);

        mstr_destroy(ms);
    }

    // -- 33x2 matrix -- //
    {
        ms = mstr_create(33, 2, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = mstr_calc_base(ms, 16, 0, 16);
        check_base(ms, base, 4 * 16 * 8);

        base = mstr_calc_base(ms, 32, 0, 1);
        check_base(ms, base, 4 * 32 * 8);

        mstr_destroy(ms);
    }

    // -- 33x17 matrix -- //
    {
        ms = mstr_create(33, 17, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = mstr_calc_base(ms, 0, 16, 16);
        check_base(ms, base, 4 * 16 * 16);

        base = mstr_calc_base(ms, 16, 0, 16);
        check_base(ms, base, 4 * 16 * 16 + 4 * 16 * 8);

        base = mstr_calc_base(ms, 16, 16, 16);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8);

        base = mstr_calc_base(ms, 32, 0, 1);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8 * 2);

        base = mstr_calc_base(ms, 32, 16, 1);
        check_base(ms, base, 4 * 16 * 16 * 2 + 4 * 16 * 8 * 2 + 4 * 1 * 16);

        mstr_destroy(ms);
    }
}

Test(Locate, mstr_locate_chunk)
{
    mx_stor_ptr ms = NULL;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;;
    void * chk = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_create(1, 1, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        chk = mstr_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 1);

        mstr_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_create(8, 8, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        chk = mstr_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 8);
        check_columns_in_chunk(cols_in_chk, 8);

        mstr_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_create(15, 15, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        chk = mstr_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 15);
        check_columns_in_chunk(cols_in_chk, 15);

        mstr_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_create(16, 16, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        chk = mstr_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 16);

        mstr_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_create(17, 17, sizeof(int32_t), I32_VALS_IN_V8SI, I32_VALS_IN_CACHE_LINE);

        chk = mstr_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 16);

        chk = mstr_locate_chunk(ms, 0, 1, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 4 * 16 * 16);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 1);

        chk = mstr_locate_chunk(ms, 1, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 4 * 16 * 16 + 4 * 16 * 8);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 16);

        chk = mstr_locate_chunk(ms, 1, 1, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 4 * 16 * 16 + 4 * 16 * 8 + 4 * 1 * 16);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 1);

        mstr_destroy(ms);
    }
}

