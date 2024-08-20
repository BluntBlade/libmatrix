#include <criterion/criterion.h>

#ifndef MB32_STOR_SOURCE
#define MB32_STOR_SOURCE 1
#include "src/mb32_stor.c"
#endif

#define check_value(expect, val) \
    cr_expect((expect) == (val), "Wrong value - Expect %d, got %d from '%s'.", expect, val, #val)

#define check_non_null(val) \
    cr_expect((val) != NULL, "Wrong value - Expect NON-NULL, got NULL.")

Test(Initialization, mb32_init)
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
