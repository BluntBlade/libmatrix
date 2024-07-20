#include <criterion/criterion.h>

#ifndef C_SOURCE
#define C_SOURCE
#include "src/mx_v8sf.c"
#endif

// ==== TESTS FOR THE OPERATION MODULE OF MATRIX ==== //

inline static float value_at(mx_chunk_ptr chk, uint32_t row, uint32_t col)
{
    if (col < 8) {
        return mx_type_val(chk->v8sf_16x1[row][0])[col];
    } // if
    return mx_type_val(chk->v8sf_16x2[row][mx_round_to_multiples_of_8(col) / 8 - 1])[col & 0x7];
} // value_at

#define check_value(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %f, got %f.", expect, val)

#define check_value_at(val, expect, i, j) \
    cr_expect(val == expect, "Wrong value at (%d,%d) - Expect %f, got %f.", i, j, expect, val)

Test(InternalFunc, v8sf_add_chunk_fully)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} },  { .val = {  8.0,   9.0,  10.0,  11.0,  12.0,  13.0,  14.0,  15.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} },  { .val = { 24.0,  25.0,  26.0,  27.0,  28.0,  29.0,  30.0,  31.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} },  { .val = { 40.0,  41.0,  42.0,  43.0,  44.0,  45.0,  46.0,  47.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} },  { .val = { 56.0,  57.0,  58.0,  59.0,  60.0,  61.0,  62.0,  63.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} },  { .val = { 72.0,  73.0,  74.0,  75.0,  76.0,  77.0,  78.0,  79.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} },  { .val = { 88.0,  89.0,  90.0,  91.0,  92.0,  93.0,  94.0,  95.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} },  { .val = {104.0, 105.0, 106.0, 107.0, 108.0, 109.0, 110.0, 111.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} },  { .val = {120.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0} } },
                { { .val = {128.0, 129.0, 130.0, 131.0, 132.0, 133.0, 134.0, 135.0} },  { .val = {136.0, 137.0, 138.0, 139.0, 140.0, 141.0, 142.0, 143.0} } },
                { { .val = {144.0, 145.0, 146.0, 147.0, 148.0, 149.0, 150.0, 151.0} },  { .val = {152.0, 153.0, 154.0, 155.0, 156.0, 157.0, 158.0, 159.0} } },
                { { .val = {160.0, 161.0, 162.0, 163.0, 164.0, 165.0, 166.0, 167.0} },  { .val = {168.0, 169.0, 170.0, 171.0, 172.0, 173.0, 174.0, 175.0} } },
                { { .val = {176.0, 177.0, 178.0, 179.0, 180.0, 181.0, 182.0, 183.0} },  { .val = {184.0, 185.0, 186.0, 187.0, 188.0, 189.0, 190.0, 191.0} } },
                { { .val = {192.0, 193.0, 194.0, 195.0, 196.0, 197.0, 198.0, 199.0} },  { .val = {200.0, 201.0, 202.0, 203.0, 204.0, 205.0, 206.0, 207.0} } },
                { { .val = {208.0, 209.0, 210.0, 211.0, 212.0, 213.0, 214.0, 215.0} },  { .val = {216.0, 217.0, 218.0, 219.0, 220.0, 221.0, 222.0, 223.0} } },
                { { .val = {224.0, 225.0, 226.0, 227.0, 228.0, 229.0, 230.0, 231.0} },  { .val = {232.0, 233.0, 234.0, 235.0, 236.0, 237.0, 238.0, 239.0} } },
                { { .val = {240.0, 241.0, 242.0, 243.0, 244.0, 245.0, 246.0, 247.0} },  { .val = {248.0, 249.0, 250.0, 251.0, 252.0, 253.0, 254.0, 255.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_fully(&dst, &src, &src);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[0], mx_type_val(src.v8sf_16x2[i][j])[0] * 2, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[1], mx_type_val(src.v8sf_16x2[i][j])[1] * 2, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[2], mx_type_val(src.v8sf_16x2[i][j])[2] * 2, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[3], mx_type_val(src.v8sf_16x2[i][j])[3] * 2, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[4], mx_type_val(src.v8sf_16x2[i][j])[4] * 2, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[5], mx_type_val(src.v8sf_16x2[i][j])[5] * 2, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[6], mx_type_val(src.v8sf_16x2[i][j])[6] * 2, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[7], mx_type_val(src.v8sf_16x2[i][j])[7] * 2, i, j * 8 + 7);
            } // for
        } // for
    }
}

Test(InternalFunc, v8sf_add_chunk_partly)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 1x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 1, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
    }

    // -- 5x5 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = { 0.0,  1.0,  2.0,  3.0,  4.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 17.0, 18.0, 19.0, 20.0, 0.0, 0.0, 0.0} } },
                { { .val = {32.0, 33.0, 34.0, 35.0, 36.0, 0.0, 0.0, 0.0} } },
                { { .val = {48.0, 49.0, 50.0, 51.0, 52.0, 0.0, 0.0, 0.0} } },
                { { .val = {64.0, 65.0, 66.0, 67.0, 68.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 5, 5);
        for (i = 0; i < 5; i += 1) {
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[0], mx_type_val(src.v8sf_16x1[i][0])[0] * 2, i, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[1], mx_type_val(src.v8sf_16x1[i][0])[1] * 2, i, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[2], mx_type_val(src.v8sf_16x1[i][0])[2] * 2, i, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[3], mx_type_val(src.v8sf_16x1[i][0])[3] * 2, i, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[4], mx_type_val(src.v8sf_16x1[i][0])[4] * 2, i, 0 * 8 + 4);
        } // for
    }

    // -- 8x8 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 8, 8);
        for (i = 0; i < 8; i += 1) {
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[0], mx_type_val(src.v8sf_16x1[i][0])[0] * 2, i, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[1], mx_type_val(src.v8sf_16x1[i][0])[1] * 2, i, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[2], mx_type_val(src.v8sf_16x1[i][0])[2] * 2, i, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[3], mx_type_val(src.v8sf_16x1[i][0])[3] * 2, i, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[4], mx_type_val(src.v8sf_16x1[i][0])[4] * 2, i, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[5], mx_type_val(src.v8sf_16x1[i][0])[5] * 2, i, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[6], mx_type_val(src.v8sf_16x1[i][0])[6] * 2, i, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8sf_16x1[i][0])[7], mx_type_val(src.v8sf_16x1[i][0])[7] * 2, i, 0 * 8 + 7);
        } // for
    }

    // -- 9x9 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} }, { .val = {  8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} }, { .val = { 24.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} }, { .val = { 40.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} }, { .val = { 56.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} }, { .val = { 72.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} }, { .val = { 88.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} }, { .val = {104.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} }, { .val = {120.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {128.0, 129.0, 130.0, 131.0, 132.0, 133.0, 134.0, 135.0} }, { .val = {136.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 9, 9);
        for (i = 0; i < 9; i += 1) {
            check_value_at(mx_type_val(dst.v8sf_16x2[i][0])[0], mx_type_val(src.v8sf_16x2[i][0])[0] * 2, i, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8sf_16x2[i][0])[1], mx_type_val(src.v8sf_16x2[i][0])[1] * 2, i, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8sf_16x2[i][0])[2], mx_type_val(src.v8sf_16x2[i][0])[2] * 2, i, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8sf_16x2[i][0])[3], mx_type_val(src.v8sf_16x2[i][0])[3] * 2, i, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8sf_16x2[i][0])[4], mx_type_val(src.v8sf_16x2[i][0])[4] * 2, i, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8sf_16x2[i][0])[5], mx_type_val(src.v8sf_16x2[i][0])[5] * 2, i, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8sf_16x2[i][0])[6], mx_type_val(src.v8sf_16x2[i][0])[6] * 2, i, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8sf_16x2[i][0])[7], mx_type_val(src.v8sf_16x2[i][0])[7] * 2, i, 0 * 8 + 7);

            check_value_at(mx_type_val(dst.v8sf_16x2[i][1])[0], mx_type_val(src.v8sf_16x2[i][1])[0] * 2, i, 1 * 8 + 0);
        } // for
    }
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} }, { .val = {  8.0,   9.0,  10.0,  11.0,  12.0,  13.0,  14.0,  15.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} }, { .val = { 24.0,  25.0,  26.0,  27.0,  28.0,  29.0,  30.0,  31.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} }, { .val = { 40.0,  41.0,  42.0,  43.0,  44.0,  45.0,  46.0,  47.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} }, { .val = { 56.0,  57.0,  58.0,  59.0,  60.0,  61.0,  62.0,  63.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} }, { .val = { 72.0,  73.0,  74.0,  75.0,  76.0,  77.0,  78.0,  79.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} }, { .val = { 88.0,  89.0,  90.0,  91.0,  92.0,  93.0,  94.0,  95.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} }, { .val = {104.0, 105.0, 106.0, 107.0, 108.0, 109.0, 110.0, 111.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} }, { .val = {120.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0} } },
                { { .val = {128.0, 129.0, 130.0, 131.0, 132.0, 133.0, 134.0, 135.0} }, { .val = {136.0, 137.0, 138.0, 139.0, 140.0, 141.0, 142.0, 143.0} } },
                { { .val = {144.0, 145.0, 146.0, 147.0, 148.0, 149.0, 150.0, 151.0} }, { .val = {152.0, 153.0, 154.0, 155.0, 156.0, 157.0, 158.0, 159.0} } },
                { { .val = {160.0, 161.0, 162.0, 163.0, 164.0, 165.0, 166.0, 167.0} }, { .val = {168.0, 169.0, 170.0, 171.0, 172.0, 173.0, 174.0, 175.0} } },
                { { .val = {176.0, 177.0, 178.0, 179.0, 180.0, 181.0, 182.0, 183.0} }, { .val = {184.0, 185.0, 186.0, 187.0, 188.0, 189.0, 190.0, 191.0} } },
                { { .val = {192.0, 193.0, 194.0, 195.0, 196.0, 197.0, 198.0, 199.0} }, { .val = {200.0, 201.0, 202.0, 203.0, 204.0, 205.0, 206.0, 207.0} } },
                { { .val = {208.0, 209.0, 210.0, 211.0, 212.0, 213.0, 214.0, 215.0} }, { .val = {216.0, 217.0, 218.0, 219.0, 220.0, 221.0, 222.0, 223.0} } },
                { { .val = {224.0, 225.0, 226.0, 227.0, 228.0, 229.0, 230.0, 231.0} }, { .val = {232.0, 233.0, 234.0, 235.0, 236.0, 237.0, 238.0, 239.0} } },
                { { .val = {240.0, 241.0, 242.0, 243.0, 244.0, 245.0, 246.0, 247.0} }, { .val = {248.0, 249.0, 250.0, 251.0, 252.0, 253.0, 254.0, 255.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[0], mx_type_val(src.v8sf_16x2[i][j])[0] * 2, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[1], mx_type_val(src.v8sf_16x2[i][j])[1] * 2, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[2], mx_type_val(src.v8sf_16x2[i][j])[2] * 2, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[3], mx_type_val(src.v8sf_16x2[i][j])[3] * 2, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[4], mx_type_val(src.v8sf_16x2[i][j])[4] * 2, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[5], mx_type_val(src.v8sf_16x2[i][j])[5] * 2, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[6], mx_type_val(src.v8sf_16x2[i][j])[6] * 2, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[7], mx_type_val(src.v8sf_16x2[i][j])[7] * 2, i, j * 8 + 7);
            } // for
        } // for
    }

    // -- 1x5 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = { 0.0,  1.0,  2.0,  3.0,  4.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 1, 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], mx_type_val(src.v8sf_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], mx_type_val(src.v8sf_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], mx_type_val(src.v8sf_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], mx_type_val(src.v8sf_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
    }

    // -- 1x8 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 1, 8);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], mx_type_val(src.v8sf_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], mx_type_val(src.v8sf_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], mx_type_val(src.v8sf_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], mx_type_val(src.v8sf_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[5], mx_type_val(src.v8sf_16x1[0][0])[5] * 2, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[6], mx_type_val(src.v8sf_16x1[0][0])[6] * 2, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[7], mx_type_val(src.v8sf_16x1[0][0])[7] * 2, 0, 0 * 8 + 7);
    }

    // -- 1x9 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0} }, { .val = {8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 1, 9);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[0], mx_type_val(src.v8sf_16x2[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[1], mx_type_val(src.v8sf_16x2[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[2], mx_type_val(src.v8sf_16x2[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[3], mx_type_val(src.v8sf_16x2[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[4], mx_type_val(src.v8sf_16x2[0][0])[4] * 2, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[5], mx_type_val(src.v8sf_16x2[0][0])[5] * 2, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[6], mx_type_val(src.v8sf_16x2[0][0])[6] * 2, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[7], mx_type_val(src.v8sf_16x2[0][0])[7] * 2, 0, 0 * 8 + 7);

        check_value_at(mx_type_val(dst.v8sf_16x2[0][1])[0], mx_type_val(src.v8sf_16x2[0][1])[0] * 2, 0, 1 * 8 + 0);
    }

    // -- 5x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 5, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[1][0])[0], mx_type_val(src.v8sf_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[2][0])[0], mx_type_val(src.v8sf_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[3][0])[0], mx_type_val(src.v8sf_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[4][0])[0], mx_type_val(src.v8sf_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
    }

    // -- 8x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 80.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 96.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {112.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 8, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[1][0])[0], mx_type_val(src.v8sf_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[2][0])[0], mx_type_val(src.v8sf_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[3][0])[0], mx_type_val(src.v8sf_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[4][0])[0], mx_type_val(src.v8sf_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x1[5][0])[0], mx_type_val(src.v8sf_16x1[5][0])[0] * 2, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[6][0])[0], mx_type_val(src.v8sf_16x1[6][0])[0] * 2, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x1[7][0])[0], mx_type_val(src.v8sf_16x1[7][0])[0] * 2, 7, 0 * 8 + 7);
    }

    // -- 9x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 80.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 96.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {112.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {128.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_add_chunk_partly(&dst, &src, &src, 9, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[1][0])[0], mx_type_val(src.v8sf_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[2][0])[0], mx_type_val(src.v8sf_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[3][0])[0], mx_type_val(src.v8sf_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[4][0])[0], mx_type_val(src.v8sf_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x1[5][0])[0], mx_type_val(src.v8sf_16x1[5][0])[0] * 2, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[6][0])[0], mx_type_val(src.v8sf_16x1[6][0])[0] * 2, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x1[7][0])[0], mx_type_val(src.v8sf_16x1[7][0])[0] * 2, 7, 0 * 8 + 7);
        check_value_at(mx_type_val(dst.v8sf_16x1[8][1])[0], mx_type_val(src.v8sf_16x1[8][1])[0] * 2, 8, 1 * 8 + 0);
    }
}

