#include <criterion/criterion.h>

#ifndef MB32_STOR_SOURCE
#define MB32_STOR_SOURCE 1
#include "src/mb32_stor.c"
#endif

#define check_value(expect, val) \
    cr_expect((expect) == (val), "Wrong value - Expect %d, got %d from '%s'.", expect, val, #val)

#define check_pointer(expect, val) \
    cr_expect((expect) == (val), "Wrong value - Expect %p, got %p from '%s'.", expect, val, #val)

#define check_non_null(val) \
    cr_expect((val) != NULL, "Wrong value - Expect NON-NULL, got NULL.")

Test(Initialization, mb32_init)
{
    // mb32_init
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(1, ms.rnum);
        check_value(2, ms.cnum);
        check_value(4, ms.val_sz);
        check_value(8, ms.vec_len);

        check_non_null(ms.buf);
        check_non_null(ms.chks);

        check_value(0, ((size_t)ms.chks & (MB32_ALIGNMENT - 1)));
    }
}

Test(PropertyAccess, properties)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(1, mb32_rnum(&ms));
        check_value(2, mb32_cnum(&ms));
        check_value(8, mb32_padded_rnum(&ms));
        check_value(8, mb32_padded_cnum(&ms));
        check_value(1, mb32_chknum_in_height(&ms));
        check_value(1, mb32_chknum_in_width(&ms));
        check_value(1, mb32_chknum(&ms));
        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);

        check_value(1, mb32_rnum(&ms));
        check_value(8, mb32_cnum(&ms));
        check_value(8, mb32_padded_rnum(&ms));
        check_value(8, mb32_padded_cnum(&ms));
        check_value(1, mb32_chknum_in_height(&ms));
        check_value(1, mb32_chknum_in_width(&ms));
        check_value(1, mb32_chknum(&ms));
        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);

        check_value(1, mb32_rnum(&ms));
        check_value(9, mb32_cnum(&ms));
        check_value(8, mb32_padded_rnum(&ms));
        check_value(16, mb32_padded_cnum(&ms));
        check_value(1, mb32_chknum_in_height(&ms));
        check_value(2, mb32_chknum_in_width(&ms));
        check_value(2, mb32_chknum(&ms));
        check_value(512, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);

        check_value(2, mb32_rnum(&ms));
        check_value(1, mb32_cnum(&ms));
        check_value(8, mb32_padded_rnum(&ms));
        check_value(8, mb32_padded_cnum(&ms));
        check_value(1, mb32_chknum_in_height(&ms));
        check_value(1, mb32_chknum_in_width(&ms));
        check_value(1, mb32_chknum(&ms));
        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);

        check_value(8, mb32_rnum(&ms));
        check_value(1, mb32_cnum(&ms));
        check_value(8, mb32_padded_rnum(&ms));
        check_value(8, mb32_padded_cnum(&ms));
        check_value(1, mb32_chknum_in_height(&ms));
        check_value(1, mb32_chknum_in_width(&ms));
        check_value(1, mb32_chknum(&ms));
        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);

        check_value(9, mb32_rnum(&ms));
        check_value(1, mb32_cnum(&ms));
        check_value(16, mb32_padded_rnum(&ms));
        check_value(8, mb32_padded_cnum(&ms));
        check_value(2, mb32_chknum_in_height(&ms));
        check_value(1, mb32_chknum_in_width(&ms));
        check_value(2, mb32_chknum(&ms));
        check_value(512, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);

        check_value(8, mb32_rnum(&ms));
        check_value(8, mb32_cnum(&ms));
        check_value(8, mb32_padded_rnum(&ms));
        check_value(8, mb32_padded_cnum(&ms));
        check_value(1, mb32_chknum_in_height(&ms));
        check_value(1, mb32_chknum_in_width(&ms));
        check_value(1, mb32_chknum(&ms));
        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);

        check_value(9, mb32_rnum(&ms));
        check_value(9, mb32_cnum(&ms));
        check_value(16, mb32_padded_rnum(&ms));
        check_value(16, mb32_padded_cnum(&ms));
        check_value(2, mb32_chknum_in_height(&ms));
        check_value(2, mb32_chknum_in_width(&ms));
        check_value(4, mb32_chknum(&ms));
        check_value(1024, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }
}

Test(Utilities, mb32_chk_locate)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_non_null(mb32_chk_locate(&ms, 0, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 0));

        check_non_null(mb32_chk_locate(&ms, 0, 1));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 1));

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);

        check_non_null(mb32_chk_locate(&ms, 0, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 0));

        check_non_null(mb32_chk_locate(&ms, 0, 7));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 7));

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);

        check_non_null(mb32_chk_locate(&ms, 0, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 0));

        check_non_null(mb32_chk_locate(&ms, 0, 8));
        check_pointer(ms.chks + 1, mb32_chk_locate(&ms, 0, 8));

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);

        check_non_null(mb32_chk_locate(&ms, 0, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 0));

        check_non_null(mb32_chk_locate(&ms, 1, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 1, 0));

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);

        check_non_null(mb32_chk_locate(&ms, 0, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 0));

        check_non_null(mb32_chk_locate(&ms, 7, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 7, 0));

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);

        check_non_null(mb32_chk_locate(&ms, 0, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 0));

        check_non_null(mb32_chk_locate(&ms, 8, 0));
        check_pointer(ms.chks + 1, mb32_chk_locate(&ms, 8, 0));

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);

        check_non_null(mb32_chk_locate(&ms, 0, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 0));

        check_non_null(mb32_chk_locate(&ms, 7, 7));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 7, 7));

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);

        check_non_null(mb32_chk_locate(&ms, 0, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 0));

        check_non_null(mb32_chk_locate(&ms, 7, 8));
        check_pointer(ms.chks + 1, mb32_chk_locate(&ms, 7, 8));

        check_non_null(mb32_chk_locate(&ms, 8, 7));
        check_pointer(ms.chks + 2, mb32_chk_locate(&ms, 8, 7));

        check_non_null(mb32_chk_locate(&ms, 8, 8));
        check_pointer(ms.chks + 3, mb32_chk_locate(&ms, 8, 8));

        mb32_clean(&ms);
    }

    // Case: 17x17 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 17, 17, 4, 8);

        check_non_null(mb32_chk_locate(&ms, 0, 0));
        check_pointer(ms.chks, mb32_chk_locate(&ms, 0, 0));

        check_non_null(mb32_chk_locate(&ms, 8, 8));
        check_pointer(ms.chks + 4, mb32_chk_locate(&ms, 8, 8));

        check_non_null(mb32_chk_locate(&ms, 15, 15));
        check_pointer(ms.chks + 4, mb32_chk_locate(&ms, 15, 15));

        check_non_null(mb32_chk_locate(&ms, 16, 16));
        check_pointer(ms.chks + 8, mb32_chk_locate(&ms, 16, 16));

        mb32_clean(&ms);
    }
}

Test(Utilities, mb32_chk_delta)
{
    check_value(1, mb32_chk_delta(1));
    check_value(2, mb32_chk_delta(2));
    check_value(0, mb32_chk_delta(8));
    check_value(1, mb32_chk_delta(9));
}
