#include <criterion/criterion.h>

#ifndef MX_STOR_C
#include "mx_storage.c"
#define MX_STOR_C
#endif

/* ==== TESTS FOR THE STORAGE MODULE OF MATRIX ==== */

Test(Creation, v8si_create) {
    mx_stor_ptr ms = mstr_v8si_create(1, 1);

    cr_assert(ms->val_sz == 4, "Wrong size of the value in the matrix - Expect %d, got %d.", 4, ms->val_sz);
    cr_assert(ms->rows == 1, "Wrong number of rows - Expect %d, got %d.", 1, ms->rows);
    cr_assert(ms->cols == 1, "Wrong number of columns - Expect %d, got %d.", 1, ms->cols);
    cr_assert(ms->cols_padded == 8, "Wrong number of columns, including padded ones - Expect %d, got %d.", 8, ms->cols_padded);
    cr_assert(ms->bytes == 4 * 1 * 8, "Wrong size of the allocating bytes of the buffer - Expect %d, got %d.", 4 * 1 * 8, ms->bytes);
    cr_assert(ms->pck_len == 8, "Wrong length of one pack - Expect %d, got %d.", 8, ms->pck_len);
    cr_assert(ms->chk_len == 16, "Wrong length of one chunk - Expect %d, got %d.", 16, ms->chk_len);
    cr_assert(ms->last_chk_width == 1, "Wrong width of the last chunk - Expect %d, got %d.", 1, ms->last_chk_width);
    cr_assert(ms->last_chk_height == 1, "Wrong height of the last chunk - Expect %d, got %d.", 1, ms->last_chk_height);
    cr_assert(ms->chks_in_width == 1, "Wrong number of chunks in width - Expect %d, got %d.", 1, ms->chks_in_width);
    cr_assert(ms->chks_in_height == 1, "Wrong number of chunks in height - Expect %d, got %d.", 1, ms->chks_in_height);
    cr_assert(ms->alignment == 32, "Wrong alignment boundary - Expect %d, got %d.", 32, ms->alignment);
    cr_assert(((size_t)ms->data & 0x1F) == 0, "The buffer is not aligned correctly - Expect %d, got %d.", 0, ((size_t)ms->data & 0x1F));

    mstr_destroy(ms);
}