Test(InternalFunc, v8sf_subtract_chunk_fully)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} }, { .val = {  8.0,   9.0,  10.0,  11.0,  12.0,  13.0,  14.0,  15.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} }, { .val = { 24.0,  25.0,  26.0,  27.0,  28.0,  29.0,  30.0,  31.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} }, { .val = { 40.0,  41.0,  42.0,  43.0,  44.0,  45.0,  46.0,  47.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} }, { .val = { 56.0,  57.0,  58.0,  59.0,  60.0,  61.0,  62.0,  63.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} }, { .val = { 72.0,  73.0,  74.0,  75.0,  76.0,  77.0,  78.0,  79.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} }, { .val = { 88.0,  89.0,  90.0,  91.0,  92.0,  93.0,  94.0,  95.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} }, { .val = {104.0, 105.0, 106.0, 107.0, 108.0, 109.0, 110.0, 111.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} }, { .val = {120.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0} } },
                { { .val = {128.0, 129.0, 130.0, 131.0, 132.0, 133.0, 134.0, 135.0} }, { .val = {136.0, 137.0, 138.0, 139.0, 140.0, 141.0, 142.0, 143.0} } },
                { { .val = {144.0, 145.0, 146.0, 147.0, 148.0, 149.0, 150.0, 151.0} }, { .val = {152.0, 153.0, 154.0, 155.0, 156.0, 157.0, 158.0, 159.0} } },
                { { .val = {160.0, 161.0, 162.0, 163.0, 164.0, 165.0, 166.0, 167.0} }, { .val = {168.0, 169.0, 170.0, 171.0, 172.0, 173.0, 174.0, 175.0} } },
                { { .val = {176.0, 177.0, 178.0, 179.0, 180.0, 181.0, 182.0, 183.0} }, { .val = {184.0, 185.0, 186.0, 187.0, 188.0, 189.0, 190.0, 191.0} } },
                { { .val = {192.0, 193.0, 194.0, 195.0, 196.0, 197.0, 198.0, 199.0} }, { .val = {200.0, 201.0, 202.0, 203.0, 204.0, 205.0, 206.0, 207.0} } },
                { { .val = {208.0, 209.0, 210.0, 211.0, 212.0, 213.0, 214.0, 215.0} }, { .val = {216.0, 217.0, 218.0, 219.0, 220.0, 221.0, 222.0, 223.0} } },
                { { .val = {224.0, 225.0, 226.0, 227.0, 228.0, 229.0, 230.0, 231.0} }, { .val = {232.0, 233.0, 234.0, 235.0, 236.0, 237.0, 238.0, 239.0} } },
                { { .val = {240.0, 241.0, 242.0, 243.0, 244.0, 245.0, 246.0, 247.0} }, { .val = {248.0, 249.0, 250.0, 251.0, 252.0, 253.0, 254.0, 255.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_fully(&dst, &src, &src);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[0], 0, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[1], 0, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[2], 0, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[3], 0, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[4], 0, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[5], 0, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[6], 0, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[7], 0, i, j * 8 + 7);
            } // for
        } // for
    }
}

Test(InternalFunc, v8sf_subtract_chunk_partly)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 1x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 1, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
    }

    // -- 5x5 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = { 0.0,  1.0,  2.0,  3.0,  4.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 17.0, 18.0, 19.0, 20.0, 0.0, 0.0, 0.0} } },
                { { .val = {32.0, 33.0, 34.0, 35.0, 36.0, 0.0, 0.0, 0.0} } },
                { { .val = {48.0, 49.0, 50.0, 51.0, 52.0, 0.0, 0.0, 0.0} } },
                { { .val = {64.0, 65.0, 66.0, 67.0, 68.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 5, 5);
        for (i = 0; i < 5; i += 1) {
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], 0, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], 0, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], 0, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], 0, 0, 0 * 8 + 4);
        } // for
    }

    // -- 8x8 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 8, 8);
        for (i = 0; i < 8; i += 1) {
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], 0, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], 0, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], 0, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], 0, 0, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[5], 0, 0, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[6], 0, 0, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[7], 0, 0, 0 * 8 + 7);
        } // for
    }

    // -- 9x9 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} }, { .val = {  8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} }, { .val = { 24.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} }, { .val = { 40.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} }, { .val = { 56.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} }, { .val = { 72.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} }, { .val = { 88.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} }, { .val = {104.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} }, { .val = {120.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {128.0, 129.0, 130.0, 131.0, 132.0, 133.0, 134.0, 135.0} }, { .val = {136.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 9, 9);
        for (i = 0; i < 9; i += 1) {
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[0], 0, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[1], 0, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[2], 0, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[3], 0, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[4], 0, 0, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[5], 0, 0, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[6], 0, 0, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[7], 0, 0, 0 * 8 + 7);

            check_value_at(mx_type_val(dst.v8sf_16x2[0][1])[0], 0, 0, 1 * 8 + 0);
        } // for
    }
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} }, { .val = {  8.0,   9.0,  10.0,  11.0,  12.0,  13.0,  14.0,  15.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} }, { .val = { 24.0,  25.0,  26.0,  27.0,  28.0,  29.0,  30.0,  31.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} }, { .val = { 40.0,  41.0,  42.0,  43.0,  44.0,  45.0,  46.0,  47.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} }, { .val = { 56.0,  57.0,  58.0,  59.0,  60.0,  61.0,  62.0,  63.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} }, { .val = { 72.0,  73.0,  74.0,  75.0,  76.0,  77.0,  78.0,  79.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} }, { .val = { 88.0,  89.0,  90.0,  91.0,  92.0,  93.0,  94.0,  95.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} }, { .val = {104.0, 105.0, 106.0, 107.0, 108.0, 109.0, 110.0, 111.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} }, { .val = {120.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0} } },
                { { .val = {128.0, 129.0, 130.0, 131.0, 132.0, 133.0, 134.0, 135.0} }, { .val = {136.0, 137.0, 138.0, 139.0, 140.0, 141.0, 142.0, 143.0} } },
                { { .val = {144.0, 145.0, 146.0, 147.0, 148.0, 149.0, 150.0, 151.0} }, { .val = {152.0, 153.0, 154.0, 155.0, 156.0, 157.0, 158.0, 159.0} } },
                { { .val = {160.0, 161.0, 162.0, 163.0, 164.0, 165.0, 166.0, 167.0} }, { .val = {168.0, 169.0, 170.0, 171.0, 172.0, 173.0, 174.0, 175.0} } },
                { { .val = {176.0, 177.0, 178.0, 179.0, 180.0, 181.0, 182.0, 183.0} }, { .val = {184.0, 185.0, 186.0, 187.0, 188.0, 189.0, 190.0, 191.0} } },
                { { .val = {192.0, 193.0, 194.0, 195.0, 196.0, 197.0, 198.0, 199.0} }, { .val = {200.0, 201.0, 202.0, 203.0, 204.0, 205.0, 206.0, 207.0} } },
                { { .val = {208.0, 209.0, 210.0, 211.0, 212.0, 213.0, 214.0, 215.0} }, { .val = {216.0, 217.0, 218.0, 219.0, 220.0, 221.0, 222.0, 223.0} } },
                { { .val = {224.0, 225.0, 226.0, 227.0, 228.0, 229.0, 230.0, 231.0} }, { .val = {232.0, 233.0, 234.0, 235.0, 236.0, 237.0, 238.0, 239.0} } },
                { { .val = {240.0, 241.0, 242.0, 243.0, 244.0, 245.0, 246.0, 247.0} }, { .val = {248.0, 249.0, 250.0, 251.0, 252.0, 253.0, 254.0, 255.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[0], 0, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[1], 0, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[2], 0, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[3], 0, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[4], 0, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[5], 0, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[6], 0, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[7], 0, i, j * 8 + 7);
            } // for
        } // for
    }

    // -- 1x5 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = { 0.0,  1.0,  2.0,  3.0,  4.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 1, 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], 0, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], 0, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], 0, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], 0, 0, 0 * 8 + 4);
    }

    // -- 1x8 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 1, 8);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], 0, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], 0, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], 0, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], 0, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[5], 0, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[6], 0, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[7], 0, 0, 0 * 8 + 7);
    }

    // -- 1x9 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0} }, { .val = {8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 1, 9);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[1], 0, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[2], 0, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[3], 0, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[4], 0, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[5], 0, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[6], 0, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[7], 0, 0, 0 * 8 + 7);

        check_value_at(mx_type_val(dst.v8sf_16x2[0][1])[0], 0, 0, 1 * 8 + 0);
    }

    // -- 5x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 5, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[1][0])[0], 0, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[2][0])[0], 0, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[3][0])[0], 0, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[4][0])[0], 0, 4, 0 * 8 + 4);
    }

    // -- 8x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 80.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 96.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {112.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 8, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[1][0])[0], 0, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[2][0])[0], 0, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[3][0])[0], 0, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[4][0])[0], 0, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x1[5][0])[0], 0, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[6][0])[0], 0, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x1[7][0])[0], 0, 7, 0 * 8 + 7);
    }

    // -- 9x1 chunk -- //
    {
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 80.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 96.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {112.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {128.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_subtract_chunk_partly(&dst, &src, &src, 9, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], 0, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[1][0])[0], 0, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[2][0])[0], 0, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[3][0])[0], 0, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[4][0])[0], 0, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x1[5][0])[0], 0, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[6][0])[0], 0, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x1[7][0])[0], 0, 7, 0 * 8 + 7);
        check_value_at(mx_type_val(dst.v8sf_16x1[8][1])[0], 0, 8, 1 * 8 + 0);
    }
}

