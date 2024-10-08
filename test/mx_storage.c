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
        mx_stor_ptr ms = mstr_create(17, 17, 4, 8, 16);

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
        mx_stor_ptr ms = mstr_create(17, 17, 4, 8, 16);

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
        mx_stor_ptr ms = mstr_create(17, 17, 4, 8, 16);

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
        mx_stor_ptr ms = mstr_create(17, 17, 4, 8, 16);

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
    mx_stor_ptr ms = mstr_create(17, 17, 4, 8, 16);
    
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
    mx_stor_ptr ms = mstr_create(17, 17, 4, 8, 16);
    
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

Test(Iterator, mitr_get_vectors_in_row)
{
    mx_stor_ptr ms = mstr_create(18, 18, 4, 8, 16);

    for (uint32_t i = 0; i < mstr_rows(ms); i += 1) {
        for (uint32_t j = 0; j < mstr_columns(ms); j += 1) {
            mstr_set_i32(ms, i, j, i * 100 + j);
        } // for
    } // for

    // Case: Get only one vector and don't move to next.
    {
        mx_iter_t itr;
        mitr_init_for_iterating_in_rows(&itr, ms, 0, 0, 18, 18);

        v8si_t vec;
        mitr_get_vectors(&itr, false, 1, &vec, 0, 0);

        check_i32_value(mx_type_val(vec)[0], 0);
        check_i32_value(mx_type_val(vec)[1], 1);
        check_i32_value(mx_type_val(vec)[2], 2);
        check_i32_value(mx_type_val(vec)[3], 3);
        check_i32_value(mx_type_val(vec)[4], 4);
        check_i32_value(mx_type_val(vec)[5], 5);
        check_i32_value(mx_type_val(vec)[6], 6);
        check_i32_value(mx_type_val(vec)[7], 7);

        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 0);
    }

    // Case: Get five vectors and don't move to next.
    {
        mx_iter_t itr;
        mitr_init_for_iterating_in_rows(&itr, ms, 0, 0, 18, 18);
        mitr_set_default_i32(&itr, 9999);

        v8si_t vec[5];
        mitr_get_vectors(&itr, false, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 0);
        check_i32_value(mx_type_val(vec[0])[1], 1);
        check_i32_value(mx_type_val(vec[0])[2], 2);
        check_i32_value(mx_type_val(vec[0])[3], 3);
        check_i32_value(mx_type_val(vec[0])[4], 4);
        check_i32_value(mx_type_val(vec[0])[5], 5);
        check_i32_value(mx_type_val(vec[0])[6], 6);
        check_i32_value(mx_type_val(vec[0])[7], 7);

        check_i32_value(mx_type_val(vec[1])[0], 9999);
        check_i32_value(mx_type_val(vec[1])[1], 9999);
        check_i32_value(mx_type_val(vec[1])[2], 9999);
        check_i32_value(mx_type_val(vec[1])[3], 9999);
        check_i32_value(mx_type_val(vec[1])[4], 9999);
        check_i32_value(mx_type_val(vec[1])[5], 9999);
        check_i32_value(mx_type_val(vec[1])[6], 9999);
        check_i32_value(mx_type_val(vec[1])[7], 9999);

        check_i32_value(mx_type_val(vec[2])[0], 100);
        check_i32_value(mx_type_val(vec[2])[1], 101);
        check_i32_value(mx_type_val(vec[2])[2], 102);
        check_i32_value(mx_type_val(vec[2])[3], 103);
        check_i32_value(mx_type_val(vec[2])[4], 104);
        check_i32_value(mx_type_val(vec[2])[5], 105);
        check_i32_value(mx_type_val(vec[2])[6], 106);
        check_i32_value(mx_type_val(vec[2])[7], 107);

        check_i32_value(mx_type_val(vec[3])[0], 9999);
        check_i32_value(mx_type_val(vec[3])[1], 0);
        check_i32_value(mx_type_val(vec[3])[2], 1);
        check_i32_value(mx_type_val(vec[3])[3], 2);
        check_i32_value(mx_type_val(vec[3])[4], 3);
        check_i32_value(mx_type_val(vec[3])[5], 4);
        check_i32_value(mx_type_val(vec[3])[6], 5);
        check_i32_value(mx_type_val(vec[3])[7], 6);

        check_i32_value(mx_type_val(vec[4])[0], 1);
        check_i32_value(mx_type_val(vec[4])[1], 2);
        check_i32_value(mx_type_val(vec[4])[2], 3);
        check_i32_value(mx_type_val(vec[4])[3], 4);
        check_i32_value(mx_type_val(vec[4])[4], 5);
        check_i32_value(mx_type_val(vec[4])[5], 6);
        check_i32_value(mx_type_val(vec[4])[6], 7);
        check_i32_value(mx_type_val(vec[4])[7], 8);

        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 0);
    }

    // Case: Get five vectors and move to next, then get another five vectors. Do this twice.
    {
        mx_iter_t itr;
        mitr_init_for_iterating_in_rows(&itr, ms, 0, 0, 18, 18);
        mitr_set_default_i32(&itr, 9999);

        v8si_t vec[5];
        mitr_get_vectors(&itr, true, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 0);
        check_i32_value(mx_type_val(vec[0])[1], 1);
        check_i32_value(mx_type_val(vec[0])[2], 2);
        check_i32_value(mx_type_val(vec[0])[3], 3);
        check_i32_value(mx_type_val(vec[0])[4], 4);
        check_i32_value(mx_type_val(vec[0])[5], 5);
        check_i32_value(mx_type_val(vec[0])[6], 6);
        check_i32_value(mx_type_val(vec[0])[7], 7);

        check_i32_value(mx_type_val(vec[1])[0], 9999);
        check_i32_value(mx_type_val(vec[1])[1], 9999);
        check_i32_value(mx_type_val(vec[1])[2], 9999);
        check_i32_value(mx_type_val(vec[1])[3], 9999);
        check_i32_value(mx_type_val(vec[1])[4], 9999);
        check_i32_value(mx_type_val(vec[1])[5], 9999);
        check_i32_value(mx_type_val(vec[1])[6], 9999);
        check_i32_value(mx_type_val(vec[1])[7], 9999);

        check_i32_value(mx_type_val(vec[2])[0], 100);
        check_i32_value(mx_type_val(vec[2])[1], 101);
        check_i32_value(mx_type_val(vec[2])[2], 102);
        check_i32_value(mx_type_val(vec[2])[3], 103);
        check_i32_value(mx_type_val(vec[2])[4], 104);
        check_i32_value(mx_type_val(vec[2])[5], 105);
        check_i32_value(mx_type_val(vec[2])[6], 106);
        check_i32_value(mx_type_val(vec[2])[7], 107);

        check_i32_value(mx_type_val(vec[3])[0], 9999);
        check_i32_value(mx_type_val(vec[3])[1], 0);
        check_i32_value(mx_type_val(vec[3])[2], 1);
        check_i32_value(mx_type_val(vec[3])[3], 2);
        check_i32_value(mx_type_val(vec[3])[4], 3);
        check_i32_value(mx_type_val(vec[3])[5], 4);
        check_i32_value(mx_type_val(vec[3])[6], 5);
        check_i32_value(mx_type_val(vec[3])[7], 6);

        check_i32_value(mx_type_val(vec[4])[0], 1);
        check_i32_value(mx_type_val(vec[4])[1], 2);
        check_i32_value(mx_type_val(vec[4])[2], 3);
        check_i32_value(mx_type_val(vec[4])[3], 4);
        check_i32_value(mx_type_val(vec[4])[4], 5);
        check_i32_value(mx_type_val(vec[4])[5], 6);
        check_i32_value(mx_type_val(vec[4])[6], 7);
        check_i32_value(mx_type_val(vec[4])[7], 8);

        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 8);

        mitr_get_vectors(&itr, true, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 8);
        check_i32_value(mx_type_val(vec[0])[1], 9);
        check_i32_value(mx_type_val(vec[0])[2], 10);
        check_i32_value(mx_type_val(vec[0])[3], 11);
        check_i32_value(mx_type_val(vec[0])[4], 12);
        check_i32_value(mx_type_val(vec[0])[5], 13);
        check_i32_value(mx_type_val(vec[0])[6], 14);
        check_i32_value(mx_type_val(vec[0])[7], 15);

        check_i32_value(mx_type_val(vec[1])[0], 9999);
        check_i32_value(mx_type_val(vec[1])[1], 9999);
        check_i32_value(mx_type_val(vec[1])[2], 9999);
        check_i32_value(mx_type_val(vec[1])[3], 9999);
        check_i32_value(mx_type_val(vec[1])[4], 9999);
        check_i32_value(mx_type_val(vec[1])[5], 9999);
        check_i32_value(mx_type_val(vec[1])[6], 9999);
        check_i32_value(mx_type_val(vec[1])[7], 9999);

        check_i32_value(mx_type_val(vec[2])[0], 108);
        check_i32_value(mx_type_val(vec[2])[1], 109);
        check_i32_value(mx_type_val(vec[2])[2], 110);
        check_i32_value(mx_type_val(vec[2])[3], 111);
        check_i32_value(mx_type_val(vec[2])[4], 112);
        check_i32_value(mx_type_val(vec[2])[5], 113);
        check_i32_value(mx_type_val(vec[2])[6], 114);
        check_i32_value(mx_type_val(vec[2])[7], 115);

        check_i32_value(mx_type_val(vec[3])[0], 7);
        check_i32_value(mx_type_val(vec[3])[1], 8);
        check_i32_value(mx_type_val(vec[3])[2], 9);
        check_i32_value(mx_type_val(vec[3])[3], 10);
        check_i32_value(mx_type_val(vec[3])[4], 11);
        check_i32_value(mx_type_val(vec[3])[5], 12);
        check_i32_value(mx_type_val(vec[3])[6], 13);
        check_i32_value(mx_type_val(vec[3])[7], 14);

        check_i32_value(mx_type_val(vec[4])[0], 9);
        check_i32_value(mx_type_val(vec[4])[1], 10);
        check_i32_value(mx_type_val(vec[4])[2], 11);
        check_i32_value(mx_type_val(vec[4])[3], 12);
        check_i32_value(mx_type_val(vec[4])[4], 13);
        check_i32_value(mx_type_val(vec[4])[5], 14);
        check_i32_value(mx_type_val(vec[4])[6], 15);
        check_i32_value(mx_type_val(vec[4])[7], 16);

        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 16);

        mitr_get_vectors(&itr, true, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 16);
        check_i32_value(mx_type_val(vec[0])[1], 17);
        check_i32_value(mx_type_val(vec[0])[2], 9999);
        check_i32_value(mx_type_val(vec[0])[3], 9999);
        check_i32_value(mx_type_val(vec[0])[4], 9999);
        check_i32_value(mx_type_val(vec[0])[5], 9999);
        check_i32_value(mx_type_val(vec[0])[6], 9999);
        check_i32_value(mx_type_val(vec[0])[7], 9999);

        check_i32_value(mx_type_val(vec[1])[0], 9999);
        check_i32_value(mx_type_val(vec[1])[1], 9999);
        check_i32_value(mx_type_val(vec[1])[2], 9999);
        check_i32_value(mx_type_val(vec[1])[3], 9999);
        check_i32_value(mx_type_val(vec[1])[4], 9999);
        check_i32_value(mx_type_val(vec[1])[5], 9999);
        check_i32_value(mx_type_val(vec[1])[6], 9999);
        check_i32_value(mx_type_val(vec[1])[7], 9999);

        check_i32_value(mx_type_val(vec[2])[0], 116);
        check_i32_value(mx_type_val(vec[2])[1], 117);
        check_i32_value(mx_type_val(vec[2])[2], 9999);
        check_i32_value(mx_type_val(vec[2])[3], 9999);
        check_i32_value(mx_type_val(vec[2])[4], 9999);
        check_i32_value(mx_type_val(vec[2])[5], 9999);
        check_i32_value(mx_type_val(vec[2])[6], 9999);
        check_i32_value(mx_type_val(vec[2])[7], 9999);

        check_i32_value(mx_type_val(vec[3])[0], 15);
        check_i32_value(mx_type_val(vec[3])[1], 16);
        check_i32_value(mx_type_val(vec[3])[2], 17);
        check_i32_value(mx_type_val(vec[3])[3], 9999);
        check_i32_value(mx_type_val(vec[3])[4], 9999);
        check_i32_value(mx_type_val(vec[3])[5], 9999);
        check_i32_value(mx_type_val(vec[3])[6], 9999);
        check_i32_value(mx_type_val(vec[3])[7], 9999);

        check_i32_value(mx_type_val(vec[4])[0], 17);
        check_i32_value(mx_type_val(vec[4])[1], 9999);
        check_i32_value(mx_type_val(vec[4])[2], 9999);
        check_i32_value(mx_type_val(vec[4])[3], 9999);
        check_i32_value(mx_type_val(vec[4])[4], 9999);
        check_i32_value(mx_type_val(vec[4])[5], 9999);
        check_i32_value(mx_type_val(vec[4])[6], 9999);
        check_i32_value(mx_type_val(vec[4])[7], 9999);

        check_value(mitr_row(&itr), 1);
        check_value(mitr_column(&itr), 0);
    }

    // Case: Get five vectors and don't move to next. One of them is beyond the bottom of the matrix.
    {
        mx_iter_t itr;
        mitr_init_for_iterating_in_rows(&itr, ms, 17, 0, 18, 18);
        mitr_set_default_i32(&itr, 9999);

        v8si_t vec[5];
        mitr_get_vectors(&itr, false, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 1700);
        check_i32_value(mx_type_val(vec[0])[1], 1701);
        check_i32_value(mx_type_val(vec[0])[2], 1702);
        check_i32_value(mx_type_val(vec[0])[3], 1703);
        check_i32_value(mx_type_val(vec[0])[4], 1704);
        check_i32_value(mx_type_val(vec[0])[5], 1705);
        check_i32_value(mx_type_val(vec[0])[6], 1706);
        check_i32_value(mx_type_val(vec[0])[7], 1707);

        check_i32_value(mx_type_val(vec[1])[0], 1600);
        check_i32_value(mx_type_val(vec[1])[1], 1601);
        check_i32_value(mx_type_val(vec[1])[2], 1602);
        check_i32_value(mx_type_val(vec[1])[3], 1603);
        check_i32_value(mx_type_val(vec[1])[4], 1604);
        check_i32_value(mx_type_val(vec[1])[5], 1605);
        check_i32_value(mx_type_val(vec[1])[6], 1606);
        check_i32_value(mx_type_val(vec[1])[7], 1607);

        check_i32_value(mx_type_val(vec[2])[0], 9999);
        check_i32_value(mx_type_val(vec[2])[1], 9999);
        check_i32_value(mx_type_val(vec[2])[2], 9999);
        check_i32_value(mx_type_val(vec[2])[3], 9999);
        check_i32_value(mx_type_val(vec[2])[4], 9999);
        check_i32_value(mx_type_val(vec[2])[5], 9999);
        check_i32_value(mx_type_val(vec[2])[6], 9999);
        check_i32_value(mx_type_val(vec[2])[7], 9999);

        check_i32_value(mx_type_val(vec[3])[0], 9999);
        check_i32_value(mx_type_val(vec[3])[1], 1700);
        check_i32_value(mx_type_val(vec[3])[2], 1701);
        check_i32_value(mx_type_val(vec[3])[3], 1702);
        check_i32_value(mx_type_val(vec[3])[4], 1703);
        check_i32_value(mx_type_val(vec[3])[5], 1704);
        check_i32_value(mx_type_val(vec[3])[6], 1705);
        check_i32_value(mx_type_val(vec[3])[7], 1706);

        check_i32_value(mx_type_val(vec[4])[0], 1701);
        check_i32_value(mx_type_val(vec[4])[1], 1702);
        check_i32_value(mx_type_val(vec[4])[2], 1703);
        check_i32_value(mx_type_val(vec[4])[3], 1704);
        check_i32_value(mx_type_val(vec[4])[4], 1705);
        check_i32_value(mx_type_val(vec[4])[5], 1706);
        check_i32_value(mx_type_val(vec[4])[6], 1707);
        check_i32_value(mx_type_val(vec[4])[7], 1708);

        check_value(mitr_row(&itr), 17);
        check_value(mitr_column(&itr), 0);
    }

    mstr_destroy(ms);
}

