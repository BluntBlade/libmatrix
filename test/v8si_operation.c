#include <criterion/criterion.h>

#ifndef V8SI_OPER_C
#define V8SI_OPER_C
#include "src/v8si_operation.c"
#endif

// ==== TESTS FOR THE OPERATION MODULE OF MATRIX ==== //

inline static int32_t value_at(mx_chunk_ptr chk, uint32_t row, uint32_t col)
{
    if (col < 8) {
        return mx_type_val(chk->v8si_16x1[row][0])[col];
    } // if
    return mx_type_val(chk->v8si_16x2[row][mx_round_to_multiples_of_8(col) / 8 - 1])[col & 0x7];
} // value_at

#define check_value(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %d, got %d.", expect, val)

#define check_value_at(val, expect, i, j) \
    cr_expect(val == expect, "Wrong value at (%d,%d) - Expect %d, got %d.", i, j, expect, val)

Test(InternalFunc, v8si_add_chunk_fully)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} },  { .val = {  8,   9,  10,  11,  12,  13,  14,  15} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} },  { .val = { 24,  25,  26,  27,  28,  29,  30,  31} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} },  { .val = { 40,  41,  42,  43,  44,  45,  46,  47} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} },  { .val = { 56,  57,  58,  59,  60,  61,  62,  63} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} },  { .val = { 72,  73,  74,  75,  76,  77,  78,  79} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} },  { .val = { 88,  89,  90,  91,  92,  93,  94,  95} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} },  { .val = {104, 105, 106, 107, 108, 109, 110, 111} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} },  { .val = {120, 121, 122, 123, 124, 125, 126, 127} } },
                { { .val = {128, 129, 130, 131, 132, 133, 134, 135} },  { .val = {136, 137, 138, 139, 140, 141, 142, 143} } },
                { { .val = {144, 145, 146, 147, 148, 149, 150, 151} },  { .val = {152, 153, 154, 155, 156, 157, 158, 159} } },
                { { .val = {160, 161, 162, 163, 164, 165, 166, 167} },  { .val = {168, 169, 170, 171, 172, 173, 174, 175} } },
                { { .val = {176, 177, 178, 179, 180, 181, 182, 183} },  { .val = {184, 185, 186, 187, 188, 189, 190, 191} } },
                { { .val = {192, 193, 194, 195, 196, 197, 198, 199} },  { .val = {200, 201, 202, 203, 204, 205, 206, 207} } },
                { { .val = {208, 209, 210, 211, 212, 213, 214, 215} },  { .val = {216, 217, 218, 219, 220, 221, 222, 223} } },
                { { .val = {224, 225, 226, 227, 228, 229, 230, 231} },  { .val = {232, 233, 234, 235, 236, 237, 238, 239} } },
                { { .val = {240, 241, 242, 243, 244, 245, 246, 247} },  { .val = {248, 249, 250, 251, 252, 253, 254, 255} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_fully(&dst, &src, &src);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[0], mx_type_val(src.v8si_16x2[i][j])[0] * 2, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[1], mx_type_val(src.v8si_16x2[i][j])[1] * 2, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[2], mx_type_val(src.v8si_16x2[i][j])[2] * 2, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[3], mx_type_val(src.v8si_16x2[i][j])[3] * 2, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[4], mx_type_val(src.v8si_16x2[i][j])[4] * 2, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[5], mx_type_val(src.v8si_16x2[i][j])[5] * 2, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[6], mx_type_val(src.v8si_16x2[i][j])[6] * 2, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[7], mx_type_val(src.v8si_16x2[i][j])[7] * 2, i, j * 8 + 7);
            } // for
        } // for
    }
}

Test(InternalFunc, v8si_add_chunk_partly)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 1x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {0, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 1, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
    }

    // -- 5x5 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = { 0,  1,  2,  3,  4, 0, 0, 0} } },
                { { .val = {16, 17, 18, 19, 20, 0, 0, 0} } },
                { { .val = {32, 33, 34, 35, 36, 0, 0, 0} } },
                { { .val = {48, 49, 50, 51, 52, 0, 0, 0} } },
                { { .val = {64, 65, 66, 67, 68, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 5, 5);
        for (i = 0; i < 5; i += 1) {
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[0], mx_type_val(src.v8si_16x1[i][0])[0] * 2, i, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[1], mx_type_val(src.v8si_16x1[i][0])[1] * 2, i, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[2], mx_type_val(src.v8si_16x1[i][0])[2] * 2, i, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[3], mx_type_val(src.v8si_16x1[i][0])[3] * 2, i, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[4], mx_type_val(src.v8si_16x1[i][0])[4] * 2, i, 0 * 8 + 4);
        } // for
    }

    // -- 8x8 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 8, 8);
        for (i = 0; i < 8; i += 1) {
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[0], mx_type_val(src.v8si_16x1[i][0])[0] * 2, i, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[1], mx_type_val(src.v8si_16x1[i][0])[1] * 2, i, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[2], mx_type_val(src.v8si_16x1[i][0])[2] * 2, i, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[3], mx_type_val(src.v8si_16x1[i][0])[3] * 2, i, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[4], mx_type_val(src.v8si_16x1[i][0])[4] * 2, i, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[5], mx_type_val(src.v8si_16x1[i][0])[5] * 2, i, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[6], mx_type_val(src.v8si_16x1[i][0])[6] * 2, i, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8si_16x1[i][0])[7], mx_type_val(src.v8si_16x1[i][0])[7] * 2, i, 0 * 8 + 7);
        } // for
    }

    // -- 9x9 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} }, { .val = {  8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} }, { .val = { 24, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} }, { .val = { 40, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} }, { .val = { 56, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} }, { .val = { 72, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} }, { .val = { 88, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} }, { .val = {104, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} }, { .val = {120, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {128, 129, 130, 131, 132, 133, 134, 135} }, { .val = {136, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 9, 9);
        for (i = 0; i < 9; i += 1) {
            check_value_at(mx_type_val(dst.v8si_16x2[i][0])[0], mx_type_val(src.v8si_16x2[i][0])[0] * 2, i, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8si_16x2[i][0])[1], mx_type_val(src.v8si_16x2[i][0])[1] * 2, i, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8si_16x2[i][0])[2], mx_type_val(src.v8si_16x2[i][0])[2] * 2, i, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8si_16x2[i][0])[3], mx_type_val(src.v8si_16x2[i][0])[3] * 2, i, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8si_16x2[i][0])[4], mx_type_val(src.v8si_16x2[i][0])[4] * 2, i, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8si_16x2[i][0])[5], mx_type_val(src.v8si_16x2[i][0])[5] * 2, i, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8si_16x2[i][0])[6], mx_type_val(src.v8si_16x2[i][0])[6] * 2, i, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8si_16x2[i][0])[7], mx_type_val(src.v8si_16x2[i][0])[7] * 2, i, 0 * 8 + 7);

            check_value_at(mx_type_val(dst.v8si_16x2[i][1])[0], mx_type_val(src.v8si_16x2[i][1])[0] * 2, i, 1 * 8 + 0);
        } // for
    }
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} }, { .val = {  8,   9,  10,  11,  12,  13,  14,  15} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} }, { .val = { 24,  25,  26,  27,  28,  29,  30,  31} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} }, { .val = { 40,  41,  42,  43,  44,  45,  46,  47} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} }, { .val = { 56,  57,  58,  59,  60,  61,  62,  63} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} }, { .val = { 72,  73,  74,  75,  76,  77,  78,  79} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} }, { .val = { 88,  89,  90,  91,  92,  93,  94,  95} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} }, { .val = {104, 105, 106, 107, 108, 109, 110, 111} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} }, { .val = {120, 121, 122, 123, 124, 125, 126, 127} } },
                { { .val = {128, 129, 130, 131, 132, 133, 134, 135} }, { .val = {136, 137, 138, 139, 140, 141, 142, 143} } },
                { { .val = {144, 145, 146, 147, 148, 149, 150, 151} }, { .val = {152, 153, 154, 155, 156, 157, 158, 159} } },
                { { .val = {160, 161, 162, 163, 164, 165, 166, 167} }, { .val = {168, 169, 170, 171, 172, 173, 174, 175} } },
                { { .val = {176, 177, 178, 179, 180, 181, 182, 183} }, { .val = {184, 185, 186, 187, 188, 189, 190, 191} } },
                { { .val = {192, 193, 194, 195, 196, 197, 198, 199} }, { .val = {200, 201, 202, 203, 204, 205, 206, 207} } },
                { { .val = {208, 209, 210, 211, 212, 213, 214, 215} }, { .val = {216, 217, 218, 219, 220, 221, 222, 223} } },
                { { .val = {224, 225, 226, 227, 228, 229, 230, 231} }, { .val = {232, 233, 234, 235, 236, 237, 238, 239} } },
                { { .val = {240, 241, 242, 243, 244, 245, 246, 247} }, { .val = {248, 249, 250, 251, 252, 253, 254, 255} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[0], mx_type_val(src.v8si_16x2[i][j])[0] * 2, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[1], mx_type_val(src.v8si_16x2[i][j])[1] * 2, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[2], mx_type_val(src.v8si_16x2[i][j])[2] * 2, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[3], mx_type_val(src.v8si_16x2[i][j])[3] * 2, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[4], mx_type_val(src.v8si_16x2[i][j])[4] * 2, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[5], mx_type_val(src.v8si_16x2[i][j])[5] * 2, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[6], mx_type_val(src.v8si_16x2[i][j])[6] * 2, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[7], mx_type_val(src.v8si_16x2[i][j])[7] * 2, i, j * 8 + 7);
            } // for
        } // for
    }

    // -- 1x5 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = { 0,  1,  2,  3,  4, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 1, 5);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], mx_type_val(src.v8si_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], mx_type_val(src.v8si_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], mx_type_val(src.v8si_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], mx_type_val(src.v8si_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
    }

    // -- 1x8 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {0, 1, 2, 3, 4, 5, 6, 7} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 1, 8);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], mx_type_val(src.v8si_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], mx_type_val(src.v8si_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], mx_type_val(src.v8si_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], mx_type_val(src.v8si_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[5], mx_type_val(src.v8si_16x1[0][0])[5] * 2, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[6], mx_type_val(src.v8si_16x1[0][0])[6] * 2, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[7], mx_type_val(src.v8si_16x1[0][0])[7] * 2, 0, 0 * 8 + 7);
    }

    // -- 1x9 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {0, 1, 2, 3, 4, 5, 6, 7} }, { .val = {8, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 1, 9);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[0], mx_type_val(src.v8si_16x2[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[1], mx_type_val(src.v8si_16x2[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[2], mx_type_val(src.v8si_16x2[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[3], mx_type_val(src.v8si_16x2[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[4], mx_type_val(src.v8si_16x2[0][0])[4] * 2, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[5], mx_type_val(src.v8si_16x2[0][0])[5] * 2, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[6], mx_type_val(src.v8si_16x2[0][0])[6] * 2, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[7], mx_type_val(src.v8si_16x2[0][0])[7] * 2, 0, 0 * 8 + 7);

        check_value_at(mx_type_val(dst.v8si_16x2[0][1])[0], mx_type_val(src.v8si_16x2[0][1])[0] * 2, 0, 1 * 8 + 0);
    }

    // -- 5x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 5, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[1][0])[0], mx_type_val(src.v8si_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[2][0])[0], mx_type_val(src.v8si_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[3][0])[0], mx_type_val(src.v8si_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[4][0])[0], mx_type_val(src.v8si_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
    }

    // -- 8x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 80, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 96, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {112, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 8, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[1][0])[0], mx_type_val(src.v8si_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[2][0])[0], mx_type_val(src.v8si_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[3][0])[0], mx_type_val(src.v8si_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[4][0])[0], mx_type_val(src.v8si_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x1[5][0])[0], mx_type_val(src.v8si_16x1[5][0])[0] * 2, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x1[6][0])[0], mx_type_val(src.v8si_16x1[6][0])[0] * 2, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x1[7][0])[0], mx_type_val(src.v8si_16x1[7][0])[0] * 2, 7, 0 * 8 + 7);
    }

    // -- 9x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 80, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 96, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {112, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {128, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_add_chunk_partly(&dst, &src, &src, 9, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[1][0])[0], mx_type_val(src.v8si_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[2][0])[0], mx_type_val(src.v8si_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[3][0])[0], mx_type_val(src.v8si_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[4][0])[0], mx_type_val(src.v8si_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x1[5][0])[0], mx_type_val(src.v8si_16x1[5][0])[0] * 2, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x1[6][0])[0], mx_type_val(src.v8si_16x1[6][0])[0] * 2, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x1[7][0])[0], mx_type_val(src.v8si_16x1[7][0])[0] * 2, 7, 0 * 8 + 7);
        check_value_at(mx_type_val(dst.v8si_16x1[8][1])[0], mx_type_val(src.v8si_16x1[8][1])[0] * 2, 8, 1 * 8 + 0);
    }
}

