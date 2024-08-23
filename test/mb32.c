#include <criterion/criterion.h>

#ifndef MB32_SOURCE
#define MB32_SOURCE 1
#include "src/mb32.c"
#endif

#define check_value(expect, val) \
    cr_expect((expect) == (val), "Wrong value - Expect %d, got %d from '%s'.", expect, val, #val)

#define check_value_at(expect, val, x, y) \
    cr_expect((expect) == (val), "Wrong value - Expect %d, got %d from '%s' at (%d,%d).", expect, val, #val, x, y)

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

Test(PropertyAccess, mb32_rnum)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(1, mb32_rnum(&ms));

        mb32_clean(&ms);
    }
}

Test(PropertyAccess, mb32_cnum)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(2, mb32_cnum(&ms));

        mb32_clean(&ms);
    }
}

Test(PropertyAccess, mb32_padded_rnum)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(8, mb32_padded_rnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);

        check_value(8, mb32_padded_rnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);

        check_value(8, mb32_padded_rnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);

        check_value(8, mb32_padded_rnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);

        check_value(8, mb32_padded_rnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);

        check_value(16, mb32_padded_rnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);

        check_value(8, mb32_padded_rnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);

        check_value(16, mb32_padded_rnum(&ms));

        mb32_clean(&ms);
    }
}

Test(PropertyAccess, mb32_padded_cnum)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(8, mb32_padded_cnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);

        check_value(8, mb32_padded_cnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);

        check_value(16, mb32_padded_cnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);

        check_value(8, mb32_padded_cnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);

        check_value(8, mb32_padded_cnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);

        check_value(8, mb32_padded_cnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);

        check_value(8, mb32_padded_cnum(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);

        check_value(16, mb32_padded_cnum(&ms));

        mb32_clean(&ms);
    }
}

Test(PropertyAccess, mb32_chknum_in_height)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(1, mb32_chknum_in_height(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);

        check_value(1, mb32_chknum_in_height(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);

        check_value(1, mb32_chknum_in_height(&ms));

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);

        check_value(1, mb32_chknum_in_height(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);

        check_value(1, mb32_chknum_in_height(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);

        check_value(2, mb32_chknum_in_height(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);

        check_value(1, mb32_chknum_in_height(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);

        check_value(2, mb32_chknum_in_height(&ms));

        mb32_clean(&ms);
    }
}

Test(PropertyAccess, mb32_chknum_in_width)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(1, mb32_chknum_in_width(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);

        check_value(1, mb32_chknum_in_width(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);

        check_value(2, mb32_chknum_in_width(&ms));

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);

        check_value(1, mb32_chknum_in_width(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);

        check_value(1, mb32_chknum_in_width(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);

        check_value(1, mb32_chknum_in_width(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);

        check_value(1, mb32_chknum_in_width(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);

        check_value(2, mb32_chknum_in_width(&ms));

        mb32_clean(&ms);
    }
}

Test(PropertyAccess, mb32_chknum)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(1, mb32_chknum(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);

        check_value(1, mb32_chknum(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);

        check_value(2, mb32_chknum(&ms));

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);

        check_value(1, mb32_chknum(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);

        check_value(1, mb32_chknum(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);

        check_value(2, mb32_chknum(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);

        check_value(1, mb32_chknum(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);

        check_value(4, mb32_chknum(&ms));

        mb32_clean(&ms);
    }
}

Test(PropertyAccess, mb32_padded_bytes)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);

        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);

        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);

        check_value(512, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);

        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);

        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);

        check_value(512, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);

        check_value(256, mb32_padded_bytes(&ms));

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);

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

Test(Initialization, mb32_i32_init_zeros)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 17x17 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 17x17 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 17, 17, 4, 8);
        mb32_i32_init_zeros(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
            } // for
        } // for

        mb32_clean(&ms);
    }
}

