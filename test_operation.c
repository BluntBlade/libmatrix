#include <criterion/criterion.h>

#ifndef MX_OPER_C
#include "mx_operation.c"
#define MX_OPERSTOR_C
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
        for (i = 0; i < mp->lchk_rows; i += 1) {
            for (j = 0; j < mp->lchk_cols; j += 1) {
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
