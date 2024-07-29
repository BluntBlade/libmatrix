#include <criterion/criterion.h>

#include "src/mx_utils.h"

#define check_value(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %d, got %d.", expect, val)

Test(Bisearch, mx_v8si_bisearch)
{
    // Case: The length of search range is less than 16.
    {
        int32_t range[15] = {
            -10, -10, -5,  0,  5, 10, 15, 20,
             20,  20, 25, 30, 30, 35, 40,
        };
        v8si_t src = { .val = {9, -12, 37, 1, 25, 60, -5, 22} };
        v8si_t idx = { .val = {0} };

        mx_v8si_bisearch(&idx, range, sizeof(range) / sizeof(range[0]), &src);

        check_value(mx_type_val(idx)[0], 5);
        check_value(mx_type_val(idx)[1], 0);
        check_value(mx_type_val(idx)[2], 14);
        check_value(mx_type_val(idx)[3], 4);
        check_value(mx_type_val(idx)[4], 11);
        check_value(mx_type_val(idx)[5], 15);
        check_value(mx_type_val(idx)[6], 3);
        check_value(mx_type_val(idx)[7], 10);
    }

    // Case: The length of search range is equal to 16.
    {
        int32_t range[16] = {
            -10, -10, -5,  0,  5, 10, 15, 20,
             20,  20, 25, 30, 30, 35, 40, 45,
        };
        v8si_t src = { .val = {9, -12, 37, 1, 25, 60, -5, 22} };
        v8si_t idx = { .val = {0} };

        mx_v8si_bisearch(&idx, range, sizeof(range) / sizeof(range[0]), &src);

        check_value(mx_type_val(idx)[0], 5);
        check_value(mx_type_val(idx)[1], 0);
        check_value(mx_type_val(idx)[2], 14);
        check_value(mx_type_val(idx)[3], 4);
        check_value(mx_type_val(idx)[4], 11);
        check_value(mx_type_val(idx)[5], 16);
        check_value(mx_type_val(idx)[6], 3);
        check_value(mx_type_val(idx)[7], 10);
    }

    // Case: The length of search range is greater than 16.
    {
        int32_t range[19] = {
            -10, -10, -5,  0,  5, 10, 15, 20,
             20,  20, 25, 30, 30, 35, 40, 45,
             50,  55, 55,
        };
        v8si_t src = { .val = {9, -12, 37, 1, 25, 60, -5, 22} };
        v8si_t idx = { .val = {0} };

        mx_v8si_bisearch(&idx, range, sizeof(range) / sizeof(range[0]), &src);

        check_value(mx_type_val(idx)[0], 5);
        check_value(mx_type_val(idx)[1], 0);
        check_value(mx_type_val(idx)[2], 14);
        check_value(mx_type_val(idx)[3], 4);
        check_value(mx_type_val(idx)[4], 11);
        check_value(mx_type_val(idx)[5], 19);
        check_value(mx_type_val(idx)[6], 3);
        check_value(mx_type_val(idx)[7], 10);
    }
}