Test(InternalFunc, v8sf_multiply_chunk_fully)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t schk = {
            .v8sf_16x2 = {
                { { .val = {2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} }, { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = {22.0, 18.0, 18.0, 18.0, 18.0, 18.0, 18.0, 18.0} }, { .val = {18.0, 18.0, 18.0, 18.0, 18.0, 18.0, 18.0, 22.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {18.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 18.0} } },
                { { .val = {22.0, 18.0, 18.0, 18.0, 18.0, 18.0, 18.0, 18.0} }, { .val = {18.0, 18.0, 18.0, 18.0, 18.0, 18.0, 18.0, 22.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_fully(&dchk, &schk, &schk, 16, 16, 16);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8sf_multiply_chunk_half_to_half)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 1x1 chunk multiplication -- half to half -- //
    {
        mx_chunk_t schk = {
            .v8sf_16x1 = {
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = {4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_half(&dchk, &schk, &schk, 1, 1, 1);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 5x5 chunk multiplication -- half to half -- //
    {
        mx_chunk_t schk = {
            .v8sf_16x1 = {
                { { .val = {2.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = {8.0, 6.0, 6.0, 6.0, 6.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 5.0, 5.0, 5.0, 5.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 5.0, 5.0, 5.0, 5.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 5.0, 5.0, 5.0, 5.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 5.0, 5.0, 5.0, 5.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_half(&dchk, &schk, &schk, 5, 5, 5);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t schk = {
            .v8sf_16x1 = {
                { { .val = {2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
                { { .val = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = {11.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0} } },
                { { .val = { 9.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0} } },
                { { .val = { 9.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0} } },
                { { .val = { 9.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0} } },
                { { .val = { 9.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0} } },
                { { .val = { 9.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0} } },
                { { .val = { 9.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0} } },
                { { .val = { 9.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_half(&dchk, &schk, &schk, 8, 8, 8);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 1x1 and 1x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 1, 8, 1);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x1 and 1x1 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 8, 1, 1);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x1 and 1x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = { 2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0} } },
                { { .val = { 4.0,  6.0,  8.0, 10.0, 12.0, 14.0, 16.0, 18.0} } },
                { { .val = { 6.0,  9.0, 12.0, 15.0, 18.0, 21.0, 24.0, 27.0} } },
                { { .val = { 8.0, 12.0, 16.0, 20.0, 24.0, 28.0, 32.0, 36.0} } },
                { { .val = {10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0} } },
                { { .val = {12.0, 18.0, 24.0, 30.0, 36.0, 42.0, 48.0, 54.0} } },
                { { .val = {14.0, 21.0, 28.0, 35.0, 42.0, 49.0, 56.0, 63.0} } },
                { { .val = {16.0, 24.0, 32.0, 40.0, 48.0, 56.0, 64.0, 72.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 8, 8, 1);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x1 and 1x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = { 2.0,  3.0,  4.0,  5.0,  6.0,   7.0,   8.0,   9.0} } },
                { { .val = { 4.0,  6.0,  8.0, 10.0, 12.0,  14.0,  16.0,  18.0} } },
                { { .val = { 6.0,  9.0, 12.0, 15.0, 18.0,  21.0,  24.0,  27.0} } },
                { { .val = { 8.0, 12.0, 16.0, 20.0, 24.0,  28.0,  32.0,  36.0} } },
                { { .val = {10.0, 15.0, 20.0, 25.0, 30.0,  35.0,  40.0,  45.0} } },
                { { .val = {12.0, 18.0, 24.0, 30.0, 36.0,  42.0,  48.0,  54.0} } },
                { { .val = {14.0, 21.0, 28.0, 35.0, 42.0,  49.0,  56.0,  63.0} } },
                { { .val = {16.0, 24.0, 32.0, 40.0, 48.0,  56.0,  64.0,  72.0} } },
                { { .val = {18.0, 27.0, 36.0, 45.0, 54.0,  63.0,  72.0,  81.0} } },
                { { .val = {20.0, 30.0, 40.0, 50.0, 60.0,  70.0,  80.0,  90.0} } },
                { { .val = {22.0, 33.0, 44.0, 55.0, 66.0,  77.0,  88.0,  99.0} } },
                { { .val = {24.0, 36.0, 48.0, 60.0, 72.0,  84.0,  96.0, 108.0} } },
                { { .val = {26.0, 39.0, 52.0, 65.0, 78.0,  91.0, 104.0, 117.0} } },
                { { .val = {28.0, 42.0, 56.0, 70.0, 84.0,  98.0, 112.0, 126.0} } },
                { { .val = {30.0, 45.0, 60.0, 75.0, 90.0, 105.0, 120.0, 135.0} } },
                { { .val = {32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 16, 8, 1);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x2 and 2x8 chunk multiplication -- half to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = { 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = {2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {3.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {4.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {5.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {7.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {8.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {9.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = { 2.0 + 1.0,  3.0 + 1.0,  4.0 + 1.0,  5.0 + 1.0,  6.0 + 1.0,   7.0 + 1.0,   8.0 + 1.0,   9.0 + 1.0} } },
                { { .val = { 4.0 + 1.0,  6.0 + 1.0,  8.0 + 1.0, 10.0 + 1.0, 12.0 + 1.0,  14.0 + 1.0,  16.0 + 1.0,  18.0 + 1.0} } },
                { { .val = { 6.0 + 1.0,  9.0 + 1.0, 12.0 + 1.0, 15.0 + 1.0, 18.0 + 1.0,  21.0 + 1.0,  24.0 + 1.0,  27.0 + 1.0} } },
                { { .val = { 8.0 + 1.0, 12.0 + 1.0, 16.0 + 1.0, 20.0 + 1.0, 24.0 + 1.0,  28.0 + 1.0,  32.0 + 1.0,  36.0 + 1.0} } },
                { { .val = {10.0 + 1.0, 15.0 + 1.0, 20.0 + 1.0, 25.0 + 1.0, 30.0 + 1.0,  35.0 + 1.0,  40.0 + 1.0,  45.0 + 1.0} } },
                { { .val = {12.0 + 1.0, 18.0 + 1.0, 24.0 + 1.0, 30.0 + 1.0, 36.0 + 1.0,  42.0 + 1.0,  48.0 + 1.0,  54.0 + 1.0} } },
                { { .val = {14.0 + 1.0, 21.0 + 1.0, 28.0 + 1.0, 35.0 + 1.0, 42.0 + 1.0,  49.0 + 1.0,  56.0 + 1.0,  63.0 + 1.0} } },
                { { .val = {16.0 + 1.0, 24.0 + 1.0, 32.0 + 1.0, 40.0 + 1.0, 48.0 + 1.0,  56.0 + 1.0,  64.0 + 1.0,  72.0 + 1.0} } },
                { { .val = {18.0 + 1.0, 27.0 + 1.0, 36.0 + 1.0, 45.0 + 1.0, 54.0 + 1.0,  63.0 + 1.0,  72.0 + 1.0,  81.0 + 1.0} } },
                { { .val = {20.0 + 1.0, 30.0 + 1.0, 40.0 + 1.0, 50.0 + 1.0, 60.0 + 1.0,  70.0 + 1.0,  80.0 + 1.0,  90.0 + 1.0} } },
                { { .val = {22.0 + 1.0, 33.0 + 1.0, 44.0 + 1.0, 55.0 + 1.0, 66.0 + 1.0,  77.0 + 1.0,  88.0 + 1.0,  99.0 + 1.0} } },
                { { .val = {24.0 + 1.0, 36.0 + 1.0, 48.0 + 1.0, 60.0 + 1.0, 72.0 + 1.0,  84.0 + 1.0,  96.0 + 1.0, 108.0 + 1.0} } },
                { { .val = {26.0 + 1.0, 39.0 + 1.0, 52.0 + 1.0, 65.0 + 1.0, 78.0 + 1.0,  91.0 + 1.0, 104.0 + 1.0, 117.0 + 1.0} } },
                { { .val = {28.0 + 1.0, 42.0 + 1.0, 56.0 + 1.0, 70.0 + 1.0, 84.0 + 1.0,  98.0 + 1.0, 112.0 + 1.0, 126.0 + 1.0} } },
                { { .val = {30.0 + 1.0, 45.0 + 1.0, 60.0 + 1.0, 75.0 + 1.0, 90.0 + 1.0, 105.0 + 1.0, 120.0 + 1.0, 135.0 + 1.0} } },
                { { .val = {32.0 + 1.0, 48.0 + 1.0, 64.0 + 1.0, 80.0 + 1.0, 96.0 + 1.0, 112.0 + 1.0, 128.0 + 1.0, 144.0 + 1.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_half(&dchk, &lchk, &rchk, 16, 8, 2);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8sf_multiply_chunk_half_to_full)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 1x1 and 1x9 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} }, { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 1, 9, 1);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x1 and 1x9 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 8, 9, 1);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x1 and 1x9 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = { 2.0,  3.0,  4.0,  5.0,  6.0,   7.0,   8.0,   9.0} }, { .val = { 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0,  6.0,  8.0, 10.0, 12.0,  14.0,  16.0,  18.0} }, { .val = { 20.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0,  9.0, 12.0, 15.0, 18.0,  21.0,  24.0,  27.0} }, { .val = { 30.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 12.0, 16.0, 20.0, 24.0,  28.0,  32.0,  36.0} }, { .val = { 40.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 15.0, 20.0, 25.0, 30.0,  35.0,  40.0,  45.0} }, { .val = { 50.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 18.0, 24.0, 30.0, 36.0,  42.0,  48.0,  54.0} }, { .val = { 60.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 21.0, 28.0, 35.0, 42.0,  49.0,  56.0,  63.0} }, { .val = { 70.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 24.0, 32.0, 40.0, 48.0,  56.0,  64.0,  72.0} }, { .val = { 80.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {18.0, 27.0, 36.0, 45.0, 54.0,  63.0,  72.0,  81.0} }, { .val = { 90.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {20.0, 30.0, 40.0, 50.0, 60.0,  70.0,  80.0,  90.0} }, { .val = {100.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {22.0, 33.0, 44.0, 55.0, 66.0,  77.0,  88.0,  99.0} }, { .val = {110.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {24.0, 36.0, 48.0, 60.0, 72.0,  84.0,  96.0, 108.0} }, { .val = {120.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {26.0, 39.0, 52.0, 65.0, 78.0,  91.0, 104.0, 117.0} }, { .val = {130.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {28.0, 42.0, 56.0, 70.0, 84.0,  98.0, 112.0, 126.0} }, { .val = {140.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {30.0, 45.0, 60.0, 75.0, 90.0, 105.0, 120.0, 135.0} }, { .val = {150.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0} }, { .val = {160.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 16, 9, 1);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x1 and 1x16 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {17.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = { 2.0,  3.0,  4.0,  5.0,  6.0,   7.0,   8.0,   9.0} }, { .val = { 10.0,  11.0,  12.0,  13.0,  14.0,  15.0,  16.0,  17.0} } },
                { { .val = { 4.0,  6.0,  8.0, 10.0, 12.0,  14.0,  16.0,  18.0} }, { .val = { 20.0,  22.0,  24.0,  26.0,  28.0,  30.0,  32.0,  34.0} } },
                { { .val = { 6.0,  9.0, 12.0, 15.0, 18.0,  21.0,  24.0,  27.0} }, { .val = { 30.0,  33.0,  36.0,  39.0,  42.0,  45.0,  48.0,  51.0} } },
                { { .val = { 8.0, 12.0, 16.0, 20.0, 24.0,  28.0,  32.0,  36.0} }, { .val = { 40.0,  44.0,  48.0,  52.0,  56.0,  60.0,  64.0,  68.0} } },
                { { .val = {10.0, 15.0, 20.0, 25.0, 30.0,  35.0,  40.0,  45.0} }, { .val = { 50.0,  55.0,  60.0,  65.0,  70.0,  75.0,  80.0,  85.0} } },
                { { .val = {12.0, 18.0, 24.0, 30.0, 36.0,  42.0,  48.0,  54.0} }, { .val = { 60.0,  66.0,  72.0,  78.0,  84.0,  90.0,  96.0, 102.0} } },
                { { .val = {14.0, 21.0, 28.0, 35.0, 42.0,  49.0,  56.0,  63.0} }, { .val = { 70.0,  77.0,  84.0,  91.0,  98.0, 105.0, 112.0, 119.0} } },
                { { .val = {16.0, 24.0, 32.0, 40.0, 48.0,  56.0,  64.0,  72.0} }, { .val = { 80.0,  88.0,  96.0, 104.0, 112.0, 120.0, 128.0, 136.0} } },
                { { .val = {18.0, 27.0, 36.0, 45.0, 54.0,  63.0,  72.0,  81.0} }, { .val = { 90.0,  99.0, 108.0, 117.0, 126.0, 135.0, 144.0, 153.0} } },
                { { .val = {20.0, 30.0, 40.0, 50.0, 60.0,  70.0,  80.0,  90.0} }, { .val = {100.0, 110.0, 120.0, 130.0, 140.0, 150.0, 160.0, 170.0} } },
                { { .val = {22.0, 33.0, 44.0, 55.0, 66.0,  77.0,  88.0,  99.0} }, { .val = {110.0, 121.0, 132.0, 143.0, 154.0, 165.0, 176.0, 187.0} } },
                { { .val = {24.0, 36.0, 48.0, 60.0, 72.0,  84.0,  96.0, 108.0} }, { .val = {120.0, 132.0, 144.0, 156.0, 168.0, 180.0, 192.0, 204.0} } },
                { { .val = {26.0, 39.0, 52.0, 65.0, 78.0,  91.0, 104.0, 117.0} }, { .val = {130.0, 143.0, 156.0, 169.0, 182.0, 195.0, 208.0, 221.0} } },
                { { .val = {28.0, 42.0, 56.0, 70.0, 84.0,  98.0, 112.0, 126.0} }, { .val = {140.0, 154.0, 168.0, 182.0, 196.0, 210.0, 224.0, 238.0} } },
                { { .val = {30.0, 45.0, 60.0, 75.0, 90.0, 105.0, 120.0, 135.0} }, { .val = {150.0, 165.0, 180.0, 195.0, 210.0, 225.0, 240.0, 255.0} } },
                { { .val = {32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0} }, { .val = {160.0, 176.0, 192.0, 208.0, 224.0, 240.0, 256.0, 272.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 16, 16, 1);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x2 and 2x16 chunk multiplication -- half to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x1 = {
                { { .val = { 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x1 = {
                { { .val = { 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {17.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = { 2.0 + 1.0,  3.0 + 1.0,  4.0 + 1.0,  5.0 + 1.0,  6.0 + 1.0,   7.0 + 1.0,   8.0 + 1.0,   9.0 + 1.0} }, { .val = { 10.0 + 1.0,  11.0 + 1.0,  12.0 + 1.0,  13.0 + 1.0,  14.0 + 1.0,  15.0 + 1.0,  16.0 + 1.0,  17.0 + 1.0} } },
                { { .val = { 4.0 + 1.0,  6.0 + 1.0,  8.0 + 1.0, 10.0 + 1.0, 12.0 + 1.0,  14.0 + 1.0,  16.0 + 1.0,  18.0 + 1.0} }, { .val = { 20.0 + 1.0,  22.0 + 1.0,  24.0 + 1.0,  26.0 + 1.0,  28.0 + 1.0,  30.0 + 1.0,  32.0 + 1.0,  34.0 + 1.0} } },
                { { .val = { 6.0 + 1.0,  9.0 + 1.0, 12.0 + 1.0, 15.0 + 1.0, 18.0 + 1.0,  21.0 + 1.0,  24.0 + 1.0,  27.0 + 1.0} }, { .val = { 30.0 + 1.0,  33.0 + 1.0,  36.0 + 1.0,  39.0 + 1.0,  42.0 + 1.0,  45.0 + 1.0,  48.0 + 1.0,  51.0 + 1.0} } },
                { { .val = { 8.0 + 1.0, 12.0 + 1.0, 16.0 + 1.0, 20.0 + 1.0, 24.0 + 1.0,  28.0 + 1.0,  32.0 + 1.0,  36.0 + 1.0} }, { .val = { 40.0 + 1.0,  44.0 + 1.0,  48.0 + 1.0,  52.0 + 1.0,  56.0 + 1.0,  60.0 + 1.0,  64.0 + 1.0,  68.0 + 1.0} } },
                { { .val = {10.0 + 1.0, 15.0 + 1.0, 20.0 + 1.0, 25.0 + 1.0, 30.0 + 1.0,  35.0 + 1.0,  40.0 + 1.0,  45.0 + 1.0} }, { .val = { 50.0 + 1.0,  55.0 + 1.0,  60.0 + 1.0,  65.0 + 1.0,  70.0 + 1.0,  75.0 + 1.0,  80.0 + 1.0,  85.0 + 1.0} } },
                { { .val = {12.0 + 1.0, 18.0 + 1.0, 24.0 + 1.0, 30.0 + 1.0, 36.0 + 1.0,  42.0 + 1.0,  48.0 + 1.0,  54.0 + 1.0} }, { .val = { 60.0 + 1.0,  66.0 + 1.0,  72.0 + 1.0,  78.0 + 1.0,  84.0 + 1.0,  90.0 + 1.0,  96.0 + 1.0, 102.0 + 1.0} } },
                { { .val = {14.0 + 1.0, 21.0 + 1.0, 28.0 + 1.0, 35.0 + 1.0, 42.0 + 1.0,  49.0 + 1.0,  56.0 + 1.0,  63.0 + 1.0} }, { .val = { 70.0 + 1.0,  77.0 + 1.0,  84.0 + 1.0,  91.0 + 1.0,  98.0 + 1.0, 105.0 + 1.0, 112.0 + 1.0, 119.0 + 1.0} } },
                { { .val = {16.0 + 1.0, 24.0 + 1.0, 32.0 + 1.0, 40.0 + 1.0, 48.0 + 1.0,  56.0 + 1.0,  64.0 + 1.0,  72.0 + 1.0} }, { .val = { 80.0 + 1.0,  88.0 + 1.0,  96.0 + 1.0, 104.0 + 1.0, 112.0 + 1.0, 120.0 + 1.0, 128.0 + 1.0, 136.0 + 1.0} } },
                { { .val = {18.0 + 1.0, 27.0 + 1.0, 36.0 + 1.0, 45.0 + 1.0, 54.0 + 1.0,  63.0 + 1.0,  72.0 + 1.0,  81.0 + 1.0} }, { .val = { 90.0 + 1.0,  99.0 + 1.0, 108.0 + 1.0, 117.0 + 1.0, 126.0 + 1.0, 135.0 + 1.0, 144.0 + 1.0, 153.0 + 1.0} } },
                { { .val = {20.0 + 1.0, 30.0 + 1.0, 40.0 + 1.0, 50.0 + 1.0, 60.0 + 1.0,  70.0 + 1.0,  80.0 + 1.0,  90.0 + 1.0} }, { .val = {100.0 + 1.0, 110.0 + 1.0, 120.0 + 1.0, 130.0 + 1.0, 140.0 + 1.0, 150.0 + 1.0, 160.0 + 1.0, 170.0 + 1.0} } },
                { { .val = {22.0 + 1.0, 33.0 + 1.0, 44.0 + 1.0, 55.0 + 1.0, 66.0 + 1.0,  77.0 + 1.0,  88.0 + 1.0,  99.0 + 1.0} }, { .val = {110.0 + 1.0, 121.0 + 1.0, 132.0 + 1.0, 143.0 + 1.0, 154.0 + 1.0, 165.0 + 1.0, 176.0 + 1.0, 187.0 + 1.0} } },
                { { .val = {24.0 + 1.0, 36.0 + 1.0, 48.0 + 1.0, 60.0 + 1.0, 72.0 + 1.0,  84.0 + 1.0,  96.0 + 1.0, 108.0 + 1.0} }, { .val = {120.0 + 1.0, 132.0 + 1.0, 144.0 + 1.0, 156.0 + 1.0, 168.0 + 1.0, 180.0 + 1.0, 192.0 + 1.0, 204.0 + 1.0} } },
                { { .val = {26.0 + 1.0, 39.0 + 1.0, 52.0 + 1.0, 65.0 + 1.0, 78.0 + 1.0,  91.0 + 1.0, 104.0 + 1.0, 117.0 + 1.0} }, { .val = {130.0 + 1.0, 143.0 + 1.0, 156.0 + 1.0, 169.0 + 1.0, 182.0 + 1.0, 195.0 + 1.0, 208.0 + 1.0, 221.0 + 1.0} } },
                { { .val = {28.0 + 1.0, 42.0 + 1.0, 56.0 + 1.0, 70.0 + 1.0, 84.0 + 1.0,  98.0 + 1.0, 112.0 + 1.0, 126.0 + 1.0} }, { .val = {140.0 + 1.0, 154.0 + 1.0, 168.0 + 1.0, 182.0 + 1.0, 196.0 + 1.0, 210.0 + 1.0, 224.0 + 1.0, 238.0 + 1.0} } },
                { { .val = {30.0 + 1.0, 45.0 + 1.0, 60.0 + 1.0, 75.0 + 1.0, 90.0 + 1.0, 105.0 + 1.0, 120.0 + 1.0, 135.0 + 1.0} }, { .val = {150.0 + 1.0, 165.0 + 1.0, 180.0 + 1.0, 195.0 + 1.0, 210.0 + 1.0, 225.0 + 1.0, 240.0 + 1.0, 255.0 + 1.0} } },
                { { .val = {32.0 + 1.0, 48.0 + 1.0, 64.0 + 1.0, 80.0 + 1.0, 96.0 + 1.0, 112.0 + 1.0, 128.0 + 1.0, 144.0 + 1.0} }, { .val = {160.0 + 1.0, 176.0 + 1.0, 192.0 + 1.0, 208.0 + 1.0, 224.0 + 1.0, 240.0 + 1.0, 256.0 + 1.0, 272.0 + 1.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_half_to_full(&dchk, &lchk, &rchk, 16, 16, 2);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8sf_multiply_chunk_full_to_half)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 1x9 and 9x1 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} }, { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = {90.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 1, 1, 9);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 1x9 and 9x8 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} }, { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = {90.0, 90.0, 90.0, 90.0, 90.0, 90.0, 90.0, 90.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 1, 8, 9);

        for (i = 0; i < 1; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x9 and 9x1 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} }, { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0} }, { .val = {3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0} }, { .val = {4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0} }, { .val = {5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0} }, { .val = {6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {7.0, 7.0, 7.0, 7.0, 7.0, 7.0, 7.0, 7.0} }, { .val = {7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0} }, { .val = {8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = { 90.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {135.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {180.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {225.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {270.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {315.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {360.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {405.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 8, 1, 9);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 8x9 and 9x8 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} }, { .val = {2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0} }, { .val = {3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0} }, { .val = {4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0} }, { .val = {5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0} }, { .val = {6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {7.0, 7.0, 7.0, 7.0, 7.0, 7.0, 7.0, 7.0} }, { .val = {7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0, 8.0} }, { .val = {8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = { 90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0} } },
                { { .val = {135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0} } },
                { { .val = {180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0} } },
                { { .val = {225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0} } },
                { { .val = {270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0} } },
                { { .val = {315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0} } },
                { { .val = {360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0} } },
                { { .val = {405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 8, 8, 9);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x9 and 9x1 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0} }, { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0} }, { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0} }, { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0} }, { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0} }, { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0} }, { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0} }, { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0} }, { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0} }, { .val = {11.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0} }, { .val = {12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0} }, { .val = {13.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0} }, { .val = {14.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0} }, { .val = {15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0} }, { .val = {17.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = { 90.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {135.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {180.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {225.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {270.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {315.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {360.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {405.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {450.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {495.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {540.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {585.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {630.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {675.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {720.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {765.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 16, 1, 9);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 1; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x9 and 9x8 chunk multiplication -- full to half -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0} }, { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0} }, { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0} }, { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0} }, { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0} }, { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0} }, { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0} }, { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0} }, { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0} }, { .val = {11.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0} }, { .val = {12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0} }, { .val = {13.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0} }, { .val = {14.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0} }, { .val = {15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0} }, { .val = {17.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x1 = {
                { { .val = { 90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0} } },
                { { .val = {135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0} } },
                { { .val = {180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0} } },
                { { .val = {225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0} } },
                { { .val = {270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0} } },
                { { .val = {315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0} } },
                { { .val = {360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0} } },
                { { .val = {405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0} } },
                { { .val = {450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0} } },
                { { .val = {495.0, 495.0, 495.0, 495.0, 495.0, 495.0, 495.0, 495.0} } },
                { { .val = {540.0, 540.0, 540.0, 540.0, 540.0, 540.0, 540.0, 540.0} } },
                { { .val = {585.0, 585.0, 585.0, 585.0, 585.0, 585.0, 585.0, 585.0} } },
                { { .val = {630.0, 630.0, 630.0, 630.0, 630.0, 630.0, 630.0, 630.0} } },
                { { .val = {675.0, 675.0, 675.0, 675.0, 675.0, 675.0, 675.0, 675.0} } },
                { { .val = {720.0, 720.0, 720.0, 720.0, 720.0, 720.0, 720.0, 720.0} } },
                { { .val = {765.0, 765.0, 765.0, 765.0, 765.0, 765.0, 765.0, 765.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_half(&dchk, &lchk, &rchk, 16, 8, 9);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8sf_multiply_chunk_full_to_full)
{
    mx_chunk_t dchk;
    uint32_t i = 0;
    uint32_t j = 0;

    // -- 9x9 and 9x9 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0} }, { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0} }, { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0} }, { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0} }, { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0} }, { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0} }, { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0} }, { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0} }, { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = { 90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0} }, { .val = { 90.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0} }, { .val = {135.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0} }, { .val = {180.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0} }, { .val = {225.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0} }, { .val = {270.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0} }, { .val = {315.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0} }, { .val = {360.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0} }, { .val = {405.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0} }, { .val = {450.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 9, 9, 9);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 9x9 and 9x16 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0} }, { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0} }, { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0} }, { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0} }, { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0} }, { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0} }, { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0} }, { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0} }, { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = { 90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0} }, { .val = { 90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0} } },
                { { .val = {135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0} }, { .val = {135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0} } },
                { { .val = {180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0} }, { .val = {180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0} } },
                { { .val = {225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0} }, { .val = {225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0} } },
                { { .val = {270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0} }, { .val = {270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0} } },
                { { .val = {315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0} }, { .val = {315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0} } },
                { { .val = {360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0} }, { .val = {360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0} } },
                { { .val = {405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0} }, { .val = {405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0} } },
                { { .val = {450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0} }, { .val = {450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 9, 16, 9);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x9 and 9x9 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0} }, { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0} }, { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0} }, { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0} }, { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0} }, { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0} }, { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0} }, { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0} }, { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0} }, { .val = {11.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0} }, { .val = {12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0} }, { .val = {13.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0} }, { .val = {14.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0} }, { .val = {15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0} }, { .val = {17.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = { 90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0} }, { .val = { 90.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0} }, { .val = {135.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0} }, { .val = {180.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0} }, { .val = {225.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0} }, { .val = {270.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0} }, { .val = {315.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0} }, { .val = {360.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0} }, { .val = {405.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0} }, { .val = {450.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {495.0, 495.0, 495.0, 495.0, 495.0, 495.0, 495.0, 495.0} }, { .val = {495.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {540.0, 540.0, 540.0, 540.0, 540.0, 540.0, 540.0, 540.0} }, { .val = {540.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {585.0, 585.0, 585.0, 585.0, 585.0, 585.0, 585.0, 585.0} }, { .val = {585.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {630.0, 630.0, 630.0, 630.0, 630.0, 630.0, 630.0, 630.0} }, { .val = {630.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {675.0, 675.0, 675.0, 675.0, 675.0, 675.0, 675.0, 675.0} }, { .val = {675.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {720.0, 720.0, 720.0, 720.0, 720.0, 720.0, 720.0, 720.0} }, { .val = {720.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {765.0, 765.0, 765.0, 765.0, 765.0, 765.0, 765.0, 765.0} }, { .val = {765.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 16, 9, 9);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x9 and 9x16 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0} }, { .val = { 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0} }, { .val = { 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0} }, { .val = { 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0} }, { .val = { 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0} }, { .val = { 6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0} }, { .val = { 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0} }, { .val = { 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0} }, { .val = { 9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0} }, { .val = {10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0} }, { .val = {11.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0} }, { .val = {12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0} }, { .val = {13.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0} }, { .val = {14.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0} }, { .val = {15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0} }, { .val = {17.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = { 90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0} }, { .val = { 90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0} } },
                { { .val = {135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0} }, { .val = {135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0, 135.0} } },
                { { .val = {180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0} }, { .val = {180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0, 180.0} } },
                { { .val = {225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0} }, { .val = {225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0} } },
                { { .val = {270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0} }, { .val = {270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0, 270.0} } },
                { { .val = {315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0} }, { .val = {315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0, 315.0} } },
                { { .val = {360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0} }, { .val = {360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0} } },
                { { .val = {405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0} }, { .val = {405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0, 405.0} } },
                { { .val = {450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0} }, { .val = {450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0} } },
                { { .val = {495.0, 495.0, 495.0, 495.0, 495.0, 495.0, 495.0, 495.0} }, { .val = {495.0, 495.0, 495.0, 495.0, 495.0, 495.0, 495.0, 495.0} } },
                { { .val = {540.0, 540.0, 540.0, 540.0, 540.0, 540.0, 540.0, 540.0} }, { .val = {540.0, 540.0, 540.0, 540.0, 540.0, 540.0, 540.0, 540.0} } },
                { { .val = {585.0, 585.0, 585.0, 585.0, 585.0, 585.0, 585.0, 585.0} }, { .val = {585.0, 585.0, 585.0, 585.0, 585.0, 585.0, 585.0, 585.0} } },
                { { .val = {630.0, 630.0, 630.0, 630.0, 630.0, 630.0, 630.0, 630.0} }, { .val = {630.0, 630.0, 630.0, 630.0, 630.0, 630.0, 630.0, 630.0} } },
                { { .val = {675.0, 675.0, 675.0, 675.0, 675.0, 675.0, 675.0, 675.0} }, { .val = {675.0, 675.0, 675.0, 675.0, 675.0, 675.0, 675.0, 675.0} } },
                { { .val = {720.0, 720.0, 720.0, 720.0, 720.0, 720.0, 720.0, 720.0} }, { .val = {720.0, 720.0, 720.0, 720.0, 720.0, 720.0, 720.0, 720.0} } },
                { { .val = {765.0, 765.0, 765.0, 765.0, 765.0, 765.0, 765.0, 765.0} }, { .val = {765.0, 765.0, 765.0, 765.0, 765.0, 765.0, 765.0, 765.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 16, 16, 9);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }

    // -- 16x16 and 16x16 chunk multiplication -- full to full -- //
    {
        mx_chunk_t lchk = {
            .v8sf_16x2 = {
                { { .val = { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0} }, { .val = { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0} } },
                { { .val = { 3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0} }, { .val = { 3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0} } },
                { { .val = { 4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0} }, { .val = { 4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0,  4.0} } },
                { { .val = { 5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0} }, { .val = { 5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0} } },
                { { .val = { 6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0} }, { .val = { 6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0,  6.0} } },
                { { .val = { 7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0} }, { .val = { 7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0,  7.0} } },
                { { .val = { 8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0} }, { .val = { 8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0,  8.0} } },
                { { .val = { 9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0} }, { .val = { 9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0,  9.0} } },
                { { .val = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0} }, { .val = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0} } },
                { { .val = {11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0} }, { .val = {11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0, 11.0} } },
                { { .val = {12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0} }, { .val = {12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0} } },
                { { .val = {13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0} }, { .val = {13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0, 13.0} } },
                { { .val = {14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0} }, { .val = {14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0, 14.0} } },
                { { .val = {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0} }, { .val = {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0} } },
                { { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} }, { .val = {16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0} } },
                { { .val = {17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0} }, { .val = {17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0, 17.0} } },
            }
        };
        mx_chunk_t rchk = {
            .v8sf_16x2 = {
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
                { { .val = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} }, { .val = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0} } },
            }
        };
        mx_chunk_t shouldbe = {
            .v8sf_16x2 = {
                { { .val = { 272.0,  272.0,  272.0,  272.0,  272.0,  272.0,  272.0,  272.0} }, { .val = { 272.0,  272.0,  272.0,  272.0,  272.0,  272.0,  272.0,  272.0} } },
                { { .val = { 408.0,  408.0,  408.0,  408.0,  408.0,  408.0,  408.0,  408.0} }, { .val = { 408.0,  408.0,  408.0,  408.0,  408.0,  408.0,  408.0,  408.0} } },
                { { .val = { 544.0,  544.0,  544.0,  544.0,  544.0,  544.0,  544.0,  544.0} }, { .val = { 544.0,  544.0,  544.0,  544.0,  544.0,  544.0,  544.0,  544.0} } },
                { { .val = { 680.0,  680.0,  680.0,  680.0,  680.0,  680.0,  680.0,  680.0} }, { .val = { 680.0,  680.0,  680.0,  680.0,  680.0,  680.0,  680.0,  680.0} } },
                { { .val = { 816.0,  816.0,  816.0,  816.0,  816.0,  816.0,  816.0,  816.0} }, { .val = { 816.0,  816.0,  816.0,  816.0,  816.0,  816.0,  816.0,  816.0} } },
                { { .val = { 952.0,  952.0,  952.0,  952.0,  952.0,  952.0,  952.0,  952.0} }, { .val = { 952.0,  952.0,  952.0,  952.0,  952.0,  952.0,  952.0,  952.0} } },
                { { .val = {1088.0, 1088.0, 1088.0, 1088.0, 1088.0, 1088.0, 1088.0, 1088.0} }, { .val = {1088.0, 1088.0, 1088.0, 1088.0, 1088.0, 1088.0, 1088.0, 1088.0} } },
                { { .val = {1224.0, 1224.0, 1224.0, 1224.0, 1224.0, 1224.0, 1224.0, 1224.0} }, { .val = {1224.0, 1224.0, 1224.0, 1224.0, 1224.0, 1224.0, 1224.0, 1224.0} } },
                { { .val = {1360.0, 1360.0, 1360.0, 1360.0, 1360.0, 1360.0, 1360.0, 1360.0} }, { .val = {1360.0, 1360.0, 1360.0, 1360.0, 1360.0, 1360.0, 1360.0, 1360.0} } },
                { { .val = {1496.0, 1496.0, 1496.0, 1496.0, 1496.0, 1496.0, 1496.0, 1496.0} }, { .val = {1496.0, 1496.0, 1496.0, 1496.0, 1496.0, 1496.0, 1496.0, 1496.0} } },
                { { .val = {1632.0, 1632.0, 1632.0, 1632.0, 1632.0, 1632.0, 1632.0, 1632.0} }, { .val = {1632.0, 1632.0, 1632.0, 1632.0, 1632.0, 1632.0, 1632.0, 1632.0} } },
                { { .val = {1768.0, 1768.0, 1768.0, 1768.0, 1768.0, 1768.0, 1768.0, 1768.0} }, { .val = {1768.0, 1768.0, 1768.0, 1768.0, 1768.0, 1768.0, 1768.0, 1768.0} } },
                { { .val = {1904.0, 1904.0, 1904.0, 1904.0, 1904.0, 1904.0, 1904.0, 1904.0} }, { .val = {1904.0, 1904.0, 1904.0, 1904.0, 1904.0, 1904.0, 1904.0, 1904.0} } },
                { { .val = {2040.0, 2040.0, 2040.0, 2040.0, 2040.0, 2040.0, 2040.0, 2040.0} }, { .val = {2040.0, 2040.0, 2040.0, 2040.0, 2040.0, 2040.0, 2040.0, 2040.0} } },
                { { .val = {2176.0, 2176.0, 2176.0, 2176.0, 2176.0, 2176.0, 2176.0, 2176.0} }, { .val = {2176.0, 2176.0, 2176.0, 2176.0, 2176.0, 2176.0, 2176.0, 2176.0} } },
                { { .val = {2312.0, 2312.0, 2312.0, 2312.0, 2312.0, 2312.0, 2312.0, 2312.0} }, { .val = {2312.0, 2312.0, 2312.0, 2312.0, 2312.0, 2312.0, 2312.0, 2312.0} } },
            }
        };

        memset(&dchk, 0, sizeof(dchk));

        v8sf_multiply_chunk_full_to_full(&dchk, &lchk, &rchk, 16, 16, 16);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(value_at(&dchk, i, j), value_at(&shouldbe, i, j), i, j);
            } // for
        } // for
    }
}

Test(InternalFunc, v8sf_multiply_scalar_chunk_fully)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 16x16 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} }, { .val = {  8.0,   9.0,  10.0,  11.0,  12.0,  13.0,  14.0,  15.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} }, { .val = { 24.0,  25.0,  26.0,  27.0,  28.0,  29.0,  30.0,  31.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} }, { .val = { 40.0,  41.0,  42.0,  43.0,  44.0,  45.0,  46.0,  47.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} }, { .val = { 56.0,  57.0,  58.0,  59.0,  60.0,  61.0,  62.0,  63.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} }, { .val = { 72.0,  73.0,  74.0,  75.0,  76.0,  77.0,  78.0,  79.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} }, { .val = { 88.0,  89.0,  90.0,  91.0,  92.0,  93.0,  94.0,  95.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} }, { .val = {104.0, 105.0, 106.0, 107.0, 108.0, 109.0, 110.0, 111.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} }, { .val = {120.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0} } },
                { { .val = {128.0, 129.0, 130.0, 131.0, 132.0, 133.0, 134.0, 135.0} }, { .val = {136.0, 137.0, 138.0, 139.0, 140.0, 141.0, 142.0, 143.0} } },
                { { .val = {144.0, 145.0, 146.0, 147.0, 148.0, 149.0, 150.0, 151.0} }, { .val = {152.0, 153.0, 154.0, 155.0, 156.0, 157.0, 158.0, 159.0} } },
                { { .val = {160.0, 161.0, 162.0, 163.0, 164.0, 165.0, 166.0, 167.0} }, { .val = {168.0, 169.0, 170.0, 171.0, 172.0, 173.0, 174.0, 175.0} } },
                { { .val = {176.0, 177.0, 178.0, 179.0, 180.0, 181.0, 182.0, 183.0} }, { .val = {184.0, 185.0, 186.0, 187.0, 188.0, 189.0, 190.0, 191.0} } },
                { { .val = {192.0, 193.0, 194.0, 195.0, 196.0, 197.0, 198.0, 199.0} }, { .val = {200.0, 201.0, 202.0, 203.0, 204.0, 205.0, 206.0, 207.0} } },
                { { .val = {208.0, 209.0, 210.0, 211.0, 212.0, 213.0, 214.0, 215.0} }, { .val = {216.0, 217.0, 218.0, 219.0, 220.0, 221.0, 222.0, 223.0} } },
                { { .val = {224.0, 225.0, 226.0, 227.0, 228.0, 229.0, 230.0, 231.0} }, { .val = {232.0, 233.0, 234.0, 235.0, 236.0, 237.0, 238.0, 239.0} } },
                { { .val = {240.0, 241.0, 242.0, 243.0, 244.0, 245.0, 246.0, 247.0} }, { .val = {248.0, 249.0, 250.0, 251.0, 252.0, 253.0, 254.0, 255.0} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_fully(&dst, &src, &factor);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[0], mx_type_val(src.v8sf_16x2[i][j])[0] * 2, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[1], mx_type_val(src.v8sf_16x2[i][j])[1] * 2, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[2], mx_type_val(src.v8sf_16x2[i][j])[2] * 2, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[3], mx_type_val(src.v8sf_16x2[i][j])[3] * 2, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[4], mx_type_val(src.v8sf_16x2[i][j])[4] * 2, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[5], mx_type_val(src.v8sf_16x2[i][j])[5] * 2, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[6], mx_type_val(src.v8sf_16x2[i][j])[6] * 2, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[7], mx_type_val(src.v8sf_16x2[i][j])[7] * 2, i, j * 8 + 7);
            } // for
        } // for
    }
}

Test(InternalFunc, v8sf_multiply_scalar_chunk_partly)
{
    mx_chunk_t dst;
    uint32_t i = 0;
    uint32_t j = 0;
    
    // -- 1x1 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 1, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
    }
    
    // -- 5x5 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 5, 5);
        for (i = 0; i < 5; i += 1) {
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], mx_type_val(src.v8sf_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], mx_type_val(src.v8sf_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], mx_type_val(src.v8sf_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], mx_type_val(src.v8sf_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
        } // for
    }

    // -- 8x8 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 8, 8);
        for (i = 0; i < 8; i += 1) {
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], mx_type_val(src.v8sf_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], mx_type_val(src.v8sf_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], mx_type_val(src.v8sf_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], mx_type_val(src.v8sf_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[5], mx_type_val(src.v8sf_16x1[0][0])[5] * 2, 0, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[6], mx_type_val(src.v8sf_16x1[0][0])[6] * 2, 0, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[7], mx_type_val(src.v8sf_16x1[0][0])[7] * 2, 0, 0 * 8 + 7);
        } // for
    }
    
    // -- 9x9 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} }, { .val = {  8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} }, { .val = { 24.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} }, { .val = { 40.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} }, { .val = { 56.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} }, { .val = { 72.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} }, { .val = { 88.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} }, { .val = {104.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} }, { .val = {120.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {128.0, 129.0, 130.0, 131.0, 132.0, 133.0, 134.0, 135.0} }, { .val = {136.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 9, 9);
        for (i = 0; i < 9; i += 1) {
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[0], mx_type_val(src.v8sf_16x2[0][0])[0] * 2, 0, 0 * 8 + 0);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[1], mx_type_val(src.v8sf_16x2[0][0])[1] * 2, 0, 0 * 8 + 1);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[2], mx_type_val(src.v8sf_16x2[0][0])[2] * 2, 0, 0 * 8 + 2);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[3], mx_type_val(src.v8sf_16x2[0][0])[3] * 2, 0, 0 * 8 + 3);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[4], mx_type_val(src.v8sf_16x2[0][0])[4] * 2, 0, 0 * 8 + 4);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[5], mx_type_val(src.v8sf_16x2[0][0])[5] * 2, 0, 0 * 8 + 5);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[6], mx_type_val(src.v8sf_16x2[0][0])[6] * 2, 0, 0 * 8 + 6);
            check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[7], mx_type_val(src.v8sf_16x2[0][0])[7] * 2, 0, 0 * 8 + 7);

            check_value_at(mx_type_val(dst.v8sf_16x2[0][1])[0], mx_type_val(src.v8sf_16x2[0][1])[0] * 2, 0, 1 * 8 + 0);
        } // for
    }
    
    // -- 16x16 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {  0.0,   1.0,   2.0,   3.0,   4.0,   5.0,   6.0,   7.0} }, { .val = {  8.0,   9.0,  10.0,  11.0,  12.0,  13.0,  14.0,  15.0} } },
                { { .val = { 16.0,  17.0,  18.0,  19.0,  20.0,  21.0,  22.0,  23.0} }, { .val = { 24.0,  25.0,  26.0,  27.0,  28.0,  29.0,  30.0,  31.0} } },
                { { .val = { 32.0,  33.0,  34.0,  35.0,  36.0,  37.0,  38.0,  39.0} }, { .val = { 40.0,  41.0,  42.0,  43.0,  44.0,  45.0,  46.0,  47.0} } },
                { { .val = { 48.0,  49.0,  50.0,  51.0,  52.0,  53.0,  54.0,  55.0} }, { .val = { 56.0,  57.0,  58.0,  59.0,  60.0,  61.0,  62.0,  63.0} } },
                { { .val = { 64.0,  65.0,  66.0,  67.0,  68.0,  69.0,  70.0,  71.0} }, { .val = { 72.0,  73.0,  74.0,  75.0,  76.0,  77.0,  78.0,  79.0} } },
                { { .val = { 80.0,  81.0,  82.0,  83.0,  84.0,  85.0,  86.0,  87.0} }, { .val = { 88.0,  89.0,  90.0,  91.0,  92.0,  93.0,  94.0,  95.0} } },
                { { .val = { 96.0,  97.0,  98.0,  99.0, 100.0, 101.0, 102.0, 103.0} }, { .val = {104.0, 105.0, 106.0, 107.0, 108.0, 109.0, 110.0, 111.0} } },
                { { .val = {112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0} }, { .val = {120.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0} } },
                { { .val = {128.0, 129.0, 130.0, 131.0, 132.0, 133.0, 134.0, 135.0} }, { .val = {136.0, 137.0, 138.0, 139.0, 140.0, 141.0, 142.0, 143.0} } },
                { { .val = {144.0, 145.0, 146.0, 147.0, 148.0, 149.0, 150.0, 151.0} }, { .val = {152.0, 153.0, 154.0, 155.0, 156.0, 157.0, 158.0, 159.0} } },
                { { .val = {160.0, 161.0, 162.0, 163.0, 164.0, 165.0, 166.0, 167.0} }, { .val = {168.0, 169.0, 170.0, 171.0, 172.0, 173.0, 174.0, 175.0} } },
                { { .val = {176.0, 177.0, 178.0, 179.0, 180.0, 181.0, 182.0, 183.0} }, { .val = {184.0, 185.0, 186.0, 187.0, 188.0, 189.0, 190.0, 191.0} } },
                { { .val = {192.0, 193.0, 194.0, 195.0, 196.0, 197.0, 198.0, 199.0} }, { .val = {200.0, 201.0, 202.0, 203.0, 204.0, 205.0, 206.0, 207.0} } },
                { { .val = {208.0, 209.0, 210.0, 211.0, 212.0, 213.0, 214.0, 215.0} }, { .val = {216.0, 217.0, 218.0, 219.0, 220.0, 221.0, 222.0, 223.0} } },
                { { .val = {224.0, 225.0, 226.0, 227.0, 228.0, 229.0, 230.0, 231.0} }, { .val = {232.0, 233.0, 234.0, 235.0, 236.0, 237.0, 238.0, 239.0} } },
                { { .val = {240.0, 241.0, 242.0, 243.0, 244.0, 245.0, 246.0, 247.0} }, { .val = {248.0, 249.0, 250.0, 251.0, 252.0, 253.0, 254.0, 255.0} } },
            }
        };

        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[0], mx_type_val(src.v8sf_16x2[i][j])[0] * 2, i, j * 8 + 0);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[1], mx_type_val(src.v8sf_16x2[i][j])[1] * 2, i, j * 8 + 1);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[2], mx_type_val(src.v8sf_16x2[i][j])[2] * 2, i, j * 8 + 2);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[3], mx_type_val(src.v8sf_16x2[i][j])[3] * 2, i, j * 8 + 3);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[4], mx_type_val(src.v8sf_16x2[i][j])[4] * 2, i, j * 8 + 4);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[5], mx_type_val(src.v8sf_16x2[i][j])[5] * 2, i, j * 8 + 5);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[6], mx_type_val(src.v8sf_16x2[i][j])[6] * 2, i, j * 8 + 6);
                check_value_at(mx_type_val(dst.v8sf_16x2[i][j])[7], mx_type_val(src.v8sf_16x2[i][j])[7] * 2, i, j * 8 + 7);
            } // for
        } // for
    }

    // -- 1x5 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = { 0.0,  1.0,  2.0,  3.0,  4.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 1, 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], mx_type_val(src.v8sf_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], mx_type_val(src.v8sf_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], mx_type_val(src.v8sf_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], mx_type_val(src.v8sf_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
    }

    // -- 1x8 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 1, 8);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[1], mx_type_val(src.v8sf_16x1[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[2], mx_type_val(src.v8sf_16x1[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[3], mx_type_val(src.v8sf_16x1[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[4], mx_type_val(src.v8sf_16x1[0][0])[4] * 2, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[5], mx_type_val(src.v8sf_16x1[0][0])[5] * 2, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[6], mx_type_val(src.v8sf_16x1[0][0])[6] * 2, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[7], mx_type_val(src.v8sf_16x1[0][0])[7] * 2, 0, 0 * 8 + 7);
    }

    // -- 1x9 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x2 = {
                { { .val = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0} },  { .val = {8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 1, 9);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[0], mx_type_val(src.v8sf_16x2[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[1], mx_type_val(src.v8sf_16x2[0][0])[1] * 2, 0, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[2], mx_type_val(src.v8sf_16x2[0][0])[2] * 2, 0, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[3], mx_type_val(src.v8sf_16x2[0][0])[3] * 2, 0, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[4], mx_type_val(src.v8sf_16x2[0][0])[4] * 2, 0, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[5], mx_type_val(src.v8sf_16x2[0][0])[5] * 2, 0, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[6], mx_type_val(src.v8sf_16x2[0][0])[6] * 2, 0, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x2[0][0])[7], mx_type_val(src.v8sf_16x2[0][0])[7] * 2, 0, 0 * 8 + 7);

        check_value_at(mx_type_val(dst.v8sf_16x2[0][1])[0], mx_type_val(src.v8sf_16x2[0][1])[0] * 2, 0, 1 * 8 + 0);
    }

    // -- 5x1 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 5, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[1][0])[0], mx_type_val(src.v8sf_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[2][0])[0], mx_type_val(src.v8sf_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[3][0])[0], mx_type_val(src.v8sf_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[4][0])[0], mx_type_val(src.v8sf_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
    }

    // -- 8x1 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 80.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 96.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {112.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 8, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[1][0])[0], mx_type_val(src.v8sf_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[2][0])[0], mx_type_val(src.v8sf_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[3][0])[0], mx_type_val(src.v8sf_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[4][0])[0], mx_type_val(src.v8sf_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x1[5][0])[0], mx_type_val(src.v8sf_16x1[5][0])[0] * 2, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[6][0])[0], mx_type_val(src.v8sf_16x1[6][0])[0] * 2, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x1[7][0])[0], mx_type_val(src.v8sf_16x1[7][0])[0] * 2, 7, 0 * 8 + 7);
    }

    // -- 9x1 chunk -- //
    {
        v8sf_t factor = { .val = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0} };
        mx_chunk_t src = {
            .v8sf_16x1 = {
                { { .val = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 48.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 80.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = { 96.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {112.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
                { { .val = {128.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} } },
            }
        };
        memset(&dst, 0, sizeof(dst));

        v8sf_multiply_scalar_chunk_partly(&dst, &src, &factor, 9, 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[0][0])[0], mx_type_val(src.v8sf_16x1[0][0])[0] * 2, 0, 0 * 8 + 0);
        check_value_at(mx_type_val(dst.v8sf_16x1[1][0])[0], mx_type_val(src.v8sf_16x1[1][0])[0] * 2, 1, 0 * 8 + 1);
        check_value_at(mx_type_val(dst.v8sf_16x1[2][0])[0], mx_type_val(src.v8sf_16x1[2][0])[0] * 2, 2, 0 * 8 + 2);
        check_value_at(mx_type_val(dst.v8sf_16x1[3][0])[0], mx_type_val(src.v8sf_16x1[3][0])[0] * 2, 3, 0 * 8 + 3);
        check_value_at(mx_type_val(dst.v8sf_16x1[4][0])[0], mx_type_val(src.v8sf_16x1[4][0])[0] * 2, 4, 0 * 8 + 4);
        check_value_at(mx_type_val(dst.v8sf_16x1[5][0])[0], mx_type_val(src.v8sf_16x1[5][0])[0] * 2, 5, 0 * 8 + 5);
        check_value_at(mx_type_val(dst.v8sf_16x1[6][0])[0], mx_type_val(src.v8sf_16x1[6][0])[0] * 2, 6, 0 * 8 + 6);
        check_value_at(mx_type_val(dst.v8sf_16x1[7][0])[0], mx_type_val(src.v8sf_16x1[7][0])[0] * 2, 7, 0 * 8 + 7);
        check_value_at(mx_type_val(dst.v8sf_16x1[8][1])[0], mx_type_val(src.v8sf_16x1[8][1])[0] * 2, 8, 1 * 8 + 0);
    }
}

Test(Operation, mstr_v8sf_add)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr lhs = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr mx = NULL;

    // -- 1x1 matrix addition -- //
    {
        lhs = mstr_v8sf_create(1, 1);
        mstr_set_f32(lhs, 0, 0, 1.0);

        rhs = mstr_v8sf_create(1, 1);
        mstr_set_f32(rhs, 0, 0, 99.0);

        mx = mstr_v8sf_create(1, 1);

        mstr_v8sf_add(mx, lhs, rhs);

        check_value_at(mstr_get_f32(mx, 0, 0), 100.0, 0, 0);

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 5x5 matrix addition -- //
    {
        lhs = mstr_v8sf_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j));
            } // for
        } // for

        rhs = mstr_v8sf_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(5, 5);

        mstr_v8sf_add(mx, lhs, rhs);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j + i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 8x8 matrix addition -- //
    {
        lhs = mstr_v8sf_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j));
            } // for
        } // for

        rhs = mstr_v8sf_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(8, 8);

        mstr_v8sf_add(mx, lhs, rhs);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j + i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 9x9 matrix addition -- //
    {
        lhs = mstr_v8sf_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j));
            } // for
        } // for

        rhs = mstr_v8sf_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(9, 9);

        mstr_v8sf_add(mx, lhs, rhs);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j + i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 16x16 matrix addition -- //
    {
        lhs = mstr_v8sf_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j));
            } // for
        } // for

        rhs = mstr_v8sf_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(16, 16);

        mstr_v8sf_add(mx, lhs, rhs);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j + i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 17x17 matrix addition -- //
    {
        lhs = mstr_v8sf_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j));
            } // for
        } // for

        rhs = mstr_v8sf_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(17, 17);

        mstr_v8sf_add(mx, lhs, rhs);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j + i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 2x33 matrix addition -- //
    {
        lhs = mstr_v8sf_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j));
            } // for
        } // for

        rhs = mstr_v8sf_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(2, 33);

        mstr_v8sf_add(mx, lhs, rhs);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j + i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 33x17 matrix addition -- //
    {
        lhs = mstr_v8sf_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j));
            } // for
        } // for

        rhs = mstr_v8sf_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(33, 17);

        mstr_v8sf_add(mx, lhs, rhs);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j + i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }
}

