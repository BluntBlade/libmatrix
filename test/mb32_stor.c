#include <criterion/criterion.h>

#ifndef MB32_STOR_SOURCE
#define MB32_STOR_SOURCE 1
#include "src/mb32_stor.c"
#endif

#define check_value(expect, val) \
    cr_expect((expect) == (val), "Wrong value - Expect %d, got %d from '%s'.", expect, val, #val)

#define check_non_null(val) \
    cr_expect((val) != NULL, "Wrong value - Expect NON-NULL, got NULL.")

Test(MB32, Initialization)
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

Test(MB32, PropertyAccess)
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
        check_value(256, mb32_padded_bytes(&ms));
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
        check_value(256, mb32_padded_bytes(&ms));
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
        check_value(512, mb32_padded_bytes(&ms));
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
        check_value(256, mb32_padded_bytes(&ms));
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
        check_value(256, mb32_padded_bytes(&ms));
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
        check_value(512, mb32_padded_bytes(&ms));
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
        check_value(256, mb32_padded_bytes(&ms));
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
        check_value(1024, mb32_padded_bytes(&ms));
    }
}