Test(Initialization, mb32_i32_init_ones)
{
    // Case: 1x2 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 2, 4, 8);
        mb32_i32_init_ones(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i < mb32_rnum(&ms) && j < mb32_cnum(&ms)) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 1x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 8, 4, 8);
        mb32_i32_init_ones(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i < mb32_rnum(&ms) && j < mb32_cnum(&ms)) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 1x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 9, 4, 8);
        mb32_i32_init_ones(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i < mb32_rnum(&ms) && j < mb32_cnum(&ms)) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 2x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 2, 1, 4, 8);
        mb32_i32_init_ones(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i < mb32_rnum(&ms) && j < mb32_cnum(&ms)) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 8x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 1, 4, 8);
        mb32_i32_init_ones(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i < mb32_rnum(&ms) && j < mb32_cnum(&ms)) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 9x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 1, 4, 8);
        mb32_i32_init_ones(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i < mb32_rnum(&ms) && j < mb32_cnum(&ms)) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);
        mb32_i32_init_ones(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i < mb32_rnum(&ms) && j < mb32_cnum(&ms)) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);
        mb32_i32_init_ones(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i < mb32_rnum(&ms) && j < mb32_cnum(&ms)) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 17x17 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 17, 17, 4, 8);
        mb32_i32_init_ones(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i < mb32_rnum(&ms) && j < mb32_cnum(&ms)) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }
}

Test(Initialization, mb32_i32_init_identity)
{
    // Case: 1x1 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 1, 1, 4, 8);
        mb32_i32_init_identity(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i == j) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 7x7 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 7, 7, 4, 8);
        mb32_i32_init_identity(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i == j) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 8x8 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 8, 8, 4, 8);
        mb32_i32_init_identity(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i == j) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 9x9 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 9, 9, 4, 8);
        mb32_i32_init_identity(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i == j) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }

    // Case: 17x17 matrix
    {
        mb32_stor_t ms;

        mb32_init(&ms, 17, 17, 4, 8);
        mb32_i32_init_identity(&ms);

        for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
            for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
                if (i == j) {
                    check_value_at(1, mb32_i32_get(&ms, i, j), i, j);
                } else {
                    check_value_at(0, mb32_i32_get(&ms, i, j), i, j);
                } // if
            } // for
        } // for

        mb32_clean(&ms);
    }
}

Test(Internals, i32_chk_zero)
{
    mb32_chk_t dchk;
    i32_chk_zero(&dchk);

    for (int32_t i = 0; i < 8; i += 1) {
        for (int32_t j = 0; j < 8; j += 1) {
            check_value_at(0, dchk.arr.i32[i][j], i, j);
        } // for
    } // for
}

Test(Internals, i32_chk_fill)
{
    // Case: 1x1 chunk
    {
        mb32_chk_t dchk;
        i32_chk_fill(&dchk, 1, 1, 99);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                if (i < 1 && j < 1) {
                    check_value_at(99, dchk.arr.i32[i][j], i, j);
                } else {
                    check_value_at(0, dchk.arr.i32[i][j], i, j);
                } // if
            } // for
        } // for
    }

    // Case: 1x8 chunk
    {
        mb32_chk_t dchk;
        i32_chk_fill(&dchk, 1, 8, 99);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                if (i < 1 && j < 8) {
                    check_value_at(99, dchk.arr.i32[i][j], i, j);
                } else {
                    check_value_at(0, dchk.arr.i32[i][j], i, j);
                } // if
            } // for
        } // for
    }

    // Case: 8x1 chunk
    {
        mb32_chk_t dchk;
        i32_chk_fill(&dchk, 8, 1, 99);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                if (i < 8 && j < 1) {
                    check_value_at(99, dchk.arr.i32[i][j], i, j);
                } else {
                    check_value_at(0, dchk.arr.i32[i][j], i, j);
                } // if
            } // for
        } // for
    }

    // Case: 4x4 chunk
    {
        mb32_chk_t dchk;
        i32_chk_fill(&dchk, 4, 4, 88);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                if (i < 4 && j < 4) {
                    check_value_at(88, dchk.arr.i32[i][j], i, j);
                } else {
                    check_value_at(0, dchk.arr.i32[i][j], i, j);
                } // if
            } // for
        } // for
    }

    // Case: 8x8 chunk
    {
        mb32_chk_t dchk;
        i32_chk_fill(&dchk, 8, 8, 77);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(77, dchk.arr.i32[i][j], i, j);
            } // for
        } // for
    }
}

