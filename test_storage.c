#include <criterion/criterion.h>

#ifndef MX_STOR_C
#include "mx_storage.c"
#define MX_STOR_C
#endif

// ==== TESTS FOR THE STORAGE MODULE OF MATRIX ==== //

Test(Creat, v8si_create) {
    mx_stor_ptr ms = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        cr_expect(ms->val_sz == 4, "Wrong size of the value in the matrix - Expect %d, got %d.", 4, ms->val_sz);
        cr_expect(ms->rows == 1, "Wrong number of rows - Expect %d, got %d.", 1, ms->rows);
        cr_expect(ms->cols == 1, "Wrong number of columns - Expect %d, got %d.", 1, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 1 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 1 * 8, ms->bytes);
        cr_expect(ms->pck_len == 8, "Wrong length of one pack - Expect %d, got %d.", 8, ms->pck_len);
        cr_expect(ms->chk_len == 16, "Wrong length of one chunk - Expect %d, got %d.", 16, ms->chk_len);
        cr_expect(ms->last_chk_width == 1, "Wrong width of the last chunk - Expect %d, got %d.", 1, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 1, "Wrong height of the last chunk - Expect %d, got %d.", 1, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);
        cr_expect(ms->alignment == 32, "Wrong alignment boundary - Expect %d, got %d.", 32, ms->alignment);
        cr_expect(((size_t)ms->data & 0x1F) == 0, "The buffer is not aligned correctly - Expect %d, got %d.", 0, ((size_t)ms->data & 0x1F));

        mstr_v8si_destroy(ms);
    }

    // -- 7x7 matrix -- //
    {
        ms = mstr_v8si_create(7, 7);

        cr_expect(ms->rows == 7, "Wrong number of rows - Expect %d, got %d.", 7, ms->rows);
        cr_expect(ms->cols == 7, "Wrong number of columns - Expect %d, got %d.", 7, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 7 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 7 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 7, "Wrong width of the last chunk - Expect %d, got %d.", 7, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 7, "Wrong height of the last chunk - Expect %d, got %d.", 7, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        cr_expect(ms->rows == 8, "Wrong number of rows - Expect %d, got %d.", 8, ms->rows);
        cr_expect(ms->cols == 8, "Wrong number of columns - Expect %d, got %d.", 8, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 8 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 8 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 8, "Wrong width of the last chunk - Expect %d, got %d.", 8, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 8, "Wrong height of the last chunk - Expect %d, got %d.", 8, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 9x9 matrix -- //
    {
        ms = mstr_v8si_create(9, 9);

        cr_expect(ms->rows == 9, "Wrong number of rows - Expect %d, got %d.", 9, ms->rows);
        cr_expect(ms->cols == 9, "Wrong number of columns - Expect %d, got %d.", 9, ms->cols);
        cr_expect(ms->cols_padded == 16, "Wrong number of columns, including padded ones - Expect %d, got %d.", 16, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 9 * 16, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 9 * 16, ms->bytes);
        cr_expect(ms->last_chk_width == 9, "Wrong width of the last chunk - Expect %d, got %d.", 9, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 9, "Wrong height of the last chunk - Expect %d, got %d.", 9, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_v8si_create(15, 15);

        cr_expect(ms->rows == 15, "Wrong number of rows - Expect %d, got %d.", 15, ms->rows);
        cr_expect(ms->cols == 15, "Wrong number of columns - Expect %d, got %d.", 15, ms->cols);
        cr_expect(ms->cols_padded == 16, "Wrong number of columns, including padded ones - Expect %d, got %d.", 16, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 15 * 16, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 15 * 16, ms->bytes);
        cr_expect(ms->last_chk_width == 15, "Wrong width of the last chunk - Expect %d, got %d.", 15, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 15, "Wrong height of the last chunk - Expect %d, got %d.", 15, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        cr_expect(ms->rows == 16, "Wrong number of rows - Expect %d, got %d.", 16, ms->rows);
        cr_expect(ms->cols == 16, "Wrong number of columns - Expect %d, got %d.", 16, ms->cols);
        cr_expect(ms->cols_padded == 16, "Wrong number of columns, including padded ones - Expect %d, got %d.", 16, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 16 * 16, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 16 * 16, ms->bytes);
        cr_expect(ms->last_chk_width == 16, "Wrong width of the last chunk - Expect %d, got %d.", 16, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 16, "Wrong height of the last chunk - Expect %d, got %d.", 16, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        cr_expect(ms->rows == 17, "Wrong number of rows - Expect %d, got %d.", 17, ms->rows);
        cr_expect(ms->cols == 17, "Wrong number of columns - Expect %d, got %d.", 17, ms->cols);
        cr_expect(ms->cols_padded == 24, "Wrong number of columns, including padded ones - Expect %d, got %d.", 24, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 17 * 24, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 17 * 24, ms->bytes);
        cr_expect(ms->last_chk_width == 1, "Wrong width of the last chunk - Expect %d, got %d.", 1, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 1, "Wrong height of the last chunk - Expect %d, got %d.", 1, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 2, "Wrong number of chunks in width - Expect %d, got %d.", 2, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 2, "Wrong number of chunks in height - Expect %d, got %d.", 2, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 2x7 matrix -- //
    {
        ms = mstr_v8si_create(2, 7);

        cr_expect(ms->rows == 2, "Wrong number of rows - Expect %d, got %d.", 2, ms->rows);
        cr_expect(ms->cols == 7, "Wrong number of columns - Expect %d, got %d.", 7, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 2 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 2 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 7, "Wrong width of the last chunk - Expect %d, got %d.", 7, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 2, "Wrong height of the last chunk - Expect %d, got %d.", 2, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 2x8 matrix -- //
    {
        ms = mstr_v8si_create(2, 8);

        cr_expect(ms->rows == 2, "Wrong number of rows - Expect %d, got %d.", 2, ms->rows);
        cr_expect(ms->cols == 8, "Wrong number of columns - Expect %d, got %d.", 8, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 2 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 2 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 8, "Wrong width of the last chunk - Expect %d, got %d.", 8, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 2, "Wrong height of the last chunk - Expect %d, got %d.", 2, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 2x9 matrix -- //
    {
        ms = mstr_v8si_create(2, 9);

        cr_expect(ms->rows == 2, "Wrong number of rows - Expect %d, got %d.", 2, ms->rows);
        cr_expect(ms->cols == 9, "Wrong number of columns - Expect %d, got %d.", 9, ms->cols);
        cr_expect(ms->cols_padded == 16, "Wrong number of columns, including padded ones - Expect %d, got %d.", 16, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 2 * 16, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 2 * 16, ms->bytes);
        cr_expect(ms->last_chk_width == 9, "Wrong width of the last chunk - Expect %d, got %d.", 9, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 2, "Wrong height of the last chunk - Expect %d, got %d.", 2, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 2x15 matrix -- //
    {
        ms = mstr_v8si_create(2, 15);

        cr_expect(ms->rows == 2, "Wrong number of rows - Expect %d, got %d.", 2, ms->rows);
        cr_expect(ms->cols == 15, "Wrong number of columns - Expect %d, got %d.", 15, ms->cols);
        cr_expect(ms->cols_padded == 16, "Wrong number of columns, including padded ones - Expect %d, got %d.", 16, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 2 * 16, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 2 * 16, ms->bytes);
        cr_expect(ms->last_chk_width == 15, "Wrong width of the last chunk - Expect %d, got %d.", 15, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 2, "Wrong height of the last chunk - Expect %d, got %d.", 2, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 2x16 matrix -- //
    {
        ms = mstr_v8si_create(2, 16);

        cr_expect(ms->rows == 2, "Wrong number of rows - Expect %d, got %d.", 2, ms->rows);
        cr_expect(ms->cols == 16, "Wrong number of columns - Expect %d, got %d.", 16, ms->cols);
        cr_expect(ms->cols_padded == 16, "Wrong number of columns, including padded ones - Expect %d, got %d.", 16, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 2 * 16, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 2 * 16, ms->bytes);
        cr_expect(ms->last_chk_width == 16, "Wrong width of the last chunk - Expect %d, got %d.", 16, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 2, "Wrong height of the last chunk - Expect %d, got %d.", 2, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 2x17 matrix -- //
    {
        ms = mstr_v8si_create(2, 17);

        cr_expect(ms->rows == 2, "Wrong number of rows - Expect %d, got %d.", 2, ms->rows);
        cr_expect(ms->cols == 17, "Wrong number of columns - Expect %d, got %d.", 17, ms->cols);
        cr_expect(ms->cols_padded == 24, "Wrong number of columns, including padded ones - Expect %d, got %d.", 24, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 2 * 24, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 2 * 24, ms->bytes);
        cr_expect(ms->last_chk_width == 1, "Wrong width of the last chunk - Expect %d, got %d.", 16, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 2, "Wrong height of the last chunk - Expect %d, got %d.", 2, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 2, "Wrong number of chunks in width - Expect %d, got %d.", 2, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 7x2 matrix -- //
    {
        ms = mstr_v8si_create(7, 2);

        cr_expect(ms->rows == 7, "Wrong number of rows - Expect %d, got %d.", 7, ms->rows);
        cr_expect(ms->cols == 2, "Wrong number of columns - Expect %d, got %d.", 2, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 7 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 7 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 2, "Wrong width of the last chunk - Expect %d, got %d.", 2, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 7, "Wrong height of the last chunk - Expect %d, got %d.", 7, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 8x2 matrix -- //
    {
        ms = mstr_v8si_create(8, 2);

        cr_expect(ms->rows == 8, "Wrong number of rows - Expect %d, got %d.", 8, ms->rows);
        cr_expect(ms->cols == 2, "Wrong number of columns - Expect %d, got %d.", 2, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 8 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 8 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 2, "Wrong width of the last chunk - Expect %d, got %d.", 2, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 8, "Wrong height of the last chunk - Expect %d, got %d.", 8, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 9x2 matrix -- //
    {
        ms = mstr_v8si_create(9, 2);

        cr_expect(ms->rows == 9, "Wrong number of rows - Expect %d, got %d.", 9, ms->rows);
        cr_expect(ms->cols == 2, "Wrong number of columns - Expect %d, got %d.", 2, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 9 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 9 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 2, "Wrong width of the last chunk - Expect %d, got %d.", 2, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 9, "Wrong height of the last chunk - Expect %d, got %d.", 9, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 15x2 matrix -- //
    {
        ms = mstr_v8si_create(15, 2);

        cr_expect(ms->rows == 15, "Wrong number of rows - Expect %d, got %d.", 15, ms->rows);
        cr_expect(ms->cols == 2, "Wrong number of columns - Expect %d, got %d.", 2, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 15 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 15 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 2, "Wrong width of the last chunk - Expect %d, got %d.", 2, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 15, "Wrong height of the last chunk - Expect %d, got %d.", 15, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 16x2 matrix -- //
    {
        ms = mstr_v8si_create(16, 2);

        cr_expect(ms->rows == 16, "Wrong number of rows - Expect %d, got %d.", 16, ms->rows);
        cr_expect(ms->cols == 2, "Wrong number of columns - Expect %d, got %d.", 2, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 16 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 16 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 2, "Wrong width of the last chunk - Expect %d, got %d.", 2, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 16, "Wrong height of the last chunk - Expect %d, got %d.", 16, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }

    // -- 17x2 matrix -- //
    {
        ms = mstr_v8si_create(17, 2);

        cr_expect(ms->rows == 17, "Wrong number of rows - Expect %d, got %d.", 17, ms->rows);
        cr_expect(ms->cols == 2, "Wrong number of columns - Expect %d, got %d.", 2, ms->cols);
        cr_expect(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
        cr_expect(ms->bytes == 4 * 17 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 17 * 8, ms->bytes);
        cr_expect(ms->last_chk_width == 2, "Wrong width of the last chunk - Expect %d, got %d.", 2, ms->last_chk_width);
        cr_expect(ms->last_chk_height == 1, "Wrong height of the last chunk - Expect %d, got %d.", 1, ms->last_chk_height);
        cr_expect(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
        cr_expect(ms->chks_in_height == 2, "Wrong number of chunks in height - Expect %d, got %d.", 2, ms->chks_in_height);

        mstr_v8si_destroy(ms);
    }
}

Test(Locate, v8si_calc_base) {
    uint32_t off = 0;
    mx_stor_ptr ms = NULL;
    void * base = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        base = v8si_calc_base(ms, 0, 0, ms->last_chk_height);
        off = 0;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        base = v8si_calc_base(ms, 0, 0, ms->last_chk_height);
        off = 0;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        mstr_v8si_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_v8si_create(15, 15);

        base = v8si_calc_base(ms, 0, 0, ms->last_chk_height);
        off = 0;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        base = v8si_calc_base(ms, 0, 0, ms->last_chk_height);
        off = 0;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        base = v8si_calc_base(ms, 0, 0, 16);
        off = 0;
        cr_expect(base - ms->data == 0, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 0, 16, 16);
        off = 4 * 16 * 16;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 16, 0, ms->last_chk_height);
        off = 4 * 16 * 16 + 4 * 16 * 8;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 16, 16, ms->last_chk_height);
        off = 4 * 16 * 24 + 4 * 1 * 16;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        mstr_v8si_destroy(ms);
    }

    // -- 2x17 matrix -- //
    {
        ms = mstr_v8si_create(2, 17);

        base = v8si_calc_base(ms, 0, 0, 2);
        off = 0;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 0, 16, ms->last_chk_height);
        off = 4 * 2 * 16;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        mstr_v8si_destroy(ms);
    }

    // -- 2x33 matrix -- //
    {
        ms = mstr_v8si_create(2, 33);

        base = v8si_calc_base(ms, 0, 0, 2);
        off = 0;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 0, 16, 2);
        off = 4 * 2 * 16;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 0, 32, ms->last_chk_height);
        off = 4 * 2 * 16 * 2;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        mstr_v8si_destroy(ms);
    }

    // -- 17x33 matrix -- //
    {
        ms = mstr_v8si_create(17, 33);

        base = v8si_calc_base(ms, 0, 0, 16);
        off = 0;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 0, 16, 16);
        off = 4 * 16 * 16;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 0, 32, 16);
        off = 4 * 16 * 16 * 2;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 16, 0, ms->last_chk_height);
        off = 4 * 16 * 16 * 2 + 4 * 16 * 8;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);
        
        base = v8si_calc_base(ms, 16, 16, ms->last_chk_height);
        off = 4 * 16 * 16 * 2 + 4 * 16 * 8 + 4 * 1 * 16;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 16, 32, ms->last_chk_height);
        off = 4 * 16 * 16 * 2 + 4 * 16 * 8 + 4 * 1 * 16 * 2;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        mstr_v8si_destroy(ms);
    }

    // -- 17x2 matrix -- //
    {
        ms = mstr_v8si_create(17, 2);

        base = v8si_calc_base(ms, 0, 0, 16);
        off = 0;
        cr_expect(base - ms->data == 0, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 16, 0, ms->last_chk_height);
        off = 4 * 16 * 8;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        mstr_v8si_destroy(ms);
    }

    // -- 33x2 matrix -- //
    {
        ms = mstr_v8si_create(33, 2);

        base = v8si_calc_base(ms, 0, 0, 16);
        off = 0;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 16, 0, 16);
        off = 4 * 16 * 8;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 32, 0, ms->last_chk_height);
        off = 4 * 32 * 8;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        mstr_v8si_destroy(ms);
    }

    // -- 33x17 matrix -- //
    {
        ms = mstr_v8si_create(33, 17);

        base = v8si_calc_base(ms, 0, 0, 16);
        off = 0;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 0, 16, 16);
        off = 4 * 16 * 16;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 16, 0, 16);
        off = 4 * 16 * 16 + 4 * 16 * 8;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 16, 16, 16);
        off = 4 * 16 * 16 * 2 + 4 * 16 * 8;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 32, 0, ms->last_chk_height);
        off = 4 * 16 * 16 * 2 + 4 * 16 * 8 * 2;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);

        base = v8si_calc_base(ms, 32, 16, ms->last_chk_height);
        off = 4 * 16 * 16 * 2 + 4 * 16 * 8 * 2 + 4 * 1 * 16;
        cr_expect(base - ms->data == off, "Wrong base - Expect %p, got %p.", ms->data + off, base);
    }
}

Test(Locate, v8si_locate_chunk) {
    mx_stor_ptr ms = NULL;
    uint32_t off = 0;
    uint32_t rows_in_chk = 0;
    uint32_t cols_in_chk = 0;;
    bool full = false;
    mx_chunk_ptr chk = NULL;

    // -- 1x1 matrix -- //
    {
        ms = mstr_v8si_create(1, 1);

        chk = v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        off = 0;
        cr_expect((void *)chk - ms->data == off, "Wrong address of the chunk - Expect %p, got %p.", ms->data + off, chk);
        cr_expect(rows_in_chk == 1, "Wrong number of rows in the chunk - Expect %d, got %d.", 1, rows_in_chk);
        cr_expect(cols_in_chk == 1, "Wrong number of columns in the chunk - Expect %d, got %d.", 1, cols_in_chk);
        cr_expect(full == false, "Wrong full flag of the chunk - Expect %d, got %d.", false, full);

        mstr_v8si_destroy(ms);
    }

    // -- 8x8 matrix -- //
    {
        ms = mstr_v8si_create(8, 8);

        chk = v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        off = 0;
        cr_expect((void *)chk - ms->data == off, "Wrong address of the chunk - Expect %p, got %p.", ms->data + off, chk);
        cr_expect(rows_in_chk == 8, "Wrong number of rows in the chunk - Expect %d, got %d.", 8, rows_in_chk);
        cr_expect(cols_in_chk == 8, "Wrong number of columns in the chunk - Expect %d, got %d.", 8, cols_in_chk);
        cr_expect(full == false, "Wrong full flag of the chunk - Expect %d, got %d.", false, full);

        mstr_v8si_destroy(ms);
    }

    // -- 15x15 matrix -- //
    {
        ms = mstr_v8si_create(15, 15);

        chk = v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        off = 0;
        cr_expect((void *)chk - ms->data == off, "Wrong address of the chunk - Expect %p, got %p.", ms->data + off, chk);
        cr_expect(rows_in_chk == 15, "Wrong number of rows in the chunk - Expect %d, got %d.", 15, rows_in_chk);
        cr_expect(cols_in_chk == 15, "Wrong number of columns in the chunk - Expect %d, got %d.", 15, cols_in_chk);
        cr_expect(full == false, "Wrong full flag of the chunk - Expect %d, got %d.", false, full);

        mstr_v8si_destroy(ms);
    }

    // -- 16x16 matrix -- //
    {
        ms = mstr_v8si_create(16, 16);

        chk = v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        off = 0;
        cr_expect((void *)chk - ms->data == off, "Wrong address of the chunk - Expect %p, got %p.", ms->data + off, chk);
        cr_expect(rows_in_chk == 16, "Wrong number of rows in the chunk - Expect %d, got %d.", 16, rows_in_chk);
        cr_expect(cols_in_chk == 16, "Wrong number of columns in the chunk - Expect %d, got %d.", 16, cols_in_chk);
        cr_expect(full == true, "Wrong full flag of the chunk - Expect %d, got %d.", true, full);

        mstr_v8si_destroy(ms);
    }

    // -- 17x17 matrix -- //
    {
        ms = mstr_v8si_create(17, 17);

        chk = v8si_locate_chunk(ms, 0, 0, &rows_in_chk, &cols_in_chk, &full);
        off = 0;
        cr_expect((void *)chk - ms->data == off, "Wrong address of the chunk - Expect %p, got %p.", ms->data + off, chk);
        cr_expect(rows_in_chk == 16, "Wrong number of rows in the chunk - Expect %d, got %d.", 16, rows_in_chk);
        cr_expect(cols_in_chk == 16, "Wrong number of columns in the chunk - Expect %d, got %d.", 16, cols_in_chk);
        cr_expect(full == true, "Wrong full flag of the chunk - Expect %d, got %d.", true, full);

        chk = v8si_locate_chunk(ms, 0, 1, &rows_in_chk, &cols_in_chk, &full);
        off = 4 * 16 * 16;
        cr_expect((void *)chk - ms->data == off, "Wrong address of the chunk - Expect %p, got %p.", ms->data + off, chk);
        cr_expect(rows_in_chk == 16, "Wrong number of rows in the chunk - Expect %d, got %d.", 16, rows_in_chk);
        cr_expect(cols_in_chk == 1, "Wrong number of columns in the chunk - Expect %d, got %d.", 1, cols_in_chk);
        cr_expect(full == false, "Wrong full flag of the chunk - Expect %d, got %d.", false, full);

        chk = v8si_locate_chunk(ms, 1, 0, &rows_in_chk, &cols_in_chk, &full);
        off = 4 * 16 * 16 + 4 * 16 * 8;
        cr_expect((void *)chk - ms->data == off, "Wrong address of the chunk - Expect %p, got %p.", ms->data + off, chk);
        cr_expect(rows_in_chk == 1, "Wrong number of rows in the chunk - Expect %d, got %d.", 1, rows_in_chk);
        cr_expect(cols_in_chk == 16, "Wrong number of columns in the chunk - Expect %d, got %d.", 16, cols_in_chk);
        cr_expect(full == false, "Wrong full flag of the chunk - Expect %d, got %d.", false, full);

        chk = v8si_locate_chunk(ms, 1, 1, &rows_in_chk, &cols_in_chk, &full);
        off = 4 * 16 * 16 + 4 * 16 * 8 + 4 * 1 * 16;
        cr_expect((void *)chk - ms->data == off, "Wrong address of the chunk - Expect %p, got %p.", ms->data + off, chk);
        cr_expect(rows_in_chk == 1, "Wrong number of rows in the chunk - Expect %d, got %d.", 1, rows_in_chk);
        cr_expect(cols_in_chk == 1, "Wrong number of columns in the chunk - Expect %d, got %d.", 1, cols_in_chk);
        cr_expect(full == false, "Wrong full flag of the chunk - Expect %d, got %d.", false, full);

        mstr_v8si_destroy(ms);
    }
}
