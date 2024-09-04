#include <criterion/criterion.h>

#include "src/mb32.h"

#define check_value(expect, val) \
    cr_expect((expect) == (val), "Wrong value - Expect %d, got %d from '%s'.", expect, val, #val)

#define check_value_at(expect, val, x, y) \
    cr_expect((expect) == (val), "Wrong value - Expect %d, got %d from '%s' at (%d,%d).", expect, val, #val, x, y)

Test(Iterator, mb32_itr_get_v8si_in_row)
{
    mb32_stor_t ms;
    mb32_init(&ms, 17, 17, 4, 8);

    for (int32_t i = 0; i < mb32_rnum(&ms); i += 1) {
        for (int32_t j = 0; j < mb32_cnum(&ms); j += 1) {
            mb32_i32_set(&ms, i, j, i * 100 + j);
        } // for
    } // for

    // Case: Range covers 1 full chunk on the left-top corner.
    {
        mb32_iter_t it;
        mb32_itr_init_for_iterating_in_range(&it, &ms, 0, 0, 8, 8);

        mb32_off_t off[] = {
            { 0,  0}, // Get one full vector.
            {-1,  0}, // Out of range, beyond the top boundary.
            { 8,  0}, // Out of range, beyond the bottom boundary.
            { 0, -8}, // Out of range, beyond the left boundary.
            { 0,  8}, // Out of range, beyond the right boundary.
        };
        int32_t vn = sizeof(off) / sizeof(off[0]);
        v8si_t vec[vn];
        int32_t dval = 9999;

        bool ret = mb32_itr_get_v8si_in_row(&it, vec, vn, off, dval, false);

        check_value(true, ret);

        // Get one full vector.
        check_value(0, mx_type_val(vec[0])[0]);
        check_value(1, mx_type_val(vec[0])[1]);
        check_value(2, mx_type_val(vec[0])[2]);
        check_value(3, mx_type_val(vec[0])[3]);
        check_value(4, mx_type_val(vec[0])[4]);
        check_value(5, mx_type_val(vec[0])[5]);
        check_value(6, mx_type_val(vec[0])[6]);
        check_value(7, mx_type_val(vec[0])[7]);

        // Out of range, beyond the top boundary.
        check_value(dval, mx_type_val(vec[1])[0]);
        check_value(dval, mx_type_val(vec[1])[1]);
        check_value(dval, mx_type_val(vec[1])[2]);
        check_value(dval, mx_type_val(vec[1])[3]);
        check_value(dval, mx_type_val(vec[1])[4]);
        check_value(dval, mx_type_val(vec[1])[5]);
        check_value(dval, mx_type_val(vec[1])[6]);
        check_value(dval, mx_type_val(vec[1])[7]);

        // Out of range, beyond the bottom boundary.
        check_value(dval, mx_type_val(vec[2])[0]);
        check_value(dval, mx_type_val(vec[2])[1]);
        check_value(dval, mx_type_val(vec[2])[2]);
        check_value(dval, mx_type_val(vec[2])[3]);
        check_value(dval, mx_type_val(vec[2])[4]);
        check_value(dval, mx_type_val(vec[2])[5]);
        check_value(dval, mx_type_val(vec[2])[6]);
        check_value(dval, mx_type_val(vec[2])[7]);

        // Out of range, beyond the left boundary.
        check_value(dval, mx_type_val(vec[3])[0]);
        check_value(dval, mx_type_val(vec[3])[1]);
        check_value(dval, mx_type_val(vec[3])[2]);
        check_value(dval, mx_type_val(vec[3])[3]);
        check_value(dval, mx_type_val(vec[3])[4]);
        check_value(dval, mx_type_val(vec[3])[5]);
        check_value(dval, mx_type_val(vec[3])[6]);
        check_value(dval, mx_type_val(vec[3])[7]);

        // Out of range, beyond the right boundary.
        check_value(dval, mx_type_val(vec[4])[0]);
        check_value(dval, mx_type_val(vec[4])[1]);
        check_value(dval, mx_type_val(vec[4])[2]);
        check_value(dval, mx_type_val(vec[4])[3]);
        check_value(dval, mx_type_val(vec[4])[4]);
        check_value(dval, mx_type_val(vec[4])[5]);
        check_value(dval, mx_type_val(vec[4])[6]);
        check_value(dval, mx_type_val(vec[4])[7]);
    }

    mb32_clean(&ms);
}