Test(Internals, i32_chk_copy)
{
    // Case: Copy 1 chunk
    {
        mb32_chk_t dchk;
        mb32_chk_t vchk = {
            .arr.i32 = {
                {36, 72, 108, 144, 180, 216, 252, 288},
                {44, 88, 132, 176, 220, 264, 308, 352},
                {52, 104, 156, 208, 260, 312, 364, 416},
                {60, 120, 180, 240, 300, 360, 420, 480},
                {68, 136, 204, 272, 340, 408, 476, 544},
                {76, 152, 228, 304, 380, 456, 532, 608},
                {84, 168, 252, 336, 420, 504, 588, 672},
                {92, 184, 276, 368, 460, 552, 644, 736},
            },
        };

        memset(&dchk, 0, sizeof(dchk));
        i32_chk_copy(&dchk, 1, 0, &vchk);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(vchk.arr.i32[i][j], dchk.arr.i32[i][j], i, j);
            } // for
        } // for
    }

    // Case: Copy 2 chunks
    {
        mb32_chk_t dchk[2];
        mb32_chk_t vchk = {
            .arr.i32 = {
                {36, 72, 108, 144, 180, 216, 252, 288},
                {44, 88, 132, 176, 220, 264, 308, 352},
                {52, 104, 156, 208, 260, 312, 364, 416},
                {60, 120, 180, 240, 300, 360, 420, 480},
                {68, 136, 204, 272, 340, 408, 476, 544},
                {76, 152, 228, 304, 380, 456, 532, 608},
                {84, 168, 252, 336, 420, 504, 588, 672},
                {92, 184, 276, 368, 460, 552, 644, 736},
            },
        };

        memset(&dchk, 0, sizeof(dchk));
        i32_chk_copy(&dchk[0], 1, 0, &vchk);
        i32_chk_copy(&dchk[0], 2, 0, &vchk);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(vchk.arr.i32[i][j], dchk[0].arr.i32[i][j], i, j);
            } // for
        } // for
        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(vchk.arr.i32[i][j], dchk[1].arr.i32[i][j], i, j);
            } // for
        } // for
    }

    // Case: Copy 2 chunks in the last column
    {
        mb32_chk_t dchk[2][2];
        mb32_chk_t vchk = {
            .arr.i32 = {
                {36, 72, 108, 144, 180, 216, 252, 288},
                {44, 88, 132, 176, 220, 264, 308, 352},
                {52, 104, 156, 208, 260, 312, 364, 416},
                {60, 120, 180, 240, 300, 360, 420, 480},
                {68, 136, 204, 272, 340, 408, 476, 544},
                {76, 152, 228, 304, 380, 456, 532, 608},
                {84, 168, 252, 336, 420, 504, 588, 672},
                {92, 184, 276, 368, 460, 552, 644, 736},
            },
        };

        memset(&dchk, 0, sizeof(dchk));
        i32_chk_copy(&dchk[0][1], 2, 1, &vchk);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(0, dchk[0][0].arr.i32[i][j], i, j);
            } // for
        } // for
        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(vchk.arr.i32[i][j], dchk[0][1].arr.i32[i][j], i, j);
            } // for
        } // for
        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(0, dchk[1][0].arr.i32[i][j], i, j);
            } // for
        } // for
        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(vchk.arr.i32[i][j], dchk[1][1].arr.i32[i][j], i, j);
            } // for
        } // for
    }
}