Test(Operation, mstr_v8sf_subtract)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr lhs = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr mx = NULL;

    // -- 1x1 matrix subtraction -- //
    {
        lhs = mstr_v8sf_create(1, 1);
        mstr_set_f32(lhs, 0, 0, 1.0);

        rhs = mstr_v8sf_create(1, 1);
        mstr_set_f32(rhs, 0, 0, 99.0);

        mx = mstr_v8sf_create(1, 1);

        mstr_v8sf_subtract(mx, lhs, rhs);

        check_value_at(mstr_get_f32(mx, 0, 0), -98.0, 0, 0);

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 5x5 matrix subtraction -- //
    {
        lhs = mstr_v8sf_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j * 2));
            } // for
        } // for

        rhs = mstr_v8sf_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(5, 5);

        mstr_v8sf_subtract(mx, lhs, rhs);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j * 2) - (float)(i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 8x8 matrix subtraction -- //
    {
        lhs = mstr_v8sf_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j * 2));
            } // for
        } // for

        rhs = mstr_v8sf_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(8, 8);

        mstr_v8sf_subtract(mx, lhs, rhs);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j * 2) - (float)(i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 9x9 matrix subtraction -- //
    {
        lhs = mstr_v8sf_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j * 2));
            } // for
        } // for

        rhs = mstr_v8sf_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(9, 9);

        mstr_v8sf_subtract(mx, lhs, rhs);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j * 2) - (float)(i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 16x16 matrix subtraction -- //
    {
        lhs = mstr_v8sf_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j * 2));
            } // for
        } // for

        rhs = mstr_v8sf_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(16, 16);

        mstr_v8sf_subtract(mx, lhs, rhs);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j * 2) - (float)(i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 17x17 matrix subtraction -- //
    {
        lhs = mstr_v8sf_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j * 2));
            } // for
        } // for

        rhs = mstr_v8sf_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(17, 17);

        mstr_v8sf_subtract(mx, lhs, rhs);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j * 2) - (float)(i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 2x33 matrix subtraction -- //
    {
        lhs = mstr_v8sf_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j * 2));
            } // for
        } // for

        rhs = mstr_v8sf_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(2, 33);

        mstr_v8sf_subtract(mx, lhs, rhs);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j * 2) - (float)(i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 33x17 matrix subtraction -- //
    {
        lhs = mstr_v8sf_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(lhs, i, j, (float)(i * j * 2));
            } // for
        } // for

        rhs = mstr_v8sf_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(rhs, i, j, (float)(i + j));
            } // for
        } // for

        mx = mstr_v8sf_create(33, 17);

        mstr_v8sf_subtract(mx, lhs, rhs);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), (float)(i * j * 2) - (float)(i + j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }
}