Test(Iterator, mitr_get_vectors_in_column)
{
    mx_stor_ptr ms = mstr_create(18, 18, 4, 8, 16);

    for (uint32_t i = 0; i < mstr_rows(ms); i += 1) {
        for (uint32_t j = 0; j < mstr_columns(ms); j += 1) {
            mstr_set_i32(ms, i, j, i * 100 + j);
        } // for
    } // for

    // Case: Get only one vector and don't move to next.
    {
        mx_iter_t itr;
        mitr_init_for_iterating_in_columns(&itr, ms, 0, 0, 18, 18);

        v8si_t vec;
        mitr_get_vectors(&itr, false, 1, &vec, 0, 0);

        check_i32_value(mx_type_val(vec)[0], 0);
        check_i32_value(mx_type_val(vec)[1], 100);
        check_i32_value(mx_type_val(vec)[2], 200);
        check_i32_value(mx_type_val(vec)[3], 300);
        check_i32_value(mx_type_val(vec)[4], 400);
        check_i32_value(mx_type_val(vec)[5], 500);
        check_i32_value(mx_type_val(vec)[6], 600);
        check_i32_value(mx_type_val(vec)[7], 700);

        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 0);
    }

    // Case: Get five vectors and don't move to next.
    {
        mx_iter_t itr;
        mitr_init_for_iterating_in_columns(&itr, ms, 0, 0, 18, 18);
        mitr_set_default_i32(&itr, 9999);

        v8si_t vec[5];
        mitr_get_vectors(&itr, false, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 0);
        check_i32_value(mx_type_val(vec[0])[1], 100);
        check_i32_value(mx_type_val(vec[0])[2], 200);
        check_i32_value(mx_type_val(vec[0])[3], 300);
        check_i32_value(mx_type_val(vec[0])[4], 400);
        check_i32_value(mx_type_val(vec[0])[5], 500);
        check_i32_value(mx_type_val(vec[0])[6], 600);
        check_i32_value(mx_type_val(vec[0])[7], 700);

        check_i32_value(mx_type_val(vec[1])[0], 9999);
        check_i32_value(mx_type_val(vec[1])[1], 0);
        check_i32_value(mx_type_val(vec[1])[2], 100);
        check_i32_value(mx_type_val(vec[1])[3], 200);
        check_i32_value(mx_type_val(vec[1])[4], 300);
        check_i32_value(mx_type_val(vec[1])[5], 400);
        check_i32_value(mx_type_val(vec[1])[6], 500);
        check_i32_value(mx_type_val(vec[1])[7], 600);

        check_i32_value(mx_type_val(vec[2])[0], 100);
        check_i32_value(mx_type_val(vec[2])[1], 200);
        check_i32_value(mx_type_val(vec[2])[2], 300);
        check_i32_value(mx_type_val(vec[2])[3], 400);
        check_i32_value(mx_type_val(vec[2])[4], 500);
        check_i32_value(mx_type_val(vec[2])[5], 600);
        check_i32_value(mx_type_val(vec[2])[6], 700);
        check_i32_value(mx_type_val(vec[2])[7], 800);

        check_i32_value(mx_type_val(vec[3])[0], 9999);
        check_i32_value(mx_type_val(vec[3])[1], 9999);
        check_i32_value(mx_type_val(vec[3])[2], 9999);
        check_i32_value(mx_type_val(vec[3])[3], 9999);
        check_i32_value(mx_type_val(vec[3])[4], 9999);
        check_i32_value(mx_type_val(vec[3])[5], 9999);
        check_i32_value(mx_type_val(vec[3])[6], 9999);
        check_i32_value(mx_type_val(vec[3])[7], 9999);

        check_i32_value(mx_type_val(vec[4])[0], 1);
        check_i32_value(mx_type_val(vec[4])[1], 101);
        check_i32_value(mx_type_val(vec[4])[2], 201);
        check_i32_value(mx_type_val(vec[4])[3], 301);
        check_i32_value(mx_type_val(vec[4])[4], 401);
        check_i32_value(mx_type_val(vec[4])[5], 501);
        check_i32_value(mx_type_val(vec[4])[6], 601);
        check_i32_value(mx_type_val(vec[4])[7], 701);

        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 0);
    }

    // Case: Get five vectors and move to next, then get another five vectors. Do this twice.
    {
        mx_iter_t itr;
        mitr_init_for_iterating_in_columns(&itr, ms, 0, 0, 18, 18);
        mitr_set_default_i32(&itr, 9999);

        v8si_t vec[5];
        mitr_get_vectors(&itr, true, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 0);
        check_i32_value(mx_type_val(vec[0])[1], 100);
        check_i32_value(mx_type_val(vec[0])[2], 200);
        check_i32_value(mx_type_val(vec[0])[3], 300);
        check_i32_value(mx_type_val(vec[0])[4], 400);
        check_i32_value(mx_type_val(vec[0])[5], 500);
        check_i32_value(mx_type_val(vec[0])[6], 600);
        check_i32_value(mx_type_val(vec[0])[7], 700);

        check_i32_value(mx_type_val(vec[1])[0], 9999);
        check_i32_value(mx_type_val(vec[1])[1], 0);
        check_i32_value(mx_type_val(vec[1])[2], 100);
        check_i32_value(mx_type_val(vec[1])[3], 200);
        check_i32_value(mx_type_val(vec[1])[4], 300);
        check_i32_value(mx_type_val(vec[1])[5], 400);
        check_i32_value(mx_type_val(vec[1])[6], 500);
        check_i32_value(mx_type_val(vec[1])[7], 600);

        check_i32_value(mx_type_val(vec[2])[0], 100);
        check_i32_value(mx_type_val(vec[2])[1], 200);
        check_i32_value(mx_type_val(vec[2])[2], 300);
        check_i32_value(mx_type_val(vec[2])[3], 400);
        check_i32_value(mx_type_val(vec[2])[4], 500);
        check_i32_value(mx_type_val(vec[2])[5], 600);
        check_i32_value(mx_type_val(vec[2])[6], 700);
        check_i32_value(mx_type_val(vec[2])[7], 800);

        check_i32_value(mx_type_val(vec[3])[0], 9999);
        check_i32_value(mx_type_val(vec[3])[1], 9999);
        check_i32_value(mx_type_val(vec[3])[2], 9999);
        check_i32_value(mx_type_val(vec[3])[3], 9999);
        check_i32_value(mx_type_val(vec[3])[4], 9999);
        check_i32_value(mx_type_val(vec[3])[5], 9999);
        check_i32_value(mx_type_val(vec[3])[6], 9999);
        check_i32_value(mx_type_val(vec[3])[7], 9999);

        check_i32_value(mx_type_val(vec[4])[0], 1);
        check_i32_value(mx_type_val(vec[4])[1], 101);
        check_i32_value(mx_type_val(vec[4])[2], 201);
        check_i32_value(mx_type_val(vec[4])[3], 301);
        check_i32_value(mx_type_val(vec[4])[4], 401);
        check_i32_value(mx_type_val(vec[4])[5], 501);
        check_i32_value(mx_type_val(vec[4])[6], 601);
        check_i32_value(mx_type_val(vec[4])[7], 701);

        check_value(mitr_row(&itr), 8);
        check_value(mitr_column(&itr), 0);

        mitr_get_vectors(&itr, true, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 800);
        check_i32_value(mx_type_val(vec[0])[1], 900);
        check_i32_value(mx_type_val(vec[0])[2], 1000);
        check_i32_value(mx_type_val(vec[0])[3], 1100);
        check_i32_value(mx_type_val(vec[0])[4], 1200);
        check_i32_value(mx_type_val(vec[0])[5], 1300);
        check_i32_value(mx_type_val(vec[0])[6], 1400);
        check_i32_value(mx_type_val(vec[0])[7], 1500);

        check_i32_value(mx_type_val(vec[1])[0], 700);
        check_i32_value(mx_type_val(vec[1])[1], 800);
        check_i32_value(mx_type_val(vec[1])[2], 900);
        check_i32_value(mx_type_val(vec[1])[3], 1000);
        check_i32_value(mx_type_val(vec[1])[4], 1100);
        check_i32_value(mx_type_val(vec[1])[5], 1200);
        check_i32_value(mx_type_val(vec[1])[6], 1300);
        check_i32_value(mx_type_val(vec[1])[7], 1400);

        check_i32_value(mx_type_val(vec[2])[0], 900);
        check_i32_value(mx_type_val(vec[2])[1], 1000);
        check_i32_value(mx_type_val(vec[2])[2], 1100);
        check_i32_value(mx_type_val(vec[2])[3], 1200);
        check_i32_value(mx_type_val(vec[2])[4], 1300);
        check_i32_value(mx_type_val(vec[2])[5], 1400);
        check_i32_value(mx_type_val(vec[2])[6], 1500);
        check_i32_value(mx_type_val(vec[2])[7], 1600);

        check_i32_value(mx_type_val(vec[3])[0], 9999);
        check_i32_value(mx_type_val(vec[3])[1], 9999);
        check_i32_value(mx_type_val(vec[3])[2], 9999);
        check_i32_value(mx_type_val(vec[3])[3], 9999);
        check_i32_value(mx_type_val(vec[3])[4], 9999);
        check_i32_value(mx_type_val(vec[3])[5], 9999);
        check_i32_value(mx_type_val(vec[3])[6], 9999);
        check_i32_value(mx_type_val(vec[3])[7], 9999);

        check_i32_value(mx_type_val(vec[4])[0], 801);
        check_i32_value(mx_type_val(vec[4])[1], 901);
        check_i32_value(mx_type_val(vec[4])[2], 1001);
        check_i32_value(mx_type_val(vec[4])[3], 1101);
        check_i32_value(mx_type_val(vec[4])[4], 1201);
        check_i32_value(mx_type_val(vec[4])[5], 1301);
        check_i32_value(mx_type_val(vec[4])[6], 1401);
        check_i32_value(mx_type_val(vec[4])[7], 1501);

        check_value(mitr_row(&itr), 16);
        check_value(mitr_column(&itr), 0);

        mitr_get_vectors(&itr, true, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 1600);
        check_i32_value(mx_type_val(vec[0])[1], 1700);
        check_i32_value(mx_type_val(vec[0])[2], 9999);
        check_i32_value(mx_type_val(vec[0])[3], 9999);
        check_i32_value(mx_type_val(vec[0])[4], 9999);
        check_i32_value(mx_type_val(vec[0])[5], 9999);
        check_i32_value(mx_type_val(vec[0])[6], 9999);
        check_i32_value(mx_type_val(vec[0])[7], 9999);

        check_i32_value(mx_type_val(vec[1])[0], 1500);
        check_i32_value(mx_type_val(vec[1])[1], 1600);
        check_i32_value(mx_type_val(vec[1])[2], 1700);
        check_i32_value(mx_type_val(vec[1])[3], 9999);
        check_i32_value(mx_type_val(vec[1])[4], 9999);
        check_i32_value(mx_type_val(vec[1])[5], 9999);
        check_i32_value(mx_type_val(vec[1])[6], 9999);
        check_i32_value(mx_type_val(vec[1])[7], 9999);

        check_i32_value(mx_type_val(vec[2])[0], 1700);
        check_i32_value(mx_type_val(vec[2])[1], 9999);
        check_i32_value(mx_type_val(vec[2])[2], 9999);
        check_i32_value(mx_type_val(vec[2])[3], 9999);
        check_i32_value(mx_type_val(vec[2])[4], 9999);
        check_i32_value(mx_type_val(vec[2])[5], 9999);
        check_i32_value(mx_type_val(vec[2])[6], 9999);
        check_i32_value(mx_type_val(vec[2])[7], 9999);

        check_i32_value(mx_type_val(vec[3])[0], 9999);
        check_i32_value(mx_type_val(vec[3])[1], 9999);
        check_i32_value(mx_type_val(vec[3])[2], 9999);
        check_i32_value(mx_type_val(vec[3])[3], 9999);
        check_i32_value(mx_type_val(vec[3])[4], 9999);
        check_i32_value(mx_type_val(vec[3])[5], 9999);
        check_i32_value(mx_type_val(vec[3])[6], 9999);
        check_i32_value(mx_type_val(vec[3])[7], 9999);

        check_i32_value(mx_type_val(vec[4])[0], 1601);
        check_i32_value(mx_type_val(vec[4])[1], 1701);
        check_i32_value(mx_type_val(vec[4])[2], 9999);
        check_i32_value(mx_type_val(vec[4])[3], 9999);
        check_i32_value(mx_type_val(vec[4])[4], 9999);
        check_i32_value(mx_type_val(vec[4])[5], 9999);
        check_i32_value(mx_type_val(vec[4])[6], 9999);
        check_i32_value(mx_type_val(vec[4])[7], 9999);

        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 1);
    }

    // Case: Get five vectors and don't move to next. One of them is beyond the right of the matrix.
    {
        mx_iter_t itr;
        mitr_init_for_iterating_in_columns(&itr, ms, 0, 17, 18, 18);
        mitr_set_default_i32(&itr, 9999);

        v8si_t vec[5];
        mitr_get_vectors(&itr, false, 5, &vec, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1);

        check_i32_value(mx_type_val(vec[0])[0], 17);
        check_i32_value(mx_type_val(vec[0])[1], 117);
        check_i32_value(mx_type_val(vec[0])[2], 217);
        check_i32_value(mx_type_val(vec[0])[3], 317);
        check_i32_value(mx_type_val(vec[0])[4], 417);
        check_i32_value(mx_type_val(vec[0])[5], 517);
        check_i32_value(mx_type_val(vec[0])[6], 617);
        check_i32_value(mx_type_val(vec[0])[7], 717);

        check_i32_value(mx_type_val(vec[1])[0], 9999);
        check_i32_value(mx_type_val(vec[1])[1], 17);
        check_i32_value(mx_type_val(vec[1])[2], 117);
        check_i32_value(mx_type_val(vec[1])[3], 217);
        check_i32_value(mx_type_val(vec[1])[4], 317);
        check_i32_value(mx_type_val(vec[1])[5], 417);
        check_i32_value(mx_type_val(vec[1])[6], 517);
        check_i32_value(mx_type_val(vec[1])[7], 617);

        check_i32_value(mx_type_val(vec[2])[0], 117);
        check_i32_value(mx_type_val(vec[2])[1], 217);
        check_i32_value(mx_type_val(vec[2])[2], 317);
        check_i32_value(mx_type_val(vec[2])[3], 417);
        check_i32_value(mx_type_val(vec[2])[4], 517);
        check_i32_value(mx_type_val(vec[2])[5], 617);
        check_i32_value(mx_type_val(vec[2])[6], 717);
        check_i32_value(mx_type_val(vec[2])[7], 817);

        check_i32_value(mx_type_val(vec[3])[0], 16);
        check_i32_value(mx_type_val(vec[3])[1], 116);
        check_i32_value(mx_type_val(vec[3])[2], 216);
        check_i32_value(mx_type_val(vec[3])[3], 316);
        check_i32_value(mx_type_val(vec[3])[4], 416);
        check_i32_value(mx_type_val(vec[3])[5], 516);
        check_i32_value(mx_type_val(vec[3])[6], 616);
        check_i32_value(mx_type_val(vec[3])[7], 716);

        check_i32_value(mx_type_val(vec[4])[0], 9999);
        check_i32_value(mx_type_val(vec[4])[1], 9999);
        check_i32_value(mx_type_val(vec[4])[2], 9999);
        check_i32_value(mx_type_val(vec[4])[3], 9999);
        check_i32_value(mx_type_val(vec[4])[4], 9999);
        check_i32_value(mx_type_val(vec[4])[5], 9999);
        check_i32_value(mx_type_val(vec[4])[6], 9999);
        check_i32_value(mx_type_val(vec[4])[7], 9999);

        check_value(mitr_row(&itr), 0);
        check_value(mitr_column(&itr), 17);
    }

    mstr_destroy(ms);
}