Test(Internals, i32_chk_transpose)
{
    mb32_chk_t dchk;
    mb32_chk_t vchk = {
        .arr.i32 = {
            {36, 72, 108, 144, 180, 216, 252, 288},
            {44, 88, 132, 176, 220, 264, 308, 352},
            {52, 104, 156, 208, 260, 312, 364, 416},
            {60, 120, 180, 240, 300, 360, 420, 480},
            {68, 136, 204, 272, 340, 408, 476, 544},
            {76, 152, 228, 304, 380, 456, 532, 608},
            {84, 168, 252, 336, 420, 504, 588, 672},
            {92, 184, 276, 368, 460, 552, 644, 736},
        },
    };

    i32_chk_transpose(&dchk, &vchk);

    for (int32_t i = 0; i < 8; i += 1) {
        for (int32_t j = 0; j < 8; j += 1) {
            check_value_at(vchk.arr.i32[j][i], dchk.arr.i32[i][j], i, j);
        } // for
    } // for
}

Test(Internals, i32_chk_add)
{
    mb32_chk_t dchk;
    mb32_chk_t vchk = {
        .arr.i32 = {
            {36, 72, 108, 144, 180, 216, 252, 288},
            {44, 88, 132, 176, 220, 264, 308, 352},
            {52, 104, 156, 208, 260, 312, 364, 416},
            {60, 120, 180, 240, 300, 360, 420, 480},
            {68, 136, 204, 272, 340, 408, 476, 544},
            {76, 152, 228, 304, 380, 456, 532, 608},
            {84, 168, 252, 336, 420, 504, 588, 672},
            {92, 184, 276, 368, 460, 552, 644, 736},
        },
    };

    memset(&dchk, 99, sizeof(dchk));
    i32_chk_add(&dchk, &vchk, &vchk);

    for (int32_t i = 0; i < 8; i += 1) {
        for (int32_t j = 0; j < 8; j += 1) {
            check_value_at(vchk.arr.i32[i][j] * 2, dchk.arr.i32[i][j], i, j);
        } // for
    } // for
}

Test(Internals, i32_chk_sub)
{
    mb32_chk_t dchk;
    mb32_chk_t vchk = {
        .arr.i32 = {
            {36, 72, 108, 144, 180, 216, 252, 288},
            {44, 88, 132, 176, 220, 264, 308, 352},
            {52, 104, 156, 208, 260, 312, 364, 416},
            {60, 120, 180, 240, 300, 360, 420, 480},
            {68, 136, 204, 272, 340, 408, 476, 544},
            {76, 152, 228, 304, 380, 456, 532, 608},
            {84, 168, 252, 336, 420, 504, 588, 672},
            {92, 184, 276, 368, 460, 552, 644, 736},
        },
    };

    memset(&dchk, 99, sizeof(dchk));
    i32_chk_sub(&dchk, &vchk, &vchk);

    for (int32_t i = 0; i < 8; i += 1) {
        for (int32_t j = 0; j < 8; j += 1) {
            check_value_at(0, dchk.arr.i32[i][j], i, j);
        } // for
    } // for
}