Test(Operation, mstr_v8sf_multiply_scalar)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr src = NULL;
    mx_stor_ptr mx = NULL;

    // -- 1x1 matrix scalar multiplication -- //
    {
        src = mstr_v8sf_create(1, 1);
        mstr_set_f32(src, 0, 0, 99.0);

        mx = mstr_v8sf_create(1, 1);

        mstr_v8sf_multiply_scalar(mx, src, 2.0);

        check_value_at(mstr_get_f32(mx, 0, 0), 198.0, 0, 0);

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(src);
    }

    // -- 5x5 matrix scalar multiplication -- //
    {
        src = mstr_v8sf_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_f32(src, i, j, (float)(i - j));
            } // for
        } // for

        mx = mstr_v8sf_create(5, 5);

        mstr_v8sf_multiply_scalar(mx, src, 3.0);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), 3.0 * (float)(i - j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(src);
    }

    // -- 8x8 matrix scalar multiplication -- //
    {
        src = mstr_v8sf_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_f32(src, i, j, (float)(i - j));
            } // for
        } // for

        mx = mstr_v8sf_create(8, 8);

        mstr_v8sf_multiply_scalar(mx, src, 14.0);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), 14.0 * (float)(i - j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(src);
    }

    // -- 9x9 matrix scalar multiplication -- //
    {
        src = mstr_v8sf_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(src, i, j, (float)(i - j));
            } // for
        } // for

        mx = mstr_v8sf_create(9, 9);

        mstr_v8sf_multiply_scalar(mx, src, 7.0);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), 7.0 * (float)(i - j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(src);
    }

    // -- 16x16 matrix scalar multiplication -- //
    {
        src = mstr_v8sf_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_f32(src, i, j, (float)(i - j));
            } // for
        } // for

        mx = mstr_v8sf_create(16, 16);

        mstr_v8sf_multiply_scalar(mx, src, 25.0);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), 25.0 * (float)(i - j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(src);
    }

    // -- 17x17 matrix scalar multiplication -- //
    {
        src = mstr_v8sf_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(src, i, j, (float)(i - j));
            } // for
        } // for

        mx = mstr_v8sf_create(17, 17);

        mstr_v8sf_multiply_scalar(mx, src, 99.0);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), 99.0 * (float)(i - j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(src);
    }

    // -- 2x33 matrix scalar multiplication -- //
    {
        src = mstr_v8sf_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_f32(src, i, j, (float)(i - j));
            } // for
        } // for

        mx = mstr_v8sf_create(2, 33);

        mstr_v8sf_multiply_scalar(mx, src, 13.0);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), 13.0 * (float)(i - j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(src);
    }

    // -- 33x17 matrix scalar multiplication -- //
    {
        src = mstr_v8sf_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(src, i, j, (float)(i - j));
            } // for
        } // for

        mx = mstr_v8sf_create(33, 17);

        mstr_v8sf_multiply_scalar(mx, src, 77.0);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_f32(mx, i, j), 77.0 * (float)(i - j), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(mx);
        mstr_v8sf_destroy(src);
    }
}

