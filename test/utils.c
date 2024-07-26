#include <criterion/criterion.h>

#include "src/mx_utils.h"

#define check_value(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %d, got %d.", expect, val)

Test(Bisearch, mstr_v8si_bisearch)
{
    int32_t range[] = {
        -10, -10, -5,  0,  5, 10, 15, 20,
         20,  20, 25, 30, 30, 35, 40, 45,
         50,  55, 55,
        };
    v8si_t src = { .val = {9, -12, 37, 1, 25, 60, -5, 22} };
    v8si_t idx = { .val = {0} };

    mstr_v8si_bisearch(&idx, range, sizeof(range) / sizeof(range[0]), &src);

    check_value(mx_type_val(idx)[0], 5);
    check_value(mx_type_val(idx)[1], 0);
    check_value(mx_type_val(idx)[2], 14);
    check_value(mx_type_val(idx)[3], 4);
    check_value(mx_type_val(idx)[4], 11);
    check_value(mx_type_val(idx)[5], 19);
    check_value(mx_type_val(idx)[6], 3);
    check_value(mx_type_val(idx)[7], 10);
}

Test(Bisearch, mstr_v8sf_bisearch)
{
    float range[] = {
        -10.5, -10.0, -5.123,  0.0,  5.456, 10.0, 15.0, 20.2,
         20.2,  20.3, 25.123, 30.0,   30.0, 35.1, 40.3, 45.5,
         50.7,  55.0, 55.0,
        };
    v8sf_t src = { .val = {9.0, -12.6, 37.7, 1.1, 25.0, 60.0, -5.0, 22.0} };
    v8si_t idx = { .val = {0} };

    mstr_v8sf_bisearch(&idx, range, sizeof(range) / sizeof(range[0]), &src);

    check_value(mx_type_val(idx)[0], 5);
    check_value(mx_type_val(idx)[1], 0);
    check_value(mx_type_val(idx)[2], 14);
    check_value(mx_type_val(idx)[3], 4);
    check_value(mx_type_val(idx)[4], 10);
    check_value(mx_type_val(idx)[5], 19);
    check_value(mx_type_val(idx)[6], 3);
    check_value(mx_type_val(idx)[7], 10);
}
