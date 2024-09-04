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
            { 0, -1}, // In range, beyond the left boundary of the range one element.
            { 0, -2}, // In range, beyond the left boundary of the range two elements.
            { 0, -3}, // In range, beyond the left boundary of the range three elements.
            { 0, -4}, // In range, beyond the left boundary of the range four elements.
            { 0, -5}, // In range, beyond the left boundary of the range five elements.
            { 0, -6}, // In range, beyond the left boundary of the range six elements.
            { 0, -7}, // In range, beyond the left boundary of the range seven elements.
            { 0, 1}, // In range, beyond the right boundary of the range one element.
            { 0, 2}, // In range, beyond the right boundary of the range two elements.
            { 0, 3}, // In range, beyond the right boundary of the range three elements.
            { 0, 4}, // In range, beyond the right boundary of the range four elements.
            { 0, 5}, // In range, beyond the right boundary of the range five elements.
            { 0, 6}, // In range, beyond the right boundary of the range six elements.
            { 0, 7}, // In range, beyond the right boundary of the range seven elements.
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

        // In range, beyond the left boundary of the range one element.
        check_value(dval, mx_type_val(vec[5])[0]);
        check_value(   0, mx_type_val(vec[5])[1]);
        check_value(   1, mx_type_val(vec[5])[2]);
        check_value(   2, mx_type_val(vec[5])[3]);
        check_value(   3, mx_type_val(vec[5])[4]);
        check_value(   4, mx_type_val(vec[5])[5]);
        check_value(   5, mx_type_val(vec[5])[6]);
        check_value(   6, mx_type_val(vec[5])[7]);

        // In range, beyond the left boundary of the range two elements.
        check_value(dval, mx_type_val(vec[6])[0]);
        check_value(dval, mx_type_val(vec[6])[1]);
        check_value(   0, mx_type_val(vec[6])[2]);
        check_value(   1, mx_type_val(vec[6])[3]);
        check_value(   2, mx_type_val(vec[6])[4]);
        check_value(   3, mx_type_val(vec[6])[5]);
        check_value(   4, mx_type_val(vec[6])[6]);
        check_value(   5, mx_type_val(vec[6])[7]);

        // In range, beyond the left boundary of the range three elements.
        check_value(dval, mx_type_val(vec[7])[0]);
        check_value(dval, mx_type_val(vec[7])[1]);
        check_value(dval, mx_type_val(vec[7])[2]);
        check_value(   0, mx_type_val(vec[7])[3]);
        check_value(   1, mx_type_val(vec[7])[4]);
        check_value(   2, mx_type_val(vec[7])[5]);
        check_value(   3, mx_type_val(vec[7])[6]);
        check_value(   4, mx_type_val(vec[7])[7]);

        // In range, beyond the left boundary of the range four elements.
        check_value(dval, mx_type_val(vec[8])[0]);
        check_value(dval, mx_type_val(vec[8])[1]);
        check_value(dval, mx_type_val(vec[8])[2]);
        check_value(dval, mx_type_val(vec[8])[3]);
        check_value(   0, mx_type_val(vec[8])[4]);
        check_value(   1, mx_type_val(vec[8])[5]);
        check_value(   2, mx_type_val(vec[8])[6]);
        check_value(   3, mx_type_val(vec[8])[7]);

        // In range, beyond the left boundary of the range five elements.
        check_value(dval, mx_type_val(vec[9])[0]);
        check_value(dval, mx_type_val(vec[9])[1]);
        check_value(dval, mx_type_val(vec[9])[2]);
        check_value(dval, mx_type_val(vec[9])[3]);
        check_value(dval, mx_type_val(vec[9])[4]);
        check_value(   0, mx_type_val(vec[9])[5]);
        check_value(   1, mx_type_val(vec[9])[6]);
        check_value(   2, mx_type_val(vec[9])[7]);

        // In range, beyond the left boundary of the range six elements.
        check_value(dval, mx_type_val(vec[10])[0]);
        check_value(dval, mx_type_val(vec[10])[1]);
        check_value(dval, mx_type_val(vec[10])[2]);
        check_value(dval, mx_type_val(vec[10])[3]);
        check_value(dval, mx_type_val(vec[10])[4]);
        check_value(dval, mx_type_val(vec[10])[5]);
        check_value(   0, mx_type_val(vec[10])[6]);
        check_value(   1, mx_type_val(vec[10])[7]);

        // In range, beyond the left boundary of the range seven elements.
        check_value(dval, mx_type_val(vec[11])[0]);
        check_value(dval, mx_type_val(vec[11])[1]);
        check_value(dval, mx_type_val(vec[11])[2]);
        check_value(dval, mx_type_val(vec[11])[3]);
        check_value(dval, mx_type_val(vec[11])[4]);
        check_value(dval, mx_type_val(vec[11])[5]);
        check_value(dval, mx_type_val(vec[11])[6]);
        check_value(   0, mx_type_val(vec[11])[7]);

        // In range, beyond the right boundary of the range one element.
        check_value(   1, mx_type_val(vec[12])[0]);
        check_value(   2, mx_type_val(vec[12])[1]);
        check_value(   3, mx_type_val(vec[12])[2]);
        check_value(   4, mx_type_val(vec[12])[3]);
        check_value(   5, mx_type_val(vec[12])[4]);
        check_value(   6, mx_type_val(vec[12])[5]);
        check_value(   7, mx_type_val(vec[12])[6]);
        check_value(dval, mx_type_val(vec[12])[7]);

        // In range, beyond the right boundary of the range two elements.
        check_value(   2, mx_type_val(vec[13])[0]);
        check_value(   3, mx_type_val(vec[13])[1]);
        check_value(   4, mx_type_val(vec[13])[2]);
        check_value(   5, mx_type_val(vec[13])[3]);
        check_value(   6, mx_type_val(vec[13])[4]);
        check_value(   7, mx_type_val(vec[13])[5]);
        check_value(dval, mx_type_val(vec[13])[6]);
        check_value(dval, mx_type_val(vec[13])[7]);

        // In range, beyond the right boundary of the range three elements.
        check_value(   3, mx_type_val(vec[14])[0]);
        check_value(   4, mx_type_val(vec[14])[1]);
        check_value(   5, mx_type_val(vec[14])[2]);
        check_value(   6, mx_type_val(vec[14])[3]);
        check_value(   7, mx_type_val(vec[14])[4]);
        check_value(dval, mx_type_val(vec[14])[5]);
        check_value(dval, mx_type_val(vec[14])[6]);
        check_value(dval, mx_type_val(vec[14])[7]);

        // In range, beyond the right boundary of the range four elements.
        check_value(   4, mx_type_val(vec[15])[0]);
        check_value(   5, mx_type_val(vec[15])[1]);
        check_value(   6, mx_type_val(vec[15])[2]);
        check_value(   7, mx_type_val(vec[15])[3]);
        check_value(dval, mx_type_val(vec[15])[4]);
        check_value(dval, mx_type_val(vec[15])[5]);
        check_value(dval, mx_type_val(vec[15])[6]);
        check_value(dval, mx_type_val(vec[15])[7]);

        // In range, beyond the right boundary of the range five elements.
        check_value(   5, mx_type_val(vec[16])[0]);
        check_value(   6, mx_type_val(vec[16])[1]);
        check_value(   7, mx_type_val(vec[16])[2]);
        check_value(dval, mx_type_val(vec[16])[3]);
        check_value(dval, mx_type_val(vec[16])[4]);
        check_value(dval, mx_type_val(vec[16])[5]);
        check_value(dval, mx_type_val(vec[16])[6]);
        check_value(dval, mx_type_val(vec[16])[7]);

        // In range, beyond the right boundary of the range six elements.
        check_value(   6, mx_type_val(vec[17])[0]);
        check_value(   7, mx_type_val(vec[17])[1]);
        check_value(dval, mx_type_val(vec[17])[2]);
        check_value(dval, mx_type_val(vec[17])[3]);
        check_value(dval, mx_type_val(vec[17])[4]);
        check_value(dval, mx_type_val(vec[17])[5]);
        check_value(dval, mx_type_val(vec[17])[6]);
        check_value(dval, mx_type_val(vec[17])[7]);

        // In range, beyond the right boundary of the range seven elements.
        check_value(   7, mx_type_val(vec[18])[0]);
        check_value(dval, mx_type_val(vec[18])[1]);
        check_value(dval, mx_type_val(vec[18])[2]);
        check_value(dval, mx_type_val(vec[18])[3]);
        check_value(dval, mx_type_val(vec[18])[4]);
        check_value(dval, mx_type_val(vec[18])[5]);
        check_value(dval, mx_type_val(vec[18])[6]);
        check_value(dval, mx_type_val(vec[18])[7]);
    }

    mb32_clean(&ms);
}