Test(Operation, mstr_v8sf_multiply)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr lhs = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr dst = NULL;

    // -- 1x1 matrix multiplication -- //
    {
        lhs = mstr_v8sf_create(1, 1);
        mstr_set_f32(lhs, 0, 0, 11.0);

        rhs = mstr_v8sf_create(1, 1);
        mstr_set_f32(rhs, 0, 0, 2.0);

        dst = mstr_v8sf_create(1, 1);

        mstr_v8sf_multiply(dst, lhs, rhs);

        check_value_at(mstr_get_f32(dst, 0, 0), 22.0, 0, 0);

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 5x5 matrix multiplication -- //
    {
        float ret_vals[5][5] = {
            {0.0, 5.0, 10.0, 15.0, 20.0},
            {0.0, 5.0, 10.0, 15.0, 20.0},
            {0.0, 5.0, 10.0, 15.0, 20.0},
            {0.0, 5.0, 10.0, 15.0, 20.0},
            {0.0, 5.0, 10.0, 15.0, 20.0}
        };

        lhs = mstr_v8sf_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_f32(lhs, i, j, 1.0);
            } // for
        } // for

        rhs = mstr_v8sf_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_f32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8sf_create(5, 5);

        mstr_v8sf_multiply(dst, lhs, rhs);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 8x8 matrix multiplication -- //
    {
        float ret_vals[8][8] = {
            {0.0, 8.0, 16.0, 24.0, 32.0, 40.0, 48.0, 56.0},
            {0.0, 8.0, 16.0, 24.0, 32.0, 40.0, 48.0, 56.0},
            {0.0, 8.0, 16.0, 24.0, 32.0, 40.0, 48.0, 56.0},
            {0.0, 8.0, 16.0, 24.0, 32.0, 40.0, 48.0, 56.0},
            {0.0, 8.0, 16.0, 24.0, 32.0, 40.0, 48.0, 56.0},
            {0.0, 8.0, 16.0, 24.0, 32.0, 40.0, 48.0, 56.0},
            {0.0, 8.0, 16.0, 24.0, 32.0, 40.0, 48.0, 56.0},
            {0.0, 8.0, 16.0, 24.0, 32.0, 40.0, 48.0, 56.0}
        };

        lhs = mstr_v8sf_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_f32(lhs, i, j, 1.0);
            } // for
        } // for

        rhs = mstr_v8sf_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_f32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8sf_create(8, 8);

        mstr_v8sf_multiply(dst, lhs, rhs);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 9x9 matrix multiplication -- //
    {
        float ret_vals[9][9] = {
            {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0},
            {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0},
            {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0},
            {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0},
            {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0},
            {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0},
            {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0},
            {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0},
            {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0}
        };

        lhs = mstr_v8sf_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(lhs, i, j, 1.0);
            } // for
        } // for

        rhs = mstr_v8sf_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8sf_create(9, 9);

        mstr_v8sf_multiply(dst, lhs, rhs);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 16x16 matrix multiplication -- //
    {
        float ret_vals[16][16] = {
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0},
            {0.0, 16.0, 32.0, 48.0, 64.0, 80.0, 96.0, 112.0, 128.0, 144.0, 160.0, 176.0, 192.0, 208.0, 224.0, 240.0}
        };

        lhs = mstr_v8sf_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_f32(lhs, i, j, 1.0);
            } // for
        } // for

        rhs = mstr_v8sf_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_f32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8sf_create(16, 16);

        mstr_v8sf_multiply(dst, lhs, rhs);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 17x17 matrix multiplication -- //
    {
        float ret_vals[17][17] = {
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0},
            {0.0, 17.0, 34.0, 51.0, 68.0, 85.0, 102.0, 119.0, 136.0, 153.0, 170.0, 187.0, 204.0, 221.0, 238.0, 255.0, 272.0}
        };

        lhs = mstr_v8sf_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(lhs, i, j, 1.0);
            } // for
        } // for

        rhs = mstr_v8sf_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8sf_create(17, 17);

        mstr_v8sf_multiply(dst, lhs, rhs);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }

    // -- 9x33 and 33x9 matrix multiplication -- //
    {
        float ret_vals[9][9] = {
            {0.0, 33.0, 66.0, 99.0, 132.0, 165.0, 198.0, 231.0, 264.0},
            {0.0, 33.0, 66.0, 99.0, 132.0, 165.0, 198.0, 231.0, 264.0},
            {0.0, 33.0, 66.0, 99.0, 132.0, 165.0, 198.0, 231.0, 264.0},
            {0.0, 33.0, 66.0, 99.0, 132.0, 165.0, 198.0, 231.0, 264.0},
            {0.0, 33.0, 66.0, 99.0, 132.0, 165.0, 198.0, 231.0, 264.0},
            {0.0, 33.0, 66.0, 99.0, 132.0, 165.0, 198.0, 231.0, 264.0},
            {0.0, 33.0, 66.0, 99.0, 132.0, 165.0, 198.0, 231.0, 264.0},
            {0.0, 33.0, 66.0, 99.0, 132.0, 165.0, 198.0, 231.0, 264.0},
            {0.0, 33.0, 66.0, 99.0, 132.0, 165.0, 198.0, 231.0, 264.0}
        };

        lhs = mstr_v8sf_create(9, 33);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_f32(lhs, i, j, 1.0);
            } // for
        } // for

        rhs = mstr_v8sf_create(33, 9);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(rhs, i, j, j);
            } // for
        } // for

        dst = mstr_v8sf_create(9, 9);

        mstr_v8sf_multiply(dst, lhs, rhs);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(rhs);
        mstr_v8sf_destroy(lhs);
    }
}

