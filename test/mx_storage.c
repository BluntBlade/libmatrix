#include <criterion/criterion.h>

#include "src/mx_storage.h"

#define check_pointer(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %p, got %p.", expect, val)

#define check_value(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %d, got %d.", expect, val)

#define check_i32_value(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %d, got %d.", expect, val)

#define check_f32_value(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %f, got %f.", expect, val)

#define check_base(ms, base, off) \
    cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

#define check_rows_in_chunk(rows_in_chk, expect) \
    cr_expect(rows_in_chk == expect, "Wrong number of rows in the chunk - Expect %d, got %d.", expect, rows_in_chk)

#define check_columns_in_chunk(cols_in_chk, expect) \
    cr_expect(cols_in_chk == expect, "Wrong number of columns in the chunk - Expect %d, got %d.", expect, cols_in_chk)

Test(Location, mstr_calc_base)
{
    mx_stor_ptr ms = NULL;
    void * base = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_create(1, 1, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 1);
        check_base(ms, base, 0);

        mstr_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_create(8, 8, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 8);
        check_base(ms, base, 0);

        mstr_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_create(15, 15, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 15);
        check_base(ms, base, 0);

        mstr_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_create(16, 16, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        mstr_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_create(17, 17, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

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
        ms = mstr_create(2, 17, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 2);
        check_base(ms, base, 0);

        base = mstr_calc_base(ms, 0, 16, 2);
        check_base(ms, base, 4 * 2 * 16);

        mstr_destroy(ms);
    }

    // -- 2x33 matrix -- //
    {
        ms = mstr_create(2, 33, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

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
        ms = mstr_create(17, 33, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

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
        ms = mstr_create(17, 2, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        base = mstr_calc_base(ms, 0, 0, 16);
        check_base(ms, base, 0);

        base = mstr_calc_base(ms, 16, 0, 1);
        check_base(ms, base, 4 * 16 * 8);

        mstr_destroy(ms);
    }

    // -- 33x2 matrix -- //
    {
        ms = mstr_create(33, 2, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

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
        ms = mstr_create(33, 17, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

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

Test(Location, mstr_locate_chunk)
{
    mx_stor_ptr ms = NULL;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;;
    void * chk = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_create(1, 1, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        chk = mstr_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 1);
        check_columns_in_chunk(cols_in_chk, 1);

        mstr_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_create(8, 8, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        chk = mstr_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 8);
        check_columns_in_chunk(cols_in_chk, 8);

        mstr_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_create(15, 15, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        chk = mstr_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 15);
        check_columns_in_chunk(cols_in_chk, 15);

        mstr_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_create(16, 16, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

        chk = mstr_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk);
        check_base(ms, chk, 0);
        check_rows_in_chunk(rows_in_chk, 16);
        check_columns_in_chunk(cols_in_chk, 16);

        mstr_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_create(17, 17, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);

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

Test(GetterSetter, mstr_get_i32)
{
    mx_stor_ptr ms = NULL;
    int32_t * base = NULL;
    int32_t val = 0;

    // -- 1x1 matrix -- //
    {
        ms = mstr_create(1, 1, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        base[0 * 8 + 0] = 4567;
        val = mstr_get_i32(ms, 0, 0);
        check_i32_value(val, 4567);

        mstr_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_create(5, 5, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        base[4 * 8 + 4] = 4567;
        val = mstr_get_i32(ms, 4, 4);
        check_i32_value(val, 4567);

        mstr_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_create(8, 8, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        base[7 * 8] = 4567;
        val = mstr_get_i32(ms, 7, 0);
        check_i32_value(val, 4567);

        mstr_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_create(9, 9, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        base[8] = 4567;
        val = mstr_get_i32(ms, 0, 8);
        check_i32_value(val, 4567);

        mstr_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_create(16, 16, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        base[15 * 16 + 15] = 4567;
        val = mstr_get_i32(ms, 15, 15);
        check_i32_value(val, 4567);

        mstr_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_create(17, 17, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        base[16 * 16 + 0 + 0] = 9876;
        val = mstr_get_i32(ms, 0, 16);
        check_i32_value(val, 9876);

        base[16 * 16 + 16 * 8 + 0] = 1234;
        val = mstr_get_i32(ms, 16, 0);
        check_i32_value(val, 1234);

        base[16 * 16 + 16 * 8 + 1 * 16 + 0] = 4567;
        val = mstr_get_i32(ms, 16, 16);
        check_i32_value(val, 4567);

        mstr_destroy(ms);
    }

    // -- 34x34 matrix -- //
    {
        ms = mstr_create(34, 34, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        base[16 * 16 * 2 + 6 * 8 + 1] = 4567;
        val = mstr_get_i32(ms, 6, 33);
        check_i32_value(val, 4567);

        base[16 * 16 * 2 + 16 * 8 + 1 * 16 + 1] = 9876;
        val = mstr_get_i32(ms, 17, 1);
        check_i32_value(val, 9876);

        base[16 * 16 * 2 + 16 * 8 + 16 * 16 + 8 * 16 + 8] = 1234;
        val = mstr_get_i32(ms, 24, 24);
        check_i32_value(val, 1234);

        mstr_destroy(ms);
    }
}

Test(GetterSetter, mstr_set_i32)
{
    mx_stor_ptr ms = NULL;
    int32_t * base = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_create(1, 1, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_i32(ms, 0, 0, 4567);
        check_i32_value(base[0 * 8 + 0], 4567);

        mstr_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_create(5, 5, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_i32(ms, 4, 4, 4567);
        check_i32_value(base[4 * 8 + 4], 4567);

        mstr_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_create(8, 8, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_i32(ms, 7, 0, 4567);
        check_i32_value(base[7 * 8], 4567);

        mstr_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_create(9, 9, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_i32(ms, 0, 8, 4567);
        check_i32_value(base[8], 4567);

        mstr_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_create(16, 16, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_i32(ms, 15, 15, 4567);
        check_i32_value(base[15 * 16 + 15], 4567);

        mstr_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_create(17, 17, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_i32(ms, 0, 16, 9876);
        check_i32_value(base[16 * 16 + 0 + 0], 9876);

        mstr_set_i32(ms, 16, 0, 1234);
        check_i32_value(base[16 * 16 + 16 * 8 + 0], 1234);

        mstr_set_i32(ms, 16, 16, 4567);
        check_i32_value(base[16 * 16 + 16 * 8 + 1 * 16 + 0], 4567);

        mstr_destroy(ms);
    }

    // -- 34x34 matrix -- //
    {
        ms = mstr_create(34, 34, I32_SIZE, I32S_IN_V8SI, I32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_i32(ms, 6, 33, 4567);
        check_i32_value(base[16 * 16 * 2 + 6 * 8 + 1], 4567);

        mstr_set_i32(ms, 17, 1, 9876);
        check_i32_value(base[16 * 16 * 2 + 16 * 8 + 1 * 16 + 1], 9876);

        mstr_set_i32(ms, 24, 24, 1234);
        check_i32_value(base[16 * 16 * 2 + 16 * 8 + 16 * 16 + 8 * 16 + 8], 1234);

        mstr_destroy(ms);
    }
}

Test(GetterSetter, mstr_get_f32)
{
    mx_stor_ptr ms = NULL;
    float * base = NULL;
    float val = 0;

    // -- 1x1 matrix -- //
    {
        ms = mstr_create(1, 1, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        base[0 * 8 + 0] = 4567.0;
        val = mstr_get_f32(ms, 0, 0);
        check_f32_value(val, 4567.0);

        mstr_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_create(5, 5, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        base[4 * 8 + 4] = 4567.0;
        val = mstr_get_f32(ms, 4, 4);
        check_f32_value(val, 4567.0);

        mstr_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_create(8, 8, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        base[7 * 8] = 4567.0;
        val = mstr_get_f32(ms, 7, 0);
        check_f32_value(val, 4567.0);

        mstr_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_create(9, 9, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        base[8] = 4567.0;
        val = mstr_get_f32(ms, 0, 8);
        check_f32_value(val, 4567.0);

        mstr_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_create(16, 16, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        base[15 * 16 + 15] = 4567.0;
        val = mstr_get_f32(ms, 15, 15);
        check_f32_value(val, 4567.0);

        mstr_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_create(17, 17, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        base[16 * 16 + 0 + 0] = 9876;
        val = mstr_get_f32(ms, 0, 16);
        check_f32_value(val, 9876);

        base[16 * 16 + 16 * 8 + 0] = 1234;
        val = mstr_get_f32(ms, 16, 0);
        check_f32_value(val, 1234);

        base[16 * 16 + 16 * 8 + 1 * 16 + 0] = 4567.0;
        val = mstr_get_f32(ms, 16, 16);
        check_f32_value(val, 4567.0);

        mstr_destroy(ms);
    }

    // -- 34x34 matrix -- //
    {
        ms = mstr_create(34, 34, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        base[16 * 16 * 2 + 6 * 8 + 1] = 4567.0;
        val = mstr_get_f32(ms, 6, 33);
        check_f32_value(val, 4567.0);

        base[16 * 16 * 2 + 16 * 8 + 1 * 16 + 1] = 9876;
        val = mstr_get_f32(ms, 17, 1);
        check_f32_value(val, 9876);

        base[16 * 16 * 2 + 16 * 8 + 16 * 16 + 8 * 16 + 8] = 1234;
        val = mstr_get_f32(ms, 24, 24);
        check_f32_value(val, 1234);

        mstr_destroy(ms);
    }
}

Test(GetterSetter, mstr_set_f32)
{
    mx_stor_ptr ms = NULL;
    float * base = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_create(1, 1, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_f32(ms, 0, 0, 4567.0);
        check_f32_value(base[0 * 8 + 0], 4567.0);

        mstr_destroy(ms);
    }

    // -- 5x5 matrix -- //
    {
        ms = mstr_create(5, 5, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_f32(ms, 4, 4, 4567.0);
        check_f32_value(base[4 * 8 + 4], 4567.0);

        mstr_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_create(8, 8, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_f32(ms, 7, 0, 4567.0);
        check_f32_value(base[7 * 8], 4567.0);

        mstr_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_create(9, 9, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_f32(ms, 0, 8, 4567.0);
        check_f32_value(base[8], 4567.0);

        mstr_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_create(16, 16, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_f32(ms, 15, 15, 4567.0);
        check_f32_value(base[15 * 16 + 15], 4567.0);

        mstr_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_create(17, 17, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_f32(ms, 0, 16, 9876.0);
        check_f32_value(base[16 * 16 + 0 + 0], 9876.0);

        mstr_set_f32(ms, 16, 0, 1234.0);
        check_f32_value(base[16 * 16 + 16 * 8 + 0], 1234.0);

        mstr_set_f32(ms, 16, 16, 4567.0);
        check_f32_value(base[16 * 16 + 16 * 8 + 1 * 16 + 0], 4567.0);

        mstr_destroy(ms);
    }

    // -- 34x34 matrix -- //
    {
        ms = mstr_create(34, 34, F32_SIZE, F32S_IN_V8SF, F32S_IN_CACHE_LINE);
        base = ms->data;

        mstr_set_f32(ms, 6, 33, 4567.0);
        check_f32_value(base[16 * 16 * 2 + 6 * 8 + 1], 4567.0);

        mstr_set_f32(ms, 17, 1, 9876.0);
        check_f32_value(base[16 * 16 * 2 + 16 * 8 + 1 * 16 + 1], 9876.0);

        mstr_set_f32(ms, 24, 24, 1234.0);
        check_f32_value(base[16 * 16 * 2 + 16 * 8 + 16 * 16 + 8 * 16 + 8], 1234.0);

        mstr_destroy(ms);
    }
}

Test(Iterator, mitr_init_for_iterating_in_rows)
{
    mx_stor_t ms;
    mx_iter_t itr;

    mitr_init_for_iterating_in_rows(&itr, &ms, 1, 2, 3, 4);

    check_pointer(itr.ms, &ms);

    check_value(itr.row, 1);
    check_value(itr.col, 2);
    check_value(itr.row_begin, 1);
    check_value(itr.col_begin, 2);
    check_value(itr.row_end, 3);
    check_value(itr.col_end, 4);

    check_i32_value(mx_type_val(itr.dval.i32)[0], 0);
    check_i32_value(mx_type_val(itr.dval.i32)[1], 0);
    check_i32_value(mx_type_val(itr.dval.i32)[2], 0);
    check_i32_value(mx_type_val(itr.dval.i32)[3], 0);
    check_i32_value(mx_type_val(itr.dval.i32)[4], 0);
    check_i32_value(mx_type_val(itr.dval.i32)[5], 0);
    check_i32_value(mx_type_val(itr.dval.i32)[6], 0);
    check_i32_value(mx_type_val(itr.dval.i32)[7], 0);
}

Test(Iterator, mitr_init_for_iterating_in_columns)
{
    mx_stor_t ms;
    mx_iter_t itr;

    mitr_init_for_iterating_in_columns(&itr, &ms, 1, 2, 3, 4);

    check_pointer(itr.ms, &ms);

    check_value(itr.row, 1);
    check_value(itr.col, 2);
    check_value(itr.row_begin, 1);
    check_value(itr.col_begin, 2);
    check_value(itr.row_end, 3);
    check_value(itr.col_end, 4);

    check_f32_value(mx_type_val(itr.dval.f32)[0], 0.0);
    check_f32_value(mx_type_val(itr.dval.f32)[1], 0.0);
    check_f32_value(mx_type_val(itr.dval.f32)[2], 0.0);
    check_f32_value(mx_type_val(itr.dval.f32)[3], 0.0);
    check_f32_value(mx_type_val(itr.dval.f32)[4], 0.0);
    check_f32_value(mx_type_val(itr.dval.f32)[5], 0.0);
    check_f32_value(mx_type_val(itr.dval.f32)[6], 0.0);
    check_f32_value(mx_type_val(itr.dval.f32)[7], 0.0);
}

Test(Iterator, mitr_init_for_iterating_in_chunk)
{
    mx_iter_t itr;

    {
        mx_stor_ptr ms = mstr_create(17, 17, 1, 8, 16);

        mitr_init_for_iterating_in_chunk(&itr, ms, 0, 0);

        check_pointer(itr.ms, ms);

        check_value(itr.row, 0);
        check_value(itr.col, 0);
        check_value(itr.row_begin, 0);
        check_value(itr.col_begin, 0);
        check_value(itr.row_end, 16);
        check_value(itr.col_end, 16);

        check_f32_value(mx_type_val(itr.dval.f32)[0], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[1], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[2], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[3], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[4], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[5], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[6], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[7], 0.0);
        
        mstr_destroy(ms);
    }

    {
        mx_stor_ptr ms = mstr_create(17, 17, 1, 8, 16);

        mitr_init_for_iterating_in_chunk(&itr, ms, 0, 1);

        check_pointer(itr.ms, ms);

        check_value(itr.row, 0);
        check_value(itr.col, 16);
        check_value(itr.row_begin, 0);
        check_value(itr.col_begin, 16);
        check_value(itr.row_end, 16);
        check_value(itr.col_end, 17);

        check_f32_value(mx_type_val(itr.dval.f32)[0], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[1], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[2], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[3], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[4], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[5], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[6], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[7], 0.0);
        
        mstr_destroy(ms);
    }

    {
        mx_stor_ptr ms = mstr_create(17, 17, 1, 8, 16);

        mitr_init_for_iterating_in_chunk(&itr, ms, 1, 0);

        check_pointer(itr.ms, ms);

        check_value(itr.row, 16);
        check_value(itr.col, 0);
        check_value(itr.row_begin, 16);
        check_value(itr.col_begin, 0);
        check_value(itr.row_end, 17);
        check_value(itr.col_end, 16);

        check_f32_value(mx_type_val(itr.dval.f32)[0], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[1], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[2], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[3], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[4], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[5], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[6], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[7], 0.0);
        
        mstr_destroy(ms);
    }

    {
        mx_stor_ptr ms = mstr_create(17, 17, 1, 8, 16);

        mitr_init_for_iterating_in_chunk(&itr, ms, 1, 1);

        check_pointer(itr.ms, ms);

        check_value(itr.row, 16);
        check_value(itr.col, 16);
        check_value(itr.row_begin, 16);
        check_value(itr.col_begin, 16);
        check_value(itr.row_end, 17);
        check_value(itr.col_end, 17);

        check_f32_value(mx_type_val(itr.dval.f32)[0], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[1], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[2], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[3], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[4], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[5], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[6], 0.0);
        check_f32_value(mx_type_val(itr.dval.f32)[7], 0.0);
        
        mstr_destroy(ms);
    }
}

Test(Iterator, mitr_set_default_i32)
{
    mx_iter_t itr;

    mitr_set_default_i32(&itr, 1);
    check_i32_value(mx_type_val(itr.dval.i32)[0], 1);
    check_i32_value(mx_type_val(itr.dval.i32)[1], 1);
    check_i32_value(mx_type_val(itr.dval.i32)[2], 1);
    check_i32_value(mx_type_val(itr.dval.i32)[3], 1);
    check_i32_value(mx_type_val(itr.dval.i32)[4], 1);
    check_i32_value(mx_type_val(itr.dval.i32)[5], 1);
    check_i32_value(mx_type_val(itr.dval.i32)[6], 1);
    check_i32_value(mx_type_val(itr.dval.i32)[7], 1);
}

Test(Iterator, mitr_set_default_f32)
{
    mx_iter_t itr;

    mitr_set_default_f32(&itr, 2.0);
    check_f32_value(mx_type_val(itr.dval.f32)[0], 2.0);
    check_f32_value(mx_type_val(itr.dval.f32)[1], 2.0);
    check_f32_value(mx_type_val(itr.dval.f32)[2], 2.0);
    check_f32_value(mx_type_val(itr.dval.f32)[3], 2.0);
    check_f32_value(mx_type_val(itr.dval.f32)[4], 2.0);
    check_f32_value(mx_type_val(itr.dval.f32)[5], 2.0);
    check_f32_value(mx_type_val(itr.dval.f32)[6], 2.0);
    check_f32_value(mx_type_val(itr.dval.f32)[7], 2.0);
}

Test(Iterator, next_position_in_row)
{
    mx_stor_ptr ms = mstr_create(17, 17, 1, 8, 16);
    
    {
        mx_iter_t itr;
        bool ret;

        mitr_init_for_iterating_in_rows(&itr, ms, 0, 0, 17, 17);

        ret = itr.next(&itr, 1);
        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 1);
        check_value(ret, true);

        ret = itr.next(&itr, 1);
        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 2);
        check_value(ret, true);

        itr.col = 16;
        ret = itr.next(&itr, 1);
        check_value(mitr_row(&itr), 1);
        check_value(mitr_column(&itr), 0);
        check_value(ret, true);

        itr.row = 16;
        itr.col = 16;
        ret = itr.next(&itr, 1);
        check_value(mitr_row(&itr), 17);
        check_value(mitr_column(&itr), 17);
        check_value(ret, false);
    }

    {
        mx_iter_t itr;
        bool ret;

        mitr_init_for_iterating_in_rows(&itr, ms, 0, 0, 17, 17);

        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 8);
        check_value(ret, true);

        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 16);
        check_value(ret, true);

        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 1);
        check_value(mitr_column(&itr), 0);
        check_value(ret, true);

        itr.row = 0;
        itr.col = 8;
        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 16);
        check_value(ret, true);

        itr.row = 0;
        itr.col = 16;
        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 1);
        check_value(mitr_column(&itr), 0);
        check_value(ret, true);

        itr.row = 16;
        itr.col = 16;
        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 17);
        check_value(mitr_column(&itr), 17);
        check_value(ret, false);
    }

    mstr_destroy(ms);
}

Test(Iterator, next_position_in_column)
{
    mx_stor_ptr ms = mstr_create(17, 17, 1, 8, 16);
    
    {
        mx_iter_t itr;
        bool ret;

        mitr_init_for_iterating_in_columns(&itr, ms, 0, 0, 17, 17);

        ret = itr.next(&itr, 1);
        check_value(mitr_row(&itr), 1);
        check_value(mitr_column(&itr), 0);
        check_value(ret, true);

        ret = itr.next(&itr, 1);
        check_value(mitr_row(&itr), 2);
        check_value(mitr_column(&itr), 0);
        check_value(ret, true);

        itr.row = 16;
        ret = itr.next(&itr, 1);
        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 1);
        check_value(ret, true);

        itr.row = 16;
        itr.col = 16;
        ret = itr.next(&itr, 1);
        check_value(mitr_row(&itr), 17);
        check_value(mitr_column(&itr), 17);
        check_value(ret, false);
    }

    {
        mx_iter_t itr;
        bool ret;

        mitr_init_for_iterating_in_columns(&itr, ms, 0, 0, 17, 17);

        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 8);
        check_value(mitr_column(&itr), 0);
        check_value(ret, true);

        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 16);
        check_value(mitr_column(&itr), 0);
        check_value(ret, true);

        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 1);
        check_value(ret, true);

        itr.row = 8;
        itr.col = 0;
        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 16);
        check_value(mitr_column(&itr), 0);
        check_value(ret, true);

        itr.row = 16;
        itr.col = 0;
        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 1);
        check_value(ret, true);

        itr.row = 16;
        itr.col = 16;
        ret = itr.next(&itr, 8);
        check_value(mitr_row(&itr), 17);
        check_value(mitr_column(&itr), 17);
        check_value(ret, false);
    }

    mstr_destroy(ms);
}

/*
    for (uint32_t i = 0; i < mstr_rows(ms); i += 1) {
        for (uint32_t j = 0; j < mstr_columns(ms); j += 1) {
            mstr_set_i32(ms, i, j, i * mstr_columns(ms) + j);
        } // for
    } // for
*/