Test(InternalFunc, v8si_subtract_chunk_fully)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} }, { .val = {  8,   9,  10,  11,  12,  13,  14,  15} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} }, { .val = { 24,  25,  26,  27,  28,  29,  30,  31} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} }, { .val = { 40,  41,  42,  43,  44,  45,  46,  47} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} }, { .val = { 56,  57,  58,  59,  60,  61,  62,  63} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} }, { .val = { 72,  73,  74,  75,  76,  77,  78,  79} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} }, { .val = { 88,  89,  90,  91,  92,  93,  94,  95} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} }, { .val = {104, 105, 106, 107, 108, 109, 110, 111} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} }, { .val = {120, 121, 122, 123, 124, 125, 126, 127} } },
                { { .val = {128, 129, 130, 131, 132, 133, 134, 135} }, { .val = {136, 137, 138, 139, 140, 141, 142, 143} } },
                { { .val = {144, 145, 146, 147, 148, 149, 150, 151} }, { .val = {152, 153, 154, 155, 156, 157, 158, 159} } },
                { { .val = {160, 161, 162, 163, 164, 165, 166, 167} }, { .val = {168, 169, 170, 171, 172, 173, 174, 175} } },
                { { .val = {176, 177, 178, 179, 180, 181, 182, 183} }, { .val = {184, 185, 186, 187, 188, 189, 190, 191} } },
                { { .val = {192, 193, 194, 195, 196, 197, 198, 199} }, { .val = {200, 201, 202, 203, 204, 205, 206, 207} } },
                { { .val = {208, 209, 210, 211, 212, 213, 214, 215} }, { .val = {216, 217, 218, 219, 220, 221, 222, 223} } },
                { { .val = {224, 225, 226, 227, 228, 229, 230, 231} }, { .val = {232, 233, 234, 235, 236, 237, 238, 239} } },
                { { .val = {240, 241, 242, 243, 244, 245, 246, 247} }, { .val = {248, 249, 250, 251, 252, 253, 254, 255} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_fully(&dst, &src, &src);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[0], 0, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[1], 0, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[2], 0, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[3], 0, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[4], 0, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[5], 0, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[6], 0, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[7], 0, i, j * 8 + 7);
            } // for
        } // for
    }
}

Test(InternalFunc, v8si_subtract_chunk_partly)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 1x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {0, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 1, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
    }

    // -- 5x5 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = { 0,  1,  2,  3,  4, 0, 0, 0} } },
                { { .val = {16, 17, 18, 19, 20, 0, 0, 0} } },
                { { .val = {32, 33, 34, 35, 36, 0, 0, 0} } },
                { { .val = {48, 49, 50, 51, 52, 0, 0, 0} } },
                { { .val = {64, 65, 66, 67, 68, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 5, 5);
        for (i = 0; i < 5; i += 1) {
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], 0, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], 0, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], 0, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], 0, 0, 0 * 8 + 4);
        } // for
    }

    // -- 8x8 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 8, 8);
        for (i = 0; i < 8; i += 1) {
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], 0, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], 0, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], 0, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], 0, 0, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[5], 0, 0, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[6], 0, 0, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[7], 0, 0, 0 * 8 + 7);
        } // for
    }

    // -- 9x9 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} }, { .val = {  8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} }, { .val = { 24, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} }, { .val = { 40, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} }, { .val = { 56, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} }, { .val = { 72, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} }, { .val = { 88, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} }, { .val = {104, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} }, { .val = {120, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {128, 129, 130, 131, 132, 133, 134, 135} }, { .val = {136, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 9, 9);
        for (i = 0; i < 9; i += 1) {
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[0], 0, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[1], 0, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[2], 0, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[3], 0, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[4], 0, 0, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[5], 0, 0, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[6], 0, 0, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[7], 0, 0, 0 * 8 + 7);

            check_value_at(mx_type_val(dst.v8si_16x2[0][1])[0], 0, 0, 1 * 8 + 0);
        } // for
    }
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} }, { .val = {  8,   9,  10,  11,  12,  13,  14,  15} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} }, { .val = { 24,  25,  26,  27,  28,  29,  30,  31} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} }, { .val = { 40,  41,  42,  43,  44,  45,  46,  47} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} }, { .val = { 56,  57,  58,  59,  60,  61,  62,  63} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} }, { .val = { 72,  73,  74,  75,  76,  77,  78,  79} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} }, { .val = { 88,  89,  90,  91,  92,  93,  94,  95} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} }, { .val = {104, 105, 106, 107, 108, 109, 110, 111} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} }, { .val = {120, 121, 122, 123, 124, 125, 126, 127} } },
                { { .val = {128, 129, 130, 131, 132, 133, 134, 135} }, { .val = {136, 137, 138, 139, 140, 141, 142, 143} } },
                { { .val = {144, 145, 146, 147, 148, 149, 150, 151} }, { .val = {152, 153, 154, 155, 156, 157, 158, 159} } },
                { { .val = {160, 161, 162, 163, 164, 165, 166, 167} }, { .val = {168, 169, 170, 171, 172, 173, 174, 175} } },
                { { .val = {176, 177, 178, 179, 180, 181, 182, 183} }, { .val = {184, 185, 186, 187, 188, 189, 190, 191} } },
                { { .val = {192, 193, 194, 195, 196, 197, 198, 199} }, { .val = {200, 201, 202, 203, 204, 205, 206, 207} } },
                { { .val = {208, 209, 210, 211, 212, 213, 214, 215} }, { .val = {216, 217, 218, 219, 220, 221, 222, 223} } },
                { { .val = {224, 225, 226, 227, 228, 229, 230, 231} }, { .val = {232, 233, 234, 235, 236, 237, 238, 239} } },
                { { .val = {240, 241, 242, 243, 244, 245, 246, 247} }, { .val = {248, 249, 250, 251, 252, 253, 254, 255} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[0], 0, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[1], 0, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[2], 0, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[3], 0, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[4], 0, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[5], 0, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[6], 0, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[7], 0, i, j * 8 + 7);
            } // for
        } // for
    }

    // -- 1x5 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = { 0,  1,  2,  3,  4, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 1, 5);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], 0, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], 0, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], 0, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], 0, 0, 0 * 8 + 4);
    }

    // -- 1x8 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {0, 1, 2, 3, 4, 5, 6, 7} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 1, 8);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], 0, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], 0, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], 0, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], 0, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[5], 0, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[6], 0, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[7], 0, 0, 0 * 8 + 7);
    }

    // -- 1x9 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {0, 1, 2, 3, 4, 5, 6, 7} }, { .val = {8, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 1, 9);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[1], 0, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[2], 0, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[3], 0, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[4], 0, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[5], 0, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[6], 0, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[7], 0, 0, 0 * 8 + 7);

        check_value_at(mx_type_val(dst.v8si_16x2[0][1])[0], 0, 0, 1 * 8 + 0);
    }

    // -- 5x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 5, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[1][0])[0], 0, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[2][0])[0], 0, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[3][0])[0], 0, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[4][0])[0], 0, 4, 0 * 8 + 4);
    }

    // -- 8x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 80, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 96, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {112, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 8, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[1][0])[0], 0, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[2][0])[0], 0, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[3][0])[0], 0, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[4][0])[0], 0, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x1[5][0])[0], 0, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x1[6][0])[0], 0, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x1[7][0])[0], 0, 7, 0 * 8 + 7);
    }

    // -- 9x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 80, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 96, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {112, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {128, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_subtract_chunk_partly(&dst, &src, &src, 9, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[1][0])[0], 0, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[2][0])[0], 0, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[3][0])[0], 0, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[4][0])[0], 0, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x1[5][0])[0], 0, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x1[6][0])[0], 0, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x1[7][0])[0], 0, 7, 0 * 8 + 7);
        check_value_at(mx_type_val(dst.v8si_16x1[8][1])[0], 0, 8, 1 * 8 + 0);
    }
}