Test(Bisearch, mx_v8sf_bisearch)
{
    // Case: The length of search range is less than 16.
    {
        float range[15] = {
            -10.5, -10.0, -5.123,  0.0,  5.456, 10.0, 15.0, 20.2,
             20.2,  20.3, 25.123, 30.0,   30.0, 35.1, 40.3,
        };
        v8sf_t src = { .val = {9.0, -12.6, 37.7, 1.1, 25.0, 60.0, -5.0, 22.0} };
        v8si_t idx = { .val = {0} };

        mx_v8sf_bisearch(&idx, range, sizeof(range) / sizeof(range[0]), &src);

        check_value(mx_type_val(idx)[0], 5);
        check_value(mx_type_val(idx)[1], 0);
        check_value(mx_type_val(idx)[2], 14);
        check_value(mx_type_val(idx)[3], 4);
        check_value(mx_type_val(idx)[4], 10);
        check_value(mx_type_val(idx)[5], 15);
        check_value(mx_type_val(idx)[6], 3);
        check_value(mx_type_val(idx)[7], 10);
    }

    // Case: The length of search range is equal to 16.
    {
        float range[16] = {
            -10.5, -10.0, -5.123,  0.0,  5.456, 10.0, 15.0, 20.2,
             20.2,  20.3, 25.123, 30.0,   30.0, 35.1, 40.3, 45.5,
        };
        v8sf_t src = { .val = {9.0, -12.6, 37.7, 1.1, 25.0, 60.0, -5.0, 22.0} };
        v8si_t idx = { .val = {0} };

        mx_v8sf_bisearch(&idx, range, sizeof(range) / sizeof(range[0]), &src);

        check_value(mx_type_val(idx)[0], 5);
        check_value(mx_type_val(idx)[1], 0);
        check_value(mx_type_val(idx)[2], 14);
        check_value(mx_type_val(idx)[3], 4);
        check_value(mx_type_val(idx)[4], 10);
        check_value(mx_type_val(idx)[5], 16);
        check_value(mx_type_val(idx)[6], 3);
        check_value(mx_type_val(idx)[7], 10);
    }

    // Case: The length of search range is greater than 16.
    {
        float range[19] = {
            -10.5, -10.0, -5.123,  0.0,  5.456, 10.0, 15.0, 20.2,
             20.2,  20.3, 25.123, 30.0,   30.0, 35.1, 40.3, 45.5,
             50.7,  55.0, 55.0,
        };
        v8sf_t src = { .val = {9.0, -12.6, 37.7, 1.1, 25.0, 60.0, -5.0, 22.0} };
        v8si_t idx = { .val = {0} };

        mx_v8sf_bisearch(&idx, range, sizeof(range) / sizeof(range[0]), &src);

        check_value(mx_type_val(idx)[0], 5);
        check_value(mx_type_val(idx)[1], 0);
        check_value(mx_type_val(idx)[2], 14);
        check_value(mx_type_val(idx)[3], 4);
        check_value(mx_type_val(idx)[4], 10);
        check_value(mx_type_val(idx)[5], 19);
        check_value(mx_type_val(idx)[6], 3);
        check_value(mx_type_val(idx)[7], 10);
    }
}

