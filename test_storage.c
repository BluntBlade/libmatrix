#include <criterion/criterion.h>

#ifndef MX_STOR_C
#include "mx_storage.c"
#define MX_STOR_C
#endif

// ==== TESTS FOR THE STORAGE MODULE OF MATRIX ==== //

Test(Creation, v8si_create) {
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