Test(Internals, i32_chk_mul)
{
    // Case: 1x1 chunk
    {
        mb32_chk_t dchk;
        mb32_chk_t lchk = {
            .arr.i32 = {
                { 1, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
            },
        };
        mb32_chk_t rchk = { // Transposed already.
            .arr.i32 = {
                { 2, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
            },
        };
        mb32_chk_t vchk = {
            .arr.i32 = {
                { 2, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
            },
        };

        memset(&dchk, 0, sizeof(dchk));
        i32_chk_mul(&dchk, &lchk, &rchk);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(vchk.arr.i32[i][j], dchk.arr.i32[i][j], i, j);
            } // for
        } // for
    }

    // Case: 4x4 chunk
    {
        mb32_chk_t dchk;
        mb32_chk_t lchk = {
            .arr.i32 = {
                { 1, 2, 3, 4, 0, 0, 0, 0},
                { 2, 1, 2, 3, 0, 0, 0, 0},
                { 3, 2, 1, 2, 0, 0, 0, 0},
                { 4, 3, 2, 1, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
            },
        };
        mb32_chk_t rchk = { // Transposed already.
            .arr.i32 = {
                { 1, 1, 1, 1, 0, 0, 0, 0},
                { 1, 1, 1, 1, 0, 0, 0, 0},
                { 1, 1, 1, 1, 0, 0, 0, 0},
                { 1, 1, 1, 1, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0},
            },
        };
        mb32_chk_t vchk = {
            .arr.i32 = {
                {10, 10, 10, 10,  0,  0,  0,  0},
                { 8,  8,  8,  8,  0,  0,  0,  0},
                { 8,  8,  8,  8,  0,  0,  0,  0},
                {10, 10, 10, 10,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0},
            },
        };

        memset(&dchk, 0, sizeof(dchk));
        i32_chk_mul(&dchk, &lchk, &rchk);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(vchk.arr.i32[i][j], dchk.arr.i32[i][j], i, j);
            } // for
        } // for
    }

    // Case: 8x8 chunk
    {
        mb32_chk_t dchk;
        mb32_chk_t lchk = {
            .arr.i32 = {
                { 1,  2,  3,  4,  5,  6,  7,  8},
                { 2,  3,  4,  5,  6,  7,  8,  9},
                { 3,  4,  5,  6,  7,  8,  9, 10},
                { 4,  5,  6,  7,  8,  9, 10, 11},
                { 5,  6,  7,  8,  9, 10, 11, 12},
                { 6,  7,  8,  9, 10, 11, 12, 13},
                { 7,  8,  9, 10, 11, 12, 13, 14},
                { 8,  9, 10, 11, 12, 13, 14, 15},
            },
        };
        mb32_chk_t rchk = { // Transposed.
            .arr.i32 = {
                {1, 1, 1, 1, 1, 1, 1, 1},
                {2, 2, 2, 2, 2, 2, 2, 2},
                {3, 3, 3, 3, 3, 3, 3, 3},
                {4, 4, 4, 4, 4, 4, 4, 4},
                {5, 5, 5, 5, 5, 5, 5, 5},
                {6, 6, 6, 6, 6, 6, 6, 6},
                {7, 7, 7, 7, 7, 7, 7, 7},
                {8, 8, 8, 8, 8, 8, 8, 8},
            },
        };
        mb32_chk_t vchk = {
            .arr.i32 = {
                {36, 72, 108, 144, 180, 216, 252, 288},
                {44, 88, 132, 176, 220, 264, 308, 352},
                {52, 104, 156, 208, 260, 312, 364, 416},
                {60, 120, 180, 240, 300, 360, 420, 480},
                {68, 136, 204, 272, 340, 408, 476, 544},
                {76, 152, 228, 304, 380, 456, 532, 608},
                {84, 168, 252, 336, 420, 504, 588, 672},
                {92, 184, 276, 368, 460, 552, 644, 736},
            },
        };

        memset(&dchk, 0, sizeof(dchk));
        i32_chk_mul(&dchk, &lchk, &rchk);

        for (int32_t i = 0; i < 8; i += 1) {
            for (int32_t j = 0; j < 8; j += 1) {
                check_value_at(vchk.arr.i32[i][j], dchk.arr.i32[i][j], i, j);
            } // for
        } // for
    }
}

Test(Internals, i32_chk_mul_scalar)
{
    mb32_chk_t dchk;
    mb32_chk_t vchk = {
        .arr.i32 = {
            {36, 72, 108, 144, 180, 216, 252, 288},
            {44, 88, 132, 176, 220, 264, 308, 352},
            {52, 104, 156, 208, 260, 312, 364, 416},
            {60, 120, 180, 240, 300, 360, 420, 480},
            {68, 136, 204, 272, 340, 408, 476, 544},
            {76, 152, 228, 304, 380, 456, 532, 608},
            {84, 168, 252, 336, 420, 504, 588, 672},
            {92, 184, 276, 368, 460, 552, 644, 736},
        },
    };

    memset(&dchk, 9999, sizeof(dchk));
    i32_chk_mul_scalar(&dchk, &vchk, 3);

    for (int32_t i = 0; i < 8; i += 1) {
        for (int32_t j = 0; j < 8; j += 1) {
            check_value_at(vchk.arr.i32[i][j] * 3, dchk.arr.i32[i][j], i, j);
        } // for
    } // for
}