Test(InternalFunc, v8si_multiply_chunk_fully)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t schk = {
            .v8si_16x2 = {
                { { .val = {2, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 2} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {2, 1, 1, 1, 1, 1, 1, 1} }, { .val = {1, 1, 1, 1, 1, 1, 1, 2} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = {22, 18, 18, 18, 18, 18, 18, 18} }, { .val = {18, 18, 18, 18, 18, 18, 18, 22} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {18, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 18} } },
                { { .val = {22, 18, 18, 18, 18, 18, 18, 18} }, { .val = {18, 18, 18, 18, 18, 18, 18, 22} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_fully(&dchk, &schk, &schk, 16, 16, 16);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8si_multiply_chunk_half_to_half)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 1x1 chunk multiplication -- half to half -- //
    {
        mx_chunk_t schk = {
            .v8si_16x1 = {
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = {4, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_half(&dchk, &schk, &schk, 1, 1, 1);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 5x5 chunk multiplication -- half to half -- //
    {
        mx_chunk_t schk = {
            .v8si_16x1 = {
                { { .val = {2, 1, 1, 1, 1, 0, 0, 0} } },
                { { .val = {1, 1, 1, 1, 1, 0, 0, 0} } },
                { { .val = {1, 1, 1, 1, 1, 0, 0, 0} } },
                { { .val = {1, 1, 1, 1, 1, 0, 0, 0} } },
                { { .val = {1, 1, 1, 1, 1, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = {8, 6, 6, 6, 6, 0, 0, 0} } },
                { { .val = {6, 5, 5, 5, 5, 0, 0, 0} } },
                { { .val = {6, 5, 5, 5, 5, 0, 0, 0} } },
                { { .val = {6, 5, 5, 5, 5, 0, 0, 0} } },
                { { .val = {6, 5, 5, 5, 5, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_half(&dchk, &schk, &schk, 5, 5, 5);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t schk = {
            .v8si_16x1 = {
                { { .val = {2, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
                { { .val = {1, 1, 1, 1, 1, 1, 1, 1} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = {11, 9, 9, 9, 9, 9, 9, 9} } },
                { { .val = { 9, 8, 8, 8, 8, 8, 8, 8} } },
                { { .val = { 9, 8, 8, 8, 8, 8, 8, 8} } },
                { { .val = { 9, 8, 8, 8, 8, 8, 8, 8} } },
                { { .val = { 9, 8, 8, 8, 8, 8, 8, 8} } },
                { { .val = { 9, 8, 8, 8, 8, 8, 8, 8} } },
                { { .val = { 9, 8, 8, 8, 8, 8, 8, 8} } },
                { { .val = { 9, 8, 8, 8, 8, 8, 8, 8} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_half(&dchk, &schk, &schk, 8, 8, 8);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 1x1 and 1x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = {2, 2, 2, 2, 2, 2, 2, 2} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 1, 8, 1);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x1 and 1x1 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 8, 1, 1);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x1 and 1x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {8, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = { 2,  3,  4,  5,  6,  7,  8,  9} } },
                { { .val = { 4,  6,  8, 10, 12, 14, 16, 18} } },
                { { .val = { 6,  9, 12, 15, 18, 21, 24, 27} } },
                { { .val = { 8, 12, 16, 20, 24, 28, 32, 36} } },
                { { .val = {10, 15, 20, 25, 30, 35, 40, 45} } },
                { { .val = {12, 18, 24, 30, 36, 42, 48, 54} } },
                { { .val = {14, 21, 28, 35, 42, 49, 56, 63} } },
                { { .val = {16, 24, 32, 40, 48, 56, 64, 72} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 8, 8, 1);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x1 and 1x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = { 1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = { 2,  3,  4,  5,  6,   7,   8,   9} } },
                { { .val = { 4,  6,  8, 10, 12,  14,  16,  18} } },
                { { .val = { 6,  9, 12, 15, 18,  21,  24,  27} } },
                { { .val = { 8, 12, 16, 20, 24,  28,  32,  36} } },
                { { .val = {10, 15, 20, 25, 30,  35,  40,  45} } },
                { { .val = {12, 18, 24, 30, 36,  42,  48,  54} } },
                { { .val = {14, 21, 28, 35, 42,  49,  56,  63} } },
                { { .val = {16, 24, 32, 40, 48,  56,  64,  72} } },
                { { .val = {18, 27, 36, 45, 54,  63,  72,  81} } },
                { { .val = {20, 30, 40, 50, 60,  70,  80,  90} } },
                { { .val = {22, 33, 44, 55, 66,  77,  88,  99} } },
                { { .val = {24, 36, 48, 60, 72,  84,  96, 108} } },
                { { .val = {26, 39, 52, 65, 78,  91, 104, 117} } },
                { { .val = {28, 42, 56, 70, 84,  98, 112, 126} } },
                { { .val = {30, 45, 60, 75, 90, 105, 120, 135} } },
                { { .val = {32, 48, 64, 80, 96, 112, 128, 144} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 16, 8, 1);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x2 and 2x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = { 1, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 2, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 1, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = {2, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {3, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {4, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {5, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {6, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {7, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {8, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {9, 1, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = { 2 + 1,  3 + 1,  4 + 1,  5 + 1,  6 + 1,   7 + 1,   8 + 1,   9 + 1} } },
                { { .val = { 4 + 1,  6 + 1,  8 + 1, 10 + 1, 12 + 1,  14 + 1,  16 + 1,  18 + 1} } },
                { { .val = { 6 + 1,  9 + 1, 12 + 1, 15 + 1, 18 + 1,  21 + 1,  24 + 1,  27 + 1} } },
                { { .val = { 8 + 1, 12 + 1, 16 + 1, 20 + 1, 24 + 1,  28 + 1,  32 + 1,  36 + 1} } },
                { { .val = {10 + 1, 15 + 1, 20 + 1, 25 + 1, 30 + 1,  35 + 1,  40 + 1,  45 + 1} } },
                { { .val = {12 + 1, 18 + 1, 24 + 1, 30 + 1, 36 + 1,  42 + 1,  48 + 1,  54 + 1} } },
                { { .val = {14 + 1, 21 + 1, 28 + 1, 35 + 1, 42 + 1,  49 + 1,  56 + 1,  63 + 1} } },
                { { .val = {16 + 1, 24 + 1, 32 + 1, 40 + 1, 48 + 1,  56 + 1,  64 + 1,  72 + 1} } },
                { { .val = {18 + 1, 27 + 1, 36 + 1, 45 + 1, 54 + 1,  63 + 1,  72 + 1,  81 + 1} } },
                { { .val = {20 + 1, 30 + 1, 40 + 1, 50 + 1, 60 + 1,  70 + 1,  80 + 1,  90 + 1} } },
                { { .val = {22 + 1, 33 + 1, 44 + 1, 55 + 1, 66 + 1,  77 + 1,  88 + 1,  99 + 1} } },
                { { .val = {24 + 1, 36 + 1, 48 + 1, 60 + 1, 72 + 1,  84 + 1,  96 + 1, 108 + 1} } },
                { { .val = {26 + 1, 39 + 1, 52 + 1, 65 + 1, 78 + 1,  91 + 1, 104 + 1, 117 + 1} } },
                { { .val = {28 + 1, 42 + 1, 56 + 1, 70 + 1, 84 + 1,  98 + 1, 112 + 1, 126 + 1} } },
                { { .val = {30 + 1, 45 + 1, 60 + 1, 75 + 1, 90 + 1, 105 + 1, 120 + 1, 135 + 1} } },
                { { .val = {32 + 1, 48 + 1, 64 + 1, 80 + 1, 96 + 1, 112 + 1, 128 + 1, 144 + 1} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 16, 8, 2);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8si_multiply_chunk_half_to_full)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 1x1 and 1x9 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = {2, 2, 2, 2, 2, 2, 2, 2} }, { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 1, 9, 1);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x1 and 1x9 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = {2, 3, 4, 5, 6, 7, 8, 9} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 3, 4, 5, 6, 7, 8, 9} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 3, 4, 5, 6, 7, 8, 9} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 3, 4, 5, 6, 7, 8, 9} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 3, 4, 5, 6, 7, 8, 9} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 3, 4, 5, 6, 7, 8, 9} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 3, 4, 5, 6, 7, 8, 9} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 3, 4, 5, 6, 7, 8, 9} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {2, 3, 4, 5, 6, 7, 8, 9} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 8, 9, 1);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x1 and 1x9 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = { 1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = { 2,  3,  4,  5,  6,   7,   8,   9} }, { .val = { 10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4,  6,  8, 10, 12,  14,  16,  18} }, { .val = { 20, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6,  9, 12, 15, 18,  21,  24,  27} }, { .val = { 30, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 12, 16, 20, 24,  28,  32,  36} }, { .val = { 40, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 15, 20, 25, 30,  35,  40,  45} }, { .val = { 50, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 18, 24, 30, 36,  42,  48,  54} }, { .val = { 60, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 21, 28, 35, 42,  49,  56,  63} }, { .val = { 70, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 24, 32, 40, 48,  56,  64,  72} }, { .val = { 80, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {18, 27, 36, 45, 54,  63,  72,  81} }, { .val = { 90, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {20, 30, 40, 50, 60,  70,  80,  90} }, { .val = {100, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {22, 33, 44, 55, 66,  77,  88,  99} }, { .val = {110, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {24, 36, 48, 60, 72,  84,  96, 108} }, { .val = {120, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {26, 39, 52, 65, 78,  91, 104, 117} }, { .val = {130, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {28, 42, 56, 70, 84,  98, 112, 126} }, { .val = {140, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {30, 45, 60, 75, 90, 105, 120, 135} }, { .val = {150, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {32, 48, 64, 80, 96, 112, 128, 144} }, { .val = {160, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 16, 9, 1);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x1 and 1x16 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = { 1, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {17, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = { 2,  3,  4,  5,  6,   7,   8,   9} }, { .val = { 10,  11,  12,  13,  14,  15,  16,  17} } },
                { { .val = { 4,  6,  8, 10, 12,  14,  16,  18} }, { .val = { 20,  22,  24,  26,  28,  30,  32,  34} } },
                { { .val = { 6,  9, 12, 15, 18,  21,  24,  27} }, { .val = { 30,  33,  36,  39,  42,  45,  48,  51} } },
                { { .val = { 8, 12, 16, 20, 24,  28,  32,  36} }, { .val = { 40,  44,  48,  52,  56,  60,  64,  68} } },
                { { .val = {10, 15, 20, 25, 30,  35,  40,  45} }, { .val = { 50,  55,  60,  65,  70,  75,  80,  85} } },
                { { .val = {12, 18, 24, 30, 36,  42,  48,  54} }, { .val = { 60,  66,  72,  78,  84,  90,  96, 102} } },
                { { .val = {14, 21, 28, 35, 42,  49,  56,  63} }, { .val = { 70,  77,  84,  91,  98, 105, 112, 119} } },
                { { .val = {16, 24, 32, 40, 48,  56,  64,  72} }, { .val = { 80,  88,  96, 104, 112, 120, 128, 136} } },
                { { .val = {18, 27, 36, 45, 54,  63,  72,  81} }, { .val = { 90,  99, 108, 117, 126, 135, 144, 153} } },
                { { .val = {20, 30, 40, 50, 60,  70,  80,  90} }, { .val = {100, 110, 120, 130, 140, 150, 160, 170} } },
                { { .val = {22, 33, 44, 55, 66,  77,  88,  99} }, { .val = {110, 121, 132, 143, 154, 165, 176, 187} } },
                { { .val = {24, 36, 48, 60, 72,  84,  96, 108} }, { .val = {120, 132, 144, 156, 168, 180, 192, 204} } },
                { { .val = {26, 39, 52, 65, 78,  91, 104, 117} }, { .val = {130, 143, 156, 169, 182, 195, 208, 221} } },
                { { .val = {28, 42, 56, 70, 84,  98, 112, 126} }, { .val = {140, 154, 168, 182, 196, 210, 224, 238} } },
                { { .val = {30, 45, 60, 75, 90, 105, 120, 135} }, { .val = {150, 165, 180, 195, 210, 225, 240, 255} } },
                { { .val = {32, 48, 64, 80, 96, 112, 128, 144} }, { .val = {160, 176, 192, 208, 224, 240, 256, 272} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 16, 16, 1);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x2 and 2x16 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x1 = {
                { { .val = { 1, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 2, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 1, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x1 = {
                { { .val = { 2, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 1, 0, 0, 0, 0, 0, 0} } },
                { { .val = {17, 1, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = { 2 + 1,  3 + 1,  4 + 1,  5 + 1,  6 + 1,   7 + 1,   8 + 1,   9 + 1} }, { .val = { 10 + 1,  11 + 1,  12 + 1,  13 + 1,  14 + 1,  15 + 1,  16 + 1,  17 + 1} } },
                { { .val = { 4 + 1,  6 + 1,  8 + 1, 10 + 1, 12 + 1,  14 + 1,  16 + 1,  18 + 1} }, { .val = { 20 + 1,  22 + 1,  24 + 1,  26 + 1,  28 + 1,  30 + 1,  32 + 1,  34 + 1} } },
                { { .val = { 6 + 1,  9 + 1, 12 + 1, 15 + 1, 18 + 1,  21 + 1,  24 + 1,  27 + 1} }, { .val = { 30 + 1,  33 + 1,  36 + 1,  39 + 1,  42 + 1,  45 + 1,  48 + 1,  51 + 1} } },
                { { .val = { 8 + 1, 12 + 1, 16 + 1, 20 + 1, 24 + 1,  28 + 1,  32 + 1,  36 + 1} }, { .val = { 40 + 1,  44 + 1,  48 + 1,  52 + 1,  56 + 1,  60 + 1,  64 + 1,  68 + 1} } },
                { { .val = {10 + 1, 15 + 1, 20 + 1, 25 + 1, 30 + 1,  35 + 1,  40 + 1,  45 + 1} }, { .val = { 50 + 1,  55 + 1,  60 + 1,  65 + 1,  70 + 1,  75 + 1,  80 + 1,  85 + 1} } },
                { { .val = {12 + 1, 18 + 1, 24 + 1, 30 + 1, 36 + 1,  42 + 1,  48 + 1,  54 + 1} }, { .val = { 60 + 1,  66 + 1,  72 + 1,  78 + 1,  84 + 1,  90 + 1,  96 + 1, 102 + 1} } },
                { { .val = {14 + 1, 21 + 1, 28 + 1, 35 + 1, 42 + 1,  49 + 1,  56 + 1,  63 + 1} }, { .val = { 70 + 1,  77 + 1,  84 + 1,  91 + 1,  98 + 1, 105 + 1, 112 + 1, 119 + 1} } },
                { { .val = {16 + 1, 24 + 1, 32 + 1, 40 + 1, 48 + 1,  56 + 1,  64 + 1,  72 + 1} }, { .val = { 80 + 1,  88 + 1,  96 + 1, 104 + 1, 112 + 1, 120 + 1, 128 + 1, 136 + 1} } },
                { { .val = {18 + 1, 27 + 1, 36 + 1, 45 + 1, 54 + 1,  63 + 1,  72 + 1,  81 + 1} }, { .val = { 90 + 1,  99 + 1, 108 + 1, 117 + 1, 126 + 1, 135 + 1, 144 + 1, 153 + 1} } },
                { { .val = {20 + 1, 30 + 1, 40 + 1, 50 + 1, 60 + 1,  70 + 1,  80 + 1,  90 + 1} }, { .val = {100 + 1, 110 + 1, 120 + 1, 130 + 1, 140 + 1, 150 + 1, 160 + 1, 170 + 1} } },
                { { .val = {22 + 1, 33 + 1, 44 + 1, 55 + 1, 66 + 1,  77 + 1,  88 + 1,  99 + 1} }, { .val = {110 + 1, 121 + 1, 132 + 1, 143 + 1, 154 + 1, 165 + 1, 176 + 1, 187 + 1} } },
                { { .val = {24 + 1, 36 + 1, 48 + 1, 60 + 1, 72 + 1,  84 + 1,  96 + 1, 108 + 1} }, { .val = {120 + 1, 132 + 1, 144 + 1, 156 + 1, 168 + 1, 180 + 1, 192 + 1, 204 + 1} } },
                { { .val = {26 + 1, 39 + 1, 52 + 1, 65 + 1, 78 + 1,  91 + 1, 104 + 1, 117 + 1} }, { .val = {130 + 1, 143 + 1, 156 + 1, 169 + 1, 182 + 1, 195 + 1, 208 + 1, 221 + 1} } },
                { { .val = {28 + 1, 42 + 1, 56 + 1, 70 + 1, 84 + 1,  98 + 1, 112 + 1, 126 + 1} }, { .val = {140 + 1, 154 + 1, 168 + 1, 182 + 1, 196 + 1, 210 + 1, 224 + 1, 238 + 1} } },
                { { .val = {30 + 1, 45 + 1, 60 + 1, 75 + 1, 90 + 1, 105 + 1, 120 + 1, 135 + 1} }, { .val = {150 + 1, 165 + 1, 180 + 1, 195 + 1, 210 + 1, 225 + 1, 240 + 1, 255 + 1} } },
                { { .val = {32 + 1, 48 + 1, 64 + 1, 80 + 1, 96 + 1, 112 + 1, 128 + 1, 144 + 1} }, { .val = {160 + 1, 176 + 1, 192 + 1, 208 + 1, 224 + 1, 240 + 1, 256 + 1, 272 + 1} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 16, 16, 2);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8si_multiply_chunk_full_to_half)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 1x9 and 9x1 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = {2, 2, 2, 2, 2, 2, 2, 2} }, { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = {90, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 1, 1, 9);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 1x9 and 9x8 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = {2, 2, 2, 2, 2, 2, 2, 2} }, { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = {90, 90, 90, 90, 90, 90, 90, 90} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 1, 8, 9);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x9 and 9x1 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = {2, 2, 2, 2, 2, 2, 2, 2} }, { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {3, 3, 3, 3, 3, 3, 3, 3} }, { .val = {3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {4, 4, 4, 4, 4, 4, 4, 4} }, { .val = {4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {5, 5, 5, 5, 5, 5, 5, 5} }, { .val = {5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {6, 6, 6, 6, 6, 6, 6, 6} }, { .val = {6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {7, 7, 7, 7, 7, 7, 7, 7} }, { .val = {7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {8, 8, 8, 8, 8, 8, 8, 8} }, { .val = {8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {9, 9, 9, 9, 9, 9, 9, 9} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = { 90, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {135, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {180, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {225, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {270, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {315, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {360, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {405, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 8, 1, 9);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x9 and 9x8 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = {2, 2, 2, 2, 2, 2, 2, 2} }, { .val = {2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {3, 3, 3, 3, 3, 3, 3, 3} }, { .val = {3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {4, 4, 4, 4, 4, 4, 4, 4} }, { .val = {4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {5, 5, 5, 5, 5, 5, 5, 5} }, { .val = {5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {6, 6, 6, 6, 6, 6, 6, 6} }, { .val = {6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {7, 7, 7, 7, 7, 7, 7, 7} }, { .val = {7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {8, 8, 8, 8, 8, 8, 8, 8} }, { .val = {8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {9, 9, 9, 9, 9, 9, 9, 9} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = { 90,  90,  90,  90,  90,  90,  90,  90} } },
                { { .val = {135, 135, 135, 135, 135, 135, 135, 135} } },
                { { .val = {180, 180, 180, 180, 180, 180, 180, 180} } },
                { { .val = {225, 225, 225, 225, 225, 225, 225, 225} } },
                { { .val = {270, 270, 270, 270, 270, 270, 270, 270} } },
                { { .val = {315, 315, 315, 315, 315, 315, 315, 315} } },
                { { .val = {360, 360, 360, 360, 360, 360, 360, 360} } },
                { { .val = {405, 405, 405, 405, 405, 405, 405, 405} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 8, 8, 9);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x9 and 9x1 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = { 2,  2,  2,  2,  2,  2,  2,  2} }, { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3,  3,  3,  3,  3,  3,  3,  3} }, { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4,  4,  4,  4,  4,  4,  4,  4} }, { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5,  5,  5,  5,  5,  5,  5,  5} }, { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6,  6,  6,  6,  6,  6,  6,  6} }, { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7,  7,  7,  7,  7,  7,  7,  7} }, { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8,  8,  8,  8,  8,  8,  8,  8} }, { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9,  9,  9,  9,  9,  9,  9,  9} }, { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 10, 10, 10, 10, 10, 10, 10} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 11, 11, 11, 11, 11, 11, 11} }, { .val = {11, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 12, 12, 12, 12, 12, 12, 12} }, { .val = {12, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 13, 13, 13, 13, 13, 13, 13} }, { .val = {13, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 14, 14, 14, 14, 14, 14, 14} }, { .val = {14, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 15, 15, 15, 15, 15, 15, 15} }, { .val = {15, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {17, 17, 17, 17, 17, 17, 17, 17} }, { .val = {17, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = { 90, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {135, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {180, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {225, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {270, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {315, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {360, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {405, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {450, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {495, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {540, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {585, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {630, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {675, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {720, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {765, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 16, 1, 9);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x9 and 9x8 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = { 2,  2,  2,  2,  2,  2,  2,  2} }, { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3,  3,  3,  3,  3,  3,  3,  3} }, { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4,  4,  4,  4,  4,  4,  4,  4} }, { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5,  5,  5,  5,  5,  5,  5,  5} }, { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6,  6,  6,  6,  6,  6,  6,  6} }, { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7,  7,  7,  7,  7,  7,  7,  7} }, { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8,  8,  8,  8,  8,  8,  8,  8} }, { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9,  9,  9,  9,  9,  9,  9,  9} }, { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 10, 10, 10, 10, 10, 10, 10} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 11, 11, 11, 11, 11, 11, 11} }, { .val = {11, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 12, 12, 12, 12, 12, 12, 12} }, { .val = {12, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 13, 13, 13, 13, 13, 13, 13} }, { .val = {13, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 14, 14, 14, 14, 14, 14, 14} }, { .val = {14, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 15, 15, 15, 15, 15, 15, 15} }, { .val = {15, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {17, 17, 17, 17, 17, 17, 17, 17} }, { .val = {17, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x1 = {
                { { .val = { 90,  90,  90,  90,  90,  90,  90,  90} } },
                { { .val = {135, 135, 135, 135, 135, 135, 135, 135} } },
                { { .val = {180, 180, 180, 180, 180, 180, 180, 180} } },
                { { .val = {225, 225, 225, 225, 225, 225, 225, 225} } },
                { { .val = {270, 270, 270, 270, 270, 270, 270, 270} } },
                { { .val = {315, 315, 315, 315, 315, 315, 315, 315} } },
                { { .val = {360, 360, 360, 360, 360, 360, 360, 360} } },
                { { .val = {405, 405, 405, 405, 405, 405, 405, 405} } },
                { { .val = {450, 450, 450, 450, 450, 450, 450, 450} } },
                { { .val = {495, 495, 495, 495, 495, 495, 495, 495} } },
                { { .val = {540, 540, 540, 540, 540, 540, 540, 540} } },
                { { .val = {585, 585, 585, 585, 585, 585, 585, 585} } },
                { { .val = {630, 630, 630, 630, 630, 630, 630, 630} } },
                { { .val = {675, 675, 675, 675, 675, 675, 675, 675} } },
                { { .val = {720, 720, 720, 720, 720, 720, 720, 720} } },
                { { .val = {765, 765, 765, 765, 765, 765, 765, 765} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 16, 8, 9);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8si_multiply_chunk_full_to_full)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 9x9 and 9x9 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = { 2,  2,  2,  2,  2,  2,  2,  2} }, { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3,  3,  3,  3,  3,  3,  3,  3} }, { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4,  4,  4,  4,  4,  4,  4,  4} }, { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5,  5,  5,  5,  5,  5,  5,  5} }, { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6,  6,  6,  6,  6,  6,  6,  6} }, { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7,  7,  7,  7,  7,  7,  7,  7} }, { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8,  8,  8,  8,  8,  8,  8,  8} }, { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9,  9,  9,  9,  9,  9,  9,  9} }, { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 10, 10, 10, 10, 10, 10, 10} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = { 90,  90,  90,  90,  90,  90,  90,  90} }, { .val = { 90, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {135, 135, 135, 135, 135, 135, 135, 135} }, { .val = {135, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {180, 180, 180, 180, 180, 180, 180, 180} }, { .val = {180, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {225, 225, 225, 225, 225, 225, 225, 225} }, { .val = {225, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {270, 270, 270, 270, 270, 270, 270, 270} }, { .val = {270, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {315, 315, 315, 315, 315, 315, 315, 315} }, { .val = {315, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {360, 360, 360, 360, 360, 360, 360, 360} }, { .val = {360, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {405, 405, 405, 405, 405, 405, 405, 405} }, { .val = {405, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {450, 450, 450, 450, 450, 450, 450, 450} }, { .val = {450, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 9, 9, 9);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 9x9 and 9x16 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = { 2,  2,  2,  2,  2,  2,  2,  2} }, { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3,  3,  3,  3,  3,  3,  3,  3} }, { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4,  4,  4,  4,  4,  4,  4,  4} }, { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5,  5,  5,  5,  5,  5,  5,  5} }, { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6,  6,  6,  6,  6,  6,  6,  6} }, { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7,  7,  7,  7,  7,  7,  7,  7} }, { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8,  8,  8,  8,  8,  8,  8,  8} }, { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9,  9,  9,  9,  9,  9,  9,  9} }, { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 10, 10, 10, 10, 10, 10, 10} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = { 90,  90,  90,  90,  90,  90,  90,  90} }, { .val = { 90,  90,  90,  90,  90,  90,  90,  90} } },
                { { .val = {135, 135, 135, 135, 135, 135, 135, 135} }, { .val = {135, 135, 135, 135, 135, 135, 135, 135} } },
                { { .val = {180, 180, 180, 180, 180, 180, 180, 180} }, { .val = {180, 180, 180, 180, 180, 180, 180, 180} } },
                { { .val = {225, 225, 225, 225, 225, 225, 225, 225} }, { .val = {225, 225, 225, 225, 225, 225, 225, 225} } },
                { { .val = {270, 270, 270, 270, 270, 270, 270, 270} }, { .val = {270, 270, 270, 270, 270, 270, 270, 270} } },
                { { .val = {315, 315, 315, 315, 315, 315, 315, 315} }, { .val = {315, 315, 315, 315, 315, 315, 315, 315} } },
                { { .val = {360, 360, 360, 360, 360, 360, 360, 360} }, { .val = {360, 360, 360, 360, 360, 360, 360, 360} } },
                { { .val = {405, 405, 405, 405, 405, 405, 405, 405} }, { .val = {405, 405, 405, 405, 405, 405, 405, 405} } },
                { { .val = {450, 450, 450, 450, 450, 450, 450, 450} }, { .val = {450, 450, 450, 450, 450, 450, 450, 450} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 9, 16, 9);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x9 and 9x9 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = { 2,  2,  2,  2,  2,  2,  2,  2} }, { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3,  3,  3,  3,  3,  3,  3,  3} }, { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4,  4,  4,  4,  4,  4,  4,  4} }, { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5,  5,  5,  5,  5,  5,  5,  5} }, { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6,  6,  6,  6,  6,  6,  6,  6} }, { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7,  7,  7,  7,  7,  7,  7,  7} }, { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8,  8,  8,  8,  8,  8,  8,  8} }, { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9,  9,  9,  9,  9,  9,  9,  9} }, { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 10, 10, 10, 10, 10, 10, 10} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 11, 11, 11, 11, 11, 11, 11} }, { .val = {11, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 12, 12, 12, 12, 12, 12, 12} }, { .val = {12, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 13, 13, 13, 13, 13, 13, 13} }, { .val = {13, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 14, 14, 14, 14, 14, 14, 14} }, { .val = {14, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 15, 15, 15, 15, 15, 15, 15} }, { .val = {15, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {17, 17, 17, 17, 17, 17, 17, 17} }, { .val = {17, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = { 90,  90,  90,  90,  90,  90,  90,  90} }, { .val = { 90, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {135, 135, 135, 135, 135, 135, 135, 135} }, { .val = {135, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {180, 180, 180, 180, 180, 180, 180, 180} }, { .val = {180, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {225, 225, 225, 225, 225, 225, 225, 225} }, { .val = {225, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {270, 270, 270, 270, 270, 270, 270, 270} }, { .val = {270, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {315, 315, 315, 315, 315, 315, 315, 315} }, { .val = {315, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {360, 360, 360, 360, 360, 360, 360, 360} }, { .val = {360, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {405, 405, 405, 405, 405, 405, 405, 405} }, { .val = {405, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {450, 450, 450, 450, 450, 450, 450, 450} }, { .val = {450, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {495, 495, 495, 495, 495, 495, 495, 495} }, { .val = {495, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {540, 540, 540, 540, 540, 540, 540, 540} }, { .val = {540, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {585, 585, 585, 585, 585, 585, 585, 585} }, { .val = {585, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {630, 630, 630, 630, 630, 630, 630, 630} }, { .val = {630, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {675, 675, 675, 675, 675, 675, 675, 675} }, { .val = {675, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {720, 720, 720, 720, 720, 720, 720, 720} }, { .val = {720, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {765, 765, 765, 765, 765, 765, 765, 765} }, { .val = {765, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 16, 9, 9);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x9 and 9x16 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = { 2,  2,  2,  2,  2,  2,  2,  2} }, { .val = { 2, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 3,  3,  3,  3,  3,  3,  3,  3} }, { .val = { 3, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 4,  4,  4,  4,  4,  4,  4,  4} }, { .val = { 4, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 5,  5,  5,  5,  5,  5,  5,  5} }, { .val = { 5, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 6,  6,  6,  6,  6,  6,  6,  6} }, { .val = { 6, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 7,  7,  7,  7,  7,  7,  7,  7} }, { .val = { 7, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 8,  8,  8,  8,  8,  8,  8,  8} }, { .val = { 8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 9,  9,  9,  9,  9,  9,  9,  9} }, { .val = { 9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {10, 10, 10, 10, 10, 10, 10, 10} }, { .val = {10, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {11, 11, 11, 11, 11, 11, 11, 11} }, { .val = {11, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {12, 12, 12, 12, 12, 12, 12, 12} }, { .val = {12, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {13, 13, 13, 13, 13, 13, 13, 13} }, { .val = {13, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {14, 14, 14, 14, 14, 14, 14, 14} }, { .val = {14, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {15, 15, 15, 15, 15, 15, 15, 15} }, { .val = {15, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {16, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {17, 17, 17, 17, 17, 17, 17, 17} }, { .val = {17, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = { 90,  90,  90,  90,  90,  90,  90,  90} }, { .val = { 90,  90,  90,  90,  90,  90,  90,  90} } },
                { { .val = {135, 135, 135, 135, 135, 135, 135, 135} }, { .val = {135, 135, 135, 135, 135, 135, 135, 135} } },
                { { .val = {180, 180, 180, 180, 180, 180, 180, 180} }, { .val = {180, 180, 180, 180, 180, 180, 180, 180} } },
                { { .val = {225, 225, 225, 225, 225, 225, 225, 225} }, { .val = {225, 225, 225, 225, 225, 225, 225, 225} } },
                { { .val = {270, 270, 270, 270, 270, 270, 270, 270} }, { .val = {270, 270, 270, 270, 270, 270, 270, 270} } },
                { { .val = {315, 315, 315, 315, 315, 315, 315, 315} }, { .val = {315, 315, 315, 315, 315, 315, 315, 315} } },
                { { .val = {360, 360, 360, 360, 360, 360, 360, 360} }, { .val = {360, 360, 360, 360, 360, 360, 360, 360} } },
                { { .val = {405, 405, 405, 405, 405, 405, 405, 405} }, { .val = {405, 405, 405, 405, 405, 405, 405, 405} } },
                { { .val = {450, 450, 450, 450, 450, 450, 450, 450} }, { .val = {450, 450, 450, 450, 450, 450, 450, 450} } },
                { { .val = {495, 495, 495, 495, 495, 495, 495, 495} }, { .val = {495, 495, 495, 495, 495, 495, 495, 495} } },
                { { .val = {540, 540, 540, 540, 540, 540, 540, 540} }, { .val = {540, 540, 540, 540, 540, 540, 540, 540} } },
                { { .val = {585, 585, 585, 585, 585, 585, 585, 585} }, { .val = {585, 585, 585, 585, 585, 585, 585, 585} } },
                { { .val = {630, 630, 630, 630, 630, 630, 630, 630} }, { .val = {630, 630, 630, 630, 630, 630, 630, 630} } },
                { { .val = {675, 675, 675, 675, 675, 675, 675, 675} }, { .val = {675, 675, 675, 675, 675, 675, 675, 675} } },
                { { .val = {720, 720, 720, 720, 720, 720, 720, 720} }, { .val = {720, 720, 720, 720, 720, 720, 720, 720} } },
                { { .val = {765, 765, 765, 765, 765, 765, 765, 765} }, { .val = {765, 765, 765, 765, 765, 765, 765, 765} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 16, 16, 9);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x16 and 16x16 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8si_16x2 = {
                { { .val = { 2,  2,  2,  2,  2,  2,  2,  2} }, { .val = { 2,  2,  2,  2,  2,  2,  2,  2} } },
                { { .val = { 3,  3,  3,  3,  3,  3,  3,  3} }, { .val = { 3,  3,  3,  3,  3,  3,  3,  3} } },
                { { .val = { 4,  4,  4,  4,  4,  4,  4,  4} }, { .val = { 4,  4,  4,  4,  4,  4,  4,  4} } },
                { { .val = { 5,  5,  5,  5,  5,  5,  5,  5} }, { .val = { 5,  5,  5,  5,  5,  5,  5,  5} } },
                { { .val = { 6,  6,  6,  6,  6,  6,  6,  6} }, { .val = { 6,  6,  6,  6,  6,  6,  6,  6} } },
                { { .val = { 7,  7,  7,  7,  7,  7,  7,  7} }, { .val = { 7,  7,  7,  7,  7,  7,  7,  7} } },
                { { .val = { 8,  8,  8,  8,  8,  8,  8,  8} }, { .val = { 8,  8,  8,  8,  8,  8,  8,  8} } },
                { { .val = { 9,  9,  9,  9,  9,  9,  9,  9} }, { .val = { 9,  9,  9,  9,  9,  9,  9,  9} } },
                { { .val = {10, 10, 10, 10, 10, 10, 10, 10} }, { .val = {10, 10, 10, 10, 10, 10, 10, 10} } },
                { { .val = {11, 11, 11, 11, 11, 11, 11, 11} }, { .val = {11, 11, 11, 11, 11, 11, 11, 11} } },
                { { .val = {12, 12, 12, 12, 12, 12, 12, 12} }, { .val = {12, 12, 12, 12, 12, 12, 12, 12} } },
                { { .val = {13, 13, 13, 13, 13, 13, 13, 13} }, { .val = {13, 13, 13, 13, 13, 13, 13, 13} } },
                { { .val = {14, 14, 14, 14, 14, 14, 14, 14} }, { .val = {14, 14, 14, 14, 14, 14, 14, 14} } },
                { { .val = {15, 15, 15, 15, 15, 15, 15, 15} }, { .val = {15, 15, 15, 15, 15, 15, 15, 15} } },
                { { .val = {16, 16, 16, 16, 16, 16, 16, 16} }, { .val = {16, 16, 16, 16, 16, 16, 16, 16} } },
                { { .val = {17, 17, 17, 17, 17, 17, 17, 17} }, { .val = {17, 17, 17, 17, 17, 17, 17, 17} } },
            }
        };
        mx_chunk_t rchk = {
            .v8si_16x2 = {
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
                { { .val = {1, 2, 3, 4, 5, 6, 7, 8} }, { .val = {9, 10, 11, 12, 13, 14, 15, 16} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8si_16x2 = {
                { { .val = { 272,  272,  272,  272,  272,  272,  272,  272} }, { .val = { 272,  272,  272,  272,  272,  272,  272,  272} } },
                { { .val = { 408,  408,  408,  408,  408,  408,  408,  408} }, { .val = { 408,  408,  408,  408,  408,  408,  408,  408} } },
                { { .val = { 544,  544,  544,  544,  544,  544,  544,  544} }, { .val = { 544,  544,  544,  544,  544,  544,  544,  544} } },
                { { .val = { 680,  680,  680,  680,  680,  680,  680,  680} }, { .val = { 680,  680,  680,  680,  680,  680,  680,  680} } },
                { { .val = { 816,  816,  816,  816,  816,  816,  816,  816} }, { .val = { 816,  816,  816,  816,  816,  816,  816,  816} } },
                { { .val = { 952,  952,  952,  952,  952,  952,  952,  952} }, { .val = { 952,  952,  952,  952,  952,  952,  952,  952} } },
                { { .val = {1088, 1088, 1088, 1088, 1088, 1088, 1088, 1088} }, { .val = {1088, 1088, 1088, 1088, 1088, 1088, 1088, 1088} } },
                { { .val = {1224, 1224, 1224, 1224, 1224, 1224, 1224, 1224} }, { .val = {1224, 1224, 1224, 1224, 1224, 1224, 1224, 1224} } },
                { { .val = {1360, 1360, 1360, 1360, 1360, 1360, 1360, 1360} }, { .val = {1360, 1360, 1360, 1360, 1360, 1360, 1360, 1360} } },
                { { .val = {1496, 1496, 1496, 1496, 1496, 1496, 1496, 1496} }, { .val = {1496, 1496, 1496, 1496, 1496, 1496, 1496, 1496} } },
                { { .val = {1632, 1632, 1632, 1632, 1632, 1632, 1632, 1632} }, { .val = {1632, 1632, 1632, 1632, 1632, 1632, 1632, 1632} } },
                { { .val = {1768, 1768, 1768, 1768, 1768, 1768, 1768, 1768} }, { .val = {1768, 1768, 1768, 1768, 1768, 1768, 1768, 1768} } },
                { { .val = {1904, 1904, 1904, 1904, 1904, 1904, 1904, 1904} }, { .val = {1904, 1904, 1904, 1904, 1904, 1904, 1904, 1904} } },
                { { .val = {2040, 2040, 2040, 2040, 2040, 2040, 2040, 2040} }, { .val = {2040, 2040, 2040, 2040, 2040, 2040, 2040, 2040} } },
                { { .val = {2176, 2176, 2176, 2176, 2176, 2176, 2176, 2176} }, { .val = {2176, 2176, 2176, 2176, 2176, 2176, 2176, 2176} } },
                { { .val = {2312, 2312, 2312, 2312, 2312, 2312, 2312, 2312} }, { .val = {2312, 2312, 2312, 2312, 2312, 2312, 2312, 2312} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8si_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 16, 16, 16);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8si_multiply_scalar_chunk_fully)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 16x16 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} }, { .val = {  8,   9,  10,  11,  12,  13,  14,  15} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} }, { .val = { 24,  25,  26,  27,  28,  29,  30,  31} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} }, { .val = { 40,  41,  42,  43,  44,  45,  46,  47} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} }, { .val = { 56,  57,  58,  59,  60,  61,  62,  63} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} }, { .val = { 72,  73,  74,  75,  76,  77,  78,  79} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} }, { .val = { 88,  89,  90,  91,  92,  93,  94,  95} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} }, { .val = {104, 105, 106, 107, 108, 109, 110, 111} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} }, { .val = {120, 121, 122, 123, 124, 125, 126, 127} } },
                { { .val = {128, 129, 130, 131, 132, 133, 134, 135} }, { .val = {136, 137, 138, 139, 140, 141, 142, 143} } },
                { { .val = {144, 145, 146, 147, 148, 149, 150, 151} }, { .val = {152, 153, 154, 155, 156, 157, 158, 159} } },
                { { .val = {160, 161, 162, 163, 164, 165, 166, 167} }, { .val = {168, 169, 170, 171, 172, 173, 174, 175} } },
                { { .val = {176, 177, 178, 179, 180, 181, 182, 183} }, { .val = {184, 185, 186, 187, 188, 189, 190, 191} } },
                { { .val = {192, 193, 194, 195, 196, 197, 198, 199} }, { .val = {200, 201, 202, 203, 204, 205, 206, 207} } },
                { { .val = {208, 209, 210, 211, 212, 213, 214, 215} }, { .val = {216, 217, 218, 219, 220, 221, 222, 223} } },
                { { .val = {224, 225, 226, 227, 228, 229, 230, 231} }, { .val = {232, 233, 234, 235, 236, 237, 238, 239} } },
                { { .val = {240, 241, 242, 243, 244, 245, 246, 247} }, { .val = {248, 249, 250, 251, 252, 253, 254, 255} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_fully(&dst, &src, &factor);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[0], mx_type_val(src.v8si_16x2[i][j])[0] * 2, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[1], mx_type_val(src.v8si_16x2[i][j])[1] * 2, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[2], mx_type_val(src.v8si_16x2[i][j])[2] * 2, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[3], mx_type_val(src.v8si_16x2[i][j])[3] * 2, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[4], mx_type_val(src.v8si_16x2[i][j])[4] * 2, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[5], mx_type_val(src.v8si_16x2[i][j])[5] * 2, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[6], mx_type_val(src.v8si_16x2[i][j])[6] * 2, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[7], mx_type_val(src.v8si_16x2[i][j])[7] * 2, i, j * 8 + 7);
            } // for
        } // for
    }
}

Test(InternalFunc, v8si_multiply_scalar_chunk_partly)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 1x1 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {0, 1, 2, 3, 4, 5, 6, 7} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 1, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
    }
    
    // -- 5x5 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0,   1,   2,   3,   4, 0, 0, 0} } },
                { { .val = { 16,  17,  18,  19,  20, 0, 0, 0} } },
                { { .val = { 32,  33,  34,  35,  36, 0, 0, 0} } },
                { { .val = { 48,  49,  50,  51,  52, 0, 0, 0} } },
                { { .val = { 64,  65,  66,  67,  68, 0, 0, 0} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 5, 5);
        for (i = 0; i < 5; i += 1) {
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], mx_type_val(src.v8si_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], mx_type_val(src.v8si_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], mx_type_val(src.v8si_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], mx_type_val(src.v8si_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
        } // for
    }

    // -- 8x8 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 8, 8);
        for (i = 0; i < 8; i += 1) {
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], mx_type_val(src.v8si_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], mx_type_val(src.v8si_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], mx_type_val(src.v8si_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], mx_type_val(src.v8si_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[5], mx_type_val(src.v8si_16x1[0][0])[5] * 2, 0, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[6], mx_type_val(src.v8si_16x1[0][0])[6] * 2, 0, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8si_16x1[0][0])[7], mx_type_val(src.v8si_16x1[0][0])[7] * 2, 0, 0 * 8 + 7);
        } // for
    }
    
    // -- 9x9 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} }, { .val = {  8, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} }, { .val = { 24, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} }, { .val = { 40, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} }, { .val = { 56, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} }, { .val = { 72, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} }, { .val = { 88, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} }, { .val = {104, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} }, { .val = {120, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {128, 129, 130, 131, 132, 133, 134, 135} }, { .val = {136, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 9, 9);
        for (i = 0; i < 9; i += 1) {
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[0], mx_type_val(src.v8si_16x2[0][0])[0] * 2, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[1], mx_type_val(src.v8si_16x2[0][0])[1] * 2, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[2], mx_type_val(src.v8si_16x2[0][0])[2] * 2, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[3], mx_type_val(src.v8si_16x2[0][0])[3] * 2, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[4], mx_type_val(src.v8si_16x2[0][0])[4] * 2, 0, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[5], mx_type_val(src.v8si_16x2[0][0])[5] * 2, 0, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[6], mx_type_val(src.v8si_16x2[0][0])[6] * 2, 0, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8si_16x2[0][0])[7], mx_type_val(src.v8si_16x2[0][0])[7] * 2, 0, 0 * 8 + 7);

            check_value_at(mx_type_val(dst.v8si_16x2[0][1])[0], mx_type_val(src.v8si_16x2[0][1])[0] * 2, 0, 1 * 8 + 0);
        } // for
    }
    
    // -- 16x16 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {  0,   1,   2,   3,   4,   5,   6,   7} }, { .val = {  8,   9,  10,  11,  12,  13,  14,  15} } },
                { { .val = { 16,  17,  18,  19,  20,  21,  22,  23} }, { .val = { 24,  25,  26,  27,  28,  29,  30,  31} } },
                { { .val = { 32,  33,  34,  35,  36,  37,  38,  39} }, { .val = { 40,  41,  42,  43,  44,  45,  46,  47} } },
                { { .val = { 48,  49,  50,  51,  52,  53,  54,  55} }, { .val = { 56,  57,  58,  59,  60,  61,  62,  63} } },
                { { .val = { 64,  65,  66,  67,  68,  69,  70,  71} }, { .val = { 72,  73,  74,  75,  76,  77,  78,  79} } },
                { { .val = { 80,  81,  82,  83,  84,  85,  86,  87} }, { .val = { 88,  89,  90,  91,  92,  93,  94,  95} } },
                { { .val = { 96,  97,  98,  99, 100, 101, 102, 103} }, { .val = {104, 105, 106, 107, 108, 109, 110, 111} } },
                { { .val = {112, 113, 114, 115, 116, 117, 118, 119} }, { .val = {120, 121, 122, 123, 124, 125, 126, 127} } },
                { { .val = {128, 129, 130, 131, 132, 133, 134, 135} }, { .val = {136, 137, 138, 139, 140, 141, 142, 143} } },
                { { .val = {144, 145, 146, 147, 148, 149, 150, 151} }, { .val = {152, 153, 154, 155, 156, 157, 158, 159} } },
                { { .val = {160, 161, 162, 163, 164, 165, 166, 167} }, { .val = {168, 169, 170, 171, 172, 173, 174, 175} } },
                { { .val = {176, 177, 178, 179, 180, 181, 182, 183} }, { .val = {184, 185, 186, 187, 188, 189, 190, 191} } },
                { { .val = {192, 193, 194, 195, 196, 197, 198, 199} }, { .val = {200, 201, 202, 203, 204, 205, 206, 207} } },
                { { .val = {208, 209, 210, 211, 212, 213, 214, 215} }, { .val = {216, 217, 218, 219, 220, 221, 222, 223} } },
                { { .val = {224, 225, 226, 227, 228, 229, 230, 231} }, { .val = {232, 233, 234, 235, 236, 237, 238, 239} } },
                { { .val = {240, 241, 242, 243, 244, 245, 246, 247} }, { .val = {248, 249, 250, 251, 252, 253, 254, 255} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[0], mx_type_val(src.v8si_16x2[i][j])[0] * 2, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[1], mx_type_val(src.v8si_16x2[i][j])[1] * 2, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[2], mx_type_val(src.v8si_16x2[i][j])[2] * 2, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[3], mx_type_val(src.v8si_16x2[i][j])[3] * 2, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[4], mx_type_val(src.v8si_16x2[i][j])[4] * 2, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[5], mx_type_val(src.v8si_16x2[i][j])[5] * 2, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[6], mx_type_val(src.v8si_16x2[i][j])[6] * 2, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8si_16x2[i][j])[7], mx_type_val(src.v8si_16x2[i][j])[7] * 2, i, j * 8 + 7);
            } // for
        } // for
    }

    // -- 1x5 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = { 0,  1,  2,  3,  4, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 1, 5);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], mx_type_val(src.v8si_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], mx_type_val(src.v8si_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], mx_type_val(src.v8si_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], mx_type_val(src.v8si_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
    }

    // -- 1x8 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {0, 1, 2, 3, 4, 5, 6, 7} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 1, 8);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[1], mx_type_val(src.v8si_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[2], mx_type_val(src.v8si_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[3], mx_type_val(src.v8si_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[4], mx_type_val(src.v8si_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[5], mx_type_val(src.v8si_16x1[0][0])[5] * 2, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[6], mx_type_val(src.v8si_16x1[0][0])[6] * 2, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[7], mx_type_val(src.v8si_16x1[0][0])[7] * 2, 0, 0 * 8 + 7);
    }

    // -- 1x9 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x2 = {
                { { .val = {0, 1, 2, 3, 4, 5, 6, 7} },  { .val = {8, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 1, 9);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[0], mx_type_val(src.v8si_16x2[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[1], mx_type_val(src.v8si_16x2[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[2], mx_type_val(src.v8si_16x2[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[3], mx_type_val(src.v8si_16x2[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[4], mx_type_val(src.v8si_16x2[0][0])[4] * 2, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[5], mx_type_val(src.v8si_16x2[0][0])[5] * 2, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[6], mx_type_val(src.v8si_16x2[0][0])[6] * 2, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x2[0][0])[7], mx_type_val(src.v8si_16x2[0][0])[7] * 2, 0, 0 * 8 + 7);

        check_value_at(mx_type_val(dst.v8si_16x2[0][1])[0], mx_type_val(src.v8si_16x2[0][1])[0] * 2, 0, 1 * 8 + 0);
    }

    // -- 5x1 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 5, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[1][0])[0], mx_type_val(src.v8si_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[2][0])[0], mx_type_val(src.v8si_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[3][0])[0], mx_type_val(src.v8si_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[4][0])[0], mx_type_val(src.v8si_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
    }

    // -- 8x1 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 80, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 96, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {112, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 8, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[1][0])[0], mx_type_val(src.v8si_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[2][0])[0], mx_type_val(src.v8si_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[3][0])[0], mx_type_val(src.v8si_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[4][0])[0], mx_type_val(src.v8si_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x1[5][0])[0], mx_type_val(src.v8si_16x1[5][0])[0] * 2, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x1[6][0])[0], mx_type_val(src.v8si_16x1[6][0])[0] * 2, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x1[7][0])[0], mx_type_val(src.v8si_16x1[7][0])[0] * 2, 7, 0 * 8 + 7);
    }

    // -- 9x1 chunk -- //
    {
        v8si_t factor = { .val = {2, 2, 2, 2, 2, 2, 2, 2} };
        mx_chunk_t src = {
            .v8si_16x1 = {
                { { .val = {  0, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 16, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 32, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 48, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 64, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 80, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = { 96, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {112, 0, 0, 0, 0, 0, 0, 0} } },
                { { .val = {128, 0, 0, 0, 0, 0, 0, 0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8si_multiply_scalar_chunk_partly(&dst, &src, &factor, 9, 1);
        check_value_at(mx_type_val(dst.v8si_16x1[0][0])[0], mx_type_val(src.v8si_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8si_16x1[1][0])[0], mx_type_val(src.v8si_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8si_16x1[2][0])[0], mx_type_val(src.v8si_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8si_16x1[3][0])[0], mx_type_val(src.v8si_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8si_16x1[4][0])[0], mx_type_val(src.v8si_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8si_16x1[5][0])[0], mx_type_val(src.v8si_16x1[5][0])[0] * 2, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8si_16x1[6][0])[0], mx_type_val(src.v8si_16x1[6][0])[0] * 2, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8si_16x1[7][0])[0], mx_type_val(src.v8si_16x1[7][0])[0] * 2, 7, 0 * 8 + 7);
        check_value_at(mx_type_val(dst.v8si_16x1[8][1])[0], mx_type_val(src.v8si_16x1[8][1])[0] * 2, 8, 1 * 8 + 0);
    }
}

Test(Operation, mops_v8si_add)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr lhs = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr mx = NULL;

    // -- 1x1 matrix addition -- //
    {
        lhs = mstr_v8si_create(1, 1);
        mstr_set_i32(lhs, 0, 0, 1);

        rhs = mstr_v8si_create(1, 1);
        mstr_set_i32(rhs, 0, 0, 99);

        mx = mstr_v8si_create(1, 1);

        mops_v8si_add(mx, lhs, rhs);

        check_value_at(mstr_get_i32(mx, 0, 0), 100, 0, 0);

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 5x5 matrix addition -- //
    {
        lhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(5, 5);

        mops_v8si_add(mx, lhs, rhs);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 8x8 matrix addition -- //
    {
        lhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(8, 8);

        mops_v8si_add(mx, lhs, rhs);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 9x9 matrix addition -- //
    {
        lhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(9, 9);

        mops_v8si_add(mx, lhs, rhs);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 16x16 matrix addition -- //
    {
        lhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(16, 16);

        mops_v8si_add(mx, lhs, rhs);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 17x17 matrix addition -- //
    {
        lhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(17, 17);

        mops_v8si_add(mx, lhs, rhs);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 2x33 matrix addition -- //
    {
        lhs = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(2, 33);

        mops_v8si_add(mx, lhs, rhs);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 33x17 matrix addition -- //
    {
        lhs = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(33, 17);

        mops_v8si_add(mx, lhs, rhs);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }
}

Test(Operation, mops_v8si_subtract)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr lhs = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr mx = NULL;

    // -- 1x1 matrix subtraction -- //
    {
        lhs = mstr_v8si_create(1, 1);
        mstr_set_i32(lhs, 0, 0, 1);

        rhs = mstr_v8si_create(1, 1);
        mstr_set_i32(rhs, 0, 0, 99);

        mx = mstr_v8si_create(1, 1);

        mops_v8si_subtract(mx, lhs, rhs);

        check_value_at(mstr_get_i32(mx, 0, 0), -98, 0, 0);

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 5x5 matrix subtraction -- //
    {
        lhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(5, 5);

        mops_v8si_subtract(mx, lhs, rhs);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 8x8 matrix subtraction -- //
    {
        lhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(8, 8);

        mops_v8si_subtract(mx, lhs, rhs);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 9x9 matrix subtraction -- //
    {
        lhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(9, 9);

        mops_v8si_subtract(mx, lhs, rhs);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 16x16 matrix subtraction -- //
    {
        lhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(16, 16);

        mops_v8si_subtract(mx, lhs, rhs);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 17x17 matrix subtraction -- //
    {
        lhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(17, 17);

        mops_v8si_subtract(mx, lhs, rhs);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 2x33 matrix subtraction -- //
    {
        lhs = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(2, 33);

        mops_v8si_subtract(mx, lhs, rhs);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 33x17 matrix subtraction -- //
    {
        lhs = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(rhs, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(33, 17);

        mops_v8si_subtract(mx, lhs, rhs);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }
}

Test(Operation, mops_v8si_multiply_scalar)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr src = NULL;
    mx_stor_ptr mx = NULL;

    // -- 1x1 matrix scalar multiplication -- //
    {
        src = mstr_v8si_create(1, 1);
        mstr_set_i32(src, 0, 0, 99);

        mx = mstr_v8si_create(1, 1);

        mops_v8si_multiply_scalar(mx, src, 2);

        check_value_at(mstr_get_i32(mx, 0, 0), 198, 0, 0);

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(src);
    }

    // -- 5x5 matrix scalar multiplication -- //
    {
        src = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_i32(src, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(5, 5);

        mops_v8si_multiply_scalar(mx, src, 3);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), 3 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(src);
    }

    // -- 8x8 matrix scalar multiplication -- //
    {
        src = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_i32(src, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(8, 8);

        mops_v8si_multiply_scalar(mx, src, 14);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), 14 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(src);
    }

    // -- 9x9 matrix scalar multiplication -- //
    {
        src = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(src, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(9, 9);

        mops_v8si_multiply_scalar(mx, src, 7);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), 7 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(src);
    }

    // -- 16x16 matrix scalar multiplication -- //
    {
        src = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_i32(src, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(16, 16);

        mops_v8si_multiply_scalar(mx, src, 25);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), 25 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(src);
    }

    // -- 17x17 matrix scalar multiplication -- //
    {
        src = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(src, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(17, 17);

        mops_v8si_multiply_scalar(mx, src, 99);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), 99 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(src);
    }

    // -- 2x33 matrix scalar multiplication -- //
    {
        src = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_i32(src, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(2, 33);

        mops_v8si_multiply_scalar(mx, src, 13);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), 13 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(src);
    }

    // -- 33x17 matrix scalar multiplication -- //
    {
        src = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(src, i, j, i - j);
            } // for
        } // for

        mx = mstr_v8si_create(33, 17);

        mops_v8si_multiply_scalar(mx, src, 77);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_i32(mx, i, j), 77 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(mx);
        mstr_v8si_destroy(src);
    }
}

Test(Operation, mops_v8si_multiply)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr lhs = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr dst = NULL;

    // -- 1x1 matrix multiplication -- //
    {
        lhs = mstr_v8si_create(1, 1);
        mstr_set_i32(lhs, 0, 0, 11);

        rhs = mstr_v8si_create(1, 1);
        mstr_set_i32(rhs, 0, 0, 2);

        dst = mstr_v8si_create(1, 1);

        mops_v8si_multiply(dst, lhs, rhs);

        check_value_at(mstr_get_i32(dst, 0, 0), 22, 0, 0);

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 5x5 matrix multiplication -- //
    {
        int32_t ret_vals[5][5] = {
            {0, 5, 10, 15, 20},
            {0, 5, 10, 15, 20},
            {0, 5, 10, 15, 20},
            {0, 5, 10, 15, 20},
            {0, 5, 10, 15, 20}
        };

        lhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_i32(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_i32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8si_create(5, 5);

        mops_v8si_multiply(dst, lhs, rhs);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 8x8 matrix multiplication -- //
    {
        int32_t ret_vals[8][8] = {
            {0, 8, 16, 24, 32, 40, 48, 56},
            {0, 8, 16, 24, 32, 40, 48, 56},
            {0, 8, 16, 24, 32, 40, 48, 56},
            {0, 8, 16, 24, 32, 40, 48, 56},
            {0, 8, 16, 24, 32, 40, 48, 56},
            {0, 8, 16, 24, 32, 40, 48, 56},
            {0, 8, 16, 24, 32, 40, 48, 56},
            {0, 8, 16, 24, 32, 40, 48, 56}
        };

        lhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_i32(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_i32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8si_create(8, 8);

        mops_v8si_multiply(dst, lhs, rhs);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 9x9 matrix multiplication -- //
    {
        int32_t ret_vals[9][9] = {
            {0, 9, 18, 27, 36, 45, 54, 63, 72},
            {0, 9, 18, 27, 36, 45, 54, 63, 72},
            {0, 9, 18, 27, 36, 45, 54, 63, 72},
            {0, 9, 18, 27, 36, 45, 54, 63, 72},
            {0, 9, 18, 27, 36, 45, 54, 63, 72},
            {0, 9, 18, 27, 36, 45, 54, 63, 72},
            {0, 9, 18, 27, 36, 45, 54, 63, 72},
            {0, 9, 18, 27, 36, 45, 54, 63, 72},
            {0, 9, 18, 27, 36, 45, 54, 63, 72}
        };

        lhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8si_create(9, 9);

        mops_v8si_multiply(dst, lhs, rhs);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 16x16 matrix multiplication -- //
    {
        int32_t ret_vals[16][16] = {
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
            {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240}
        };

        lhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_i32(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_i32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8si_create(16, 16);

        mops_v8si_multiply(dst, lhs, rhs);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 17x17 matrix multiplication -- //
    {
        int32_t ret_vals[17][17] = {
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272},
            {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272}
        };

        lhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8si_create(17, 17);

        mops_v8si_multiply(dst, lhs, rhs);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }

    // -- 9x33 and 33x9 matrix multiplication -- //
    {
        int32_t ret_vals[9][9] = {
            {0, 33, 66, 99, 132, 165, 198, 231, 264},
            {0, 33, 66, 99, 132, 165, 198, 231, 264},
            {0, 33, 66, 99, 132, 165, 198, 231, 264},
            {0, 33, 66, 99, 132, 165, 198, 231, 264},
            {0, 33, 66, 99, 132, 165, 198, 231, 264},
            {0, 33, 66, 99, 132, 165, 198, 231, 264},
            {0, 33, 66, 99, 132, 165, 198, 231, 264},
            {0, 33, 66, 99, 132, 165, 198, 231, 264},
            {0, 33, 66, 99, 132, 165, 198, 231, 264}
        };

        lhs = mstr_v8si_create(9, 33);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_i32(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(33, 9);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8si_create(9, 9);

        mops_v8si_multiply(dst, lhs, rhs);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
    }
}

Test(Operation, mops_v8si_transpose)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr src = NULL;
    mx_stor_ptr dst = NULL;

    // -- 1x1 matrix transpose -- //
    {
        src = mstr_v8si_create(1, 1);
        mstr_set_i32(src, 0, 0, 11);

        dst = mstr_v8si_create(1, 1);

        mops_v8si_transpose(dst, src);

        check_value_at(mstr_get_i32(dst, 0, 0), 11, 0, 0);

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
    }

    // -- 5x5 matrix transpose -- //
    {
        src = mstr_v8si_create(5, 5);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_i32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(5, 5);

        mops_v8si_transpose(dst, src);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), mstr_get_i32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
    }

    // -- 8x8 matrix transpose -- //
    {
        src = mstr_v8si_create(8, 8);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_i32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(8, 8);

        mops_v8si_transpose(dst, src);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), mstr_get_i32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
    }

    // -- 9x9 matrix transpose -- //
    {
        src = mstr_v8si_create(9, 9);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(9, 9);

        mops_v8si_transpose(dst, src);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), mstr_get_i32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
    }

    // -- 16x16 matrix transpose -- //
    {
        src = mstr_v8si_create(16, 16);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_i32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(16, 16);

        mops_v8si_transpose(dst, src);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), mstr_get_i32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
    }

    // -- 17x17 matrix transpose -- //
    {

        src = mstr_v8si_create(17, 17);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_i32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(17, 17);

        mops_v8si_transpose(dst, src);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), mstr_get_i32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
    }

    // -- 2x9 matrix transpose -- //
    {
        src = mstr_v8si_create(2, 9);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_i32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(9, 2);

        mops_v8si_transpose(dst, src);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), mstr_get_i32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
    }

    // -- 9x2 matrix transpose -- //
    {
        src = mstr_v8si_create(9, 2);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 2; j += 1) {
                mstr_set_i32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(2, 9);

        mops_v8si_transpose(dst, src);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), mstr_get_i32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
    }

    // -- 17x33 matrix transpose -- //
    {
        src = mstr_v8si_create(17, 33);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_i32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(33, 17);

        mops_v8si_transpose(dst, src);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_i32(dst, i, j), mstr_get_i32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
    }
}
