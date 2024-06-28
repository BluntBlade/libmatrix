#include <criterion/criterion.h>

#ifndef V8SI_OPER_C
#include "src/v8si_operation.c"
#define V8SI_OPER_C
#endif

// ==== TESTS FOR THE OPERATION MODULE OF MATRIX ==== //

#define check_value(val, expect) \
    cr_expect(val == expect, "Wrong value - Expect %d, got %d.", expect, val)

#define check_value_at(val, expect, i, j) \
    cr_expect(val == expect, "Wrong value at (%d,%d) - Expect %d, got %d.", i, j, expect, val)

static mx_chunk_t sample = {
    .i32_vals = {
        {  0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15},
        { 16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31},
        { 32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47},
        { 48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63},
        { 64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79},
        { 80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95},
        { 96,   97,   98,   99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111},
        {112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127},
        {128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143},
        {144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159},
        {160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  170,  171,  172,  173,  174,  175},
        {176,  177,  178,  179,  180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  190,  191},
        {192,  193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207},
        {208,  209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,  221,  222,  223},
        {224,  225,  226,  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239},
        {240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,  255}
    }
};

Test(InternalFunc, v8si_add_chunk_fully)
{
    mx_chunk_t ret;
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    
    // -- 16x16 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 16;
        mp->lchk_cols = 16;

        v8si_add_chunk_fully(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
}

Test(InternalFunc, v8si_add_chunk_partly)
{
    mx_chunk_t ret;
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;

    // -- 1x1 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 1;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 5x5 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 5;
        mp->lchk_cols = 5;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 8x8 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 8;
        mp->lchk_cols = 8;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 9x9 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 9;
        mp->lchk_cols = 9;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
    
    // -- 16x16 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 16;
        mp->lchk_cols = 16;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 1x5 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 5;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 1x8 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 8;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 1x9 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 9;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 5x1 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 5;
        mp->lchk_cols = 1;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 8x1 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 8;
        mp->lchk_cols = 1;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 9x1 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 9;
        mp->lchk_cols = 1;

        v8si_add_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], sample.i32_vals[i][j] * 2, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
}

Test(InternalFunc, v8si_subtract_chunk_fully)
{
    mx_chunk_t ret;
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    
    // -- 16x16 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 16;
        mp->lchk_cols = 16;

        v8si_subtract_chunk_fully(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], 0, i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
}

Test(InternalFunc, v8si_subtract_chunk_partly)
{
    mx_chunk_t ret;
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;

    // -- 1x1 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 1;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 5x5 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 5;
        mp->lchk_cols = 5;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 8x8 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 8;
        mp->lchk_cols = 8;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 9x9 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 9;
        mp->lchk_cols = 9;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
    
    // -- 16x16 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 16;
        mp->lchk_cols = 16;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], 0, i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 1x5 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 5;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 1x8 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 8;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 1x9 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 9;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 5x1 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 5;
        mp->lchk_cols = 1;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 8x1 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 8;
        mp->lchk_cols = 1;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 9x1 chunk -- //
    {
        mp = mops_v8si_create();
        memset(&ret, 4321, sizeof(ret));

        mp->lchk_rows = 9;
        mp->lchk_cols = 1;

        v8si_subtract_chunk_partly(&ret, &sample, &sample, mp);
        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                if (i <= mp->lchk_rows && j <= mp->lchk_cols) {
                    check_value_at(ret.i32_vals[i][j], 0, i, j);
                } else {
                    check_value_at(ret.i32_vals[i][j], 4321, i, j);
                } // if
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
}

static mx_chunk_t qrcode = {
    .i32_vals = {
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2}
    }
};