Test(Operation, mstr_v8sf_transpose)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_stor_ptr src = NULL;
    mx_stor_ptr dst = NULL;

    // -- 1x1 matrix transpose -- //
    {
        src = mstr_v8sf_create(1, 1);
        mstr_set_f32(src, 0, 0, 11.0);

        dst = mstr_v8sf_create(1, 1);

        mstr_v8sf_transpose(dst, src);

        check_value_at(mstr_get_f32(dst, 0, 0), 11.0, 0, 0);

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(src);
    }

    // -- 5x5 matrix transpose -- //
    {
        src = mstr_v8sf_create(5, 5);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_set_f32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8sf_create(5, 5);

        mstr_v8sf_transpose(dst, src);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), mstr_get_f32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(src);
    }

    // -- 8x8 matrix transpose -- //
    {
        src = mstr_v8sf_create(8, 8);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_set_f32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8sf_create(8, 8);

        mstr_v8sf_transpose(dst, src);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), mstr_get_f32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(src);
    }

    // -- 9x9 matrix transpose -- //
    {
        src = mstr_v8sf_create(9, 9);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8sf_create(9, 9);

        mstr_v8sf_transpose(dst, src);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), mstr_get_f32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(src);
    }

    // -- 16x16 matrix transpose -- //
    {
        src = mstr_v8sf_create(16, 16);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_set_f32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8sf_create(16, 16);

        mstr_v8sf_transpose(dst, src);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), mstr_get_f32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(src);
    }

    // -- 17x17 matrix transpose -- //
    {

        src = mstr_v8sf_create(17, 17);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_set_f32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8sf_create(17, 17);

        mstr_v8sf_transpose(dst, src);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), mstr_get_f32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(src);
    }

    // -- 2x9 matrix transpose -- //
    {
        src = mstr_v8sf_create(2, 9);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_set_f32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8sf_create(9, 2);

        mstr_v8sf_transpose(dst, src);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), mstr_get_f32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(src);
    }

    // -- 9x2 matrix transpose -- //
    {
        src = mstr_v8sf_create(9, 2);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 2; j += 1) {
                mstr_set_f32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8sf_create(2, 9);

        mstr_v8sf_transpose(dst, src);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), mstr_get_f32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(src);
    }

    // -- 17x33 matrix transpose -- //
    {
        src = mstr_v8sf_create(17, 33);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_set_f32(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8sf_create(33, 17);

        mstr_v8sf_transpose(dst, src);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_get_f32(dst, i, j), mstr_get_f32(src, j, i), i, j);
            } // for
        } // for

        mstr_v8sf_destroy(dst);
        mstr_v8sf_destroy(src);
    }
}