Test(Interpolation, mx_v8si_interpolate)
{
    // Case: The number of x is less than 8.
    {
        int32_t xp[] = {3, 6, 9, 12};
        int32_t fp[] = {3, 6, 9, 12};
        int32_t x[] = {4, 8, 10, 3, 12, -1, 13};
        int32_t y[sizeof(x) / sizeof(x[0])];

        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, NULL, NULL, NULL, sizeof(x) / sizeof(x[0]), x);

        check_value(y[0], 4);
        check_value(y[1], 8);
        check_value(y[2], 10);
        check_value(y[3], 3);
        check_value(y[4], 12);
        check_value(y[5], 3);
        check_value(y[6], 12);
    }

    // Case: The number of x is equal to 8.
    {
        int32_t xp[] = {1,  5, 10, 12};
        int32_t fp[] = {3, 17, 35, 42};
        int32_t x[] = {4, 8, 10, 1, 12, -1, 13, 7};
        int32_t y[sizeof(x) / sizeof(x[0])];

        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, NULL, NULL, NULL, sizeof(x) / sizeof(x[0]), x);

        check_value(y[0], 13);
        check_value(y[1], 28);
        check_value(y[2], 35);
        check_value(y[3], 3);
        check_value(y[4], 42);
        check_value(y[5], 3);
        check_value(y[6], 42);
        check_value(y[7], 24);
    }

    // Case: The number of x is greater than 8.
    {
        int32_t xp[] = {1,  5, 10, 12};
        int32_t fp[] = {3, 17, 35, 42};
        int32_t x[] = {4, 8, 10, 1, 12, -1, 13, 7, 5};
        int32_t y[sizeof(x) / sizeof(x[0])];

        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, NULL, NULL, NULL, sizeof(x) / sizeof(x[0]), x);

        check_value(y[0], 13);
        check_value(y[1], 28);
        check_value(y[2], 35);
        check_value(y[3], 3);
        check_value(y[4], 42);
        check_value(y[5], 3);
        check_value(y[6], 42);
        check_value(y[7], 24);
        check_value(y[8], 17);
    }

    // Case: The number of x is less than 8 and use pre-calculated slope table.
    {
        int32_t xp[] = {3, 6, 9, 12};
        int32_t fp[] = {3, 6, 9, 12};
        float slp[] = {1.0, 1.0, 1.0, 1.0};
        int32_t x[] = {4, 8, 10, 3, 12, -1, 13};
        int32_t y[sizeof(x) / sizeof(x[0])];

        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, slp, NULL, NULL, sizeof(x) / sizeof(x[0]), x);

        check_value(y[0], 4);
        check_value(y[1], 8);
        check_value(y[2], 10);
        check_value(y[3], 3);
        check_value(y[4], 12);
        check_value(y[5], 3);
        check_value(y[6], 12);
    }

    // Case: The number of x is equal to 8 and use pre-calculated slope table.
    {
        int32_t xp[] = {1,  5, 10, 12};
        int32_t fp[] = {3, 17, 35, 42};
        float slp[] = {3.5, 3.5, 3.5, 3.5};
        int32_t x[] = {4, 8, 10, 1, 12, -1, 13, 7};
        int32_t y[sizeof(x) / sizeof(x[0])];

        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, slp, NULL, NULL, sizeof(x) / sizeof(x[0]), x);

        check_value(y[0], 13);
        check_value(y[1], 27);
        check_value(y[2], 35);
        check_value(y[3], 3);
        check_value(y[4], 42);
        check_value(y[5], 3);
        check_value(y[6], 42);
        check_value(y[7], 24);
    }

    // Case: The number of x is greater than 8 and use pre-calculated slope table.
    {
        int32_t xp[] = {1,  5, 10, 12};
        int32_t fp[] = {3, 17, 35, 42};
        float slp[] = {3.5, 3.5, 3.5, 3.5};
        int32_t x[] = {4, 8, 10, 1, 12, -1, 13, 7, 5};
        int32_t y[sizeof(x) / sizeof(x[0])];

        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, slp, NULL, NULL, sizeof(x) / sizeof(x[0]), x);

        check_value(y[0], 13);
        check_value(y[1], 27);
        check_value(y[2], 35);
        check_value(y[3], 3);
        check_value(y[4], 42);
        check_value(y[5], 3);
        check_value(y[6], 42);
        check_value(y[7], 24);
        check_value(y[8], 17);
    }

    // Case: The number of x is less than 8 and the number of search range is odd.
    {
        int32_t xp[] = {3, 6, 9, 12, 15};
        int32_t fp[] = {3, 6, 9, 12, 15};
        int32_t x[] = {4, 8, 10, 3, 15, -1, 16};
        int32_t y[sizeof(x) / sizeof(x[0])];

        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, NULL, NULL, NULL, sizeof(x) / sizeof(x[0]), x);

        check_value(y[0], 4);
        check_value(y[1], 8);
        check_value(y[2], 10);
        check_value(y[3], 3);
        check_value(y[4], 15);
        check_value(y[5], 3);
        check_value(y[6], 15);
    }

    // Case: The number of x is equal to 8 the number of search range is odd.
    {
        int32_t xp[] = {1,  5, 10, 12, 17};
        int32_t fp[] = {3, 17, 35, 42, 59};
        int32_t x[] = {4, 8, 10, 1, 17, -1, 13, 18};
        int32_t y[sizeof(x) / sizeof(x[0])];

        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, NULL, NULL, NULL, sizeof(x) / sizeof(x[0]), x);

        check_value(y[0], 13);
        check_value(y[1], 28);
        check_value(y[2], 35);
        check_value(y[3], 3);
        check_value(y[4], 59);
        check_value(y[5], 3);
        check_value(y[6], 45);
        check_value(y[7], 59);
    }

    // Case: The number of x is greater than 8 the number of search range is odd.
    {
        int32_t xp[] = {1,  5, 10, 12, 17};
        int32_t fp[] = {3, 17, 35, 42, 59};
        int32_t x[] = {4, 8, 10, 1, 12, -1, 13, 7, 5, 17, 18};
        int32_t y[sizeof(x) / sizeof(x[0])];

        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, NULL, NULL, NULL, sizeof(x) / sizeof(x[0]), x);

        check_value(y[0], 13);
        check_value(y[1], 28);
        check_value(y[2], 35);
        check_value(y[3], 3);
        check_value(y[4], 42);
        check_value(y[5], 3);
        check_value(y[6], 45);
        check_value(y[7], 24);
        check_value(y[8], 17);
        check_value(y[9], 59);
        check_value(y[10], 59);
    }
}