Test(InternalFunc, v8si_multiply_chunk_fully)
{
    mx_chunk_t ret;
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t ret_qrcode = {
            .i32_vals = {
                {22, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 22},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {22, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 22},
            }
        };

        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 16;
        mp->lchk_cols = 16;
        mp->rchk_rows = 16;
        mp->rchk_cols = 16;
        mp->mchk_rows = 16;
        mp->mchk_cols = 16;

        v8si_multiply_chunk_fully(&ret, &qrcode, &qrcode, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_qrcode.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
}

Test(InternalFunc, v8si_multiply_chunk_partly)
{
    mx_chunk_t ret;
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    
    // -- 1x1 chunk -- //
    {
        mx_chunk_t ret_qrcode = {
            .i32_vals = {
                {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            }
        };
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 1;
        mp->rchk_rows = 1;
        mp->rchk_cols = 1;
        mp->mchk_rows = 1;
        mp->mchk_cols = 1;

        v8si_multiply_chunk_partly(&ret, &qrcode, &qrcode, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_qrcode.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 5x5 chunk -- //
    {
        mx_chunk_t ret_qrcode = {
            .i32_vals = {
                {8, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {6, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {6, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {6, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {6, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            }
        };
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 5;
        mp->lchk_cols = 5;
        mp->rchk_rows = 5;
        mp->rchk_cols = 5;
        mp->mchk_rows = 5;
        mp->mchk_cols = 5;

        v8si_multiply_chunk_partly(&ret, &qrcode, &qrcode, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_qrcode.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 8x8 chunk -- //
    {
        mx_chunk_t ret_qrcode = {
            .i32_vals = {
                {11, 9, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0},
                { 9, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0},
                { 9, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0},
                { 9, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0},
                { 9, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0},
                { 9, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0},
                { 9, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0},
                { 9, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            }
        };
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 8;
        mp->lchk_cols = 8;
        mp->rchk_rows = 8;
        mp->rchk_cols = 8;
        mp->mchk_rows = 8;
        mp->mchk_cols = 8;

        v8si_multiply_chunk_partly(&ret, &qrcode, &qrcode, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_qrcode.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 9x9 chunk -- //
    {
        mx_chunk_t ret_qrcode = {
            .i32_vals = {
                {12, 10, 10, 10, 10, 10, 10, 10, 10,  0,  0,  0,  0,  0,  0,  0},
                {10,  9,  9,  9,  9,  9,  9,  9,  9,  0,  0,  0,  0,  0,  0,  0},
                {10,  9,  9,  9,  9,  9,  9,  9,  9,  0,  0,  0,  0,  0,  0,  0},
                {10,  9,  9,  9,  9,  9,  9,  9,  9,  0,  0,  0,  0,  0,  0,  0},
                {10,  9,  9,  9,  9,  9,  9,  9,  9,  0,  0,  0,  0,  0,  0,  0},
                {10,  9,  9,  9,  9,  9,  9,  9,  9,  0,  0,  0,  0,  0,  0,  0},
                {10,  9,  9,  9,  9,  9,  9,  9,  9,  0,  0,  0,  0,  0,  0,  0},
                {10,  9,  9,  9,  9,  9,  9,  9,  9,  0,  0,  0,  0,  0,  0,  0},
                {10,  9,  9,  9,  9,  9,  9,  9,  9,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
                { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
            }
        };
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 9;
        mp->lchk_cols = 9;
        mp->rchk_rows = 9;
        mp->rchk_cols = 9;
        mp->mchk_rows = 9;
        mp->mchk_cols = 9;

        v8si_multiply_chunk_partly(&ret, &qrcode, &qrcode, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_qrcode.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
    
    // -- 16x16 chunk -- //
    {
        mx_chunk_t ret_qrcode = {
            .i32_vals = {
                {22, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 22},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18},
                {22, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 22},
            }
        };

        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 16;
        mp->lchk_cols = 16;
        mp->rchk_rows = 16;
        mp->rchk_cols = 16;
        mp->mchk_rows = 16;
        mp->mchk_cols = 16;

        v8si_multiply_chunk_partly(&ret, &qrcode, &qrcode, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_qrcode.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
    
    // -- 5x2 and 2x3 chunk -- //
    {
        mx_chunk_t ret_qrcode = {
            .i32_vals = {
                {5, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            }
        };
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 5;
        mp->lchk_cols = 2;
        mp->rchk_rows = 3; // Chunk on the right hand side has swapped rows and cols.
        mp->rchk_cols = 2; // Chunk on the right hand side has swapped rows and cols.
        mp->mchk_rows = 5;
        mp->mchk_cols = 3;

        v8si_multiply_chunk_partly(&ret, &qrcode, &qrcode, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_qrcode.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
    
    // -- 1x9 and 9x8 chunk -- //
    {
        mx_chunk_t ret_qrcode = {
            .i32_vals = {
                {12, 10, 10, 10, 10, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
                { 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
            }
        };
        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->lchk_rows = 1;
        mp->lchk_cols = 9;
        mp->rchk_rows = 9;
        mp->rchk_cols = 8;
        mp->mchk_rows = 1;
        mp->mchk_cols = 8;

        v8si_multiply_chunk_partly(&ret, &qrcode, &qrcode, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_qrcode.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
}

Test(InternalFunc, v8si_scalar_multiply_chunk_fully)
{
    mx_chunk_t ret;
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    
    // -- 16x16 chunk -- //
    {
        v8si_t factor = {2, 2, 2, 2, 2, 2, 2, 2};
        mx_chunk_t ret_sample = {
            .i32_vals = {
                0,    2,    4,    6,    8,    10,   12,   14,   16,   18,   20,   22,   24,   26,   28,   30,
                32,   34,   36,   38,   40,   42,   44,   46,   48,   50,   52,   54,   56,   58,   60,   62,
                64,   66,   68,   70,   72,   74,   76,   78,   80,   82,   84,   86,   88,   90,   92,   94,
                96,   98,   100,  102,  104,  106,  108,  110,  112,  114,  116,  118,  120,  122,  124,  126,
                128,  130,  132,  134,  136,  138,  140,  142,  144,  146,  148,  150,  152,  154,  156,  158,
                160,  162,  164,  166,  168,  170,  172,  174,  176,  178,  180,  182,  184,  186,  188,  190,
                192,  194,  196,  198,  200,  202,  204,  206,  208,  210,  212,  214,  216,  218,  220,  222,
                224,  226,  228,  230,  232,  234,  236,  238,  240,  242,  244,  246,  248,  250,  252,  254,
                256,  258,  260,  262,  264,  266,  268,  270,  272,  274,  276,  278,  280,  282,  284,  286,
                288,  290,  292,  294,  296,  298,  300,  302,  304,  306,  308,  310,  312,  314,  316,  318,
                320,  322,  324,  326,  328,  330,  332,  334,  336,  338,  340,  342,  344,  346,  348,  350,
                352,  354,  356,  358,  360,  362,  364,  366,  368,  370,  372,  374,  376,  378,  380,  382,
                384,  386,  388,  390,  392,  394,  396,  398,  400,  402,  404,  406,  408,  410,  412,  414,
                416,  418,  420,  422,  424,  426,  428,  430,  432,  434,  436,  438,  440,  442,  444,  446,
                448,  450,  452,  454,  456,  458,  460,  462,  464,  466,  468,  470,  472,  474,  476,  478,
                480,  482,  484,  486,  488,  490,  492,  494,  496,  498,  500,  502,  504,  506,  508,  510
            }
        };

        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->rchk_rows = 16;
        mp->rchk_cols = 16;
        mp->mchk_rows = 16;
        mp->mchk_cols = 16;

        v8si_scalar_multiply_chunk_fully(&ret, &factor, &sample, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_sample.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
}

Test(InternalFunc, v8si_scalar_multiply_chunk_partly)
{
    mx_chunk_t ret;
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    
    // -- 1x1 chunk -- //
    {
        v8si_t factor = {2, 2, 2, 2, 2, 2, 2, 2};
        mx_chunk_t ret_sample = {
            .i32_vals = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            }
        };

        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->rchk_rows = 1;
        mp->rchk_cols = 1;
        mp->mchk_rows = 1;
        mp->mchk_cols = 1;

        v8si_scalar_multiply_chunk_partly(&ret, &factor, &sample, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_sample.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
    
    // -- 5x5 chunk -- //
    {
        v8si_t factor = {2, 2, 2, 2, 2, 2, 2, 2};
        mx_chunk_t ret_sample = {
            .i32_vals = {
                  0,   2,   4,   6,   8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 32,  34,  36,  38,  40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 64,  66,  68,  70,  72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 96,  98, 100, 102, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                128, 130, 132, 134, 136, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            }
        };

        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->rchk_rows = 5;
        mp->rchk_cols = 5;
        mp->mchk_rows = 5;
        mp->mchk_cols = 5;

        v8si_scalar_multiply_chunk_partly(&ret, &factor, &sample, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_sample.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }

    // -- 8x8 chunk -- //
    {
        v8si_t factor = {2, 2, 2, 2, 2, 2, 2, 2};
        mx_chunk_t ret_sample = {
            .i32_vals = {
                0,    2,    4,    6,    8,    10,   12,   14,   0,  0,  0,  0,  0,  0,  0,  0,
                32,   34,   36,   38,   40,   42,   44,   46,   0,  0,  0,  0,  0,  0,  0,  0,
                64,   66,   68,   70,   72,   74,   76,   78,   0,  0,  0,  0,  0,  0,  0,  0,
                96,   98,   100,  102,  104,  106,  108,  110,  0,  0,  0,  0,  0,  0,  0,  0,
                128,  130,  132,  134,  136,  138,  140,  142,  0,  0,  0,  0,  0,  0,  0,  0,
                160,  162,  164,  166,  168,  170,  172,  174,  0,  0,  0,  0,  0,  0,  0,  0,
                192,  194,  196,  198,  200,  202,  204,  206,  0,  0,  0,  0,  0,  0,  0,  0,
                224,  226,  228,  230,  232,  234,  236,  238,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,  0
            }
        };

        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->rchk_rows = 8;
        mp->rchk_cols = 8;
        mp->mchk_rows = 8;
        mp->mchk_cols = 8;

        v8si_scalar_multiply_chunk_partly(&ret, &factor, &sample, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_sample.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
    
    // -- 9x9 chunk -- //
    {
        v8si_t factor = {2, 2, 2, 2, 2, 2, 2, 2};
        mx_chunk_t ret_sample = {
            .i32_vals = {
                0,    2,    4,    6,    8,    10,   12,   14,   16,   0,  0,  0,  0,  0,  0,  0,
                32,   34,   36,   38,   40,   42,   44,   46,   48,   0,  0,  0,  0,  0,  0,  0,
                64,   66,   68,   70,   72,   74,   76,   78,   80,   0,  0,  0,  0,  0,  0,  0,
                96,   98,   100,  102,  104,  106,  108,  110,  112,  0,  0,  0,  0,  0,  0,  0,
                128,  130,  132,  134,  136,  138,  140,  142,  144,  0,  0,  0,  0,  0,  0,  0,
                160,  162,  164,  166,  168,  170,  172,  174,  176,  0,  0,  0,  0,  0,  0,  0,
                192,  194,  196,  198,  200,  202,  204,  206,  208,  0,  0,  0,  0,  0,  0,  0,
                224,  226,  228,  230,  232,  234,  236,  238,  240,  0,  0,  0,  0,  0,  0,  0,
                256,  258,  260,  262,  264,  266,  268,  270,  272,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0,
                  0,    0,    0,    0,    0,    0,    0,    0,    0,  0,  0,  0,  0,  0,  0,  0
            }
        };

        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->rchk_rows = 9;
        mp->rchk_cols = 9;
        mp->mchk_rows = 9;
        mp->mchk_cols = 9;

        v8si_scalar_multiply_chunk_partly(&ret, &factor, &sample, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_sample.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
    
    // -- 16x16 chunk -- //
    {
        v8si_t factor = {2, 2, 2, 2, 2, 2, 2, 2};
        mx_chunk_t ret_sample = {
            .i32_vals = {
                0,    2,    4,    6,    8,    10,   12,   14,   16,   18,   20,   22,   24,   26,   28,   30,
                32,   34,   36,   38,   40,   42,   44,   46,   48,   50,   52,   54,   56,   58,   60,   62,
                64,   66,   68,   70,   72,   74,   76,   78,   80,   82,   84,   86,   88,   90,   92,   94,
                96,   98,   100,  102,  104,  106,  108,  110,  112,  114,  116,  118,  120,  122,  124,  126,
                128,  130,  132,  134,  136,  138,  140,  142,  144,  146,  148,  150,  152,  154,  156,  158,
                160,  162,  164,  166,  168,  170,  172,  174,  176,  178,  180,  182,  184,  186,  188,  190,
                192,  194,  196,  198,  200,  202,  204,  206,  208,  210,  212,  214,  216,  218,  220,  222,
                224,  226,  228,  230,  232,  234,  236,  238,  240,  242,  244,  246,  248,  250,  252,  254,
                256,  258,  260,  262,  264,  266,  268,  270,  272,  274,  276,  278,  280,  282,  284,  286,
                288,  290,  292,  294,  296,  298,  300,  302,  304,  306,  308,  310,  312,  314,  316,  318,
                320,  322,  324,  326,  328,  330,  332,  334,  336,  338,  340,  342,  344,  346,  348,  350,
                352,  354,  356,  358,  360,  362,  364,  366,  368,  370,  372,  374,  376,  378,  380,  382,
                384,  386,  388,  390,  392,  394,  396,  398,  400,  402,  404,  406,  408,  410,  412,  414,
                416,  418,  420,  422,  424,  426,  428,  430,  432,  434,  436,  438,  440,  442,  444,  446,
                448,  450,  452,  454,  456,  458,  460,  462,  464,  466,  468,  470,  472,  474,  476,  478,
                480,  482,  484,  486,  488,  490,  492,  494,  496,  498,  500,  502,  504,  506,  508,  510
            }
        };

        mp = mops_v8si_create();
        memset(&ret, 0, sizeof(ret));

        mp->rchk_rows = 16;
        mp->rchk_cols = 16;
        mp->mchk_rows = 16;
        mp->mchk_cols = 16;

        v8si_scalar_multiply_chunk_partly(&ret, &factor, &sample, mp);

        for (i = 0; i < mp->mchk_rows; i += 1) {
            for (j = 0; j < mp->mchk_cols; j += 1) {
                check_value_at(ret.i32_vals[i][j], ret_sample.i32_vals[i][j], i, j);
            } // for
        } // for

        mops_v8si_destroy(mp);
    }
}

Test(Operation, mops_v8si_add)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    mx_stor_ptr lhs = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr ret = NULL;

    // -- 1x1 matrix addition -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(1, 1);
        mstr_v8si_set(lhs, 0, 0, 1);

        rhs = mstr_v8si_create(1, 1);
        mstr_v8si_set(rhs, 0, 0, 99);

        ret = mstr_v8si_create(1, 1);

        mops_v8si_add(mp, lhs, rhs, ret);

        check_value_at(mstr_v8si_get(ret, 0, 0), 100, 0, 0);

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 5x5 matrix addition -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(5, 5);

        mops_v8si_add(mp, lhs, rhs, ret);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 8x8 matrix addition -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(8, 8);

        mops_v8si_add(mp, lhs, rhs, ret);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 9x9 matrix addition -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(9, 9);

        mops_v8si_add(mp, lhs, rhs, ret);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 16x16 matrix addition -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(16, 16);

        mops_v8si_add(mp, lhs, rhs, ret);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 17x17 matrix addition -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(17, 17);

        mops_v8si_add(mp, lhs, rhs, ret);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 2x33 matrix addition -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(2, 33);

        mops_v8si_add(mp, lhs, rhs, ret);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 33x17 matrix addition -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(33, 17);

        mops_v8si_add(mp, lhs, rhs, ret);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j + i - j, i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }
}

Test(Operation, mops_v8si_subtract)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    mx_stor_ptr lhs = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr ret = NULL;

    // -- 1x1 matrix subtraction -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(1, 1);
        mstr_v8si_set(lhs, 0, 0, 1);

        rhs = mstr_v8si_create(1, 1);
        mstr_v8si_set(rhs, 0, 0, 99);

        ret = mstr_v8si_create(1, 1);

        mops_v8si_subtract(mp, lhs, rhs, ret);

        check_value_at(mstr_v8si_get(ret, 0, 0), -98, 0, 0);

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 5x5 matrix subtraction -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(5, 5);

        mops_v8si_subtract(mp, lhs, rhs, ret);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 8x8 matrix subtraction -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(8, 8);

        mops_v8si_subtract(mp, lhs, rhs, ret);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 9x9 matrix subtraction -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(9, 9);

        mops_v8si_subtract(mp, lhs, rhs, ret);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 16x16 matrix subtraction -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(16, 16);

        mops_v8si_subtract(mp, lhs, rhs, ret);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 17x17 matrix subtraction -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(17, 17);

        mops_v8si_subtract(mp, lhs, rhs, ret);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 2x33 matrix subtraction -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(2, 33);

        mops_v8si_subtract(mp, lhs, rhs, ret);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }

    // -- 33x17 matrix subtraction -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(lhs, i, j, i * j);
            } // for
        } // for

        rhs = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(33, 17);

        mops_v8si_subtract(mp, lhs, rhs, ret);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), i * j - (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }
}

Test(Operation, mops_v8si_scalar_multiply)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr ret = NULL;

    // -- 1x1 matrix scalar multiplication -- //
    {
        mp = mops_v8si_create();

        rhs = mstr_v8si_create(1, 1);
        mstr_v8si_set(rhs, 0, 0, 99);

        ret = mstr_v8si_create(1, 1);

        mops_v8si_scalar_multiply(mp, 2, rhs, ret);

        check_value_at(mstr_v8si_get(ret, 0, 0), 198, 0, 0);

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mops_v8si_destroy(mp);
    }

    // -- 5x5 matrix scalar multiplication -- //
    {
        mp = mops_v8si_create();

        rhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(5, 5);

        mops_v8si_scalar_multiply(mp, 3, rhs, ret);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), 3 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mops_v8si_destroy(mp);
    }

    // -- 8x8 matrix scalar multiplication -- //
    {
        mp = mops_v8si_create();

        rhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(8, 8);

        mops_v8si_scalar_multiply(mp, 14, rhs, ret);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), 14 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mops_v8si_destroy(mp);
    }

    // -- 9x9 matrix scalar multiplication -- //
    {
        mp = mops_v8si_create();

        rhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(9, 9);

        mops_v8si_scalar_multiply(mp, 7, rhs, ret);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), 7 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mops_v8si_destroy(mp);
    }

    // -- 16x16 matrix scalar multiplication -- //
    {
        mp = mops_v8si_create();

        rhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(16, 16);

        mops_v8si_scalar_multiply(mp, 25, rhs, ret);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), 25 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mops_v8si_destroy(mp);
    }

    // -- 17x17 matrix scalar multiplication -- //
    {
        mp = mops_v8si_create();

        rhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(17, 17);

        mops_v8si_scalar_multiply(mp, 99, rhs, ret);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), 99 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mops_v8si_destroy(mp);
    }

    // -- 2x33 matrix scalar multiplication -- //
    {
        mp = mops_v8si_create();

        rhs = mstr_v8si_create(2, 33);
        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(2, 33);

        mops_v8si_scalar_multiply(mp, 13, rhs, ret);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 33; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), 13 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mops_v8si_destroy(mp);
    }

    // -- 33x17 matrix scalar multiplication -- //
    {
        mp = mops_v8si_create();

        rhs = mstr_v8si_create(33, 17);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(rhs, i, j, i - j);
            } // for
        } // for

        ret = mstr_v8si_create(33, 17);

        mops_v8si_scalar_multiply(mp, 77, rhs, ret);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), 77 * (i - j), i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mops_v8si_destroy(mp);
    }
}

Test(Operation, mops_v8si_multiply)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    mx_stor_ptr lhs = NULL;
    mx_stor_ptr rhs = NULL;
    mx_stor_ptr ret = NULL;

    // -- 1x1 matrix multiplication -- //
    {
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(1, 1);
        mstr_v8si_set(lhs, 0, 0, 11);

        rhs = mstr_v8si_create(1, 1);
        mstr_v8si_set(rhs, 0, 0, 2);

        ret = mstr_v8si_create(1, 1);

        mops_v8si_multiply(mp, lhs, rhs, ret);

        check_value_at(mstr_v8si_get(ret, 0, 0), 22, 0, 0);

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
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
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_v8si_set(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(5, 5);
        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_v8si_set(rhs, i, j, j);
            } // for
        } // for

        ret = mstr_v8si_create(5, 5);

        mops_v8si_multiply(mp, lhs, rhs, ret);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
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
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_v8si_set(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(8, 8);
        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_v8si_set(rhs, i, j, j);
            } // for
        } // for

        ret = mstr_v8si_create(8, 8);

        mops_v8si_multiply(mp, lhs, rhs, ret);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
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
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(9, 9);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(rhs, i, j, j);
            } // for
        } // for

        ret = mstr_v8si_create(9, 9);

        mops_v8si_multiply(mp, lhs, rhs, ret);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
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
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_v8si_set(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(16, 16);
        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_v8si_set(rhs, i, j, j);
            } // for
        } // for

        ret = mstr_v8si_create(16, 16);

        mops_v8si_multiply(mp, lhs, rhs, ret);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
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
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(17, 17);
        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(rhs, i, j, j);
            } // for
        } // for

        ret = mstr_v8si_create(17, 17);

        mops_v8si_multiply(mp, lhs, rhs, ret);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
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
        mp = mops_v8si_create();

        lhs = mstr_v8si_create(9, 33);
        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_v8si_set(lhs, i, j, 1);
            } // for
        } // for

        rhs = mstr_v8si_create(33, 9);
        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(rhs, i, j, j);
            } // for
        } // for

        ret = mstr_v8si_create(9, 9);

        mops_v8si_multiply(mp, lhs, rhs, ret);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(ret, i, j), ret_vals[i][j], i, j);
            } // for
        } // for

        mstr_v8si_destroy(ret);
        mstr_v8si_destroy(rhs);
        mstr_v8si_destroy(lhs);
        mops_v8si_destroy(mp);
    }
}

Test(Operation, mops_v8si_transpose)
{
    uint32_t i = 0;
    uint32_t j = 0;
    mx_oper_ptr mp = NULL;
    mx_stor_ptr src = NULL;
    mx_stor_ptr dst = NULL;

    // -- 1x1 matrix transpose -- //
    {
        mp = mops_v8si_create();

        src = mstr_v8si_create(1, 1);
        mstr_v8si_set(src, 0, 0, 11);

        dst = mstr_v8si_create(1, 1);
        mops_v8si_transpose(mp, src, dst);

        check_value_at(mstr_v8si_get(dst, 0, 0), 11, 0, 0);

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
        mops_v8si_destroy(mp);
    }

    // -- 5x5 matrix transpose -- //
    {
        mp = mops_v8si_create();

        src = mstr_v8si_create(5, 5);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                mstr_v8si_set(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(5, 5);
        mops_v8si_transpose(mp, src, dst);

        for (i = 0; i < 5; i += 1) {
            for (j = 0; j < 5; j += 1) {
                check_value_at(mstr_v8si_get(dst, i, j), mstr_v8si_get(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
        mops_v8si_destroy(mp);
    }

    // -- 8x8 matrix transpose -- //
    {
        mp = mops_v8si_create();

        src = mstr_v8si_create(8, 8);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                mstr_v8si_set(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(8, 8);
        mops_v8si_transpose(mp, src, dst);

        for (i = 0; i < 8; i += 1) {
            for (j = 0; j < 8; j += 1) {
                check_value_at(mstr_v8si_get(dst, i, j), mstr_v8si_get(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
        mops_v8si_destroy(mp);
    }

    // -- 9x9 matrix transpose -- //
    {
        mp = mops_v8si_create();

        src = mstr_v8si_create(9, 9);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(9, 9);
        mops_v8si_transpose(mp, src, dst);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(dst, i, j), mstr_v8si_get(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
        mops_v8si_destroy(mp);
    }

    // -- 16x16 matrix transpose -- //
    {
        mp = mops_v8si_create();

        src = mstr_v8si_create(16, 16);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                mstr_v8si_set(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(16, 16);
        mops_v8si_transpose(mp, src, dst);

        for (i = 0; i < 16; i += 1) {
            for (j = 0; j < 16; j += 1) {
                check_value_at(mstr_v8si_get(dst, i, j), mstr_v8si_get(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
        mops_v8si_destroy(mp);
    }

    // -- 17x17 matrix transpose -- //
    {
        mp = mops_v8si_create();

        src = mstr_v8si_create(17, 17);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                mstr_v8si_set(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(17, 17);
        mops_v8si_transpose(mp, src, dst);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(dst, i, j), mstr_v8si_get(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
        mops_v8si_destroy(mp);
    }

    // -- 2x9 matrix transpose -- //
    {
        mp = mops_v8si_create();

        src = mstr_v8si_create(2, 9);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 9; j += 1) {
                mstr_v8si_set(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(9, 2);
        mops_v8si_transpose(mp, src, dst);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 2; j += 1) {
                check_value_at(mstr_v8si_get(dst, i, j), mstr_v8si_get(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
        mops_v8si_destroy(mp);
    }

    // -- 9x2 matrix transpose -- //
    {
        mp = mops_v8si_create();

        src = mstr_v8si_create(9, 2);

        for (i = 0; i < 9; i += 1) {
            for (j = 0; j < 2; j += 1) {
                mstr_v8si_set(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(2, 9);
        mops_v8si_transpose(mp, src, dst);

        for (i = 0; i < 2; i += 1) {
            for (j = 0; j < 9; j += 1) {
                check_value_at(mstr_v8si_get(dst, i, j), mstr_v8si_get(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
        mops_v8si_destroy(mp);
    }

    // -- 17x33 matrix transpose -- //
    {
        mp = mops_v8si_create();

        src = mstr_v8si_create(17, 33);

        for (i = 0; i < 17; i += 1) {
            for (j = 0; j < 33; j += 1) {
                mstr_v8si_set(src, i, j, i * j);
            } // for
        } // for

        dst = mstr_v8si_create(33, 17);
        mops_v8si_transpose(mp, src, dst);

        for (i = 0; i < 33; i += 1) {
            for (j = 0; j < 17; j += 1) {
                check_value_at(mstr_v8si_get(dst, i, j), mstr_v8si_get(src, j, i), i, j);
            } // for
        } // for

        mstr_v8si_destroy(dst);
        mstr_v8si_destroy(src);
        mops_v8si_destroy(mp);
    }
}
